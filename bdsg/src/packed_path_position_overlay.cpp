#include "bdsg/overlays/packed_path_position_overlay.hpp"
#include "bdsg/internal/utility.hpp"

#include <omp.h> // BINDER_IGNORE because Binder can't find this

//#define debug

namespace bdsg {

PackedPositionOverlay::PackedPositionOverlay(const PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names, size_t steps_per_index) : graph(graph), steps_per_index(steps_per_index) {
    index_path_positions(extra_path_names);
}

bool PackedPositionOverlay::has_node(nid_t node_id) const {
    return graph->has_node(node_id);
}

handle_t PackedPositionOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
    return graph->get_handle(node_id, is_reverse);
}

nid_t PackedPositionOverlay::get_id(const handle_t& handle) const {
    return graph->get_id(handle);
}

bool PackedPositionOverlay::get_is_reverse(const handle_t& handle) const {
    return graph->get_is_reverse(handle);
}

handle_t PackedPositionOverlay::flip(const handle_t& handle) const {
    return graph->flip(handle);
}

size_t PackedPositionOverlay::get_length(const handle_t& handle) const {
    return graph->get_length(handle);
}

string PackedPositionOverlay::get_sequence(const handle_t& handle) const {
    return graph->get_sequence(handle);
}

bool PackedPositionOverlay::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    return graph->follow_edges(handle, go_left, iteratee);
}

bool PackedPositionOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    return graph->for_each_handle(iteratee, parallel);
}

size_t PackedPositionOverlay::get_degree(const handle_t& handle, bool go_left) const {
    return graph->get_degree(handle, go_left);
}

bool PackedPositionOverlay::has_edge(const handle_t& left, const handle_t& right) const {
    return graph->has_edge(left, right);
}

char PackedPositionOverlay::get_base(const handle_t& handle, size_t index) const {
    return graph->get_base(handle, index);
}

std::string PackedPositionOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
    return graph->get_subsequence(handle, index, size);
}

size_t PackedPositionOverlay::get_node_count(void) const {
    return graph->get_node_count();
}

nid_t PackedPositionOverlay::min_node_id(void) const {
    return graph->min_node_id();
}

nid_t PackedPositionOverlay::max_node_id(void) const {
    return graph->max_node_id();
}

size_t PackedPositionOverlay::get_path_count() const {
    return graph->get_path_count();
}

bool PackedPositionOverlay::has_path(const std::string& path_name) const {
    return graph->has_path(path_name);
}

path_handle_t PackedPositionOverlay::get_path_handle(const std::string& path_name) const {
    return graph->get_path_handle(path_name);
}

string PackedPositionOverlay::get_path_name(const path_handle_t& path_handle) const {
    return graph->get_path_name(path_handle);
}

bool PackedPositionOverlay::get_is_circular(const path_handle_t& path_handle) const {
    return graph->get_is_circular(path_handle);
}

size_t PackedPositionOverlay::get_step_count(const path_handle_t& path_handle) const {
    return graph->get_step_count(path_handle);
}

handle_t PackedPositionOverlay::get_handle_of_step(const step_handle_t& step_handle) const {
    return graph->get_handle_of_step(step_handle);
}

step_handle_t PackedPositionOverlay::path_begin(const path_handle_t& path_handle) const {
    return graph->path_begin(path_handle);
}

step_handle_t PackedPositionOverlay::path_end(const path_handle_t& path_handle) const {
    return graph->path_end(path_handle);
}

step_handle_t PackedPositionOverlay::path_back(const path_handle_t& path_handle) const {
    return graph->path_back(path_handle);
}

step_handle_t PackedPositionOverlay::path_front_end(const path_handle_t& path_handle) const {
    return graph->path_front_end(path_handle);
}

bool PackedPositionOverlay::has_next_step(const step_handle_t& step_handle) const {
    return graph->has_next_step(step_handle);
}

bool PackedPositionOverlay::has_previous_step(const step_handle_t& step_handle) const {
    return graph->has_previous_step(step_handle);
}

step_handle_t PackedPositionOverlay::get_next_step(const step_handle_t& step_handle) const {
    return graph->get_next_step(step_handle);
}

step_handle_t PackedPositionOverlay::get_previous_step(const step_handle_t& step_handle) const {
    return graph->get_previous_step(step_handle);
}

path_handle_t PackedPositionOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
    return graph->get_path_handle_of_step(step_handle);
}

