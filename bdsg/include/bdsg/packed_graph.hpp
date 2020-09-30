//
//  packed_graph.hpp
//  
//  Contains a implementation of a sequence graph based on bit-packed integer
//  vectors.
//

#ifndef BDSG_PACKED_GRAPH_HPP_INCLUDED
#define BDSG_PACKED_GRAPH_HPP_INCLUDED

#include <utility>

#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/serializable_handle_graph.hpp>

#include "bdsg/overlays/strand_split_overlay.hpp"
#include "bdsg/internal/packed_structs.hpp"
#include "bdsg/internal/hash_map.hpp"
#include "bdsg/internal/utility.hpp"
#include "bdsg/internal/eades_algorithm.hpp"


namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/**
 * PackedGraph is a HandleGraph implementation designed to use very little
 * memory. It stores its data in bit-packed integer vectors, which are
 * dynamically widened as needed in O(1) amortized time. Within these vectors,
 * graphs are stored using adjacency linked lists.
 *
 * Since removals of elements can cause slots in the internal vectors to become
 * unused, the graph will occasionally defragment itself after some
 * modification operations, which involves copying its internal data
 * structures.
 *
 * This implementation is a good choice when working with very large graphs,
 * where the final memory usage of the constructed graph must be minimized. It
 * is not a good choice when large fractions of the graph will need to be
 * deleted and replaced; ODGI or HashGraph may be better for such workloads.
 */
class PackedGraph : public MutablePathDeletableHandleGraph, public SerializableHandleGraph {
        
public:
    PackedGraph();
    ~PackedGraph();
    
    ////////////////////////////////////////////////////////////////////////////
    // I/O methods
    ////////////////////////////////////////////////////////////////////////////
    
    /// Construct from a stream
    PackedGraph(istream& in);
    
private:
    
    /// Write the graph to an out stream (called from the inherited 'serialize'  method)
    void serialize_members(ostream& out) const;
    
    /// Read the graph from an in stream (called from the inherited 'deserialize'  method)
    void deserialize_members(istream& in);
    
public:
    
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
    std::string get_subsequence(const handle_t& handle, size_t index, size_t size) const;
    
    /// Return the number of nodes in the graph
    size_t get_node_count(void) const;
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t min_node_id(void) const;
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    nid_t max_node_id(void) const;

    /// Create a new node with the given sequence and return the handle.
    /// The sequence may not be empty.
    handle_t create_handle(const std::string& sequence);

    /// Create a new node with the given id and sequence, then return the handle.
    /// The sequence may not be empty.
    /// The ID must be strictly greater than 0.
    handle_t create_handle(const std::string& sequence, const nid_t& id);
    
    /// Remove the node belonging to the given handle and all of its edges.
    /// Destroys any paths in which the node participates.
    /// Invalidates the destroyed handle.
    /// May be called during serial for_each_handle iteration **ONLY** on the node being iterated.
    /// May **NOT** be called during parallel for_each_handle iteration.
    /// May **NOT** be called on the node from which edges are being followed during follow_edges.
    /// May **NOT** be called during iteration over paths, if it would destroy a path.
    /// May **NOT** be called during iteration along a path, if it would destroy that path.
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
    /// Optionally compact the id space of the graph to match the ordering, from 1->|ordering|.
    /// This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.
    void apply_ordering(const vector<handle_t>& order, bool compact_ids = false);
    
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
    
    /// Execute a function on each path in the graph
    bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const;
    
    /// Calls the given function for each step of the given handle on a path.
    bool for_each_step_on_handle_impl(const handle_t& handle,
                                      const function<bool(const step_handle_t&)>& iteratee) const;
    
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
    
    /**
     * Delete a segment of a path and rewrite it as some other sequence of
     * steps. Returns a pair of step_handle_t's that indicate the range of the
     * new segment in the path. The segment to delete should be designated by
     * the first (begin) and past-last (end) step handles.  If the step that is
     * returned by path_begin is deleted, path_begin will now return the first
     * step from the new segment or, in the case that the new segment is empty,
     * the step used as segment_end. Empty ranges consist of two copies of the
     * same step handle. Empty ranges in empty paths consist of two copies of
     * the end sentinel handle for the path. Rewriting an empty range inserts
     * before the provided end handle.
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

    /**
     * Set a minimum id to increment the id space by, used as a hint during construction.
     * May have no effect on a backing implementation.
     */
    void set_id_increment(const nid_t& min_id);

