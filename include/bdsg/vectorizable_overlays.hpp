//
//  vectorizable_overlays.hpp
//  
//  Contains generic overlays for HandleGraph's that add the
//  VectorizableHandleGraph interface methods for querying steps by base-pair
//  position.
//

#ifndef BDSG_VECTORIZABLE_OVERLAYS_HPP_INCLUDED
#define BDSG_VECTORIZABLE_OVERLAYS_HPP_INCLUDED

#include <unordered_map>
#include <map>

#include "sdsl/bit_vectors.hpp"

#include "bdsg/hash_map.hpp"

#include <handlegraph/handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the VectorizableHandleGraph interface to a HandleGraph
 * by augmenting it with relatively simple data structures.
 *
 */
class VectorizableOverlay : public VectorizableHandleGraph, public ExpandingOverlayGraph {
        
public:
    
    VectorizableOverlay(const HandleGraph* graph);
    VectorizableOverlay();
    ~VectorizableOverlay();

    ////////////////////////////////////////////////////////////////////////////
    // HandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Method to check if a node exists by ID
    bool has_node(nid_t node_id) const;
    
    /// Look up the handle for the node with the given ID in the given orientation
    handle_t get_handle(const nid_t& node_id, bool is_reverse = false) const;
    
    /// Get the ID from a handle
    nid_t get_id(const handle_t& handle) const;
    
    /// Get the orientation of a handle
    bool get_is_reverse(const handle_t& handle) const;
    
    /// Invert the orientation of a handle (potentially without getting its ID)
    handle_t flip(const handle_t& handle) const;
    
    /// Get the length of a node
    size_t get_length(const handle_t& handle) const;
    
    /// Get the sequence of a node, presented in the handle's local forward orientation.
    string get_sequence(const handle_t& handle) const;
    
private:
    
    /// Loop over all the handles to next/previous (right/left) nodes. Passes
    /// them to a callback which returns false to stop iterating and true to
    /// continue. Returns true if we finished and false if we stopped early.
    bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
    
    /// Loop over all the nodes in the graph in their local forward
    /// orientations, in their internal stored order. Stop if the iteratee
    /// returns false. Can be told to run in parallel, in which case stopping
    /// after a false return value is on a best-effort basis and iteration
    /// order is not defined.
    bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
    
public:
    
    /// Get the number of edges on the right (go_left = false) or left (go_left
    /// = true) side of the given handle. The default implementation is O(n) in
    /// the number of edges returned, but graph implementations that track this
    /// information more efficiently can override this method.
    size_t get_degree(const handle_t& handle, bool go_left) const;
    
    /// Returns true if there is an edge that allows traversal from the left
    /// handle to the right handle. By default O(n) in the number of edges
    /// on left, but can be overridden with more efficient implementations.
    bool has_edge(const handle_t& left, const handle_t& right) const;
    
    /// Returns one base of a handle's sequence, in the orientation of the
    /// handle.
    char get_base(const handle_t& handle, size_t index) const;
    
    /// Returns a substring of a handle's sequence, in the orientation of the
    /// handle. If the indicated substring would extend beyond the end of the
    /// handle's sequence, the return value is truncated to the sequence's end.
    std::string get_subsequence(const handle_t& handle, size_t index, size_t size) const;
    
    /// Return the number of nodes in the graph
    size_t get_node_count(void) const;
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t min_node_id(void) const;
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t max_node_id(void) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Vectorizable handle interface
    ////////////////////////////////////////////////////////////////////////////

    /// Return the start position of the node in a (possibly implict) sorted array
    /// constructed from the concatenation of the node sequences
    virtual size_t node_vector_offset(const nid_t& node_id) const;

    /// Return the node overlapping the given offset in the implicit node vector
    virtual nid_t node_at_vector_offset(const size_t& offset) const;

    /// Return a unique index among edges in the graph
    virtual size_t edge_index(const edge_t& edge) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Expanding overlay interface
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Returns the handle in the underlying graph that corresponds to a handle in the
     * overlay
     */
    handle_t get_underlying_handle(const handle_t& handle) const;
    
protected:
    
    /// Construct the index over path positions
    void index_nodes_and_edges();
    
    /// The graph we're overlaying
    const HandleGraph* graph = nullptr;

    /// Edge to rank
    // (I can't get it the pair_hash_map to compile with handle_t's, so using integers
    //  directly until I ca figure it out)
    pair_hash_map<pair<uint64_t, uint64_t>, size_t> edge_to_rank;
    
    /// Rank to node
    vector<nid_t> rank_to_node;

    /// Node to rank (make no assumptions about id-space so use map instead of vector)
    hash_map<nid_t, size_t> node_to_rank;

    /// Map between global sequence position and node rank
    sdsl::bit_vector s_bv;
    sdsl::rank_support_v<1> s_bv_rank;
    sdsl::bit_vector::select_1_type s_bv_select;
    
    /// Getter for graph
    inline const HandleGraph* get_graph() const {
        return graph;
    }
};
    
}

#endif
