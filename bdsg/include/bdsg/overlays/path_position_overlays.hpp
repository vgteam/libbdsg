//
//  path_position_overlays.hpp
//  
//  Contains generic overlays for PathHandleGraph's that add the
//  PathPositionHandleGraph interface methods for querying steps by base-pair
//  position.
//

#ifndef BDSG_PATH_POSITION_OVERLAYS_HPP_INCLUDED
#define BDSG_PATH_POSITION_OVERLAYS_HPP_INCLUDED

#include <unordered_map>
#include <map>

#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>

#include "bdsg/graph_proxy.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the PathPositionHandleGraph interface to a PathHandleGraph
 * by augmenting it with relatively simple data structures.
 *
 * To also provide mutable methods, see MutablePositionOverlay below.
 */
class PositionOverlay : public PathPositionHandleGraph, public ExpandingOverlayGraph, public PathHandleGraphProxy<PathHandleGraph> {
        
public:
    
    PositionOverlay(PathHandleGraph* graph);
    PositionOverlay();
    ~PositionOverlay();

    ////////////////////////////////////////////////////////////////////////////
    // HandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
   
    // Most path handle graph stuff is provided by the proxy.
    // But we override some methods, because we need to hide paths we don't end up indexing.

public:
    /// Loop through all the paths matching the given query. Query elements
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
                                     
    /// Determine if a path name exists and is legal to get a path handle for.
    bool has_path(const std::string& path_name) const;
    
    // We don't need to intercept get_path_handle because it's not allowed to
    // be called if has_path is false.
    
public:
    
    ////////////////////////////////////////////////////////////////////////////
    // Path position interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Returns the length of a path measured in bases of sequence.
    size_t get_path_length(const path_handle_t& path_handle) const;
    
    /// Returns the position along the path of the beginning of this step measured in
    /// bases of sequence. In a circular path, positions start at the step returned by
    /// path_begin().
    size_t get_position_of_step(const step_handle_t& step) const;
    
    /// Returns the step at this position, measured in bases of sequence starting at
    /// the step returned by path_begin(). If the position is past the end of the
    /// path, returns path_end().
    step_handle_t get_step_at_position(const path_handle_t& path,
                                               const size_t& position) const;
    
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
    void index_path_positions();
    
    /// The graph we're overlaying
    PathHandleGraph* graph = nullptr;
    
    /// The "zero-point" of the offsets
    /// TODO: really only needed for the mutable inheritor
    unordered_map<path_handle_t, int64_t> min_path_offset;
    
    /// Index of step by position
    unordered_map<path_handle_t, map<int64_t, step_handle_t>> step_by_position;
    
    /// Index of position by step
    unordered_map<step_handle_t, int64_t> offset_by_step;
    
    /// Getter for graph
    inline const PathHandleGraph* get() const {
        return graph;
    }
};

    
/*
 * An overlay that adds the PathPositionHandleGraph interface to a MutablePathDeletableHandleGraph
 * by augmenting it with relatively simple data structures.
 */
class MutablePositionOverlay : public PositionOverlay, virtual public MutablePathDeletableHandleGraph, public MutablePathDeletableProxy<MutablePathDeletableHandleGraph> {

    // Because virtual base classes and multiple inheritance are involved, we
    // can't really safely go back from the PathHandleGraph* our base class
    // stores to the MutablePathDeletableHandleGraph* we sometimes need. So we
    // need to store it again or Clang will complain.

    // TODO: Can we hack around this some other way?
    MutablePathDeletableHandleGraph* mutable_graph;
    
public:
    
    MutablePositionOverlay(MutablePathDeletableHandleGraph* graph);
    MutablePositionOverlay();
    ~MutablePositionOverlay();
    
    ////////////////////////////////////////////////////////////////////////////
    // MutableHandleGraph interface overrides
    ////////////////////////////////////////////////////////////////////////////
    
    /// Remove all nodes and edges. Does not update any stored paths.
    void clear(void);
    
    /// Alter the node that the given handle corresponds to so the orientation
    /// indicated by the handle becomes the node's local forward orientation.
    /// Rewrites all edges pointing to the node and the node's sequence to
    /// reflect this. Invalidates all handles to the node (including the one
    /// passed). Returns a new, valid handle to the node in its new forward
    /// orientation. Note that it is possible for the node's ID to change.
    /// Does not update any stored paths. May change the ordering of the underlying
    /// graph.
    handle_t apply_orientation(const handle_t& handle);
    
