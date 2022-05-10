#include "bdsg/overlays/vectorizable_overlays.hpp"
#include "handlegraph/util.hpp"
#include "bdsg/internal/utility.hpp"

namespace bdsg {

VectorizableOverlay::VectorizableOverlay(const HandleGraph* graph) :
    underlying_graph(graph) {
    assert(underlying_graph != nullptr);
    index_nodes_and_edges();
}

VectorizableOverlay::VectorizableOverlay() {
    // Nothing to do!
}
    
VectorizableOverlay::~VectorizableOverlay() {
    // Nothing to do!
}
    
bool VectorizableOverlay::has_node(nid_t node_id) const {
    return underlying_graph->has_node(node_id);
}
    
handle_t VectorizableOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
    return underlying_graph->get_handle(node_id, is_reverse);
}
    
nid_t VectorizableOverlay::get_id(const handle_t& handle) const {
    return underlying_graph->get_id(handle);
}
    
bool VectorizableOverlay::get_is_reverse(const handle_t& handle) const {
    return underlying_graph->get_is_reverse(handle);
}
    
handle_t VectorizableOverlay::flip(const handle_t& handle) const {
    return underlying_graph->flip(handle);
}
    
size_t VectorizableOverlay::get_length(const handle_t& handle) const {
    return underlying_graph->get_length(handle);
}
    
string VectorizableOverlay::get_sequence(const handle_t& handle) const {
    return underlying_graph->get_sequence(handle);
}
    
bool VectorizableOverlay::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    return underlying_graph->follow_edges(handle, go_left, iteratee);
}
    
bool VectorizableOverlay::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    return underlying_graph->for_each_handle(iteratee, parallel);
}
    
size_t VectorizableOverlay::get_degree(const handle_t& handle, bool go_left) const {
    return underlying_graph->get_degree(handle, go_left);
}
    
bool VectorizableOverlay::has_edge(const handle_t& left, const handle_t& right) const {
    return underlying_graph->has_edge(left, right);
}
    
char VectorizableOverlay::get_base(const handle_t& handle, size_t index) const {
    return underlying_graph->get_base(handle, index);
}
    
std::string VectorizableOverlay::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
    return underlying_graph->get_subsequence(handle, index, size);
}
    
size_t VectorizableOverlay::get_node_count(void) const {
    return underlying_graph->get_node_count();
}
    
nid_t VectorizableOverlay::min_node_id(void) const {
    return underlying_graph->min_node_id();
}
    
nid_t VectorizableOverlay::max_node_id(void) const {
    return underlying_graph->max_node_id();
}

// this gives a 0-based offset of the node
size_t VectorizableOverlay::node_vector_offset(const nid_t& node_id) const {
    return s_bv_select(node_to_rank->lookup(node_id) + 1);
}

// this expects a 1-based offset of the node
nid_t VectorizableOverlay::node_at_vector_offset(const size_t& offset) const {
    return rank_to_node.at(s_bv_rank(offset));
}

size_t VectorizableOverlay::edge_index(const edge_t& edge) const {
    return edge_to_rank->lookup(canonicalize_edge(edge)) + 1;
}

size_t VectorizableOverlay::id_to_rank(const nid_t& node_id) const {
    return node_to_rank->lookup(node_id) + 1;
}

nid_t VectorizableOverlay::rank_to_id(const size_t& rank) const {
    return rank_to_node.at(rank);
}
    
handle_t VectorizableOverlay::get_underlying_handle(const handle_t& handle) const {
    return handle;
}
    