bool PackedPositionOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
    return graph->for_each_path_handle(iteratee);
}

bool PackedPositionOverlay::for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                                  const std::unordered_set<std::string>* samples,
                                                  const std::unordered_set<std::string>* loci,
                                                  const std::function<bool(const path_handle_t&)>& iteratee) const {
    return graph->for_each_path_matching(senses, samples, loci, iteratee);
}

bool PackedPositionOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                         const function<bool(const step_handle_t&)>& iteratee) const {
    return graph->for_each_step_on_handle(handle, iteratee);
}

bool PackedPositionOverlay::for_each_step_of_sense_impl(const handle_t& visited, const PathSense& sense,
                                                  const std::function<bool(const step_handle_t&)>& iteratee) const {
    return graph->for_each_step_of_sense(visited, sense, iteratee);
}


size_t PackedPositionOverlay::get_path_length(const path_handle_t& path_handle) const {
    const auto& range = path_range.at(as_integer(path_handle));
    if (range.start == range.end) {
        return 0;
    }
    step_handle_t step;
    as_integers(step)[0] = indexes[range.index_number].steps_0.get(range.end - 1);
    as_integers(step)[1] = indexes[range.index_number].steps_1.get(range.end - 1);
    return indexes[range.index_number].positions.get(range.end - 1) + get_length(get_handle_of_step(step));
}

size_t PackedPositionOverlay::get_position_of_step(const step_handle_t& step) const {
    auto path = get_path_handle_of_step(step); 
    if (step == path_end(path)) {
        return get_path_length(path);
    }
    else {
        auto& range = path_range.at(as_integer(path));
        const boomphf::mphf<step_handle_t, StepHash>& const_step_hash = indexes[range.index_number].step_hash.back();
        // We can't use the lookup function on a const mphf, because it isn't
        // marked const. But it is thread safe and really ought to be const. So
        // we cast away the const here.
        boomphf::mphf<step_handle_t, StepHash>& step_hash = const_cast<boomphf::mphf<step_handle_t, StepHash>&>(const_step_hash);
        return indexes[range.index_number].step_positions.get(step_hash.lookup(step));
    }
}

step_handle_t PackedPositionOverlay::get_step_at_position(const path_handle_t& path,
                                                          const size_t& position) const {
    
    const auto& range = path_range.at(as_integer(path));
    
    // check if position it outside the range (handles edge case of an empty path too)
    if (position >= get_path_length(path)) {
        return path_end(path);
    }
    
    // bisect search within the range to find the index with the steps
    size_t low = range.start;
    size_t hi = range.end;
    while (hi > low + 1) {
        size_t mid = (hi + low) / 2;
        if (position < indexes[range.index_number].positions.get(mid)) {
            hi = mid;
        }
        else {
            low = mid;
        }
    }
    
    // unpack the integers at the same index into a step
    step_handle_t step;
    as_integers(step)[0] = indexes[range.index_number].steps_0.get(low);
    as_integers(step)[1] = indexes[range.index_number].steps_1.get(low);
    return step;
}

handle_t PackedPositionOverlay::get_underlying_handle(const handle_t& handle) const {
    return handle;
}