    /// Split a handle's underlying node at the given offsets in the handle's
    /// orientation. Returns all of the handles to the parts. Other handles to
    /// the node being split may be invalidated. The split pieces stay in the
    /// same local forward orientation as the original node, but the returned
    /// handles come in the order and orientation appropriate for the handle
    /// passed in.
    /// Updates stored paths.
    vector<handle_t> divide_handle(const handle_t& handle, const std::vector<size_t>& offsets);
    
    /// Adjust the representation of the graph in memory to improve performance.
    /// Optionally, allow the node IDs to be reassigned to further improve
    /// performance.
    /// Note: Ideally, this method is called one time once there is expected to be
    /// few graph modifications in the future.
    void optimize(bool allow_id_reassignment = true);
    
    /// Reorder the graph's internal structure to match that given.
    /// This sets the order that is used for iteration in functions like for_each_handle.
    /// If compact_ids is true, may (but will not necessarily) compact the id space of the graph to match the ordering, from 1->|ordering|.
    /// In other cases, node IDs will be preserved.
    /// This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.
    /// This may invalidate outstanding handles.
    /// Returns true if node IDs actually were adjusted to match the given order, and false if they remain unchanged.
    bool apply_ordering(const vector<handle_t>& order, bool compact_ids = false);
    
    /// No-op function (required by MutableHandleGraph interface)
    void set_id_increment(const nid_t& min_id);
    
    /// Add the given value to all node IDs.
    /// Has a default implementation in terms of reassign_node_ids, but can be
    /// implemented more efficiently in some graphs.
    void increment_node_ids(nid_t increment);
    
    /// Renumber all node IDs using the given function, which, given an old ID, returns the new ID.
    /// Modifies the graph in place. Invalidates all outstanding handles.
    /// If the graph supports paths, they also must be updated.
    /// The mapping function may return 0. In this case, the input ID will
    /// remain unchanged. The mapping function should not return any ID for
    /// which it would return 0.
    void reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id);
    
    ////////////////////////////////////////////////////////////////////////////
    // MutablePathHandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Destroy the given path. Invalidates handles to the path and its node steps.
     */
    void destroy_path(const path_handle_t& path);
    
    /**
     * Create a path with the given name. The caller must ensure that no path
     * with the given name exists already, or the behavior is undefined.
     * Returns a handle to the created empty path. Handles to other paths must
     * remain valid.
     */
    path_handle_t create_path_handle(const string& name, bool is_circular = false);
    
    /**
     * Append a visit to a node to the given path. Returns a handle to the new
     * final step on the path which is appended. Handles to prior
     * steps on the path, and to other paths, must remain valid.
     */
    step_handle_t append_step(const path_handle_t& path, const handle_t& to_append);
    
    /**
     * Prepend a visit to a node to the given path. Returns a handle to the new
     * first step on the path which is appended. If the path is cirular, the new
     * step is placed between the steps considered "last" and "first" by the
     * method path_begin. Handles to later steps on the path, and to other paths,
     * must remain valid.
     */
    step_handle_t prepend_step(const path_handle_t& path, const handle_t& to_prepend);
    
    /// WARNING: online path position indexing is inefficient for this method.
    /**
     * Delete a segment of a path and rewrite it as some other sequence of steps. Returns a pair
     * of step_handle_t's that indicate the range of the new segment in the path. The segment to
     * delete should be designated by the first and the past-the-last step handle.  If the step
     * that is returned by path_begin is deleted, path_begin will now return the first step from
     * the new segment or, in the case that the new segment is empty, segment_end.
     */
    pair<step_handle_t, step_handle_t> rewrite_segment(const step_handle_t& segment_begin,
                                                       const step_handle_t& segment_end,
                                                       const vector<handle_t>& new_segment);
    
private:
    
    /// Clear indexes and rebuild them
    void reindex_path_position();
    
    /// Indexes all of the steps contiguous with this one that are missing
    /// from the offset indexes (e.g. after an edit operation). Does nothing
    /// if the step is already indexed.
    void reindex_contiguous_segment(const step_handle_t& step);
    
    /// Override the get from the PositionOverlay with a mutable cast (which
    /// must be valid if the constructor was valid).
    MutablePathDeletableHandleGraph* get();
    
    /// Keep a const version of the graph getter.
    const MutablePathDeletableHandleGraph* get() const;
    
};
    
}

#endif
