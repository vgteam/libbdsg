#include "bdsg/overlays/packed_path_position_overlay.hpp"
#include "bdsg/overlays/packed_reference_path_overlay.hpp"

#include "bdsg/internal/utility.hpp"

#include <omp.h> // BINDER_IGNORE because Binder can't find this

//#define debug

namespace bdsg {

PackedReferencePathOverlay::PackedReferencePathOverlay(const PathHandleGraph* graph, size_t steps_per_index) : PackedPositionOverlay() {
    // We can't just chain to the base class constructor with these arguments
    // because we need virtual methods in this class to be available before the
    // index build starts.
    this->graph = graph;
    this->steps_per_index = steps_per_index;

    // Now do the index build
    index_path_positions();

    // Now make the step_handle -> path_handle index:    
    // rank our steps with a bbhash
    size_t step_count = 0;
    vector<path_handle_t> ref_path_handles;
    for_each_path_handle([&](const path_handle_t& path_handle) {
            ref_path_handles.push_back(path_handle);
            step_count += graph->get_step_count(path_handle);
        });
    step_to_rank.reset(new boomphf::mphf<step_handle_t, StepHash>(step_count,
                                                                  BBHashHelper(graph, ref_path_handles.begin(), ref_path_handles.end()),
                                                                  get_thread_count(), 2.0, false, false));
    // map the step rank back to the path with a packed vector
    step_rank_to_path.resize(step_count);
    for (const path_handle_t& ref_path : ref_path_handles) {
        for_each_step_in_path(ref_path, [&](const step_handle_t& step_handle) {
                step_rank_to_path.set(step_to_rank->lookup(step_handle), handlegraph::as_integer(ref_path));
            });
    }
}

path_handle_t PackedReferencePathOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
    return handlegraph::as_path_handle(step_rank_to_path.get(step_to_rank->lookup(step_handle)));
}

bool PackedReferencePathOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                              const function<bool(const step_handle_t&)>& iteratee) const {

    // Actually we work by node ID for this query, not oriented handle.
    // We want to see paths that visit the node in the other orientation, since
    // we're still on them in an orientation.
    nid_t node_id = this->get_id(handle);
    // Now we are going to poll all the indexes because we don't have anything
    // like path_range to find the right index.
    // TODO: Change to one big MPHF over the graph's nodes or something??? This will be slow!
    for (size_t index_num = 0; index_num < this->indexes.size(); index_num++) {
    
#ifdef debug
        std::cerr << "Check for node " << node_id << " in index " << index_num << std::endl;
#endif
    
        // Look at the index data for this collection of paths
        auto& index = this->indexes[index_num];
        auto& visit_index = this->visit_indexes[index_num];
        
        // De-const the MPHF because its lookup should really be const and isn't.
        boomphf::mphf<nid_t, boomphf::SingleHashFunctor<nid_t>>& node_hash = const_cast<boomphf::mphf<nid_t, boomphf::SingleHashFunctor<nid_t>>&>(visit_index.node_hash.back());
        
        // See where the node would be if it was anywhere
        size_t hash = node_hash.lookup(node_id);
        
#ifdef debug
        std::cerr << "Hashes to slot " << hash << "/" << visit_index.visit_ranks_start.size() << std::endl;
#endif
        
        // The MPHF can't actually tell if a thing is actually in it. So we
        // need to deal with garbage or indexes of other handles.
        if (hash >= visit_index.visit_ranks_start.size()) {
            // Hashed to out of range somehow
            continue;
        }
        
        // Get the range of visit ranks we are supposed to be looking at.
        size_t range_start = visit_index.visit_ranks_start.get(hash);
        size_t range_end = range_start + visit_index.visit_ranks_length.get(hash);
        
#ifdef debug
        std::cerr << "Corresponds to rank range " << range_start << "-" << range_end << std::endl;
#endif
        
        for (size_t i = range_start; i < range_end; i++) {
            // Get the rank of this visit to what ought to be this node, out of all steps in the index
            size_t rank = visit_index.visit_ranks.get(i);
            
            // Reconstruct the step at that rank
            step_handle_t step;
            as_integers(step)[0] = index.steps_0.get(rank);
            as_integers(step)[1] = index.steps_1.get(rank);
            
#ifdef debug
            std::cerr << "Found step of path " << this->get_path_name(this->get_path_handle_of_step(step)) << " on node " << this->get_id(this->get_handle_of_step(step)) << " orientation " << this->get_is_reverse(this->get_handle_of_step(step)) << std::endl;
#endif
            
            if (i == range_start) {
                // For the first step on each handle, make sure we actually
                // found visits to the node we hashed.
                nid_t found_id = this->get_id(this->get_handle_of_step(step));
                if (found_id != node_id) {
                    // We had a hash collision, which means the node we
                    // actually wanted isn't visited by anything in this index.
                    // Skip to the next.
#ifdef debug
                    std::cerr << "Wrong node; node not in index." << std::endl;
#endif
                    break;
                }
            }
            
            // Show this step on the node to the iteratee
            if (!iteratee(step)) {
                // The iteratee wants us to stop.
#ifdef debug
                std::cerr << "Iteratee stopped." << std::endl;
#endif
                return false;
            }
        }
    }
    // If we get here we made it through all the indexes withput being told to
    // stop.
    return true;
}

