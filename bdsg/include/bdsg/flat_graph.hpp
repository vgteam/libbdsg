//
//  flat_graph.hpp
//  
//  Contains a implementation of a simple memory-mapped sequence graph.
//

#ifndef BDSG_FLAT_GRAPH_HPP_INCLUDED
#define BDSG_FLAT_GRAPH_HPP_INCLUDED

#include <utility>

#include <handlegraph/deletable_handle_graph.hpp>
#include <handlegraph/serializable_handle_graph.hpp>
#include <handlegraph/trivially_serializable.hpp>

namespace bdsg{
    
using namespace handlegraph;

/**
 * Store a graph in a memory-mapped file, in a simple, flat representation.
 *
 * Nodes are stored as a doubly-linked structure of node records.
 */
class FlatGraph : public TriviallySerializable, public DeletableHandleGraph, public SerializableHandleGraph {
        
public:
    FlatGraph();
    ~FlatGraph() = default;
    
protected:
    
    // TODO: Wrap all record fields in endianness warppers.

    struct header_t {
        /// Stores the offset of the first node, or max limit if no nodes.
        size_t first_node;
        /// Stores the offset of the last node, or max limit if no nodes.
        size_t last_node;
        /// Stores the number of nodes
        size_t num_nodes;
        /// Stores the cached minimum node ID, if known, or min limit if not.
        nid_t min_node_id;
        /// Stores the cached maximum node ID, if known, or max limit if not.
        nid_t max_node_id;
    }

    /// Node record. First, if any is stored immediately after the header.
    struct node_t {
        /// ID of the node
        nid_t id;
        /// Number of sequence bytes stored immediately after the node.
        uint64_t seq_bytes;
        /// Number of edges stored immediately after the sequence.
        /// Alignment is for wimps.
        uint64_t edge_count;
        /// Offset of the previous node, or max value if no previous node.
        /// Needed so we can remove nodes and link up their neighbors.
        uint64_t prev_node;
        /// Amount of free space between the last edge and the next node.
        uint64_t free_space;
    };
    
    /// Edge record. Stored immediately after the sequence.
    /// Edges are stored once for each involved node side.
    struct edge_t {
        /// Whether the edge is on the start of this node
        bool on_start;
        /// Whether the edge is on the end of the other node.
        bool on_other_end;
        /// Offset of the to node.
        size_t other;
    };
    
    // No indexes; this graph really is trivial. If you want to get_handle you
    // have to do a linear scan.
    
    /// Accessor to get a typed pointer at the given offset.
    template<typename T>
    T* get(const size_t& offset) {
        char* base = serialized_data();
        return base ? (T*) (base + offset) : (T*) nullptr;
    }
    
    /// Accessor to get a typed const pointer at the given offset.
    template<typename T>
    const T* get(const size_t& offset) const {
        const char* base = serialized_data();
        return base ? (const T*) (base + offset) : (const T*) nullptr;
    }
    
    /// Grab out a range of our data as a string.
    inline std::string get_bytes(size_t start, size_t length) const {
        const char* base = serialized_data();
        return base ? std::string(((char*) base) + start, length) : "";
    }
    
    /// Put a string into our data
    inline void put_bytes(size_t start, const std::string& bytes) const {
        char* base = serialized_data();
        assert(base);
        ::memcpy((void*) (base + start), (void*) bytes.c_str(), bytes.size());
    }
    
    /// Make sure we are big enough to have the given amount of free space at
    /// the end. Returns the amount we actually have. May reallocate, so you
    /// need to get the header again.
    size_t ensure_trailing_space(size_t needed);
    
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
    std::string get_sequence(const handle_t& handle) const;
    
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
    
    /// Remove all nodes and edges. 
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
    
    /**
     * Reassign all node IDs as specified by the old->new mapping function.
     */
    void reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id);

    ////////////////////////////////////////////////////////////////////////////
    // I/O helper function
    ////////////////////////////////////////////////////////////////////////////

    /// Returns a static high-entropy number to indicate the class
    uint32_t get_magic_number() const;
};
    
    
    

} 

#endif
