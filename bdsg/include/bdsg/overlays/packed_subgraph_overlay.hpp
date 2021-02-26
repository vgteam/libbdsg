//
//  packed_subgraph_overlay.hpp
//  
//  Defines a memory efficient, generic overlay to specify a subgraph of a
//  HandleGraph without duplicating the internal data.
//

#ifndef BDSG_PACKED_SUBGRAPH_OVERLAY_HPP_INCLUDED
#define BDSG_PACKED_SUBGRAPH_OVERLAY_HPP_INCLUDED

#include <handlegraph/handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>
#include <handlegraph/util.hpp>

#include "bdsg/internal/packed_structs.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * A memory-efficient overlay that acts as a subgraph of another
 * graph. Subgraph must consist of a subset of the parent graph's
 * nodes and all of the edges in the parent graph that connect them.
 */
class PackedSubgraphOverlay : public ExpandingOverlayGraph {
        
public:
    
    /// Initialize the overlay with the graph it sits on top of
    PackedSubgraphOverlay(const HandleGraph* graph);
    
    /// Default constructor (not functionally useful)
    PackedSubgraphOverlay();
    
    /// Destrutor
    ~PackedSubgraphOverlay();
    
    ////////////////////////////////////////////////////////////////////////////
    // Subgraph specification
    ////////////////////////////////////////////////////////////////////////////

    /// Add a node from the parent graph to the subgraph. Handle must
    /// come from the parent graph. Has no effect if the handle is already in
    /// the subgraph.
    void add_node(const handle_t& handle);
    
    /// Remove a node from the subgraph. Handle must come from the
    /// parent graph. Has no effect if the handle is not in the subgraph.
    void remove_node(const handle_t& handle);
    
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
    // Expanding overlay interface
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Returns the handle in the underlying graph that corresponds to a handle in the
     * overlay
     */
    handle_t get_underlying_handle(const handle_t& handle) const;
    
protected:
    
    /// The graph we're overlaying
    const HandleGraph* graph = nullptr;
    
    /// The handles that are included in the subgraph
    PackedSet<> subgraph_handles;
    
    /// Max node ID
    nid_t max_id = numeric_limits<nid_t>::min();
    
    /// Min node ID
    nid_t min_id = numeric_limits<nid_t>::max();
};
    
}

#endif