    /**
     * Add the given value to all node IDs
     */
    void increment_node_ids(nid_t increment);
    
    /**
     * Reassign all node IDs as specified by the old->new mapping function.
     */
    void reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id);

    ////////////////////////////////////////////////////////////////////////////
    // I/O helper function
    ////////////////////////////////////////////////////////////////////////////

    /// Returns a static high-entropy number to indicate the class
    uint32_t get_magic_number() const;
    
private:
    
    // Forward declaration so we can use it as an argument to methods
    struct PackedPath;
    
    /// Attempt to compress data into less memory, possibly using more memory temporarily
    /// (especially useful before serializing). Node handles remain valid, but path and
    /// step handles are invalidated.
    void tighten(void);
    
    /// Compact the node ID space to [1, num_nodes] according the indicated order. Every node
    /// must be present in the vector exactly one time to be valid.
    void compact_ids(const vector<handle_t>& order);
    
    /// Initialize all of the data corresponding with a new node and return
    /// it's 1-based offset
    size_t new_node_record(nid_t node_id);
    
    /// Find and edge on given handle, to a given handle, and remove it from the edge list
    void remove_edge_reference(const handle_t& on, const handle_t& to);
    
    /// If we've deleted any paths, remove them from the paths vector and reassign path IDs
    void eject_deleted_paths();
    
    /// Check if have orphaned enough records in the graph's various linked lists to
    /// warrant reallocating and defragmenting them. If so, do it. Optionally, defragment
    /// even if we have not deleted many things.
    void defragment(bool force = false);
    
    /// Check if have orphaned enough records in the linked list of the path to warrant
    /// reallocating and defragmenting it. If so, do it. Optionally, defragment even if
    /// we have not deleted many things.
    /// WARNING: invalidates step_handle_t's to this path.
    void defragment_path(const int64_t& path_idx, bool force = false);
    
    /// Convert a path name into an integer vector, assigning new chars as necessary.
    PackedVector encode_and_assign_path_name(const string& path_name);
    
    /// Convert a path name into an integer vector using only existing char assignments
    /// If the path name contains previously unseen characters, returns an empty vector.
    PackedVector encode_path_name(const string& path_name) const;
    
    /// Encode the path name into the internal representation and append it to the master
    /// list of path names.
    void append_path_name(const string& path_name);
    
    /// Decode the internal representation of a path name and return it as a string.
    string decode_path_name(const int64_t& path_idx) const;
    
    /// Extract the internal representation of a path name, but do not decode it.
    PackedVector extract_encoded_path_name(const int64_t& path_idx) const;
    
    /// Defragment data structures when the orphaned records are this fraction of the whole.
    const static double defrag_factor;
    
    /// We use standard page widths for page-compressed vectors
    const static size_t NARROW_PAGE_WIDTH;
    const static size_t WIDE_PAGE_WIDTH;
    
    /// The maximum ID in the graph
    nid_t max_id = 0;
    /// The minimum ID in the graph
    nid_t min_id = std::numeric_limits<nid_t>::max();
    
    // TODO: some of these offsets are a little silly and only are around as legacy.
    // They could be removed once the factoring stabilizes, but optimization will also
    // probably handle it.
    
    /// Encodes the topology of the graph. Consists of fixed width records that represent
    /// offsets in edge_lists_iv.
    /// {start edge list index, end edge list index}
    PagedVector graph_iv;
    const static size_t GRAPH_RECORD_SIZE;
    const static size_t GRAPH_START_EDGES_OFFSET;
    const static size_t GRAPH_END_EDGES_OFFSET;
    
    /// Encodes the start of a node's sequence in seq_iv. Matches the order of graph_iv.
    PagedVector seq_start_iv;
    const static size_t SEQ_START_RECORD_SIZE;
    
    /// Encodes the length of a node's sequence in seq_iv. Matches the order of graph_iv.
    PackedVector seq_length_iv;
    const static size_t SEQ_LENGTH_RECORD_SIZE;

    // TODO: split up the edge lists into separate vectors
    
    /// Encodes a series of edges lists of nodes.
    /// {ID|orientation (bit-packed), next edge index}
    PagedVector edge_lists_iv;
    const static size_t EDGE_RECORD_SIZE;
    const static size_t EDGE_TRAV_OFFSET;
    const static size_t EDGE_NEXT_OFFSET;
    
    // TODO: template out the deque and back nid_to_graph_iv with a paged vector? might
    // provide better compression now that it can handle 0's gracefully. unsure how the
    // wrapping around would act with pages though...
    
    /// Encodes the 1-based offset of an ID in graph_iv in units of GRAPH_RECORD_SIZE.
    /// If no node with that ID exists, contains a 0. The index of a given ID is
    /// computed by (ID - min ID).
    PackedDeque nid_to_graph_iv;

    /// Encodes all of the sequences of all nodes in the graph.
    PackedVector seq_iv;
    
    /// Encodes the membership of a node in all paths. In the same order as graph_iv.
    /// Consists of 1-based offset to the corresponding heads of linked lists in
    /// path_membership_value_iv, which contains the actual pointers into the paths.
    PagedVector path_membership_node_iv;
    const static size_t NODE_MEMBER_RECORD_SIZE;
    
    /// Encodes a series of linked lists of the memberships within paths. The nodes
    /// in the linked list are split over three separate vectors, with the entry at
    /// the same index in each vector corresponding to the same linked list node.
    /// Path ID (0-based index)
    PagedVector path_membership_id_iv;
    /// 1-based offset of the occurrence of the node in the corresponding PackedPath vector.
    PagedVector path_membership_offset_iv;
    /// 1-based offset of the next occurrence of this node on a path within this vector (or
    /// 0 if there is none)
    PagedVector path_membership_next_iv;
    const static size_t MEMBERSHIP_ID_RECORD_SIZE;
    const static size_t MEMBERSHIP_OFFSET_RECORD_SIZE;
    const static size_t MEMBERSHIP_NEXT_RECORD_SIZE;
    
    /// We will reassign char values from the path names to small integers
    hash_map<char, uint64_t> char_assignment;
    /// The inverse mapping from integer to the char value
    string inverse_char_assignment;
    
    /// All path names, encoded according to the char assignments and concatenated in
    /// a single vector
    PackedVector path_names_iv;
    
    /// The starting index of the path's name in path_names_iv for the path with the
    /// same index in paths
    PagedVector path_name_start_iv;
    
    /// The length of the path's name for the path with the same index in paths
    PackedVector path_name_length_iv;
    
    /// Bit-vector that marks whether the path at the same index has been deleted
    PackedVector path_is_deleted_iv;
    
    /// Bit-vector that marks whether the path at the same index is circular
    PackedVector path_is_circular_iv;
    
    /// The 1-based index of the head of the linked list in steps_iv of the path
    /// with the same index in paths
    PagedVector path_head_iv;
    
    /// The 1-based index of the tail of the linked list in steps_iv of the path
    /// with the same index in paths
    PagedVector path_tail_iv;
    
    /// The number of steps that have have deleted from the path at the same index
    PackedVector path_deleted_steps_iv;
    
    /*
     * A struct to package the data associated with a path through the graph.
     */
    struct PackedPath {
        PackedPath() : steps_iv(NARROW_PAGE_WIDTH), links_iv(NARROW_PAGE_WIDTH) {}
        
        /// Linked list records that encode the oriented nodes of the path. Indexes are
        /// 1-based, with 0 used as a sentinel to indicate none further.
        /// {prev index, next index}
        RobustPagedVector links_iv;
        /// The traversal value is stored in a separate vector at the matching index.
        /// {ID|orientation (bit-packed)}
        RobustPagedVector steps_iv;
    };
    const static size_t PATH_RECORD_SIZE;
    const static size_t PATH_PREV_OFFSET;
    const static size_t PATH_NEXT_OFFSET;
    
    const static size_t STEP_RECORD_SIZE;
    
    /// Map from path names to index in the paths vector.
    string_hash_map<PackedVector, int64_t> path_id;
    
    /// Vector of the embedded paths in the graph
    vector<PackedPath> paths;
    static const double PATH_RESIZE_FACTOR;
    
    ///////////////////////////////////////////////////////////////////////
    /// Convenience functions to translate between encodings in the vectors
    ///////////////////////////////////////////////////////////////////////
    
    /// Map nucleotides into [0, 4]
    inline uint64_t encode_nucleotide(const char& nt) const;
    /// Map [0, 4] to nucleotides
    inline char decode_nucleotide(const uint64_t& val) const;
    /// Complement nucleotide encoded as [0, 4]
    inline uint64_t complement_encoded_nucleotide(const uint64_t& val) const;
    
    /// Get the integer assignment of a char, or numeric_limits<uint64_t>::max()
    /// if no assignment has been made
    inline uint64_t get_assignment(const char& c) const;
    /// Get the integer assignment of a char, assigning a new one if necessary
    inline uint64_t get_or_make_assignment(const char& c);
    /// Get the char assigned to an integer (must be already assigned)
    inline char get_char(const uint64_t& assignment) const;
    
    inline size_t graph_iv_index(const handle_t& handle) const;
    
    inline uint64_t graph_index_to_seq_len_index(const size_t& graph_index) const;
    inline uint64_t graph_index_to_seq_start_index(const size_t& graph_index) const;
    inline uint64_t graph_index_to_node_member_index(const size_t& graph_index) const;
    
    inline const uint64_t& encode_traversal(const handle_t& handle) const;
    inline const handle_t& decode_traversal(const uint64_t& val) const;
    
    inline uint64_t get_next_edge_index(const uint64_t& edge_index) const;
    inline uint64_t get_edge_target(const uint64_t& edge_index) const;
    inline void set_edge_target(const uint64_t& edge_index, const handle_t& handle);
    
    inline uint64_t get_next_membership(const uint64_t& membership_index) const;
    inline uint64_t get_membership_step(const uint64_t& membership_index) const;
    inline uint64_t get_membership_path(const uint64_t& membership_index) const;
    inline void set_next_membership(const uint64_t& membership_index, const uint64_t& next);
    inline void set_membership_step(const uint64_t& membership_index, const uint64_t& step);
    inline void set_membership_path(const uint64_t& membership_index, const uint64_t& path);
    
    inline uint64_t get_step_trav(const PackedPath& path, const uint64_t& step_index) const;
    inline uint64_t get_step_prev(const PackedPath& path, const uint64_t& step_index) const;
    inline uint64_t get_step_next(const PackedPath& path, const uint64_t& step_index) const;
    inline void set_step_trav(PackedPath& path, const uint64_t& step_index, const uint64_t& trav);
    inline void set_step_prev(PackedPath& path, const uint64_t& step_index, const uint64_t& prev_index);
    inline void set_step_next(PackedPath& path, const uint64_t& step_index, const uint64_t& next_index);
    
    uint64_t deleted_node_records = 0;
    uint64_t deleted_edge_records = 0;
    uint64_t deleted_membership_records = 0;
    uint64_t deleted_bases = 0;
    uint64_t reversing_self_edge_records = 0;
    uint64_t deleted_reversing_self_edge_records = 0;
    