void PackedPositionOverlay::index_path_positions(const std::unordered_set<std::string>& extra_path_names) {
    
    // I'm not sure how to pass handles to OMP tasks by value, when we'd return
    // out of the functions that created the tasks and are holding the tasks'
    // locals. So first we'll collect all the path handles.
    // TODO: deduplicate with BBHashHelper's copy of all the path handles?
    std::vector<path_handle_t> path_handles;
    for_each_path_handle([&](const path_handle_t& path_handle) {
        path_handles.push_back(path_handle);
    });
    if (!extra_path_names.empty()) {
        // Skip any named paths we already visited
        std::unordered_set<path_handle_t> seen(path_handles.begin(), path_handles.end());
        for (auto& name : extra_path_names) {
            path_handle_t path = get_path_handle(name);
            if (!seen.count(path)) {
                // But remember to visit any we haven't yet.
                path_handles.push_back(path);
            }
        }
    }
    
    // Then get the lengths of all the paths
    std::vector<size_t> path_lengths(path_handles.size(), 0);
    // And any per-path scan data that we might need later
    std::vector<void*> path_user_data(path_handles.size(), nullptr);
    #pragma omp parallel for
    for (size_t i = 0; i < path_handles.size(); i++) {
        // Step counting requires a scan in some graph implementations, so do it in parallel.
#ifdef debug
        #pragma omp critical (cerr)
        std::cerr << "T" << omp_get_thread_num() << ": Getting length of path " << get_path_name(path_handles[i]) << std::endl;
#endif
        path_lengths[i] = this->scan_path(path_handles[i], path_user_data[i]);
        
#ifdef debug
        #pragma omp critical (cerr)
        std::cerr << "T" << omp_get_thread_num() << ": For path " << i << " = " << get_path_name(path_handles[i]) << " length is " << path_lengths[i] << " and user data is " << path_user_data[i] << " at " << &path_user_data[i] << std::endl;
#endif
        
    }
    
    // Then find the collections of paths to index together.
    // These offsets into path_handles will be range bounds of paths to put in the indexes.
    std::vector<size_t> bounds;
    bounds.push_back(0);
    
#ifdef debug
    #pragma omp critical (cerr)
    std::cerr << "Starting a new index " << (bounds.size() - 1) << " with path " << get_path_name(path_handles[bounds.back()]) << std::endl;
#endif
    
    // And this will be the cumulative path length of all the paths in each collection.
    std::vector<size_t> path_set_steps;
    size_t total_length = 0;
    size_t accumulated_length = 0;
    for (size_t i = 0; i < path_handles.size(); i++) {
        if (accumulated_length >= steps_per_index) {
            // We need to start a new index with this path
            bounds.push_back(i);
            
#ifdef debug
            #pragma omp critical (cerr)
            std::cerr << "Starting a new index " << (bounds.size() - 1) << " with path " << get_path_name(path_handles[bounds.back()]) << std::endl;
#endif
            
            path_set_steps.push_back(accumulated_length);
            total_length += accumulated_length;
            accumulated_length = 0;
        }
        // Remember that this path's steps went into this index.
        accumulated_length += path_lengths[i];
    }
    bounds.push_back(path_handles.size());
    path_set_steps.push_back(accumulated_length);
    total_length += accumulated_length;
    
    // Now we know how many indexes we need
    this->set_index_count(path_set_steps.size());
            
#ifdef debug
        #pragma omp critical (cerr)
        std::cerr << "Using " << indexes.size() << " indexes for " << total_length << " total steps" << std::endl;
#endif
    
    #pragma omp parallel for
    for (size_t i = 0; i < path_set_steps.size(); i++) {
        // For each set of paths to index together
        std::vector<path_handle_t>::const_iterator begin_path = path_handles.cbegin() + bounds[i];
        std::vector<path_handle_t>::const_iterator end_path = path_handles.cbegin() + bounds[i + 1];
        // And the number of steps on its paths
        const size_t& cumul_path_size = path_set_steps[i];
        // And the user data for the first path. All of them must be stored contiguously.
        void** user_data_base = &path_user_data[bounds[i]];
        
        // Compute the index for this collection of paths.
        this->index_paths(i, begin_path, end_path, cumul_path_size, user_data_base);
    }
    
}

size_t PackedPositionOverlay::scan_path(const path_handle_t& path_handle, void*& user_data) {
    user_data = nullptr;
    return get_step_count(path_handle);
}

void PackedPositionOverlay::set_index_count(size_t count) {
    indexes.resize(count);
}

