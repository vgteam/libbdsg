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
}

bool PackedReferencePathOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                              const function<bool(const step_handle_t&)>& iteratee) const {

    for (size_t index_num = 0; index_num < this->indexes.size(); index_num++) {
        auto& index = this->indexes[i];
        auto& visit_index = this->visit_indexes[i];
        
        // De-const the MPHF because its lookup should really be const and isn't.
        boomphf::mphf<handle_t, HandleHash>& handle_hash = const_cast<boomphf::mphf<handle_t, HandleHash>&>(const_step_hash);
        
        // Check if the handle is in the MPHF
        
    }

}

size_t PackedReferencePathOverlay::scan_path(const path_handle_t& path_handle, void*& user_data) {
    // Instead of just getting the path step count, scan the whole path and
    // make a multimap from handle to rnaks it occurs at.
    std::unordered_multimap<handle_t, size_t>* visit_ranks = new std::unordered_multimap<handle_t, size_t>();
    size_t rank = 0;
    for (handle_t h : graph->scan_path(path_handle)) {
        // Keep all the ranks that the handles happen at
        visit_ranks->emplace(h, rank);
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
    // Compute all the indexes for path positions
    PackedPositionOverlay::index_paths(index_num, begin_path, end_path, cumul_path_size, user_data_base);
    
    // Compose all the user datas into one
    std::unordered_multimap<handle_t, size_t> all_visit_ranks;
    // When composing, we need to offset each path's visits by the previous
    // path's past-end rank, to unify the rank spaces.
    size_t rank_offset = 0;
    // We also need the number of unique handle keys
    size_t unique_keys = 0;
    void** user_data_it = user_data_base;
    for (auto it = begin_path; it != end_path; ++it) {
        std::unordered_multimap<handle_t, size_t>* user_data = (std::unordered_multimap<handle_t, size_t>*) *user_data_it;
        ++user_data_it;
        // Copy all the items by hand
        // TODO: When we get C++17, do something with nodes
        for (const std::pair<handle_t, size_t>& item : *user_data) {
            auto found = all_visit_ranks.find(item.first);
            if (found == all_visit_ranks.end()) {
                // This is a new unique key
                unique_keys++;
            }
            // Add the value, shifted past used ranks.
            all_visit_ranks.emplace_hint(found, item.first, item.second + rank_offset);
        }
        // Record the ranks used
        rank_offset += user_data->size();
        // Consume the user data
        delete user_data;
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
    visit_index.handle_hash.emplace_back(cumul_path_size, UniqueKeyRange<std::unordered_multimap<handle_t, size_t>>(all_visit_ranks), get_thread_count(), 2.0, false, false);
    
    // Compress down all_visit_ranks using the MPHF
    // TODO: Can we do this without making a whole copy in all_visit_ranks? And just make another pass?
    size_t start_visit_number = 0;
    size_t visit_number = 0;
    size_t prev_hash = unique_keys;
    for (auto& kv : all_visit_ranks) {
        size_t cur_hash = visit_index.handle_hash.back().lookup(kv.first);
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
}

}
