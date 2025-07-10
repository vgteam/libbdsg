//
//  packed_path_position_overlay.hpp
//  
//  Contains a memory efficient, generic overlays for PathHandleGraph's that add
//  the PathPositionHandleGraph interface methods for querying steps by base-pair
//  position.
//

#ifndef BDSG_PACKED_PATH_POSITION_OVERLAY_HPP_INCLUDED
#define BDSG_PACKED_PATH_POSITION_OVERLAY_HPP_INCLUDED

#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>
#include <handlegraph/util.hpp>
#include <BooPHF.h>

#include "bdsg/internal/hash_map.hpp"
#include "bdsg/internal/packed_structs.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the PathPositionHandleGraph interface to a static PathHandleGraph
 * by augmenting it with compressed index data structures.
 *
 * TODO: Make the overlay transparent so that paths hidden in the base graph
 * remain accessible through the path metadata queries. 
 */
class PackedPositionOverlay : public PathPositionHandleGraph, public ExpandingOverlayGraph {

protected:
    PackedPositionOverlay() = default;

public:
    
    /// Make a new PackedPositionOverlay, on the given graph. Glom short paths
    /// together to make internal indexes each over at least the given number
    /// of steps. Indexes any hidden paths that appear in extra_path_names.
    PackedPositionOverlay(const PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names = {}, size_t steps_per_index = 20000000);
    PackedPositionOverlay(const PackedPositionOverlay& other) = default;
    PackedPositionOverlay(PackedPositionOverlay&& other) = default;
    ~PackedPositionOverlay() = default;
    PackedPositionOverlay& operator=(const PackedPositionOverlay& other) = default;
    PackedPositionOverlay& operator=(PackedPositionOverlay&& other) = default;

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
    
protected:


    /// Execute a function on each path in the graph that matches the given sense, samples, and loci
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
    
    
    // local BBHash style hash function for step handles
    struct StepHash {
        uint64_t operator()(const step_handle_t& step, uint64_t seed = 0xAAAAAAAA55555555ULL) const;
    };
    
    /// Construct the index over path positions.
    /// Always includes paths with names in the given set, even if they are hidden.
    void index_path_positions(const std::unordered_set<std::string>& extra_path_names = {});
    
    /// Get the length in steps of the given path. Also do any scanning necessary for the path to generate per-path user data.
    virtual size_t scan_path(const path_handle_t& path_handle, void*& user_data);

    /// Set the number of distinct indexes we will use.
    virtual void set_index_count(size_t count);

    /// Into index i, index the given range of paths, with the given total size in steps. Consumes and destroys any per-path user data.
    virtual void index_paths(size_t index_num, const std::vector<path_handle_t>::const_iterator& begin_path, const std::vector<path_handle_t>::const_iterator& end_path, size_t cumul_path_size, void** user_data_base);
    
    /// The graph we're overlaying
    const PathHandleGraph* graph = nullptr;
    
    /// The number of steps we target when coalescing small paths into larger indexes.
    size_t steps_per_index;
    
    /// To facillitate parallel construction, we keep the index info for each
    /// path (or collection of tiny paths) in a separate object.
    struct PathIndex {
        /// The first half of the steps
        PagedVector<> steps_0;
        
        /// The second half of the steps
        PagedVector<> steps_1;
        
        /// The positions of the steps
        PagedVector<> positions;
        
        /// A perfect minimal hash function for the step handles on the path(s)
        /// We keep this in a vector so that we can be copyable, which the Python bindings want.
        /// TODO: replace with std::optional when we upgrade to C++17.
        std::vector<boomphf::mphf<step_handle_t, StepHash>> step_hash;
        
        /// The position of the step that hashes to a given index
        PackedVector<> step_positions;
    };
    
    /// This holds the indexes, each of which belongs to a path or collection
    /// of short paths.
    vector<PathIndex> indexes;
    
    /// And this represents a reference to an offset range in a PathIndex, where a path can be found.
    struct PathRange {
        size_t index_number;
        size_t start;
        size_t end;
    };
    
    /// Map from path_handle to the index and range of positions that contain
    /// its records in the steps and positions vectors. Note that access to
    /// existing entries is not thread-safe with the addition of new entries!
    /// Adding a new entry may deallocate existing entries, and will invalidate
    /// pointers and references to them! This is different from how the STL
    /// unordered_map behaves!
    hash_map<int64_t, PathRange> path_range;
};

/*
 * A wrapper for constructing the perfect minimal hash function that sequentially
 * returns all steps of a PathHandleGraph from an iterator struct
 */
struct BBHashHelper {
public:
    /// Set up for iteration over all paths in the given graph
    BBHashHelper(const PathHandleGraph* graph);
    /// Set up for iteration over the given paths (as iterators over path_handle_ts)
    template<typename InputIt>
    BBHashHelper(const PathHandleGraph* graph, InputIt first_path, InputIt last_path);
    BBHashHelper() = delete;
    ~BBHashHelper() = default;
    
    struct iterator {
    public:
        iterator(const iterator& other) = default;
        iterator() = delete;
        ~iterator() = default;
        iterator& operator=(const iterator& other) = default;
        iterator& operator++();
        step_handle_t operator*() const;
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        
    private:
        
        iterator(const BBHashHelper* iteratee, size_t path_handle_idx);
        
        size_t path_handle_idx = 0;
        step_handle_t step;
        const BBHashHelper* iteratee;
        
        friend class BBHashHelper;
    };
    
    /// C++ style range begin over steps
    iterator begin() const;
    
    /// C++ style range end over steps
    iterator end() const;
    
private:
    /// The graph whose steps this iterates over
    const PathHandleGraph* graph;
    /// An ordering of the path handles so we can refer to them by index
    vector<path_handle_t> path_handles;
    
    friend class iterator;
};

template<typename InputIt>
BBHashHelper::BBHashHelper(const PathHandleGraph* graph, InputIt first_path, InputIt last_path) : graph(graph) {
    path_handles.reserve(last_path - first_path);
    for (InputIt it = first_path; it != last_path; ++it) {
        if (!graph->is_empty(*it)) {
            // this path contains steps, we want to iterate over it
            path_handles.push_back(*it);
        }
    };
}
    
}

#endif