public:
    
    /// Debugging function, measures memory and prints a report to an ostream.
    /// Optionally reports memory usage for every path individually.
    void report_memory(ostream& out, bool individual_paths = false) const;
};
    
    
    
inline uint64_t PackedGraph::encode_nucleotide(const char& nt) const {
    
    uint64_t encoded;
    switch (nt) {
        case 'a':
        case 'A':
            encoded = 0;
            break;
            
        case 'c':
        case 'C':
            encoded = 1;
            break;
            
        case 'g':
        case 'G':
            encoded = 2;
            break;
            
        case 't':
        case 'T':
            encoded = 3;
            break;
            
        default:
            // all others, but probably N's
            encoded = 4;
            break;
    }
    
    return encoded;
}
    
inline uint64_t PackedGraph::complement_encoded_nucleotide(const uint64_t& val) const {
    return val == 4 ? 4 : 3 - val;
}
    
inline char PackedGraph::decode_nucleotide(const uint64_t& val) const {
    static const char* alphabet = "ACGTN";
    return alphabet[val];
}
    
inline size_t PackedGraph::graph_iv_index(const handle_t& handle) const {
    return (nid_to_graph_iv.get(get_id(handle) - min_id) - 1) * GRAPH_RECORD_SIZE;
}

inline uint64_t PackedGraph::graph_index_to_seq_len_index(const size_t& graph_index) const {
    return (graph_index * SEQ_LENGTH_RECORD_SIZE) / GRAPH_RECORD_SIZE;
}