void VectorizableOverlay::index_nodes_and_edges() {


    // First we establish a sort order on the nodes.
    
    // index our node ranks
    rank_to_node.clear();
    size_t node_count = get_node_count();
    // Eventually the forst slot here is reserved for the nonexistent rank 0.
    rank_to_node.reserve(node_count + 1);
    // First we will fill this in just as a buffer, with all the node IDs.
    rank_to_node.resize(node_count);
    node_to_rank.reset();
    
    std::atomic<size_t> seq_length(0);
    std::atomic<size_t> next_free_slot(0);
    // Scan all the graph's nodes in parallel
    underlying_graph->for_each_handle([&](const handle_t& handle) {
        seq_length += underlying_graph->get_length(handle);
        // We are just using rank_to_node as a buffer for now.
        // Grab ourselves a slot nobody else is writing to.
        size_t slot = next_free_slot++;
        rank_to_node[slot] = underlying_graph->get_id(handle);
    }, true);
    
    vector<pair<pair<nid_t, bool>, pair<nid_t, bool>>> edge_buffer;
    
    // Do our node sort in parallel with our edge scan and sort.
    // TODO: Make each sort parallel instead with C++17?
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
            {
                // Sort the handles by node ID, ascending.
                // This means the minimal perfect hash function should always see the same
                // input, so it should always produce the same ordering.
                std::sort(rank_to_node.begin(), rank_to_node.end());
            }
            
            #pragma omp task
            {
                // We also need to do the edges. We need to impose an arbitrary order besed
                // entirely on node IDs and orientations.
                // TODO: It would be nice to do this in parallel, but we'd really want an
                // overall edge count forst, and most graph implementations seem to
                // implement that as a serial scan of the graph, so it wouldn't really
                // help.
                // TODO: Can we use the one parallel loop over the nodes to
                // replace this somehow?
                edge_to_rank.reset();
                underlying_graph->for_each_edge([&](const edge_t& edge) {
                    // Fill the buffer with ID, orientation representations of the edges.
                    edge_buffer.push_back(canonicalize_edge(edge));
                });
            
                // Sort edges in some consistent order, so we always feed the same thing to
                // the minimal perfect hash function for the same graph.
                std::sort(edge_buffer.begin(), edge_buffer.end());
            }
            #pragma omp taskwait
        }
    }
    
    // Make edge PMHF. Does its own threading. Do it first so we can drop the edge buffer.
    // note: we're mapping to 0-based rank, so need to add one after lookup
    edge_to_rank.reset(new boomphf::mphf<pair<pair<nid_t, bool>, pair<nid_t, bool>>, boomph_pair_pair_hash<nid_t, bool, nid_t, bool>>(
        edge_buffer.size(), edge_buffer, get_thread_count(), 2.0, false, false));
    edge_buffer.clear();
    
    // Make node PMHF. Does its own threading.
    // Note: we're mapping to 0-based rank, so need to add one after lookup
    node_to_rank.reset(new boomphf::mphf<nid_t, boomphf::SingleHashFunctor<nid_t>>(rank_to_node.size(), rank_to_node,
                                                                                   get_thread_count(), 2.0, false, false));
    
    
    // Add one slot to keep ranks in this table 1-based.
    rank_to_node.resize(node_count + 1);
    // Rank slot 0 should never be read. Put the 0 node ID which should never exist.
    rank_to_node[0] = 0;
    
    // We also want node lengths in rank order.
    // This might be a bit big, but we compress it into a bit vector later.
    std::vector<size_t> node_lengths;
    node_lengths.resize(rank_to_node.size());
    node_lengths[0] = 0;
    
    // Fill in the ranks by reading back from the boomph, which can't be iterated by itself.
    // It defines its own order that it wants us to use.
    // Make sure to do this in parallel too.
    underlying_graph->for_each_handle([&](const handle_t& handle) {
        // Note that we don't depend on this iteration being in any particular order.
        nid_t id = underlying_graph->get_id(handle);
        size_t rank = node_to_rank->lookup(id) + 1;
        rank_to_node[rank] = id;
        
        // Now get the node length. Save a get_handle by doing it here.
        node_lengths[rank] =  underlying_graph->get_length(handle);
    }, true);
    
    
    // Index our node offsets along the linearized sequence
    sdsl::util::assign(s_bv, sdsl::bit_vector(seq_length));
    seq_length = 0;
    for (size_t node_rank = 1; node_rank < rank_to_node.size(); ++node_rank) {
        s_bv[seq_length] = 1;
        seq_length += node_lengths[node_rank];
    }
    node_lengths.clear();
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
            {
                // Populate rank data structures
                sdsl::util::assign(s_bv_rank, sdsl::rank_support_v<1>(&s_bv));
            }
            #pragma omp task
            {
                // And populate select data structures in parallel
                sdsl::util::assign(s_bv_select, sdsl::bit_vector::select_1_type(&s_bv));
            }
            #pragma omp taskwait
        }
    }
}

pair<pair<nid_t, bool>, pair<nid_t, bool>> VectorizableOverlay::canonicalize_edge(const edge_t& edge) const {
    // Unpack the handles
    pair<nid_t, bool> left = std::make_pair(underlying_graph->get_id(edge.first), underlying_graph->get_is_reverse(edge.first));
    pair<nid_t, bool> right = std::make_pair(underlying_graph->get_id(edge.second), underlying_graph->get_is_reverse(edge.second));
    
    pair<nid_t, bool> flipped_right = std::make_pair(right.first, !right.second);
    pair<nid_t, bool> flipped_left = std::make_pair(left.first, !left.second);
    
    // Consider the edge in both orientations and return the one that compares smaller.
    return std::min(std::make_pair(left, right), std::make_pair(flipped_right, flipped_left));
}

