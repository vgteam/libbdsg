#include "bdsg/overlays/path_position_overlays.hpp"

namespace bdsg {

    PositionOverlay::PositionOverlay(PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names) : graph(graph) {
        index_path_positions(false, extra_path_names);
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
        return get_graph()->get_is_reverse(handle);
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

    bool PositionOverlay::for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                                      const std::unordered_set<std::string>* samples,
                                                      const std::unordered_set<std::string>* loci,
                                                      const std::function<bool(const path_handle_t&)>& iteratee) const {
        return get_graph()->for_each_path_matching(senses, samples, loci, iteratee);
    }
    
    bool PositionOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                       const function<bool(const step_handle_t&)>& iteratee) const {
        return get_graph()->for_each_step_on_handle(handle, iteratee);
    }

    bool PositionOverlay::for_each_step_of_sense_impl(const handle_t& visited, const PathSense& sense, 
                                                      const std::function<bool(const step_handle_t&)>& iteratee) const {
        return get_graph()->for_each_step_of_sense(visited, sense, iteratee);
    }
    
    size_t PositionOverlay::get_path_length(const path_handle_t& path_handle) const {
        auto found = step_by_position.find(path_handle);
        if (found == step_by_position.end()) {
            // This path is not indexed.
            throw std::logic_error("Path " + get_graph()->get_path_name(path_handle) + " is not indexed for " + std::string(__func__));
        }

        const auto& path_step_by_position = found->second;
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
        path_handle_t path_handle = get_path_handle_of_step(step);
        auto found = min_path_offset.find(path_handle);
        if (found == min_path_offset.end()) {
            // This path is not indexed.
            throw std::logic_error("Path " + get_graph()->get_path_name(path_handle) + " is not indexed for " + std::string(__func__));
        }

        return offset_by_step.at(step) - found->second;
    }
    
    step_handle_t PositionOverlay::get_step_at_position(const path_handle_t& path,
                                                        const size_t& position) const {
        
        auto found = min_path_offset.find(path);
        if (found == min_path_offset.end()) {
            // This path is not indexed.
            throw std::logic_error("Path " + get_graph()->get_path_name(path) + " is not indexed for " + std::string(__func__));
        }

        // TODO: Why isn't this a subtraction?
        int64_t lookup_position = position + found->second;
        
        const auto& path_step_by_position = step_by_position.at(path);
        if (path_step_by_position.empty()) {
            return get_graph()->path_end(path);
        }
        
        auto iter = --path_step_by_position.upper_bound(lookup_position);
        if (lookup_position - iter->first >= get_graph()->get_length(get_graph()->get_handle_of_step(iter->second))) {
            // this only occurs if the position was past the last base in the path
            return get_graph()->path_end(path);
        }
        else {
            return iter->second;
        }
    }
    
    handle_t PositionOverlay::get_underlying_handle(const handle_t& handle) const {
        return handle;
    }
    
    void PositionOverlay::index_path_positions(bool all_paths, const std::unordered_set<std::string>& extra_path_names) {
        
        auto visit_path = [&](const path_handle_t& path) {
            int64_t offset = 0;
            min_path_offset[path] = offset;
            auto& path_step_by_position = step_by_position[path];
            get_graph()->for_each_step_in_path(path, [&](const step_handle_t& step) {
                offset_by_step[step] = offset;
                path_step_by_position[offset] = step;
                offset += get_graph()->get_length(get_graph()->get_handle_of_step(step));
            });
        };
        
        std::unordered_set<PathSense> senses = {PathSense::REFERENCE, PathSense::GENERIC};
        get_graph()->for_each_path_matching(all_paths ? nullptr : &senses, nullptr, nullptr, visit_path);

        for (auto& name : extra_path_names) {
            path_handle_t path = get_graph()->get_path_handle(name);
            if (!min_path_offset.count(path)) {
                // We haven't already visited this path
                visit_path(path);
            }
        }
    }
    
    MutablePositionOverlay::MutablePositionOverlay(MutablePathDeletableHandleGraph* graph, bool all_paths, const std::unordered_set<std::string>& extra_path_names) 
        : PositionOverlay(graph),
          mutable_graph(graph),
          all_paths(all_paths),
          extra_path_names(extra_path_names)
    {
        // Nothing to do!
    }
    MutablePositionOverlay::MutablePositionOverlay(MutablePathDeletableHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names)
        : MutablePositionOverlay(graph, false, extra_path_names) {
        
        // Nothing to do!
    }
    
    MutablePositionOverlay::~MutablePositionOverlay() {
        // Nothing to do!
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
        get_graph()->create_edge(left, right);
    }
    
    void MutablePositionOverlay::destroy_edge(const handle_t& left, const handle_t& right) {
        get_graph()->destroy_edge(left, right);
    }
    
    void MutablePositionOverlay::clear(void) {
        get_graph()->clear();
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
        handle_t new_handle = get_graph()->apply_orientation(handle);
        
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
        
        auto new_handles = get_graph()->divide_handle(handle, offsets);
        
        for_each_step_on_handle(new_handles.front(), [&](const step_handle_t& new_step) {
            reindex_contiguous_segment(new_step);
        });
        
        return new_handles;
    }
    
    void MutablePositionOverlay::optimize(bool allow_id_reassignment) {
        // optimization may include arbitrary changes that invalidate all handles, need to reindex
        get_graph()->optimize(allow_id_reassignment);
        reindex_path_position();
    }
    
    bool MutablePositionOverlay::apply_ordering(const vector<handle_t>& order, bool compact_ids) {
        // depending on the implementation, this may change the values of the handles, which
        // may change the value of the steps, so the index could be entirely invalidated
        bool result = get_graph()->apply_ordering(order, compact_ids);
        reindex_path_position();
        return result;
    }
    
    void MutablePositionOverlay::set_id_increment(const nid_t& min_id) {
        // no-op
    }
    
    void MutablePositionOverlay::increment_node_ids(nid_t increment) {
        // this can invalidate step handles, so there's no real option except to reindex completely
        get_graph()->increment_node_ids(increment);
        reindex_path_position();
    }

    void MutablePositionOverlay::reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id) {
        // this can invalidate step handles, so there's no real option except to reindex completely
        get_graph()->reassign_node_ids(get_new_id);
        reindex_path_position();
    }

    void MutablePositionOverlay::destroy_path(const path_handle_t& path) {
        
        // erase the path's indexes
        step_by_position.erase(path);
        min_path_offset.erase(path);
        for_each_step_in_path(path, [&](const step_handle_t& step) {
            offset_by_step.erase(step);
        });
        
        get_graph()->destroy_path(path);
    }
    
    path_handle_t MutablePositionOverlay::create_path_handle(const string& name, bool is_circular) {
        path_handle_t path_handle = get_graph()->create_path_handle(name, is_circular);
        // New paths are always indexed (until we reindex).
        min_path_offset[path_handle] = 0;
        step_by_position[path_handle] = map<int64_t, step_handle_t>();
        // Remember we want to re-index this one if we reindex
        extra_path_names.insert(get_path_name(path_handle));
        return path_handle;
    }
    
    step_handle_t MutablePositionOverlay::append_step(const path_handle_t& path, const handle_t& to_append) {
        step_handle_t step = get_graph()->append_step(path, to_append);

        auto found = min_path_offset.find(path);
        if (found != min_path_offset.end()) {
            // Update the index if the path is indexed
            int64_t position = get_path_length(path) + found->second;
            step_by_position[path][position] = step;
            offset_by_step[step] = position;
        }
        return step;
    }
    
    step_handle_t MutablePositionOverlay::prepend_step(const path_handle_t& path, const handle_t& to_prepend) {
        step_handle_t step = get_graph()->prepend_step(path, to_prepend);
        
        auto found = min_path_offset.find(path);
        if (found != min_path_offset.end()) {
            found->second -= get_length(to_prepend);
            offset_by_step[step] = found->second;
            step_by_position[path][found->second] = step;
        }
        return step;
    }
    
    pair<step_handle_t, step_handle_t> MutablePositionOverlay::rewrite_segment(const step_handle_t& segment_begin,
                                                                               const step_handle_t& segment_end,
                                                                               const vector<handle_t>& new_segment) {
        
        
        path_handle_t path = get_path_handle_of_step(segment_begin);
        auto found = step_by_position.find(path);
        int64_t offset;
        if (found != step_by_position.end()) {
            // This path is indexed, so update the index

            // TODO: there really is no way to do this efficiently since it can shift the offsets downstream...
            
            offset = offset_by_step[segment_begin];
            
            // erase the records of all steps from the beginning of the segment onwards
            auto& path_step_by_position = found->second;
            path_step_by_position.erase(path_step_by_position.find(offset), path_step_by_position.end());
            for (auto step = segment_begin; step != path_end(path); step = get_next_step(step)) {
                offset_by_step.erase(step);
            }
        }

        // Do the rewrite and invalidate/replace steps
        auto new_range = get_graph()->rewrite_segment(segment_begin, segment_end, new_segment);

        if (found != step_by_position.end()) {

            auto& path_step_by_position = found->second;
            
            // reindex the new suffix of the path
            for (auto step = new_range.first; step != path_end(path); step = get_next_step(step)) {
                offset_by_step[step] = offset;
                path_step_by_position[offset] = step;
                offset += get_length(get_handle_of_step(step));
            }
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
        index_path_positions(this->all_paths, this->extra_path_names);
    }
    
    void MutablePositionOverlay::reindex_contiguous_segment(const step_handle_t& step) {
        
        path_handle_t path = get_path_handle_of_step(step);
        auto found = min_path_offset.find(path);
        if (found != min_path_offset.end()) {
            // The path is indexed, so update the index

            // we may have already re-annotated this occurrence starting from a different step
            if (offset_by_step.count(step)) {
                return;
            }
            
            // walk backwards until the beginning of the path or a step that still has its
            // position recorded
            auto walker = step;
            while (!offset_by_step.count(walker)
                   && walker != path_begin(path)) {
                walker = get_previous_step(walker);
            }
            
            // compute the position of the next step without an offset annotation
            int64_t position;
            if (offset_by_step.count(walker)) {
                position = (found->second
                            + offset_by_step[walker]
                            + get_length(get_handle_of_step(walker)));
                // point the walker at the next unindexed position
                walker = get_next_step(walker);
            }
            else {
                // we must have have hit path_begin to have exited the previous while loop
                position = found->second;
            }
            
            // add position annotations for all of the new handles (can also soak up adjacent
            // occurrences of the same node on this path)
            auto& path_step_by_position = step_by_position[path];
            for (; walker != path_end(path) && !offset_by_step.count(walker);
                 walker = get_next_step(walker)) {
                
                path_step_by_position[position] = walker;
                offset_by_step[walker] = position;
                
                position += get_length(get_handle_of_step(walker));
            }
        }
    }
    
    MutablePathDeletableHandleGraph* MutablePositionOverlay::get_graph() {
        return mutable_graph;
    }
}