inline uint64_t PackedGraph::graph_index_to_seq_start_index(const size_t& graph_index) const {
    return (graph_index * SEQ_START_RECORD_SIZE) / GRAPH_RECORD_SIZE;
}

inline uint64_t PackedGraph::graph_index_to_node_member_index(const size_t& graph_index) const {
    return (graph_index * NODE_MEMBER_RECORD_SIZE) / GRAPH_RECORD_SIZE;
}
    
inline const uint64_t& PackedGraph::encode_traversal(const handle_t& handle) const {
    return reinterpret_cast<const uint64_t&>(handle);
}
    
inline const handle_t& PackedGraph::decode_traversal(const uint64_t& val) const {
    return reinterpret_cast<const handle_t&>(val);
}

inline uint64_t PackedGraph::get_next_edge_index(const uint64_t& edge_index) const {
    return edge_lists_iv.get((edge_index - 1) * EDGE_RECORD_SIZE + EDGE_NEXT_OFFSET);
}

inline uint64_t PackedGraph::get_edge_target(const uint64_t& edge_index) const {
    return edge_lists_iv.get((edge_index - 1) * EDGE_RECORD_SIZE + EDGE_TRAV_OFFSET);
}
    
inline void PackedGraph::set_edge_target(const uint64_t& edge_index, const handle_t& handle) {
    edge_lists_iv.set((edge_index - 1) * EDGE_RECORD_SIZE + EDGE_TRAV_OFFSET, encode_traversal(handle));
}
    
