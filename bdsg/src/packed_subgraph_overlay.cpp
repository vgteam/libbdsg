//
//  packed_subgraph_overlay.cpp
//
//  Contains the implementation of PackedSubgraphOverlay
//

#include "bdsg/overlays/packed_subgraph_overlay.hpp"
#include "bdsg/internal/utility.hpp"

namespace bdsg {

PackedSubgraphOverlay::PackedSubgraphOverlay(const HandleGraph* graph) : graph(graph) {
    
}

PackedSubgraphOverlay::PackedSubgraphOverlay() {
    // nothing to do
}

PackedSubgraphOverlay::~PackedSubgraphOverlay() {
    // nothing to do
}

void PackedSubgraphOverlay::add_node(const handle_t& handle) {
    nid_t node_id = graph->get_id(handle);
    max_id = max(node_id, max_id);
    min_id = min(node_id, min_id);
    subgraph_handles.insert(handlegraph::as_integer(graph->forward(handle)));
}

void PackedSubgraphOverlay::remove_node(const handle_t& handle) {
    subgraph_handles.remove(handlegraph::as_integer(graph->forward(handle)));
}

bool PackedSubgraphOverlay::has_node(nid_t node_id) const {
    if (graph->has_node(node_id)) {
        return subgraph_handles.find(handlegraph::as_integer(graph->get_handle(node_id)));
    }
    else {
        return false;
    }
}

handle_t PackedSubgraphOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
    return graph->get_handle(node_id, is_reverse);
}

nid_t PackedSubgraphOverlay::get_id(const handle_t& handle) const {
    return graph->get_id(handle);
}

bool PackedSubgraphOverlay::get_is_reverse(const handle_t& handle) const {
    return graph->get_is_reverse(handle);
}

handle_t PackedSubgraphOverlay::flip(const handle_t& handle) const {
    return graph->flip(handle);
}

size_t PackedSubgraphOverlay::get_length(const handle_t& handle) const {
    return graph->get_length(handle);
}

string PackedSubgraphOverlay::get_sequence(const handle_t& handle) const {
    return graph->get_sequence(handle);
}

bool PackedSubgraphOverlay::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    return graph->follow_edges(handle, go_left, [&](const handle_t& next) {
        if (subgraph_handles.find(handlegraph::as_integer(graph->forward(next)))) {
            return iteratee(next);
        }
        else {
            return true;
        }
    });
}

bool PackedSubgraphOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    // parallel is difficult to implement for a hash set, so it's all actually
    // going to be serial
    // TODO: wouldn't be hard to return a "mesh" of iterators that are approximately equally spaced
    bool keep_going = true;
    for (auto iter = subgraph_handles.begin(), end = subgraph_handles.end(); keep_going && iter != end; ++iter) {
        keep_going = iteratee(handlegraph::as_handle(*iter));
    }
    return keep_going;
}

char PackedSubgraphOverlay::get_base(const handle_t& handle, size_t index) const {
    return graph->get_base(handle, index);
}

std::string PackedSubgraphOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
    return graph->get_subsequence(handle, index, size);
}

size_t PackedSubgraphOverlay::get_node_count(void) const {
    return subgraph_handles.size();
}

nid_t PackedSubgraphOverlay::min_node_id(void) const {
    return min_id;
}

nid_t PackedSubgraphOverlay::max_node_id(void) const {
    return max_id;
}

handle_t PackedSubgraphOverlay::get_underlying_handle(const handle_t& handle) const {
    return handle;
}
}
