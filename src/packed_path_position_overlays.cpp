#include "bdsg/packed_path_position_overlays.hpp"


namespace bdsg {

    PackedPositionOverlay::PackedPositionOverlay(const PathHandleGraph* graph) : graph(graph), steps_0(1024), steps_1(256), positions(256) {
        index_path_positions();
    }

    PackedPositionOverlay::PackedPositionOverlay() : steps_0(1), steps_1(1), positions(1) {
        
    }
    
    PackedPositionOverlay::~PackedPositionOverlay() {
        delete step_hash;
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
    
    bool PackedPositionOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                       const function<bool(const step_handle_t&)>& iteratee) const {
        return graph->for_each_step_on_handle(handle, iteratee);
    }
    
    size_t PackedPositionOverlay::get_path_length(const path_handle_t& path_handle) const {
        const auto& range = path_range.at(as_integer(path_handle));
        if (range.first == range.second) {
            return 0;
        }
        step_handle_t step;
        as_integers(step)[0] = steps_0.get(range.second - 1);
        as_integers(step)[1] = steps_1.get(range.second - 1);
        return positions.get(range.second - 1) + get_length(get_handle_of_step(step));
    }
    
    size_t PackedPositionOverlay::get_position_of_step(const step_handle_t& step) const {
        if (step == path_end(get_path_handle_of_step(step))) {
            return get_path_length(get_path_handle_of_step(step));
        }
        else {
            return step_positions.get(step_hash->lookup(step));
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
        size_t low = range.first;
        size_t hi = range.second;
        while (hi > low + 1) {
            size_t mid = (hi + low) / 2;
            if (position < positions.get(mid)) {
                hi = mid;
            }
            else {
                low = mid;
            }
        }
        
        // unpack the integers at the same index into a step
        step_handle_t step;
        as_integers(step)[0] = steps_0.get(low);
        as_integers(step)[1] = steps_1.get(low);
        return step;
    }
    
    handle_t PackedPositionOverlay::get_underlying_handle(const handle_t& handle) const {
        return handle;
    }
    
    void PackedPositionOverlay::index_path_positions() {
        
        
        size_t cumul_path_size = 0;
        
        for_each_path_handle([&](const path_handle_t& path_handle) {
            cumul_path_size += get_step_count(path_handle);
        });
        
        // resize the vectors to the number of step handles
        steps_0.resize(cumul_path_size);
        steps_1.resize(cumul_path_size);
        positions.resize(cumul_path_size);
        step_positions.resize(cumul_path_size);
        
        // make a perfect minimal hash over the step handles
        step_hash = new boomphf::mphf<step_handle_t, StepHash>(cumul_path_size, BBHashHelper(graph), 1, 1.0);
        
        size_t i = 0;
        for_each_path_handle([&](const path_handle_t& path_handle) {
            // get
            auto& range = path_range[as_integer(path_handle)];
            range.first = i;
            size_t position = 0;
            for_each_step_in_path(path_handle, [&](const step_handle_t& step) {
                
                // fill in the position to step index
                steps_0.set(i, as_integers(step)[0]);
                steps_1.set(i, as_integers(step)[1]);
                positions.set(i, position);
                
                // fill in the step to position index
                step_positions.set(step_hash->lookup(step), position);
                
                position += get_length(get_handle_of_step(step));
                ++i;
            });
            range.second = i;
        });
    }
    
    uint64_t PackedPositionOverlay::StepHash::operator()(const step_handle_t& step, uint64_t seed) const {
        return hash<step_handle_t>()(step) ^ seed;
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
        return iterator(*this, 0);
    }
    
    BBHashHelper::iterator BBHashHelper::end() const {
        return iterator(*this, path_handles.size());
    }
    
    BBHashHelper::iterator::iterator(const BBHashHelper& iteratee, size_t path_handle_idx) : iteratee(iteratee), path_handle_idx(path_handle_idx) {
        if (path_handle_idx < iteratee.path_handles.size()) {
            step = iteratee.graph->path_begin(iteratee.path_handles[path_handle_idx]);
        }
    }
    
    BBHashHelper::iterator& BBHashHelper::iterator::operator++() {
        step = iteratee.graph->get_next_step(step);
        if (step == iteratee.graph->path_begin(iteratee.path_handles[path_handle_idx]) ||
            step == iteratee.graph->path_end(iteratee.path_handles[path_handle_idx])) {
            // we either went off the end or looped around a circular path to the beginning again
            ++path_handle_idx;
            step = iteratee.graph->path_begin(iteratee.path_handles[path_handle_idx]);
        }
        return *this;
    }
    
    step_handle_t BBHashHelper::iterator::operator*() const {
        return step;
    }
    
    bool BBHashHelper::iterator::operator==(const BBHashHelper::iterator& other) const {
        // on the end iterator, we don't care what the step is, only that we're past-the-last
        // path handle
        return (&iteratee == &other.iteratee
                && path_handle_idx == other.path_handle_idx
                && (step == other.step || path_handle_idx == iteratee.path_handles.size()));
    }
    
    bool BBHashHelper::iterator::operator!=(const BBHashHelper::iterator& other) const {
        return !(*this == other);
    }
}
