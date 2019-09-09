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
class VectorizableOverlay : virtual public VectorizableHandleGraph, virtual public ExpandingOverlayGraph {
        
public:
    
    VectorizableOverlay(const HandleGraph* graph);
    VectorizableOverlay();
    virtual ~VectorizableOverlay();

    ////////////////////////////////////////////////////////////////////////////
    // HandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Method to check if a node exists by ID
    virtual bool has_node(nid_t node_id) const;
    
    /// Look up the handle for the node with the given ID in the given orientation
    virtual  handle_t get_handle(const nid_t& node_id, bool is_reverse = false) const;
    
    /// Get the ID from a handle
    virtual nid_t get_id(const handle_t& handle) const;
    
    /// Get the orientation of a handle
    virtual  bool get_is_reverse(const handle_t& handle) const;
    
    /// Invert the orientation of a handle (potentially without getting its ID)
    virtual handle_t flip(const handle_t& handle) const;
    
    /// Get the length of a node
    virtual size_t get_length(const handle_t& handle) const;
    
    /// Get the sequence of a node, presented in the handle's local forward orientation.
    virtual string get_sequence(const handle_t& handle) const;
    
private:
    
    /// Loop over all the handles to next/previous (right/left) nodes. Passes
    /// them to a callback which returns false to stop iterating and true to
    /// continue. Returns true if we finished and false if we stopped early.
    virtual bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
    
    /// Loop over all the nodes in the graph in their local forward
    /// orientations, in their internal stored order. Stop if the iteratee
    /// returns false. Can be told to run in parallel, in which case stopping
    /// after a false return value is on a best-effort basis and iteration
    /// order is not defined.
    virtual bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
    
public:
    
    /// Get the number of edges on the right (go_left = false) or left (go_left
    /// = true) side of the given handle. The default implementation is O(n) in
    /// the number of edges returned, but graph implementations that track this
    /// information more efficiently can override this method.
    virtual size_t get_degree(const handle_t& handle, bool go_left) const;
    
    /// Returns true if there is an edge that allows traversal from the left
    /// handle to the right handle. By default O(n) in the number of edges
    /// on left, but can be overridden with more efficient implementations.
    virtual bool has_edge(const handle_t& left, const handle_t& right) const;
    
    /// Returns one base of a handle's sequence, in the orientation of the
    /// handle.
    virtual char get_base(const handle_t& handle, size_t index) const;
    
    /// Returns a substring of a handle's sequence, in the orientation of the
    /// handle. If the indicated substring would extend beyond the end of the
    /// handle's sequence, the return value is truncated to the sequence's end.
    virtual std::string get_subsequence(const handle_t& handle, size_t index, size_t size) const;
    
    /// Return the number of nodes in the graph
    virtual size_t get_node_count(void) const;
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t min_node_id(void) const;
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t max_node_id(void) const;
    
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
    virtual handle_t get_underlying_handle(const handle_t& handle) const;
    
protected:
    
    /// Construct the index over path positions
    virtual void index_nodes_and_edges();
    
    /// The graph we're overlaying
    const HandleGraph* underlying_graph = nullptr;

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
    
};

class PathVectorizableOverlay : public VectorizableOverlay, virtual public PathHandleGraph {

public:
    
    PathVectorizableOverlay(const PathHandleGraph* path_graph);
    PathVectorizableOverlay();
    virtual ~PathVectorizableOverlay();

public:

    ////////////////////////////////////////////////////////////////////////////
    // Path handle interface that needs to be implemented
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
    
    /// Keep this around to avoid dynamic_casting this->underlying_graph every time we need path stuff
    const PathHandleGraph* underlying_path_graph = nullptr;
};


class PathPositionVectorizableOverlay : public PathVectorizableOverlay, virtual public PathPositionHandleGraph {

public:
    
    PathPositionVectorizableOverlay(const PathPositionHandleGraph* path_position_graph);
    PathPositionVectorizableOverlay();
    virtual ~PathPositionVectorizableOverlay();

public:

    ////////////////////////////////////////////////////////////////////////////
    // Path Position handle Interface that needs to be implemented
    ////////////////////////////////////////////////////////////////////////////

    /// Returns the length of a path measured in bases of sequence.
    virtual size_t get_path_length(const path_handle_t& path_handle) const;
    
    /// Returns the position along the path of the beginning of this step measured in
    /// bases of sequence. In a circular path, positions start at the step returned by
    /// path_begin().
    virtual size_t get_position_of_step(const step_handle_t& step) const;
    
    /// Returns the step at this position, measured in bases of sequence starting at
    /// the step returned by path_begin(). If the position is past the end of the
    /// path, returns path_end().
    virtual step_handle_t get_step_at_position(const path_handle_t& path,
                                               const size_t& position) const;
    
protected:
    
    /// Keep this around to avoid dynamic_casting this->underlying_graph every time we need path stuff
    const PathPositionHandleGraph* underlying_path_position_graph = nullptr;
};



}

#endif
