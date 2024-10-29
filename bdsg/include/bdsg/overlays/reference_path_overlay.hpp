//
//  reference_path_overlay.hpp
//  
//  Contains an overlay that adds fast access to paths so they can be used as
//  references.
//

#ifndef BDSG_REFERENCE_PATH_OVERLAY_HPP_INCLUDED
#define BDSG_REFERENCE_PATH_OVERLAY_HPP_INCLUDED

#include <unordered_map>
#include <unordered_set>

#include <handlegraph/path_position_handle_graph.hpp>
#include <sdsl/bit_vectors.hpp>

#include "bdsg/internal/hash_map.hpp"
#include "bdsg/internal/packed_structs.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds fast access to paths in addition to allowing path
 * position queries on them. The original graph's handle_t's and path_handle_t's
 * remain valid for the overlay, but not the step_t's.
 *
 * Note that paths that are not indexed as reference paths (i.e. those that are
 * hidden from for_each_path_handle by the backing graph and not listed in
 * extra_path_names on construction) *will not be accessible through the
 * overlay*! You can ask if they exist, but trying to get handles to steps on
 * them will not work. To actually look at them you will need to go back to the
 * base graph.
 *
 * TODO: Make the overlay transparent so that paths that don't get indexed in
 * the overlay remain accessible but without the (fast versions of?) the
 * position queries.
 */
class ReferencePathOverlay : public PathPositionHandleGraph {
        
public:
    
    /// Create a ReferencePathOverlay indexing all non-hidden paths in the
    /// backing graph (which show up in for_each_path_handle()). For path names
    /// in extra_path_names, look them up and index them too, even if they are
    /// hidden.
    ReferencePathOverlay(const PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names = {});
    ReferencePathOverlay() = default;
    ~ReferencePathOverlay() = default;
    
    ////////////////////////////////////////////////////////////////////////////
    // HandleGraph interface implementations
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
    
    /// Get the sequence of a node, presented in the handle's local forward
    /// orientation.
    std::string get_sequence(const handle_t& handle) const;
    
    /// Return the number of nodes in the graph
    size_t get_node_count() const;
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t min_node_id() const;
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t max_node_id() const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Additional HandleGraph interface
    ////////////////////////////////////////////////////////////////////////////
    ///
    /// Get the number of edges on the right (go_left = false) or left (go_left
    /// = true) side of the given handle. The default implementation is O(n) in
    /// the number of edges returned, but graph implementations that track this
    /// information more efficiently can override this method.
    size_t get_degree(const handle_t& handle, bool go_left) const;
    
    /// Returns true if there is an edge that allows traversal from the left
    /// handle to the right handle. By default O(n) in the number of edges
    /// on left, but can be overridden with more efficient implementations.
    bool has_edge(const handle_t& left, const handle_t& right) const;
    
    /// Return the total number of edges in the graph. If not overridden,
    /// counts them all in linear time.
    size_t get_edge_count() const;
    
    /// Return the total length of all nodes in the graph, in bp. If not
    /// overridden, loops over all nodes in linear time.
    size_t get_total_length() const;
    
    /// Returns one base of a handle's sequence, in the orientation of the
    /// handle.
    char get_base(const handle_t& handle, size_t index) const;
    
    /// Returns a substring of a handle's sequence, in the orientation of the
    /// handle. If the indicated substring would extend beyond the end of the
    /// handle's sequence, the return value is truncated to the sequence's end.
    /// By default O(n) in the size of the handle's sequence, but can be overriden.
    std::string get_subsequence(const handle_t& handle, size_t index, size_t size) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Path handle interface implementations
    ////////////////////////////////////////////////////////////////////////////
    
    /// Returns the number of paths stored in the graph
    size_t get_path_count() const;
    
    /// Determine if a path name exists and is legal to get a path handle for.
    bool has_path(const std::string& path_name) const;
    
    /// Look up the path handle for the given path name.
    /// The path with that name must exist.
    path_handle_t get_path_handle(const std::string& path_name) const;
    
