#include "bdsg/overlays/reference_path_overlay.hpp"

#include <atomic>
#include <thread>
#include <mutex>
#include <omp.h>

#include <handlegraph/util.hpp>
#include <handlegraph/types.hpp>

namespace bdsg {

using namespace std;
using namespace handlegraph;

ReferencePathOverlay::ReferencePathOverlay(const PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names) : graph(graph) {
    
    // Get step counts for all paths we want to process, once.
    std::unordered_map<path_handle_t, size_t> cached_step_counts;
    graph->for_each_path_handle([&](const path_handle_t& path) {
        // Find and measure all the non-hidden paths.
        // TODO: If we made the overlay transparent so we could access paths
        // that didn't get indexed, we wouldn't be weirdly indexing haplotype
        // paths from backends that don't hide them in the "reference" path
        // overlay.
        cached_step_counts[path] = graph->get_step_count(path);
    });
    for (auto& path_name : extra_path_names) {
        // Also index hidden paths that the user is asking for by name.
        if (graph->has_path(path_name)) {
            // The graph actually has this path.
            path_handle_t path = graph->get_path_handle(path_name);
            auto found = cached_step_counts.find(path);
            if (found == cached_step_counts.end()) {
                // And it's not already reference sense.
                // Count steps and remember it
                cached_step_counts.emplace_hint(found, path, graph->get_step_count(path));
            }
        }
    }

    // Now use the cache as a source of truth and make a vector of the paths. 
    std::vector<path_handle_t> path_handles;
    // We also track the numerically max path handle
    uint64_t max_path_handle = 0;
    for (auto& handle_and_length : cached_step_counts) {
        const path_handle_t& path = handle_and_length.first;
        path_handles.push_back(path);
        
        // Each of the paths needs a PathRecord
        reference_paths.insert(pair<path_handle_t, PathRecord>(path, PathRecord()));
        // And needs to be maxed into the max handles.
        max_path_handle = std::max<uint64_t>(max_path_handle, handlegraph::as_integer(path));
    }

    // sort in descending order by length to limit parallel scheduling makespan
    std::sort(path_handles.begin(), path_handles.end(), [&](path_handle_t a, path_handle_t b) {
        return cached_step_counts.at(a) > cached_step_counts.at(b);
    });
    
    std::vector<std::atomic<size_t>> num_steps(graph->max_node_id() + 1);
    
#pragma omp parallel for schedule(dynamic,1)
    for (size_t i = 0; i < path_handles.size(); ++i) {
        auto path = path_handles[i];
        auto& path_record = reference_paths.at(path);
        
        // init the step vectors
        size_t path_size = cached_step_counts.at(path);
        path_record.steps.resize(path_size);
        
        // record the steps and the path length
        size_t path_length = 0;
        size_t j = 0;
        for (auto step = graph->path_begin(path), end = graph->path_end(path); step != end; step = graph->get_next_step(step)) {
            handle_t handle = graph->get_handle_of_step(step);
            path_record.steps.set(j, handlegraph::as_integer(handle));
            path_length += graph->get_length(handle);
            ++num_steps[graph->get_id(handle)];
            ++j;
        }
        
        // set up the offsets bit vector
        path_record.offsets.resize(path_length);
        // it starts as uninitialized memory, so we have to manually 0 it out
        auto data = path_record.offsets.data();
        for (size_t j = 0, n = path_record.offsets.capacity() / 64; j < n; ++j) {
            data[j] = 0ull;
        }
        
        size_t offset = -1;
        for (size_t j = 0; j < path_size; ++j) {
            handle_t handle = handlegraph::as_handle(path_record.steps.get(j));
            offset += graph->get_length(handle);
            path_record.offsets[offset] = 1;
        }
        
        // initialize rank and select queries
        sdsl::util::init_support(path_record.offsets_rank, &path_record.offsets);
        sdsl::util::init_support(path_record.offsets_select, &path_record.offsets);
    }
    
    // make a prefix sum over the number of steps in the paths so we can use it to more efficiently parallelize
    // over paths later (also used here to count total steps)
    std::vector<size_t> steps_prefix_sum(path_handles.size() + 1, 0);
    for (size_t i = 0; i < path_handles.size(); ++i) {
        steps_prefix_sum[i + 1] = steps_prefix_sum[i] + get_step_count(path_handles[i]);
    }
    
    // seed the (non-atomic) prefix sum vector with the (atomic) num steps values
    steps_begin.width(sdsl::bits::length(steps_prefix_sum.back()));
    steps_begin.resize(num_steps.size() + 1);
    // how many positions over do we need to look to guarantee that no entries share a 64-bit window?
    size_t steps_begin_stripe_gap = (63 + steps_begin.width() - 1) / steps_begin.width() + 1;
    // we have to copy over in "stripes" to avoid two threads from writing bit-compressed
    // entries in the same underlying 64 bit integer
    for (size_t stripe_start = 0; stripe_start < steps_begin_stripe_gap; ++stripe_start) {
#pragma omp parallel for schedule(dynamic,8192)
        for (size_t i = stripe_start; i < num_steps.size(); i += steps_begin_stripe_gap) {
            steps_begin[i + 1] = num_steps[i];
        }
    }
    {
        auto dummy = std::move(num_steps);
    }

    // TODO: copied from VG, ugly
    int thread_count = 1;
#pragma omp parallel
    {
#pragma omp master
        thread_count = omp_get_num_threads();
    }
    
    static const size_t pref_sum_block_size = 32768; // 2^15
    // seed a prefix sum vector over blocks and construct block-local prefix sums
    const size_t num_blocks = (steps_begin.size() + pref_sum_block_size - 1) / pref_sum_block_size;
    std::vector<size_t> block_prefix_sum(num_blocks + 1, 0);
    
    // manually do the parallelism over this one so that i don't need to use atomic integers
    // (because all computations take place within one block)
    // note: we split this into and even and odd value loop to avoid threads from fighting over
    // the bit-compressed values' underlying 64 bit ints
    for (int odd_even : {0, 1}) {
        std::atomic<size_t> block_idx(odd_even);
        std::vector<std::thread> workers;
        for (int t = 0; t < thread_count; ++t) {
            workers.emplace_back([&]() {
                while (block_idx.load() < num_blocks) {
                    size_t block = block_idx.fetch_add(2);
                    if (block >= num_blocks) {
                        // all of the blocks have been assigned
                        return;
                    }
                    size_t begin = block * pref_sum_block_size;
                    size_t n = std::min<size_t>(begin + pref_sum_block_size, steps_begin.size());
                    // construct the local prefix sum in this block
                    for (size_t i = begin + 1; i < n; ++i) {
                        steps_begin[i] += steps_begin[i - 1];
                    }
                    // enter the sum of the block's values into the block prefix sum
                    block_prefix_sum[block + 1] = steps_begin[n - 1];
                }
            });
        }
        for (auto& worker : workers) {
            worker.join();
        }
    }
    
    // compute the block prefix sum in serial
    for (size_t i = 1; i < block_prefix_sum.size(); ++i) {
        block_prefix_sum[i] += block_prefix_sum[i - 1];
    }
    
    // add the block prefix sums back into the block-local prefix sums, using stripes to avoid
    // writing into the same backing 64 bit int
    for (size_t stripe_start = 0; stripe_start < steps_begin_stripe_gap; ++stripe_start) {
#pragma omp parallel for schedule(dynamic,8192)
        for (size_t i = stripe_start; i < steps_begin.size(); i += steps_begin_stripe_gap) {
            steps_begin[i] += block_prefix_sum[i / pref_sum_block_size];
        }
    }
    {
        auto dummy = std::move(block_prefix_sum);
    }
    
    size_t bits_path_handle = std::max<size_t>(1, sdsl::bits::length(max_path_handle));
    size_t bits_step;
    if (path_handles.empty()) {
        bits_step = 1;
    }
    else {
        // note: the paths were sorted in descending order by length
        bits_step = std::max<size_t>(1, sdsl::bits::length(get_step_count(path_handles.front())));
    }
    
    // resize the steps vectors
    step_1.width(bits_path_handle);
    step_2.width(bits_step);
    step_1.resize(steps_begin[steps_begin.size() - 1]);
    step_2.resize(steps_begin[steps_begin.size() - 1]);
    
    // copy over the step begin to use as a bank of indexes into the next location
    std::vector<std::atomic<size_t>> next_step_idx(steps_begin.size());
#pragma omp parallel for schedule(dynamic,8192)
    for (size_t i = 0; i < steps_begin.size(); ++i) {
        next_step_idx[i].store(steps_begin[i]);
    }

    // locks for the underlying integers in the step vectors
    std::vector<std::mutex> step_1_locks(step_1.capacity() / 64);
    std::vector<std::mutex> step_2_locks(step_2.capacity() / 64);
    
    static const size_t step_batch_size = 16384;
    const size_t num_batches = (steps_prefix_sum.back() + step_batch_size - 1) / step_batch_size;
    
    // somewhat complicated number theoretic method to generate a permutation of the numbers 1..p-1 in
    // O(1) time each and O(1) space overhead. the goal is to shuffle the batches so that they're less likely
    // to be writing in the same part of the vector and therefore causing less contention on the mutexes
    // TODO: copied from VG
    // prime numbers spaced at approximately logarithmic intervals
    static constexpr size_t spaced_primes[62] = {2ull, 5ull, 13ull, 29ull, 53ull, 127ull, 227ull, 487ull, 967ull, 2039ull, 4093ull, 8191ull, 16381ull, 32749ull, 65521ull, 131071ull, 262139ull, 524287ull, 1048573ull, 2097143ull, 4194301ull, 8388593ull, 16777213ull, 33554393ull, 67108859ull, 134217689ull, 268435399ull, 536870909ull, 1073741789ull, 2147483647ull, 4294967291ull, 8589934583ull, 17179869143ull, 34359738337ull, 68719476731ull, 137438953447ull, 274877906899ull, 549755813881ull, 1099511627689ull, 2199023255531ull, 4398046511093ull, 8796093022151ull, 17592186044399ull, 35184372088777ull, 70368744177643ull, 140737488355213ull, 281474976710597ull, 562949953421231ull, 1125899906842597ull, 2251799813685119ull, 4503599627370449ull, 9007199254740881ull, 18014398509481951ull, 36028797018963913ull, 72057594037927931ull, 144115188075855859ull, 288230376151711717ull, 576460752303423433ull, 1152921504606846883ull, 2305843009213693951ull, 4611686018427387847ull, 9223372036854775783ull};
    // precomputed primitive roots of unity paired with these primes (chosen randomly from the 20 smallest roots)
    static constexpr size_t primitive_roots_of_unity[62] = {1ull, 3ull, 2ull, 21ull, 27ull, 56ull, 17ull, 45ull, 40ull, 28ull, 69ull, 70ull, 40ull, 31ull, 119ull, 75ull, 42ull, 61ull, 60ull, 46ull, 21ull, 13ull, 39ull, 13ull, 29ull, 15ull, 29ull, 32ull, 37ull, 73ull, 56ull, 45ull, 13ull, 90ull, 51ull, 12ull, 32ull, 11ull, 39ull, 24ull, 8ull, 39ull, 7ull, 51ull, 38ull, 67ull, 2ull, 34ull, 62ull, 19ull, 13ull, 30ull, 12ull, 45ull, 31ull, 57ull, 6ull, 57ull, 3ull, 37ull, 68ull, 54ull};
    size_t prime_idx = 0;
    while (spaced_primes[prime_idx] < num_batches + 1) {
        ++prime_idx;
    }
    
    std::vector<std::thread> workers;
    std::mutex next_batch_lock;
    std::atomic<size_t> batches_taken(0);
    size_t permuted_batch = 1;
    
    for (int t = 0; t < thread_count; ++t) {
        workers.emplace_back([&, t]() {
            while (batches_taken.load() < num_batches) {
                next_batch_lock.lock();
                if (batches_taken >= num_batches) {
                    // all of the batches got assigned while we were waiting for the lock
                    next_batch_lock.unlock();
                    return;
                }
                size_t batch_here = permuted_batch - 1;
                // advance k in (r^k mod p) to iterate through 1...p-1
                permuted_batch = (permuted_batch * primitive_roots_of_unity[prime_idx]) % spaced_primes[prime_idx];
                if (batch_here < num_batches) {
                    // this index corresponds to a real batch (we did not overshoot
                    ++batches_taken;
                }
                next_batch_lock.unlock();
                if (batch_here >= num_batches) {
                    continue;
                }
                
                size_t begin_idx = batch_here * step_batch_size;
                
                auto it = std::upper_bound(steps_prefix_sum.begin(), steps_prefix_sum.end(), begin_idx);
                --it;
                size_t path_idx = (it - steps_prefix_sum.begin());
                const auto* path_record = &reference_paths[path_handles[path_idx]];
                size_t step_idx = begin_idx - *it;
                
                // add the steps from this batch into the handle to step map
                for (size_t steps_added = 0; steps_added < step_batch_size; ++steps_added) {
                    
                    // record the step at this node's next in the steps vectors location
                    nid_t node_id = get_id(handlegraph::as_handle(path_record->steps.get(step_idx)));
                    
                    auto idx = next_step_idx[node_id]++;
                    
                    uint64_t step_1_entry = handlegraph::as_integer(path_handles[path_idx]);
                    size_t mut_idx_1 = step_1.width() * idx / 64;
                    size_t mut_idx_2 = (step_1.width() * (idx + 1) - 1) / 64;
                    step_1_locks[mut_idx_1].lock();
                    if (mut_idx_2 != mut_idx_1) {
                        step_1_locks[mut_idx_2].lock();
                    }
                    step_1[idx] = step_1_entry;
                    step_1_locks[mut_idx_1].unlock();
                    if (mut_idx_2 != mut_idx_1) {
                        step_1_locks[mut_idx_2].unlock();
                    }
                    
                    mut_idx_1 = step_2.width() * idx / 64;
                    mut_idx_2 = (step_2.width() * (idx + 1) - 1) / 64;
                    step_2_locks[mut_idx_1].lock();
                    if (mut_idx_2 != mut_idx_1) {
                        step_2_locks[mut_idx_2].lock();
                    }
                    step_2[idx] = step_idx;
                    step_2_locks[mut_idx_1].unlock();
                    if (mut_idx_2 != mut_idx_1) {
                        step_2_locks[mut_idx_2].unlock();
                    }
                    
                    // advance to the next step
                    ++step_idx;
                    if (step_idx == path_record->steps.size()) {
                        // roll over to the next path
                        ++path_idx;
                        step_idx = 0;
                        if (path_idx == path_handles.size()) {
                            break;
                        }
                        path_record = &reference_paths[path_handles[path_idx]];
                    }
                }
            }
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
}

bool ReferencePathOverlay::has_node(nid_t node_id) const {
    return graph->has_node(node_id);
}

handle_t ReferencePathOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
    return graph->get_handle(node_id, is_reverse);
}

nid_t ReferencePathOverlay::get_id(const handle_t& handle) const {
    return graph->get_id(handle);
}

bool ReferencePathOverlay::get_is_reverse(const handle_t& handle) const {
    return graph->get_is_reverse(handle);
}

handle_t ReferencePathOverlay::flip(const handle_t& handle) const {
    return graph->flip(handle);
}

size_t ReferencePathOverlay::get_length(const handle_t& handle) const {
    return graph->get_length(handle);
}

std::string ReferencePathOverlay::get_sequence(const handle_t& handle) const {
    return graph->get_sequence(handle);
}

size_t ReferencePathOverlay::get_node_count() const {
    return graph->get_node_count();
}

nid_t ReferencePathOverlay::min_node_id() const {
    return graph->min_node_id();
}

nid_t ReferencePathOverlay::max_node_id() const {
    return graph->max_node_id();
}

size_t ReferencePathOverlay::get_degree(const handle_t& handle, bool go_left) const {
    return graph->get_degree(handle, go_left);
}

bool ReferencePathOverlay::has_edge(const handle_t& left, const handle_t& right) const {
    return graph->has_edge(left, right);
}

size_t ReferencePathOverlay::get_edge_count() const {
    return graph->get_edge_count();
}

size_t ReferencePathOverlay::get_total_length() const {
    return graph->get_total_length();
}

char ReferencePathOverlay::get_base(const handle_t& handle, size_t index) const {
    return graph->get_base(handle, index);
}

std::string ReferencePathOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
    return graph->get_subsequence(handle, index, size);
}

size_t ReferencePathOverlay::get_path_count() const {
    return reference_paths.size();
}

bool ReferencePathOverlay::has_path(const std::string& path_name) const {
    return graph->has_path(path_name);
}

path_handle_t ReferencePathOverlay::get_path_handle(const std::string& path_name) const {
    return graph->get_path_handle(path_name);
}

std::string ReferencePathOverlay::get_path_name(const path_handle_t& path_handle) const {
    return graph->get_path_name(path_handle);
}

bool ReferencePathOverlay::get_is_circular(const path_handle_t& path_handle) const {
    return graph->get_is_circular(path_handle);
}

size_t ReferencePathOverlay::get_step_count(const path_handle_t& path_handle) const {
    return reference_paths.at(path_handle).steps.size();
}

handle_t ReferencePathOverlay::get_handle_of_step(const step_handle_t& step_handle) const {
    const auto& path_record = reference_paths.at(get_path_handle_of_step(step_handle));
    return handlegraph::as_handle(path_record.steps.get(handlegraph::as_integers(step_handle)[1]));
}

path_handle_t ReferencePathOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
    return handlegraph::as_path_handle(handlegraph::as_integers(step_handle)[0]);
}

step_handle_t ReferencePathOverlay::path_begin(const path_handle_t& path_handle) const {
    step_handle_t step;
    handlegraph::as_integers(step)[0] = handlegraph::as_integer(path_handle);
    handlegraph::as_integers(step)[1] = 0;
    return step;
}

step_handle_t ReferencePathOverlay::path_end(const path_handle_t& path_handle) const {
    step_handle_t step;
    handlegraph::as_integers(step)[0] = handlegraph::as_integer(path_handle);
    handlegraph::as_integers(step)[1] = get_step_count(path_handle);
    return step;
}

step_handle_t ReferencePathOverlay::path_back(const path_handle_t& path_handle) const {
    step_handle_t step;
    handlegraph::as_integers(step)[0] = handlegraph::as_integer(path_handle);
    handlegraph::as_integers(step)[1] = get_step_count(path_handle) - 1;
    return step;
}

step_handle_t ReferencePathOverlay::path_front_end(const path_handle_t& path_handle) const {
    step_handle_t step;
    handlegraph::as_integers(step)[0] = handlegraph::as_integer(path_handle);
    handlegraph::as_integers(step)[1] = -1;
    return step;
}

bool ReferencePathOverlay::has_next_step(const step_handle_t& step_handle) const {
    auto path = get_path_handle_of_step(step_handle);
    return int64_t(handlegraph::as_integers(step_handle)[1] + 1) < int64_t(get_step_count(path)) || get_is_circular(path);
}

bool ReferencePathOverlay::has_previous_step(const step_handle_t& step_handle) const {
    return int64_t(handlegraph::as_integers(step_handle)[1]) > 0 || get_is_circular(get_path_handle_of_step(step_handle));
}

step_handle_t ReferencePathOverlay::get_next_step(const step_handle_t& step_handle) const {
    step_handle_t next;
    handlegraph::as_integers(next)[0] = handlegraph::as_integers(step_handle)[0];
    handlegraph::as_integers(next)[1] = handlegraph::as_integers(step_handle)[1] + 1;
    if (get_is_circular(get_path_handle_of_step(step_handle))) {
        if (handlegraph::as_integers(next)[1] == get_step_count(get_path_handle_of_step(next))) {
            handlegraph::as_integers(next)[1] = 0;
        }
    }
    return next;
}

step_handle_t ReferencePathOverlay::get_previous_step(const step_handle_t& step_handle) const {
    step_handle_t next;
    handlegraph::as_integers(next)[0] = handlegraph::as_integers(step_handle)[0];
    handlegraph::as_integers(next)[1] = handlegraph::as_integers(step_handle)[1] - 1;
    if (get_is_circular(get_path_handle_of_step(step_handle))) {
        if (handlegraph::as_integers(next)[1] == -1) {
            handlegraph::as_integers(next)[1] = get_step_count(get_path_handle_of_step(next)) - 1;
        }
    }
    return next;
}

size_t ReferencePathOverlay::get_path_length(const path_handle_t& path_handle) const {
    return reference_paths.at(path_handle).offsets.size();
}

size_t ReferencePathOverlay::get_position_of_step(const step_handle_t& step) const {
    size_t idx = handlegraph::as_integers(step)[1];
    if (idx == 0) {
        return 0;
    }
    else {
        return reference_paths.at(get_path_handle_of_step(step)).offsets_select(idx) + 1;
    }
}

step_handle_t ReferencePathOverlay::get_step_at_position(const path_handle_t& path,
                                                         const size_t& position) const {
    step_handle_t step;
    handlegraph::as_integers(step)[0] = handlegraph::as_integer(path);
    auto& ref_path = reference_paths.at(path);
    handlegraph::as_integers(step)[1] = position < ref_path.offsets.size() ? ref_path.offsets_rank(position) : ref_path.steps.size();
    return step;
}

bool ReferencePathOverlay::follow_edges_impl(const handle_t& handle, bool go_left,
                                             const std::function<bool(const handle_t&)>& iteratee) const {
    return graph->follow_edges(handle, go_left, iteratee);
}

bool ReferencePathOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    return graph->for_each_handle(iteratee, parallel);
}

bool ReferencePathOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
    for (const auto& path_rec : reference_paths) {
        if (!iteratee(path_rec.first)) {
            return false;
        }
    }
    return true;
}

bool ReferencePathOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                        const std::function<bool(const step_handle_t&)>& iteratee) const {
    
    nid_t node_id = get_id(handle);
    for (size_t i = steps_begin[node_id], n = steps_begin[node_id + 1]; i < n; ++i) {
        step_handle_t step;
        handlegraph::as_integers(step)[0] = step_1[i];
        handlegraph::as_integers(step)[1] = step_2[i];
        if (!iteratee(step)) {
            return false;
        }
    }
    return true;
}

}