size_t PackedReferencePathOverlay::scan_path(const path_handle_t& path_handle, void*& user_data) {
    // Instead of just getting the path step count, scan the whole path and
    // make a multimap from visited node ID to ranks it occurs at.
    std::unordered_multimap<nid_t, size_t>* visit_ranks = new std::unordered_multimap<nid_t, size_t>();
    size_t rank = 0;
    for (handle_t h : graph->scan_path(path_handle)) {
        // Keep all the ranks that the handles happen at
        visit_ranks->emplace(graph->get_id(h), rank);
        // And count all the steps
        rank++;
    }
    
    // Keep the visits as the user data for this path
    user_data = (void*) visit_ranks;
    // Total steps is used to group paths into co-indexed sets and so can't be user data.
    return rank;
}

void PackedReferencePathOverlay::set_index_count(size_t count) {
    // Resize the base class indexes
    PackedPositionOverlay::set_index_count(count);
    // Resize our additional indexes
    visit_indexes.resize(count);
}

void PackedReferencePathOverlay::index_paths(size_t index_num, const std::vector<path_handle_t>::const_iterator& begin_path, const std::vector<path_handle_t>::const_iterator& end_path, size_t cumul_path_size, void** user_data_base) {
    
    // Compose all the user datas into one
    std::unordered_multimap<nid_t, size_t> all_visit_ranks;
    // When composing, we need to offset each path's visits by the previous
    // path's past-end rank, to unify the rank spaces.
    size_t rank_offset = 0;
    // We also need the number of unique handle keys
    size_t unique_keys = 0;
    std::unordered_multimap<nid_t, size_t>** user_data_it = (std::unordered_multimap<nid_t, size_t>**) user_data_base;
    for (auto it = begin_path; it != end_path; ++it) {
    
#ifdef debug
        #pragma omp critical (cerr)
        std::cerr << "T" << omp_get_thread_num() << ": For path " << get_path_name(*it) << " we see user data " << *user_data_it << " at " << user_data_it << std::endl;
#endif
        
        // Copy all the items by hand
        // TODO: When we get C++17, do something with nodes
        assert(*user_data_it != nullptr); 
        for (const std::pair<nid_t, size_t>& item : **user_data_it) {
            auto found = all_visit_ranks.find(item.first);
            if (found == all_visit_ranks.end()) {
                // This is a new unique key
                unique_keys++;
            }
            // Add the value, shifted past used ranks.
            all_visit_ranks.emplace_hint(found, item.first, item.second + rank_offset);
        }
        // Record the ranks used
        rank_offset += (*user_data_it)->size();
        // Consume the user data
        delete *user_data_it;
        // Null it out so the base class can have its null
        *user_data_it = nullptr;
        ++user_data_it;
    }
    
    // Grab the additional index we are building into
    auto& visit_index = visit_indexes[index_num];

    // Resize the storage for steps to the number of steps
    visit_index.visit_ranks.resize(cumul_path_size);
    // Resize the storage for ranges of steps to the number of unique handles
    visit_index.visit_ranks_start.resize(unique_keys);
    visit_index.visit_ranks_length.resize(unique_keys);
    
    // Make a perfect minimal hash over the handles on the selected paths
    // Use the number of threads a child OMP team would get.
    visit_index.node_hash.emplace_back(cumul_path_size, UniqueKeyRange<std::unordered_multimap<nid_t, size_t>>(all_visit_ranks), get_thread_count(), 2.0, false, false);
    
    // Compress down all_visit_ranks using the MPHF
    // TODO: Can we do this without making a whole copy in all_visit_ranks? And just make another pass?
    size_t start_visit_number = 0;
    size_t visit_number = 0;
    size_t prev_hash = unique_keys;
    for (auto& kv : all_visit_ranks) {
        size_t cur_hash = visit_index.node_hash.back().lookup(kv.first);
        if (cur_hash != prev_hash) {
            if (prev_hash != unique_keys) {
                // We have a legit previous visit rank list to finish
                visit_index.visit_ranks_length.set(prev_hash, visit_number - start_visit_number);
            }
            // We have a new visit rank list to start
            visit_index.visit_ranks_start.set(cur_hash, visit_number);
            start_visit_number = visit_number;
            
            prev_hash = cur_hash;
        }
        
        // Save what rank the visit is to
        visit_index.visit_ranks.set(visit_number, kv.second);
        
        // Move the cursor
        visit_number++;
    }
    if (visit_number != start_visit_number) {
        // We have an active last range.
        // Finish the last range.
        visit_index.visit_ranks_length.set(prev_hash, visit_number - start_visit_number);
    }
    
    // Compute all the indexes for path positions
    PackedPositionOverlay::index_paths(index_num, begin_path, end_path, cumul_path_size, user_data_base);
}

}