void PackedPositionOverlay::index_paths(size_t index_num, const std::vector<path_handle_t>::const_iterator& begin_path, const std::vector<path_handle_t>::const_iterator& end_path, size_t cumul_path_size, void** user_data_base) {
    // Grab the index we are building into
    auto& index = indexes[index_num];

    // resize the vectors to the number of step handles
    index.steps_0.resize(cumul_path_size);
    index.steps_1.resize(cumul_path_size);
    index.positions.resize(cumul_path_size);
    index.step_positions.resize(cumul_path_size);
    
#ifdef debug
    #pragma omp critical (cerr)
    std::cerr << "T" << omp_get_thread_num() << ": Sized index " << index_num << " for " << cumul_path_size << " steps" << std::endl;
#endif
    
    // Make a perfect minimal hash over the step handles on the selected paths
    // Use the number of threads a child OMP team would get.
    index.step_hash.emplace_back(cumul_path_size, BBHashHelper(graph, begin_path, end_path), get_thread_count(), 2.0, false, false);
    
    // Walk a cursor through steps among the path set
    size_t step_overall = 0;
    for (size_t j = 0; j < end_path - begin_path; j++) {
        // For each path we are indexing
        auto& path_handle = *(begin_path + j);
        // And its user data
        void* user_data = *(user_data_base + j);

        // Make sure there's no user data
        assert(user_data == nullptr);

        // Initialize a PathRange on the stack
        PathRange range;
        
        // Populate the index and start info
        range.index_number = index_num;
        range.start = step_overall;
        // And walk a base position cursor along the path
        size_t position = 0;
        for_each_step_in_path(path_handle, [&](const step_handle_t& step) {
            
            // fill in the position to step index
            index.steps_0.set(step_overall, as_integers(step)[0]);
            index.steps_1.set(step_overall, as_integers(step)[1]);
            index.positions.set(step_overall, position);
            
            // fill in the step to position index
            index.step_positions.set(index.step_hash.back().lookup(step), position);
            
            position += get_length(get_handle_of_step(step));
            ++step_overall;
        });
        // Populate the end info
        range.end = step_overall;
        
#ifdef debug
        #pragma omp critical (cerr)
        std::cerr << "T" << omp_get_thread_num() << ": Path " << get_path_name(path_handle) << " takes up range " << range.start << " to " << range.end << " in index " << range.index_number << std::endl;
#endif
        
        // Commit to the map from path to path range. We must protect all
        // access in a critical section, not just the hash table lookup.
        //
        // If we worked on a pointer to an item in the hash table, another
        // thread could add a new item and make our item be deallocated
        // while we were working on it.
        #pragma omp critical (path_range)
        path_range.emplace(as_integer(path_handle), std::move(range));
    }
}

uint64_t PackedPositionOverlay::StepHash::operator()(const step_handle_t& step, uint64_t seed) const {
    const int64_t* int_step = as_integers(step);
    uint64_t hsh1 = boomphf::SingleHashFunctor<int64_t>()(int_step[0], seed);
    uint64_t hsh2 = boomphf::SingleHashFunctor<int64_t>()(int_step[1], seed);
    // Boost combine for hash values
    return hsh1 ^ (hsh2 + 0x9e3779b9 + (hsh1<<6) + (hsh1>>2));
}

BBHashHelper::BBHashHelper(const PathHandleGraph* graph) : graph(graph) {
    path_handles.reserve(graph->get_path_count());
    graph->for_each_path_handle([&](const path_handle_t& path_handle) {
        if (!graph->is_empty(path_handle)) {
            // this path contains steps, we want to iterate over it
            path_handles.push_back(path_handle);
        }
    });
}

BBHashHelper::iterator BBHashHelper::begin() const {
    return iterator(this, 0);
}

BBHashHelper::iterator BBHashHelper::end() const {
    return iterator(this, path_handles.size());
}

BBHashHelper::iterator::iterator(const BBHashHelper* _iteratee, size_t _path_handle_idx) : iteratee(_iteratee), path_handle_idx(_path_handle_idx) {
    if (path_handle_idx < this->iteratee->path_handles.size()) {
        step = this->iteratee->graph->path_begin(this->iteratee->path_handles[path_handle_idx]);
    }
}

BBHashHelper::iterator& BBHashHelper::iterator::operator++() {
    step = iteratee->graph->get_next_step(step);
    if (step == iteratee->graph->path_begin(iteratee->path_handles[path_handle_idx]) ||
        step == iteratee->graph->path_end(iteratee->path_handles[path_handle_idx])) {
        // we either went off the end or looped around a circular path to the beginning again
        ++path_handle_idx;
        if (path_handle_idx < iteratee->path_handles.size()) {
            step = iteratee->graph->path_begin(iteratee->path_handles[path_handle_idx]);
        }
    }
    return *this;
}

step_handle_t BBHashHelper::iterator::operator*() const {
    return step;
}

bool BBHashHelper::iterator::operator==(const BBHashHelper::iterator& other) const {
    // on the end iterator, we don't care what the step is, only that we're past-the-last
    // path handle
    if (iteratee == other.iteratee && path_handle_idx == other.path_handle_idx) {
        if (path_handle_idx == iteratee->path_handles.size() || step == other.step) {
            return true;
        }
    }
    return false;
}

bool BBHashHelper::iterator::operator!=(const BBHashHelper::iterator& other) const {
    return !(*this == other);
}
}
