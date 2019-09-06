#include "bdsg/vectorizable_overlays.hpp"
#include "handlegraph/util.hpp"

namespace bdsg {

VectorizableOverlay::VectorizableOverlay(const HandleGraph* graph) : graph(graph) {
    index_nodes_and_edges();
}

VectorizableOverlay::VectorizableOverlay() {
        
}
    
VectorizableOverlay::~VectorizableOverlay() {
        
}
    
bool VectorizableOverlay::has_node(nid_t node_id) const {
    return get_graph()->has_node(node_id);
}
    
handle_t VectorizableOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
    return get_graph()->get_handle(node_id, is_reverse);
}
    
nid_t VectorizableOverlay::get_id(const handle_t& handle) const {
    return get_graph()->get_id(handle);
}
    
bool VectorizableOverlay::get_is_reverse(const handle_t& handle) const {
    return get_graph()->get_is_reverse(handle);
}
    
handle_t VectorizableOverlay::flip(const handle_t& handle) const {
    return get_graph()->flip(handle);
}
    
size_t VectorizableOverlay::get_length(const handle_t& handle) const {
    return get_graph()->get_length(handle);
}
    
string VectorizableOverlay::get_sequence(const handle_t& handle) const {
    return get_graph()->get_sequence(handle);
}
    
bool VectorizableOverlay::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    return get_graph()->follow_edges(handle, go_left, iteratee);
}
    
bool VectorizableOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    return get_graph()->for_each_handle(iteratee, parallel);
}
    
size_t VectorizableOverlay::get_degree(const handle_t& handle, bool go_left) const {
    return get_graph()->get_degree(handle, go_left);
}
    
bool VectorizableOverlay::has_edge(const handle_t& left, const handle_t& right) const {
    return get_graph()->has_edge(left, right);
}
    
char VectorizableOverlay::get_base(const handle_t& handle, size_t index) const {
    return get_graph()->get_base(handle, index);
}
    
std::string VectorizableOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
    return get_graph()->get_subsequence(handle, index, size);
}
    
size_t VectorizableOverlay::get_node_count(void) const {
    return get_graph()->get_node_count();
}
    
nid_t VectorizableOverlay::min_node_id(void) const {
    return get_graph()->min_node_id();
}
    
nid_t VectorizableOverlay::max_node_id(void) const {
    return get_graph()->max_node_id();
}

// this gives a 0-based offset of the node
size_t VectorizableOverlay::node_vector_offset(const nid_t& node_id) const {
    return s_bv_select(node_to_rank.at(node_id));
}

// this expects a 1-based offset of the node
nid_t VectorizableOverlay::node_at_vector_offset(const size_t& offset) const {
    return rank_to_node.at(s_bv_rank(offset));
}

size_t VectorizableOverlay::edge_index(const edge_t& edge) const {
    return edge_to_rank.at(make_pair(as_integer(edge.first), as_integer(edge.second)));
}
    
handle_t VectorizableOverlay::get_underlying_handle(const handle_t& handle) const {
    return handle;
}
    
void VectorizableOverlay::index_nodes_and_edges() {

    // index the edges
    //edge_to_rank.clear();
    size_t edge_rank = 1;
    get_graph()->for_each_edge([&](const edge_t& edge) {
            edge_to_rank[make_pair(as_integer(edge.first), as_integer(edge.second))] = edge_rank++;
        });

    // index our node ranks
    rank_to_node.clear();
    node_to_rank.clear();    
    rank_to_node.resize(get_node_count() + 1);
    node_to_rank.reserve(get_node_count());
    size_t seq_length = 0;
    size_t node_rank = 1;
    get_graph()->for_each_handle([&](const handle_t& handle) {
            seq_length += get_graph()->get_length(handle);
            nid_t node_id = get_graph()->get_id(handle);
            rank_to_node[node_rank] = node_id;
            node_to_rank[node_id] = node_rank;
            ++node_rank;
        });

    // index our node offsets
    sdsl::util::assign(s_bv, sdsl::bit_vector(seq_length));
    seq_length = 0;
    for (node_rank = 1; node_rank < rank_to_node.size(); ++node_rank) {
        s_bv[seq_length] = 1;
        seq_length += get_graph()->get_length(get_graph()->get_handle(rank_to_node[node_rank]));
    }
    sdsl::util::assign(s_bv_rank, sdsl::rank_support_v<1>(&s_bv));
    sdsl::util::assign(s_bv_select, sdsl::bit_vector::select_1_type(&s_bv));
}
    
}