PathVectorizableOverlay::PathVectorizableOverlay(const PathHandleGraph* path_graph) :
    VectorizableOverlay::VectorizableOverlay(path_graph),
    underlying_path_graph(path_graph) {
    assert(underlying_graph != nullptr);
    assert(underlying_path_graph != nullptr);
}

PathVectorizableOverlay::PathVectorizableOverlay() {
        
}
    
PathVectorizableOverlay::~PathVectorizableOverlay() {
        
}

size_t PathVectorizableOverlay::get_path_count() const {
    return underlying_path_graph->get_path_count();
}
    
bool PathVectorizableOverlay::has_path(const std::string& path_name) const {
    return underlying_path_graph->has_path(path_name);
}
    
path_handle_t PathVectorizableOverlay::get_path_handle(const std::string& path_name) const {
    return underlying_path_graph->get_path_handle(path_name);
}
    
string PathVectorizableOverlay::get_path_name(const path_handle_t& path_handle) const {
    return underlying_path_graph->get_path_name(path_handle);
}
    
bool PathVectorizableOverlay::get_is_circular(const path_handle_t& path_handle) const {
    return underlying_path_graph->get_is_circular(path_handle);
}
    
size_t PathVectorizableOverlay::get_step_count(const path_handle_t& path_handle) const {
    return underlying_path_graph->get_step_count(path_handle);
}
    
handle_t PathVectorizableOverlay::get_handle_of_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->get_handle_of_step(step_handle);
}
    
step_handle_t PathVectorizableOverlay::path_begin(const path_handle_t& path_handle) const {
    return underlying_path_graph->path_begin(path_handle);
}
    
step_handle_t PathVectorizableOverlay::path_end(const path_handle_t& path_handle) const {
    return underlying_path_graph->path_end(path_handle);
}
    
step_handle_t PathVectorizableOverlay::path_back(const path_handle_t& path_handle) const {
    return underlying_path_graph->path_back(path_handle);
}
    
step_handle_t PathVectorizableOverlay::path_front_end(const path_handle_t& path_handle) const {
    return underlying_path_graph->path_front_end(path_handle);
}
    
bool PathVectorizableOverlay::has_next_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->has_next_step(step_handle);
}
    
bool PathVectorizableOverlay::has_previous_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->has_previous_step(step_handle);
}
    
step_handle_t PathVectorizableOverlay::get_next_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->get_next_step(step_handle);
}
    
step_handle_t PathVectorizableOverlay::get_previous_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->get_previous_step(step_handle);
}
    
path_handle_t PathVectorizableOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
    return underlying_path_graph->get_path_handle_of_step(step_handle);
}
    
bool PathVectorizableOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
    return underlying_path_graph->for_each_path_handle(iteratee);
}
    
bool PathVectorizableOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                           const function<bool(const step_handle_t&)>& iteratee) const {
    return underlying_path_graph->for_each_step_on_handle(handle, iteratee);
}
    

PathPositionVectorizableOverlay::PathPositionVectorizableOverlay(const PathPositionHandleGraph* path_position_graph) :
    PathVectorizableOverlay::PathVectorizableOverlay(path_position_graph),
    underlying_path_position_graph(path_position_graph) {
    assert(underlying_graph != nullptr);
    assert(underlying_path_graph != nullptr);
    assert(underlying_path_position_graph != nullptr);
}

PathPositionVectorizableOverlay::PathPositionVectorizableOverlay() {
        
}
    
PathPositionVectorizableOverlay::~PathPositionVectorizableOverlay() {
        
}

size_t PathPositionVectorizableOverlay::get_path_length(const path_handle_t& path_handle) const {
    return underlying_path_position_graph->get_path_length(path_handle);
}

size_t PathPositionVectorizableOverlay::get_position_of_step(const step_handle_t& step) const {
    return underlying_path_position_graph->get_position_of_step(step);
}
    
step_handle_t PathPositionVectorizableOverlay::get_step_at_position(const path_handle_t& path,
                                                                    const size_t& position) const {
    return underlying_path_position_graph->get_step_at_position(path, position);
}


}
