#ifndef BDSG_SUBGRAPH_OVERLAY_HPP_INCLUDED
#define BDSG_SUBGRAPH_OVERLAY_HPP_INCLUDED

/**
 * \file subgraph_overlay.hpp
 *
 * Provides SubgraphOverlay, a HandleGraph implementation that presents a
 * subset of the nodes in an existing graph.
 *
 */

#include <handlegraph/handle_graph.hpp>
#include <handlegraph/util.hpp>

#include <unordered_set>

#include "bdsg/graph_proxy.hpp"

namespace bdsg {

using namespace std;
using namespace handlegraph;

// TODO: use ExpandingOverlayGraph

/**
 * Present a HandleGraph that is a backing HandleGraph but restricted
 * to a subset of nodes.  It won't give handles to nodes not in the 
 * subset, but it's not bulletproof: handles from outside the subset
 * won't undergo any special checks.  
 */
class SubgraphOverlay : virtual public HandleGraph, public HandleGraphProxy<HandleGraph> {

public:
    /**
     * Make a new PathSubgraphOverlay. The backing graph must not be modified
     * while the overlay exists.
     *
     */
    SubgraphOverlay(const HandleGraph* backing, const unordered_set<nid_t>* node_subset);

    virtual ~SubgraphOverlay();

    ////////////////////////////////////////////////////////////////////////////
    // Handle-based interface
    ////////////////////////////////////////////////////////////////////////////

    /// Method to check if a node exists by ID
    virtual bool has_node(nid_t node_id) const;
    
    /// Get a handle to the given node in the given orientation. Node must exist.
    handle_t get_handle(const nid_t& node_id, bool is_reverse) const;
   
    /// Return the number of nodes in the graph
    virtual size_t get_node_count() const;
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t min_node_id() const;
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t max_node_id() const;

protected:
    
    /// Loop over all the handles to next/previous (right/left) nodes. Passes
    /// them to a callback which returns false to stop iterating and true to
    /// continue. Returns true if we finished and false if we stopped early.
    virtual bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
    
    /// Loop over all the nodes in the graph in their local forward
    /// orientations, in their internal stored order. Stop if the iteratee
    /// returns false. Can be told to run in parallel, in which case stopping
    /// after a false return value is on a best-effort basis and iteration
    /// order is not defined. Returns true if we finished and false if we 
    /// stopped early.
    virtual bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;

protected:
    
    /// Get the HandleGraph to which most methods should be delegated.
    inline const HandleGraph* get() const {
        return backing_graph;
    }
    
    /// the backing graph
    const HandleGraph* backing_graph;

    /// the node subset. note, we don't own this so its up to client to keep in scope,
    /// just like the backing graph
    const unordered_set<nid_t>* node_subset;
    
    /// keep min_node_id() and max_node_id() constant
    nid_t min_node = 0;
    nid_t max_node = 0;
};

}

#endif