    /// Look up the name of a path from a handle to it
    std::string get_path_name(const path_handle_t& path_handle) const;
    
    /// Look up whether a path is circular
    bool get_is_circular(const path_handle_t& path_handle) const;
    
    /// Returns the number of node steps in the path
    size_t get_step_count(const path_handle_t& path_handle) const;
    
    /// Get a node handle (node ID and orientation) from a handle to an step on a path
    handle_t get_handle_of_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the path that an step is on
    path_handle_t get_path_handle_of_step(const step_handle_t& step_handle) const;
    
    /// Get a handle to the first step, which will be an arbitrary step in a circular path
    /// that we consider "first" based on our construction of the path. If the path is empty,
    /// then the implementation must return the same value as path_end().
    step_handle_t path_begin(const path_handle_t& path_handle) const;
    
    /// Get a handle to a fictitious position past the end of a path. This position is
    /// returned by get_next_step for the final step in a path in a non-circular path.
    /// Note: get_next_step will *NEVER* return this value for a circular path.
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
    /// of a non-circular path, this method has undefined behavior. In a circular path,
    /// the "last" step will loop around to the "first" step.
    step_handle_t get_next_step(const step_handle_t& step_handle) const;
    
    /// Returns a handle to the previous step on the path. If the given step is the first
    /// step of a non-circular path, this method has undefined behavior. In a circular path,
    /// it will loop around from the "first" step (i.e. the one returned by path_begin) to
    /// the "last" step.
    step_handle_t get_previous_step(const step_handle_t& step_handle) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // PathPositionHandleGraph interface
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

protected:
    
    ////////////////////////////////////////////////////////////////////////////
    // HandleGraph backing implementations
    ////////////////////////////////////////////////////////////////////////////
    
    /// Loop over all the handles to next/previous (right/left) nodes. Passes
    /// them to a callback which returns false to stop iterating and true to
    /// continue. Returns true if we finished and false if we stopped early.
    bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
    
    /// Loop over all the nodes in the graph in their local forward
    /// orientations, in their internal stored order. Stop if the iteratee
    /// returns false. Can be told to run in parallel, in which case stopping
    /// after a false return value is on a best-effort basis and iteration
    /// order is not defined. Returns true if we finished and false if we
    /// stopped early.
    bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
    
    
    ////////////////////////////////////////////////////////////////////////////
    // PathHandleGraph backing implementations
    ////////////////////////////////////////////////////////////////////////////

    /// Execute a function on each path in the graph. If it returns false, stop
    /// iteration. Returns true if we finished and false if we stopped early.
    ///
    /// If the graph contains compressed haplotype paths and properly
    /// implements for_each_path_of_sense to retrieve them, they should not be
    /// visible here. Only reference or generic named paths should be visible.
    bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const;
    
    /// Execute a function on each step of a handle in any path. If it
    /// returns false, stop iteration. Returns true if we finished and false if
    /// we stopped early.
    ///
    /// If the graph contains compressed haplotype paths and properly
    /// implements for_each_step_of_sense to find them, they should not be
    /// visible here. Only reference or generic named paths should be visible.
    bool for_each_step_on_handle_impl(const handle_t& handle,
                                      const std::function<bool(const step_handle_t&)>& iteratee) const;
    
    
    const PathHandleGraph* graph = nullptr;
    
    struct PathRecord {
    public:
        PathRecord() = default;
        ~PathRecord() = default;
        
        PackedVector<> steps;
        sdsl::bit_vector offsets;
        sdsl::bit_vector::rank_1_type offsets_rank;
        sdsl::bit_vector::select_1_type offsets_select;
    };
    
    std::unordered_map<path_handle_t, PathRecord> reference_paths;
    
    // indexed by node ID, the index that the node's steps begin in the step vectors
    sdsl::int_vector<> steps_begin;
    sdsl::int_vector<> step_1;
    sdsl::int_vector<> step_2;
};

}


#endif
