#include "bdsg/overlays/path_position_overlays.hpp"

namespace bdsg {

    PositionOverlay::PositionOverlay(PathHandleGraph* graph) : graph(graph) {
        index_path_positions();
    }

    PositionOverlay::PositionOverlay() {
        
    }
    
    PositionOverlay::~PositionOverlay() {
        
    }
    
    bool PositionOverlay::for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                                            const std::unordered_set<std::string>* samples,
                                                            const std::unordered_set<std::string>* loci,
                                                            const std::function<bool(const path_handle_t&)>& iteratee) const {
        
        return graph->for_each_path_matching(senses, samples, loci, [&](const path_handle_t& path) -> bool {
            if (graph->get_sense(path) != PathSense::HAPLOTYPE) {
                // Allow non-haplotype paths, which we indexed.
                return iteratee(path);
            } else {
                // Skip haplotype paths.
                return true;
            }
        });
    }

    bool PositionOverlay::for_each_step_of_sense_impl(const handle_t& visited,
                                                            const PathSense& sense,
                                                            const std::function<bool(const step_handle_t&)>& iteratee) const {

        return graph->for_each_step_of_sense(visited, sense, [&](const step_handle_t& step) -> bool {
            path_handle_t path = graph->get_path_handle_of_step(step);
            if (graph->get_sense(path) != PathSense::HAPLOTYPE) {
                // Allow non-haplotype paths, which we indexed.
                return iteratee(step);
            } else {
                // Skip haplotype paths.
                return true;
            }
        });

    }
                                                             
    bool PositionOverlay::has_path(const std::string& path_name) const {
        if (!graph->has_path(path_name)) {
            return false;
        }
        path_handle_t path = graph->get_path_handle(path_name);
        // Haplotype paths officially don't exist, since they aren't indexed.
        return graph->get_sense(path) != PathSense::HAPLOTYPE;
    }
    
    size_t PositionOverlay::get_path_length(const path_handle_t& path_handle) const {
        const auto& path_step_by_position = step_by_position.at(path_handle);
        auto last = path_step_by_position.rbegin();
        if (last == path_step_by_position.rend()) {
            return 0;
        }
        else {
            return (last->first
                    + get()->get_length(get()->get_handle_of_step(last->second))
                    - min_path_offset.at(path_handle));
        }
        
    }
    
    size_t PositionOverlay::get_position_of_step(const step_handle_t& step) const {
        return offset_by_step.at(step) - min_path_offset.at(get_path_handle_of_step(step));
    }
    
    step_handle_t PositionOverlay::get_step_at_position(const path_handle_t& path,
                                                        const size_t& position) const {
        
        int64_t lookup_position = position + min_path_offset.at(path);
        
        const auto& path_step_by_position = step_by_position.at(path);
        if (path_step_by_position.empty()) {
            return get()->path_end(path);
        }
        
        auto iter = --path_step_by_position.upper_bound(lookup_position);
        if (lookup_position - iter->first >= get()->get_length(get()->get_handle_of_step(iter->second))) {
            // this only occurs if the position was past the last base in the path
            return get()->path_end(path);
        }
        else {
            return iter->second;
        }
    }
    
    handle_t PositionOverlay::get_underlying_handle(const handle_t& handle) const {
        return handle;
    }
    
    void PositionOverlay::index_path_positions() {
        
        get()->for_each_path_handle([&](const path_handle_t& path) {
            int64_t offset = 0;
            min_path_offset[path] = offset;
            auto& path_step_by_position = step_by_position[path];
            get()->for_each_step_in_path(path, [&](const step_handle_t& step) {
                offset_by_step[step] = offset;
                path_step_by_position[offset] = step;
                offset += get()->get_length(get()->get_handle_of_step(step));
            });
        });
    }
    
    MutablePositionOverlay::MutablePositionOverlay(MutablePathDeletableHandleGraph* graph) : PositionOverlay(graph), mutable_graph(graph) {
        
    }
    
    MutablePositionOverlay::MutablePositionOverlay() {
        
    }
    
    MutablePositionOverlay::~MutablePositionOverlay() {
        
    }
    
    void MutablePositionOverlay::clear(void) {
        get()->clear();
        offset_by_step.clear();
        step_by_position.clear();
        min_path_offset.clear();
    }
    
    handle_t MutablePositionOverlay::apply_orientation(const handle_t& handle) {
        // this will reverse the orientation of the handle on paths, so we need to update
        // the corresponding steps
        
        // remove the previous annotations by step
        for_each_step_on_handle(handle, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        // do the flip
        handle_t new_handle = get()->apply_orientation(handle);
        
        for_each_step_on_handle(new_handle, [&](const step_handle_t& new_step) {
            reindex_contiguous_segment(new_step);
        });
        
        return new_handle;
    }
    
    vector<handle_t> MutablePositionOverlay::divide_handle(const handle_t& handle, const std::vector<size_t>& offsets) {
        // the old steps need to be erased in the hash table
        for_each_step_on_handle(handle, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        auto new_handles = get()->divide_handle(handle, offsets);
        
        for_each_step_on_handle(new_handles.front(), [&](const step_handle_t& new_step) {
            reindex_contiguous_segment(new_step);
        });
        
        return new_handles;
    }
    
    void MutablePositionOverlay::optimize(bool allow_id_reassignment) {
        // optimization may include arbitrary changes that invalidate all handles, need to reindex
        get()->optimize(allow_id_reassignment);
        reindex_path_position();
    }
    
    bool MutablePositionOverlay::apply_ordering(const vector<handle_t>& order, bool compact_ids) {
        // depending on the implementation, this may change the values of the handles, which
        // may change the value of the steps, so the index could be entirely invalidated
        bool result = get()->apply_ordering(order, compact_ids);
        reindex_path_position();
        return result;
    }
    
    void MutablePositionOverlay::set_id_increment(const nid_t& min_id) {
        // no-op
    }
    
    void MutablePositionOverlay::increment_node_ids(nid_t increment) {
        // this can invalidate step handles, so there's no real option except to reindex completely
        get()->increment_node_ids(increment);
        reindex_path_position();
    }

    void MutablePositionOverlay::reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id) {
        // this can invalidate step handles, so there's no real option except to reindex completely
        get()->reassign_node_ids(get_new_id);
        reindex_path_position();
    }

    void MutablePositionOverlay::destroy_path(const path_handle_t& path) {
        
        // erase the path's indexes
        step_by_position.erase(path);
        min_path_offset.erase(path);
        for_each_step_in_path(path, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        get()->destroy_path(path);
    }
    
    path_handle_t MutablePositionOverlay::create_path_handle(const string& name, bool is_circular) {
        path_handle_t path_handle = get()->create_path_handle(name, is_circular);
        min_path_offset[path_handle] = 0;
        step_by_position[path_handle] = map<int64_t, step_handle_t>();
        return path_handle;
    }
    
    step_handle_t MutablePositionOverlay::append_step(const path_handle_t& path, const handle_t& to_append) {
        int64_t position = get_path_length(path) + min_path_offset[path];
        step_handle_t step = get()->append_step(path, to_append);
        step_by_position[path][position] = step;
        offset_by_step[step] = position;
        return step;
    }
    
    step_handle_t MutablePositionOverlay::prepend_step(const path_handle_t& path, const handle_t& to_prepend) {
        min_path_offset[path] -= get_length(to_prepend);
        step_handle_t step = get()->prepend_step(path, to_prepend);
        offset_by_step[step] = min_path_offset[path];
        step_by_position[path][min_path_offset[path]] = step;
        return step;
    }
    
    pair<step_handle_t, step_handle_t> MutablePositionOverlay::rewrite_segment(const step_handle_t& segment_begin,
                                                                               const step_handle_t& segment_end,
                                                                               const vector<handle_t>& new_segment) {
        
        // TODO: there really is no way to do this efficiently since it can shift the offsets downstream...
        
        int64_t offset = offset_by_step[segment_begin];
        
        // erase the records of all steps from the beginning of the segment onwards
        auto& path_step_by_position = step_by_position[get_path_handle_of_step(segment_begin)];
        path_step_by_position.erase(path_step_by_position.find(offset), path_step_by_position.end());
        for (auto step = segment_begin; step != path_end(get_path_handle_of_step(segment_begin)); step = get_next_step(step)) {
            offset_by_step.erase(step);
        }
        
        auto new_range = get()->rewrite_segment(segment_begin, segment_end, new_segment);
        
        // reindex the new suffix of the path
        for (auto step = new_range.first; step != path_end(get_path_handle_of_step(new_range.first)); step = get_next_step(step)) {
            offset_by_step[step] = offset;
            path_step_by_position[offset] = step;
            offset += get_length(get_handle_of_step(step));
        }
        
        return new_range;
    }
    
    void MutablePositionOverlay::reindex_path_position() {
        
        // rebuild the indexes
        step_by_position.clear();
        offset_by_step.clear();
        min_path_offset.clear();
        index_path_positions();
    }
    
    void MutablePositionOverlay::reindex_contiguous_segment(const step_handle_t& step) {
        
        // we may have already re-annotated this occurrence starting from a different step
        if (offset_by_step.count(step)) {
            return;
        }
        
        // walk backwards until the beginning of the path or a step that still has its
        // position recorded
        auto walker = step;
        while (!offset_by_step.count(walker)
               && walker != path_begin(get_path_handle_of_step(step))) {
            walker = get_previous_step(walker);
        }
        
        // compute the position of the next step without an offset annotation
        int64_t position;
        if (offset_by_step.count(walker)) {
            position = (min_path_offset[get_path_handle_of_step(step)]
                        + offset_by_step[walker]
                        + get_length(get_handle_of_step(walker)));
            // point the walker at the next unindexed position
            walker = get_next_step(walker);
        }
        else {
            // we must have have hit path_begin to have exited the previous while loop
            position = min_path_offset[get_path_handle_of_step(step)];
        }
        
        // add position annotations for all of the new handles (can also soak up adjacent
        // occurrences of the same node on this path)
        auto& path_step_by_position = step_by_position[get_path_handle_of_step(step)];
        for (; walker != path_end(get_path_handle_of_step(step)) && !offset_by_step.count(walker);
             walker = get_next_step(walker)) {
            
            path_step_by_position[position] = walker;
            offset_by_step[walker] = position;
            
            position += get_length(get_handle_of_step(walker));
        }
    }
    
    MutablePathDeletableHandleGraph* MutablePositionOverlay::get() {
        return mutable_graph;
    }
    
    const MutablePathDeletableHandleGraph* MutablePositionOverlay::get() const {
        return mutable_graph;
    }
}
