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
class PathSubgraphOverlay : virtual public SubgraphOverlay, virtual public PathHandleGraph, public PathProxy<PathHandleGraph> {

public:
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
    
    /// Get a handle for a path by name. The path must exist.
    virtual path_handle_t get_path_handle(const std::string& path_name) const;
    
protected:    

    /// Execute a function on each path in the graph. If it returns false, stop
    /// iteration. Returns true if we finished and false if we stopped early.
    virtual bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const;

    /// Execute a function on each step of a handle in any path. If it
    /// returns false, stop iteration. Returns true if we finished and false if
    /// we stopped early.
    virtual bool for_each_step_on_handle_impl(const handle_t& handle,
                                              const std::function<bool(const step_handle_t&)>& iteratee) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // PathMetadata interface
    ////////////////////////////////////////////////////////////////////////////
                                              
    // Loop through all the paths matching the given query. Query elements
    /// which are null match everything. Returns false and stops if the
    /// iteratee returns false.
    bool for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                     const std::unordered_set<std::string>* samples,
                                     const std::unordered_set<std::string>* loci,
                                     const std::function<bool(const path_handle_t&)>& iteratee) const;
    
    /// Loop through all steps on the given handle for paths with the given
    /// sense. Returns false and stops if the iteratee returns false.
    bool for_each_step_of_sense_impl(const handle_t& visited,
                                     const PathSense& sense,
                                     const std::function<bool(const step_handle_t&)>& iteratee) const;


protected:

    /// Get the HandleGraph to which most methods should be delegated.
    inline const PathHandleGraph* get() const {
        return backing_path_graph;
    }

    /// the backing path graph, just to not have to bother with dynamic cast
    const PathHandleGraph* backing_path_graph;

    /// the subset of paths from the backing graph that are entirely contained within our subgraph
    unordered_set<path_handle_t> path_subset;
};

}

#endif

