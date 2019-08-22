#include "bdsg/path_position_overlays.hpp"

namespace bdsg {

    PositionOverlay::PositionOverlay(PathHandleGraph* graph) : graph(graph) {
        index_path_positions();
    }

    PositionOverlay::PositionOverlay() {
        
    }
    
    PositionOverlay::~PositionOverlay() {
        
    }
    
    bool PositionOverlay::has_node(nid_t node_id) const {
        return get_graph()->has_node(node_id);
    }
    
    handle_t PositionOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
        return get_graph()->get_handle(node_id, is_reverse);
    }
    
    nid_t PositionOverlay::get_id(const handle_t& handle) const {
        return get_graph()->get_id(handle);
    }
    
    bool PositionOverlay::get_is_reverse(const handle_t& handle) const {
        return get_graph()->;
    }
    
    handle_t PositionOverlay::flip(const handle_t& handle) const {
        return get_graph()->flip(handle);
    }
    
    size_t PositionOverlay::get_length(const handle_t& handle) const {
        return get_graph()->get_length(handle);
    }
    
    string PositionOverlay::get_sequence(const handle_t& handle) const {
        return get_graph()->get_sequence(handle);
    }
    
    bool PositionOverlay::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
        return get_graph()->follow_edges(handle, go_left, iteratee);
    }
    
    bool PositionOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
        return get_graph()->for_each_handle(iteratee, parallel);
    }
    
    size_t PositionOverlay::get_degree(const handle_t& handle, bool go_left) const {
        return get_graph()->get_degree(handle, go_left);
    }
    
    bool PositionOverlay::has_edge(const handle_t& left, const handle_t& right) const {
        return get_graph()->has_edge(left, right);
    }
    
    char PositionOverlay::get_base(const handle_t& handle, size_t index) const {
        return get_graph()->get_base(handle, index);
    }
    
    std::string PositionOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
        return get_graph()->get_subsequence(handle, index, size);
    }
    
    size_t PositionOverlay::get_node_count(void) const {
        return get_graph()->get_node_count();
    }
    
    nid_t PositionOverlay::min_node_id(void) const {
        return get_graph()->min_node_id();
    }
    
    nid_t PositionOverlay::max_node_id(void) const {
        return get_graph()->max_node_id();
    }
    
    size_t PositionOverlay::get_path_count() const {
        return get_graph()->get_path_count();
    }
    
    bool PositionOverlay::has_path(const std::string& path_name) const {
        return get_graph()->has_path(path_name);
    }
    
    path_handle_t PositionOverlay::get_path_handle(const std::string& path_name) const {
        return get_graph()->get_path_handle(path_name);
    }
    
    string PositionOverlay::get_path_name(const path_handle_t& path_handle) const {
        return get_graph()->get_path_name(path_handle);
    }
    
    bool PositionOverlay::get_is_circular(const path_handle_t& path_handle) const {
        return get_graph()->get_is_circular(path_handle);
    }
    
    size_t PositionOverlay::get_step_count(const path_handle_t& path_handle) const {
        return get_graph()->get_step_count(path_handle);
    }
    
    handle_t PositionOverlay::get_handle_of_step(const step_handle_t& step_handle) const {
        return get_graph()->get_handle_of_step(step_handle);
    }
    
    step_handle_t PositionOverlay::path_begin(const path_handle_t& path_handle) const {
        return get_graph()->path_begin(path_handle);
    }
    
    step_handle_t PositionOverlay::path_end(const path_handle_t& path_handle) const {
        return get_graph()->path_end(path_handle);
    }
    
    step_handle_t PositionOverlay::path_back(const path_handle_t& path_handle) const {
        return get_graph()->path_back(path_handle);
    }
    
    step_handle_t PositionOverlay::path_front_end(const path_handle_t& path_handle) const {
        return get_graph()->path_front_end(path_handle);
    }
    
    bool PositionOverlay::has_next_step(const step_handle_t& step_handle) const {
        return get_graph()->has_next_step(step_handle);
    }
    
    bool PositionOverlay::has_previous_step(const step_handle_t& step_handle) const {
        return get_graph()->has_previous_step(step_handle);
    }
    
    step_handle_t PositionOverlay::get_next_step(const step_handle_t& step_handle) const {
        return get_graph()->get_next_step(step_handle);
    }
    
    step_handle_t PositionOverlay::get_previous_step(const step_handle_t& step_handle) const {
        return get_graph()->get_previous_step(step_handle);
    }
    
    path_handle_t PositionOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
        return get_graph()->get_path_handle_of_step(step_handle);
    }
    
    bool PositionOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
        return get_graph()->for_each_path_handle(iteratee);
    }
    
    bool PositionOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                       const function<bool(const step_handle_t&)>& iteratee) const {
        return get_graph()->for_each_step_on_handle(handle, iteratee);
    }
    
    size_t PositionOverlay::get_path_length(const path_handle_t& path_handle) const {
        const auto& path_step_by_position = step_by_position.at(path_handle);
        auto last = path_step_by_position.rbegin();
        if (last == path_step_by_position.rend()) {
            return 0;
        }
        else {
            return (last->first
                    + get_graph()->get_length(get_graph()->get_handle_of_step(last->second))
                    - min_path_offset.at(path_handle));
        }
        
    }
    
    size_t PositionOverlay::get_position_of_step(const step_handle_t& step) const {
        return offset_by_step.at(step) - min_path_offset.at(get_path_handle_of_step(step));
    }
    
    step_handle_t PositionOverlay::get_step_at_position(const path_handle_t& path,
                                                        const size_t& position) const {
        
        int64_t lookup_position = position + min_path_offset.at(path);
        auto iter = step_by_position.at(path).lower_bound(lookup_position);
        if (position - iter->first >= get_graph()->get_length(get_graph()->get_handle_of_step(iter->second))) {
            // this only occurs if the position was past the last base in the path
            return get_graph()->path_end();
        }
        else {
            return iter->second;
        }
    }
    
    handle_t PositionOverlay::get_underlying_handle(const handle_t& handle) const {
        return handle;
    }
    
    void PositionOverlay::index_path_positions() {
        
        get_graph()->for_each_path_handle([&](const path_handle_t& path) {
            int64_t offset = 0;
            min_path_offset[path] = offset;
            auto& path_step_by_position = step_by_position[path];
            get_graph()->for_each_step_in_path(path, [&](const step_handle_t& step) {
                offset_by_step[step] = offset;
                path_step_by_position[offset] = step;
                get_graph()->get_length(get_graph()->get_handle_of_step(step));
            });
        });
    }
    
    MutablePositionOverlay::MutablePositionOverlay(MutablePathDeletableHandleGraph* graph) : PositionOverlay(graph) {
        
    }
    
    MutablePositionOverlay::MutablePositionOverlay() {
        
    }
    
    MutablePositionOverlay::~MutablePositionOverlay() {
        
    }
    
    handle_t MutablePositionOverlay::create_handle(const std::string& sequence) {
        return get_graph()->create_handle(sequence);
    }
    
    handle_t MutablePositionOverlay::create_handle(const std::string& sequence, const nid_t& id) {
        return get_graph()->create_handle(sequence, id);
    }
    
    void MutablePositionOverlay::destroy_handle(const handle_t& handle) {
        // note: destroying a handle on a path is U.B., so don't worry about it
        get_graph()->destroy_handle(handle);
    }
    
    void MutablePositionOverlay::create_edge(const handle_t& left, const handle_t& right) {
        get_graph()->create_handle(left, right);
    }
    
    void MutablePositionOverlay::destroy_edge(const handle_t& left, const handle_t& right) {
        get_graph()->destroy_edge(left, right);
    }
    
    void clear(void) {
        get_graph()->clear();
        offset_by_step.clear();
        step_by_position.clear();
    }
    
    handle_t MutablePositionOverlay::apply_orientation(const handle_t& handle) {
        // this will reverse the orientation of the handle on paths, so we need to update
        // the corresponding steps
        
        // remove the previous annotations by step
        for_each_step_on_handle(handle, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        // do the flip
        handle_t new_handle = get_graph()->apply_orientation(handle);
        
        for_each_step_on_handle(new_handle, [&](const step_handle_t& new_step) {
            
            // walk backwards until the beginning of the path or a step that still has its
            // position recorded
            aauto prev = get_previous_step(new_step);
            size_t walked_distance = 0;
            while (!offset_by_step.count(prev)) {
                walked_distance += get_length(prev);
                // mid-loop stopping condition is also valid for circular paths
                if (prev == path_begin(get_path_handle_of_step(new_step))) {
                    break;
                }
                prev = get_previous_step(prev);
            }
            
            // compute the position based on this walk
            int64_t position;
            if (prev == path_begin(get_path_handle_of_step(new_step))) {
                position = walked_distance + min_path_offset[get_path_handle_of_step(new_step)];
            }
            else {
                position = offset_by_step[prev] + get_length(get_handle_of_step(prev)) + walked_distance;
            }
            
            offset_by_step[new_step] = position;
            step_by_position[get_handle_of_step(new_step)][position] = new_step;
        });
        
        return new_handle;
    }
    
    vector<handle_t> MutablePositionOverlay::divide_handle(const handle_t& handle, const std::vector<size_t>& offsets) {
        
        // the old steps need to be erased in the hash table
        for_each_step_on_handle(handle, [&](const step_handle_t& step) {
            offset_by_step.erase(step)
        });
        
        auto new_handles = get_graph()->divide_handle(handle, offsets);
        
        for_each_step_on_handle(new_handles.front(), [&](const step_handle_t& new_step) {
            
            // we may have already re-annotated this occurrence starting from a different step
            if (offset_by_step.count(new_step)) {
                return;
            }
            
            // walk backwards until the beginning of the path or a step that still has its
            // position recorded
            auto prev = get_previous_step(new_step);
            size_t walked_distance = 0;
            while (!offset_by_step.count(prev)) {
                walked_distance += get_length(prev);
                // mid-loop stopping condition is also valid for circular paths
                if (prev == path_begin(get_path_handle_of_step(new_step))) {
                    break;
                }
                prev = get_previous_step(prev);
            }
            
            // compute the position based on this walk
            int64_t position;
            if (prev == path_begin(get_path_handle_of_step(new_step))) {
                position = walked_distance + min_path_offset[get_path_handle_of_step(new_step)];
            }
            else {
                position = offset_by_step[prev] + get_length(get_handle_of_step(prev)) + walked_distance;
            }
            
            // add position annotations for all of the new handles (can also soak up adjacent
            // occurrences of the same node on this path)
            auto& path_step_by_position = step_by_position[get_path_handle_of_step(step)];
            for (auto step = get_next_step(prev);
                 step != path_end(get_path_handle_of_step(new_step)) && !offset_by_step.count(step);
                 step = get_next_step(step)) {
                
                path_step_by_position[position] = step;
                offset_by_step[step] = position;
                
                position += get_length(get_handle_of_step(step));
            }
        });
        
        return new_handles;
    }
    
    void MutablePositionOverlay::optimize(bool allow_id_reassignment) {
        // optimization may include arbitrary changes that invalidate all handles, need to reindex
        get_graph()->optimize(allow_id_reassignment);
        reindex_path_position();
    }
    
    void MutablePositionOverlay::apply_ordering(const vector<handle_t>& order, bool compact_ids) {
        // depending on the implementation, this may change the values of the handles, which
        // may change the value of the steps, so the index could be entirely invalidated
        get_graph->apply_ordering(order, compact_ids);
        reindex_path_position();
    }
    
    void MutablePositionOverlay::destroy_path(const path_handle_t& path) {
        
        // erase the path's indexes
        step_by_position.erase(path);
        for_each_step_in_path(path, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        get_graph()->destroy_path(path);
    }
    
    path_handle_t MutablePositionOverlay::create_path_handle(const string& name, bool is_circular) {
        path_handle_t path_handle = get_graph()->create_path_handle(name, is_circular);
        min_path_offset[path_handle] = 0;
        step_by_position[path_handle] = map<int64_t, step_handle_t>();
    }
    
    step_handle_t MutablePositionOverlay::append_step(const path_handle_t& path, const handle_t& to_append) {
        int64_t position = get_path_length(path) + min_path_offset[path];
        step_handle_t step = get_graph()->append_step(path, to_append);
        step_by_position[path][position] = step;
        offset_by_step[step] = position;
        return step;
    }
    
    step_handle_t MutablePositionOverlay::prepend_step(const path_handle_t& path, const handle_t& to_prepend) {
        min_path_offset[path] -= get_length(to_prepend);
        step_handle_t step = get_graph()->prepend_step(path, to_prepend);
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
        path_step_by_position.erase(path_step_by_position.find(starting_offset), path_step_by_position.end());
        for (auto step = segment_begin; step != path_end(get_path_handle_of_step(segment_begin)); step = get_next_step(step)) {
            offset_by_step.erase(step);
        }
        
        auto new_range = get_graph()->rewrite_segment(segment_begin, segment_end, new_segment);
        
        // reindex the new suffix of the path
        for (auto step = new_range.first; step != path_end(get_path_handle_of_step(new_range.first)); step = get_next_step(step)) {
            offset_by_step[step] = offset;
            path_step_by_position[offset] = step;
            offset += get_length(get_handle_of_step(step));
        }
        
        return new_range;
    }
    
    void MutablePositionOverlay::set_circularity(const path_handle_t& path, bool circular) {
        return get_graph()->set_circularity(path, circular);
    }
    
    void MutablePositionOverlay::reindex_path_position() {
        
        // rebuild the indexes
        step_by_position.clear();
        offset_by_step.clear();
        min_path_offset.clear();
        index_path_positions();
    }
}