inline uint64_t PackedGraph::get_next_membership(const uint64_t& membership_index) const {
    return path_membership_next_iv.get((membership_index - 1) * MEMBERSHIP_NEXT_RECORD_SIZE);
}
    
inline uint64_t PackedGraph::get_membership_step(const uint64_t& membership_index) const {
    return path_membership_offset_iv.get((membership_index - 1) * MEMBERSHIP_OFFSET_RECORD_SIZE);
}

inline uint64_t PackedGraph::get_membership_path(const uint64_t& membership_index) const {
    return path_membership_id_iv.get((membership_index - 1) * MEMBERSHIP_ID_RECORD_SIZE);
}

inline void PackedGraph::set_next_membership(const uint64_t& membership_index, const uint64_t& next) {
    path_membership_next_iv.set((membership_index - 1) * MEMBERSHIP_NEXT_RECORD_SIZE, next);
}
    
inline void PackedGraph::set_membership_step(const uint64_t& membership_index, const uint64_t& step) {
    path_membership_offset_iv.set((membership_index - 1) * MEMBERSHIP_ID_RECORD_SIZE, step);
}
    
inline void PackedGraph::set_membership_path(const uint64_t& membership_index, const uint64_t& path) {
    path_membership_id_iv.set((membership_index - 1) * MEMBERSHIP_NEXT_RECORD_SIZE, path);
}

inline uint64_t PackedGraph::get_step_trav(const PackedPath& path, const uint64_t& step_index) const {
    return path.steps_iv.get((step_index - 1) * STEP_RECORD_SIZE);
}

inline uint64_t PackedGraph::get_step_prev(const PackedPath& path, const uint64_t& step_index) const {
    return path.links_iv.get((step_index - 1) * PATH_RECORD_SIZE + PATH_PREV_OFFSET);
}

inline uint64_t PackedGraph::get_step_next(const PackedPath& path, const uint64_t& step_index) const {
    return path.links_iv.get((step_index - 1) * PATH_RECORD_SIZE + PATH_NEXT_OFFSET);
}

inline void PackedGraph::set_step_trav(PackedPath& path, const uint64_t& step_index, const uint64_t& trav) {
    path.steps_iv.set((step_index - 1) * STEP_RECORD_SIZE, trav);
}

inline void PackedGraph::set_step_prev(PackedPath& path, const uint64_t& step_index, const uint64_t& prev_index) {
    path.links_iv.set((step_index - 1) * PATH_RECORD_SIZE + PATH_PREV_OFFSET, prev_index);
}

inline void PackedGraph::set_step_next(PackedPath& path, const uint64_t& step_index, const uint64_t& next_index) {
    path.links_iv.set((step_index - 1) * PATH_RECORD_SIZE + PATH_NEXT_OFFSET, next_index);
}
    
inline uint64_t PackedGraph::get_assignment(const char& c) const {
    auto it = char_assignment.find(c);
    if (it != char_assignment.end()) {
        return it->second;
    }
    else {
        return numeric_limits<uint64_t>::max();
    }
}

inline uint64_t PackedGraph::get_or_make_assignment(const char& c) {
    auto it = char_assignment.find(c);
    if (it != char_assignment.end()) {
        return it->second;
    }
    else {
        char_assignment[c] = inverse_char_assignment.size();
        inverse_char_assignment.push_back(c);
        return inverse_char_assignment.size() - 1;
    }
}

inline char PackedGraph::get_char(const uint64_t& assignment) const {
    return inverse_char_assignment.at(assignment);
}

} // end dankness

#endif /* dgraph_hpp */
