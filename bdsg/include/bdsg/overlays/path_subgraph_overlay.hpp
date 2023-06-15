#ifndef BDSG_PATH_SUBGRAPH_OVERLAY_HPP_INCLUDED
#define BDSG_PATH_SUBGRAPH_OVERLAY_HPP_INCLUDED

/**
 * \file path_subgraph_overlay.hpp
 *
 * Provides PathSubgraphOverlay, a HandleGraph implementation that presents a
 * subset of the nodes in an existing graph, with paths.
 *
 */
 
#include <handlegraph/path_handle_graph.hpp>

#include "bdsg/overlays/subgraph_overlay.hpp"

namespace bdsg {

using namespace std;
using namespace handlegraph;

/**
 * Present a PathHandleGraph that is a backing HandleGraph but restricted
 * to a subset of nodes.
 *
 * Warning: we don't yet have a subgraph interface.  So we only consider paths
 * from the backing graph that are fully contained in the subgraph.
 */
class PathSubgraphOverlay : virtual public SubgraphOverlay, virtual public PathHandleGraph  {

public:

    PathSubgraphOverlay() = delete;

    /**
     * Make a new PathSubgraphOverlay. The backing graph must not be modified
     * while the overlay exists.
     *
     */
    PathSubgraphOverlay(const PathHandleGraph* backing, const unordered_set<nid_t>* node_subset);

    virtual ~PathSubgraphOverlay();

    ////////////////////////////////////////////////////////////////////////////
    // Path handle interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Returns the number of paths stored in the graph
    virtual size_t get_path_count() const;
    
    /// Determine if a path name exists and is legal to get a path handle for.
    virtual bool has_path(const std::string& path_name) const;
    
    /// Look up the path handle for the given path name.
    /// The path with that name must exist.
    virtual path_handle_t get_path_handle(const std::string& path_name) const;
    
    /// Look up the name of a path from a handle to it
    virtual std::string get_path_name(const path_handle_t& path_handle) const;
    
    /// Look up whether a path is circular
    virtual bool get_is_circular(const path_handle_t& path_handle) const;
    
    /// Returns the number of node steps in the path
    virtual size_t get_step_count(const path_handle_t& path_handle) const;
    
    /// Get a node handle (node ID and orientation) from a handle to an step on a path
    virtual handle_t get_handle_of_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the path that an step is on
    virtual path_handle_t get_path_handle_of_step(const step_handle_t& step_handle) const;
    
    /// Get a handle to the first step, which will be an arbitrary step in a circular path
    /// that we consider "first" based on our construction of the path. If the path is empty,
    /// then the implementation must return the same value as path_end().
    virtual step_handle_t path_begin(const path_handle_t& path_handle) const;
    
    /// Get a handle to a fictitious position past the end of a path. This position is
    /// returned by get_next_step for the final step in a path in a non-circular path.
    /// Note: get_next_step will *NEVER* return this value for a circular path.
    virtual step_handle_t path_end(const path_handle_t& path_handle) const;
    
    /// Get a handle to the last step, which will be an arbitrary step in a circular path that
    /// we consider "last" based on our construction of the path. If the path is empty
    /// then the implementation must return the same value as path_front_end().
    virtual step_handle_t path_back(const path_handle_t& path_handle) const;
    
    /// Get a handle to a fictitious position before the beginning of a path. This position is
    /// return by get_previous_step for the first step in a path in a non-circular path.
    /// Note: get_previous_step will *NEVER* return this value for a circular path.
    virtual step_handle_t path_front_end(const path_handle_t& path_handle) const;

    /// Returns true if the step is not the last step in a non-circular path.
    virtual bool has_next_step(const step_handle_t& step_handle) const;

    /// Returns true if the step is not the first step in a non-circular path.
    virtual bool has_previous_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the next step on the path. If the given step is the final step
    /// of a non-circular path, this method has undefined behavior. In a circular path,
    /// the "last" step will loop around to the "first" step.
    virtual step_handle_t get_next_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the previous step on the path. If the given step is the first
    /// step of a non-circular path, this method has undefined behavior. In a circular path,
    /// it will loop around from the "first" step (i.e. the one returned by path_begin) to
    /// the "last" step.
    virtual step_handle_t get_previous_step(const step_handle_t& step_handle) const;

protected:    

    /// Execute a function on each path in the graph. If it returns false, stop
    /// iteration. Returns true if we finished and false if we stopped early.
    virtual bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const;

    /// Execute a function on each step of a handle in any path. If it
    /// returns false, stop iteration. Returns true if we finished and false if
    /// we stopped early.
    virtual bool for_each_step_on_handle_impl(const handle_t& handle,
                                              const std::function<bool(const step_handle_t&)>& iteratee) const;


protected:

    /// the backing path graph, just to not have to bother with dynamic cast
    const PathHandleGraph* backing_path_graph;

    /// the subset of paths from the backing graph that are entirely contained within our subgraph
    unordered_set<path_handle_t> path_subset;
};

}

#endif

