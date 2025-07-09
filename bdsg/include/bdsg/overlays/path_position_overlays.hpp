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

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the PathPositionHandleGraph interface to a PathHandleGraph
 * by augmenting it with relatively simple data structures.
 *
 * To also provide mutable methods, see MutablePositionOverlay below.
 */
class PositionOverlay : public PathPositionHandleGraph, public ExpandingOverlayGraph {
        
public:
    
    /// Make a postion overlay on the graph. Indexes any hidden paths that
    /// appear in extra_path_names.
    PositionOverlay(PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names = {});
    PositionOverlay();
    ~PositionOverlay();

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
    // Path handle interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Returns the number of paths stored in the graph
    size_t get_path_count() const;

    /// Determine if a path name exists and is legal to get a path handle for.
    bool has_path(const std::string& path_name) const;

    /// Look up the path handle for the given path name.
    /// The path with that name must exist.
    path_handle_t get_path_handle(const std::string& path_name) const;

    /// Look up the name of a path from a handle to it
    string get_path_name(const path_handle_t& path_handle) const;
    
    /// Look up whether a path is circular
    bool get_is_circular(const path_handle_t& path_handle) const;

    /// Returns the number of node steps in the path
    size_t get_step_count(const path_handle_t& path_handle) const;

    /// Get a node handle (node ID and orientation) from a handle to an step on a path
    handle_t get_handle_of_step(const step_handle_t& step_handle) const;

    /// Get a handle to the first step, or in a circular path to an arbitrary step
    /// considered "first". If the path is empty, returns the past-the-last step
    /// returned by path_end.
    step_handle_t path_begin(const path_handle_t& path_handle) const;
    
    /// Get a handle to a fictitious position past the end of a path. This position is
    /// return by get_next_step for the final step in a path in a non-circular path.
    /// Note that get_next_step will *NEVER* return this value for a circular path.
    step_handle_t path_end(const path_handle_t& path_handle) const;
    
    /// Get a handle to the last step, which will be an arbitrary step in a circular path that
    /// we consider "last" based on our construction of the path. If the path is empty
    /// then the implementation must return the same value as path_front_end().
    step_handle_t path_back(const path_handle_t& path_handle) const;
    
    /// Get a handle to a fictitious position before the beginning of a path. This position is
    /// return by get_previous_step for the first step in a path in a non-circular path.
    /// Note: get_previous_step will *NEVER* return this value for a circular path.
    step_handle_t path_front_end(const path_handle_t& path_handle) const;
    
    /// Returns true if the step is not the last step in a non-circular path.
    bool has_next_step(const step_handle_t& step_handle) const;
    
    /// Returns true if the step is not the first step in a non-circular path.
    bool has_previous_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the next step on the path. If the given step is the final step
    /// of a non-circular path, returns the past-the-last step that is also returned by
    /// path_end. In a circular path, the "last" step will loop around to the "first" (i.e.
    /// the one returned by path_begin).
    /// Note: to iterate over each step one time, even in a circular path, consider
    /// for_each_step_in_path.
    step_handle_t get_next_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the previous step on the path. If the given step is the first
    /// step of a non-circular path, this method has undefined behavior. In a circular path,
    /// it will loop around from the "first" step (i.e. the one returned by path_begin) to
    /// the "last" step.
    /// Note: to iterate over each step one time, even in a circular path, consider
    /// for_each_step_in_path.
    step_handle_t get_previous_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the path that an step is on
    path_handle_t get_path_handle_of_step(const step_handle_t& step_handle) const;
    
private:

    /// Execute a function on each path in the graph that match the given senses, samples, and loci
    bool for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                     const std::unordered_set<std::string>* samples,
                                     const std::unordered_set<std::string>* loci,
                                     const std::function<bool(const path_handle_t&)>& iteratee) const;

    /// Execute a function on each path in the graph
    bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const;
    
    /// Calls the given function for each step of the given handle on a path.
    bool for_each_step_on_handle_impl(const handle_t& handle,
                                      const function<bool(const step_handle_t&)>& iteratee) const;

    /// Calls the given function for each step of the given sense on the given handle on a path.
    bool for_each_step_of_sense_impl(const handle_t& visited, const PathSense& sense, const std::function<bool(const step_handle_t&)>& iteratee) const;
    
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
    
    /// Construct the index over path positions.
    /// Always includes paths with names in the given set, even if they are hidden.
    void index_path_positions(const std::unordered_set<std::string>& extra_path_names = {});
    
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
    inline const PathHandleGraph* get_graph() const {
        return graph;
    }
};

    
/*
 * An overlay that adds the PathPositionHandleGraph interface to a MutablePathDeletableHandleGraph
 * by augmenting it with relatively simple data structures.
 */
class MutablePositionOverlay : public PositionOverlay, public MutablePathDeletableHandleGraph {

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
    // MutableHandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Create a new node with the given sequence and return the handle.
    handle_t create_handle(const std::string& sequence);
    
    /// Create a new node with the given id and sequence, then return the handle.
    handle_t create_handle(const std::string& sequence, const nid_t& id);
    
    /// Remove the node belonging to the given handle and all of its edges.
    /// Does not update any stored paths.
    /// Invalidates the destroyed handle.
    /// May be called during serial for_each_handle iteration **ONLY** on the node being iterated.
    /// May **NOT** be called during parallel for_each_handle iteration.
    /// May **NOT** be called on the node from which edges are being followed during follow_edges.
    void destroy_handle(const handle_t& handle);
    
    /// Create an edge connecting the given handles in the given order and orientations.
    /// Ignores existing edges.
    void create_edge(const handle_t& left, const handle_t& right);
    
    /// Remove the edge connecting the given handles in the given order and orientations.
    /// Ignores nonexistent edges.
    /// Does not update any stored paths.
    void destroy_edge(const handle_t& left, const handle_t& right);
    
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
    
    /**
     * Make a path circular or non-circular. If the path is becoming circular, the
     * last step is joined to the first step. If the path is becoming linear, the
     * step considered "last" is unjoined from the step considered "first" according
     * to the method path_begin.
     */
    void set_circularity(const path_handle_t& path, bool circular);
    
private:
    
    /// Clear indexes and rebuild them
    void reindex_path_position();
    
    /// Indexes all of the steps contiguous with this one that are missing
    /// from the offset indexes (e.g. after an edit operation). Does nothing
    /// if the step is already indexed.
    void reindex_contiguous_segment(const step_handle_t& step);
    
    /// Override the get_graph from the PositionOverlay with a mutable cast (which must be
    /// valid if the constructor was valid)
    MutablePathDeletableHandleGraph* get_graph();
    
};
    
}

#endif
