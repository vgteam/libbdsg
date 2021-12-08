#ifndef BDSG_GRAPH_PROXY_HPP_INCLUDED
#define BDSG_GRAPH_PROXY_HPP_INCLUDED

/**
 * \file graph_proxy.hpp
 * Defines a mechanism for implementing a handle graph by using a contained
 * object that actually does the work.
 */


#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/serializable_handle_graph.hpp>

namespace bdsg {

using namespace std;

/**
 * Base abstract class that defines the methods that must be implemented to get
 * the thing that the proxies are proxying for. You inherit one of the other
 * proxies and implement the methods in this class.
 */
template<typename BackingGraph>
struct AbstractGraphProxy {

protected:
    // The backing graph might not *actually* implement any handle graph
    // interfaces; it just needs to implement the concept, with all the right
    // methods.
    
    // These are the methods you need to implement to use the graph proxy system.
    // Then you inherit from some mix of
    // MutablePathDeletableHandleGraphProxy<BackingGraph> and/or
    // SerializableHandleGraphProxy<BackingGraph>.

    /**
     * Get the object that actually provides the graph methods.
     */
    virtual BackingGraph* get() = 0;
    
    /**
     * Get the object that actually provides the graph methods.
     */
    virtual const BackingGraph* get() const = 0;
    
};

/**
 * Defines a proxy you can inherit to implement HandleGraph by referencing a
 * different backing implementation, which implements the concept if not the
 * interface.
 *
 * Can be multiple-inherited alongsize other proxies and will use the same
 * backing implementation.
 */
template<typename BackingGraph> 
struct HandleGraphProxy : virtual public AbstractGraphProxy<BackingGraph>, virtual public HandleGraph {
public:
    // These methods implement the handle graph API in terms of just calling
    // the same methods on the backing graph, hoping that the types work out
    // given what we've been templated on.
    
    // TODO: Sure wish we had a macro to generate all these.
    
    /// Method to check if a node exists by ID
    bool has_node(nid_t node_id) const {
        return this->get()->has_node(node_id);
    }
   
    /// Look up the handle for the node with the given ID in the given orientation
    virtual handle_t get_handle(const nid_t& node_id, bool is_reverse = false) const {
        return this->get()->get_handle(node_id, is_reverse);
    }
    
    /// Get the ID from a handle
    virtual nid_t get_id(const handle_t& handle) const {
        return this->get()->get_id(handle);
    }
    
    /// Get the orientation of a handle
    virtual bool get_is_reverse(const handle_t& handle) const {
        return this->get()->get_is_reverse(handle);
    }
    
    /// Invert the orientation of a handle (potentially without getting its ID)
    virtual handle_t flip(const handle_t& handle) const {
        return this->get()->flip(handle);
    }
    
    /// Get the length of a node
    virtual size_t get_length(const handle_t& handle) const {
        return this->get()->get_length(handle);
    }
    
    /// Get the sequence of a node, presented in the handle's local forward
    /// orientation.
    virtual std::string get_sequence(const handle_t& handle) const {
        return this->get()->get_sequence(handle);
    }
    
    /// Return the number of nodes in the graph
    virtual size_t get_node_count() const {
        return this->get()->get_node_count();
    }
    
    /// Return the smallest ID in the graph, or some smaller number if the
    /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t min_node_id() const {
        return this->get()->min_node_id();
    }
    
    /// Return the largest ID in the graph, or some larger number if the
    /// largest ID is unavailable. Return value is unspecified if the graph is empty.
    virtual nid_t max_node_id() const {
        return this->get()->max_node_id();
    }
    
    /// Get the number of edges on the right (go_left = false) or left (go_left
    /// = true) side of the given handle. The default implementation is O(n) in
    /// the number of edges returned, but graph implementations that track this
    /// information more efficiently can override this method.
    virtual size_t get_degree(const handle_t& handle, bool go_left) const {
        return this->get()->get_degree(handle, go_left);
    }
    
    /// Returns true if there is an edge that allows traversal from the left
    /// handle to the right handle. By default O(n) in the number of edges
    /// on left, but can be overridden with more efficient implementations.
    virtual bool has_edge(const handle_t& left, const handle_t& right) const {
        return this->get()->has_edge(left, right);
    }
    
    /// Return the total number of edges in the graph. If not overridden,
    /// counts them all in linear time.
    virtual size_t get_edge_count() const {
        return this->get()->get_edge_count();
    }
    
    /// Return the total length of all nodes in the graph, in bp. If not
    /// overridden, loops over all nodes in linear time.
    virtual size_t get_total_length() const {
        return this->get()->get_total_length();
    }
    
    /// Returns one base of a handle's sequence, in the orientation of the
    /// handle.
    virtual char get_base(const handle_t& handle, size_t index) const {
        return this->get()->get_base(handle, index);
    }
    
    /// Returns a substring of a handle's sequence, in the orientation of the
    /// handle. If the indicated substring would extend beyond the end of the
    /// handle's sequence, the return value is truncated to the sequence's end.
    /// By default O(n) in the size of the handle's sequence, but can be overriden.
    virtual std::string get_subsequence(const handle_t& handle, size_t index, size_t size) const {
        return this->get()->get_subsequence(handle, index, size);
    }
    
protected:

    /// Loop over all the handles to next/previous (right/left) nodes. Passes
    /// them to a callback which returns false to stop iterating and true to
    /// continue. Returns true if we finished and false if we stopped early.
    virtual bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
        return this->get()->follow_edges_impl(handle, go_left, iteratee);
    }
    
    /// Loop over all the nodes in the graph in their local forward
    /// orientations, in their internal stored order. Stop if the iteratee
    /// returns false. Can be told to run in parallel, in which case stopping
    /// after a false return value is on a best-effort basis and iteration
    /// order is not defined. Returns true if we finished and false if we 
    /// stopped early.
    virtual bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const {
        return this->get()->for_each_handle_impl(iteratee, parallel);
    }

};

/**
 * Defines a proxy you can inherit to implement PathHandleGraph by referencing a
 * different backing implementation, which implements the concept if not the
 * interface.
 *
 * Can be multiple-inherited alongsize other proxies and will use the same
 * backing implementation.
 */
template<typename BackingGraph> 
struct PathHandleGraphProxy : virtual public HandleGraphProxy<BackingGraph>, virtual public PathHandleGraph {
public:
    /// Returns the number of paths stored in the graph
    virtual size_t get_path_count() const {
        return this->get()->get_path_count();
    }
    
    /// Determine if a path name exists and is legal to get a path handle for.
    virtual bool has_path(const std::string& path_name) const {
        return this->get()->has_path(path_name);
    }
    
    /// Look up the path handle for the given path name.
    /// The path with that name must exist.
    virtual path_handle_t get_path_handle(const std::string& path_name) const {
        return this->get()->get_path_handle(path_name);
    }
    
    /// Look up the name of a path from a handle to it
    virtual std::string get_path_name(const path_handle_t& path_handle) const {
        return this->get()->get_path_name(path_handle);
    }
    
    /// Look up whether a path is circular
    virtual bool get_is_circular(const path_handle_t& path_handle) const {
        return this->get()->get_is_circular(path_handle);
    }
    
    /// Returns the number of node steps in the path
    virtual size_t get_step_count(const path_handle_t& path_handle) const {
        return this->get()->get_step_count(path_handle);
    }

    /// Returns the number of node steps on a handle
    virtual size_t get_step_count(const handle_t& handle) const {
        return this->get()->get_step_count(handle);
    }
    
    /// Get a node handle (node ID and orientation) from a handle to an step on a path
    virtual handle_t get_handle_of_step(const step_handle_t& step_handle) const {
        return this->get()->get_handle_of_step(step_handle);
    }
    
    /// Returns a handle to the path that an step is on
    virtual path_handle_t get_path_handle_of_step(const step_handle_t& step_handle) const {
        return this->get()->get_path_handle_of_step(step_handle);
    }
    
    /// Get a handle to the first step, which will be an arbitrary step in a circular path
    /// that we consider "first" based on our construction of the path. If the path is empty,
    /// then the implementation must return the same value as path_end().
    virtual step_handle_t path_begin(const path_handle_t& path_handle) const {
        return this->get()->path_begin(path_handle);
    }
    
    /// Get a handle to a fictitious position past the end of a path. This position is
    /// returned by get_next_step for the final step in a path in a non-circular path.
    /// Note: get_next_step will *NEVER* return this value for a circular path.
    virtual step_handle_t path_end(const path_handle_t& path_handle) const {
        return this->get()->path_end(path_handle);
    }
    
    /// Get a handle to the last step, which will be an arbitrary step in a circular path that
    /// we consider "last" based on our construction of the path. If the path is empty
    /// then the implementation must return the same value as path_front_end().
    virtual step_handle_t path_back(const path_handle_t& path_handle) const {
        return this->get()->path_back(path_handle);
    }
    
    /// Get a handle to a fictitious position before the beginning of a path. This position is
    /// return by get_previous_step for the first step in a path in a non-circular path.
    /// Note: get_previous_step will *NEVER* return this value for a circular path.
    virtual step_handle_t path_front_end(const path_handle_t& path_handle) const {
        return this->get()->path_front_end(path_handle);
    }

    /// Returns true if the step is not the last step in a non-circular path.
    virtual bool has_next_step(const step_handle_t& step_handle) const {
        return this->get()->has_next_step(step_handle);
    }

    /// Returns true if the step is not the first step in a non-circular path.
    virtual bool has_previous_step(const step_handle_t& step_handle) const {
        return this->get()->has_previous_step(step_handle);
    }
    
    /// Returns a handle to the next step on the path. If the given step is the final step
    /// of a non-circular path, this method has undefined behavior. In a circular path,
    /// the "last" step will loop around to the "first" step.
    virtual step_handle_t get_next_step(const step_handle_t& step_handle) const {
        return this->get()->get_next_step(step_handle);
    }
    
    /// Returns a handle to the previous step on the path. If the given step is the first
    /// step of a non-circular path, this method has undefined behavior. In a circular path,
    /// it will loop around from the "first" step (i.e. the one returned by path_begin) to
    /// the "last" step.
    virtual step_handle_t get_previous_step(const step_handle_t& step_handle) const {
        return this->get()->get_previous_step(step_handle);
    }
    
    /// Returns a vector of all steps of a node on paths. Optionally restricts to
    /// steps that match the handle in orientation.
    virtual std::vector<step_handle_t> steps_of_handle(const handle_t& handle,
                                                       bool match_orientation = false) const {
        return this->get()->steps_of_handle(handle, match_orientation);
    }

    /// Returns true if the given path is empty, and false otherwise
    virtual bool is_empty(const path_handle_t& path_handle) const {
        return this->get()->is_empty(path_handle);
    }

protected:
    /// Execute a function on each path in the graph. If it returns false, stop
    /// iteration. Returns true if we finished and false if we stopped early.
    virtual bool for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
        return this->get()->for_each_path_handle_impl(iteratee);
    }
    
    /// Execute a function on each step of a handle in any path. If it
    /// returns false, stop iteration. Returns true if we finished and false if
    /// we stopped early.
    virtual bool for_each_step_on_handle_impl(const handle_t& handle,
        const std::function<bool(const step_handle_t&)>& iteratee) const {
        return this->get()->for_each_step_on_handle_impl(handle, iteratee);
    }

};

/**
 * Defines a proxy you can inherit to implement SerializableHandleGraph by referencing a
 * different backing implementation, which implements the concept if not the
 * interface.
 *
 * Can be multiple-inherited alongsize other proxies and will use the same
 * backing implementation.
 */
template<typename BackingGraph> 
struct SerializableHandleGraphProxy : virtual public AbstractGraphProxy<BackingGraph>, virtual public SerializableHandleGraph {
public:
    /// Returns a number that is specific to the serialized implementation for type
    /// checking. Does not depend on the contents of any particular instantiation
    /// (i.e. behaves as if static, but cannot be static and virtual).
    virtual uint32_t get_magic_number() const {
        return this->get()->get_magic_number();
    }
    
    /// Write the contents of this object to an ostream. Makes sure to include a
    /// leading magic number.
    virtual void serialize(std::ostream& out) const {
        this->get()->serialize(out);
    }
    
    /// Write the contents of this object to a named file. Makes sure to include
    /// a leading magic number.
    virtual void serialize(const std::string& filename) const {
        this->get()->serialize(filename);
    }
    
    /// Sets the contents of this object to the contents of a serialized object
    /// from an istream. The serialized object must be from the same
    /// implementation of the interface as is calling deserialize(). Can only
    /// be called on an empty object.
    virtual void deserialize(std::istream& in) {
        this->get()->deserialize(in);
    }
    
    /// Sets the contents of this object to the contents of a serialized object
    /// from a file. The serialized object must be from the same implementation
    /// of this interface as is calling deserialize(). Can only be called on an
    /// empty object.
    virtual void deserialize(const std::string& filename) {
        this->get()->deserialize(filename);
    }
    
    /// Write the contents of this object to an ostream. Makes sure to include a
    /// leading magic number.
    virtual void serialize(std::ostream& out) {
        this->get()->serialize(out);
    }
    
    /// Write the contents of this object to a named file. Makes sure to include
    /// a leading magic number.
    virtual void serialize(const std::string& filename) {
        this->get()->serialize(filename);
    }

protected:
    /// Underlying implementation for "serialize" method
    virtual void serialize_members(std::ostream& out) const {
        this->get()->serialize_members(out);
    }
    
    /// Underlying implementation to "deserialize" method
    virtual void deserialize_members(std::istream& in){
        this->get()->deserialize_members(in);
    }

};

/**
 * Defines a proxy you can inherit to implement MutablePathDeletableHandleGraph
 * by referencing a different backing implementation.
 *
 * The backing implementation doesn't have to inherit from anything; it just
 * has to satisfy the concept (i.e. provide all the methods).
 */
template<typename BackingGraph>
struct MutablePathDeletableHandleGraphProxy : virtual public PathHandleGraphProxy<BackingGraph>, virtual public MutablePathDeletableHandleGraph {

public:
    
    /// Create a new node with the given sequence and return the handle.
    /// The sequence may not be empty.
    virtual handle_t create_handle(const std::string& sequence) {
        return this->get()->create_handle(sequence);
    }

    /// Create a new node with the given id and sequence, then return the handle.
    /// The sequence may not be empty.
    /// The ID must be strictly greater than 0.
    virtual handle_t create_handle(const std::string& sequence, const nid_t& id) {
        return this->get()->create_handle(sequence, id);
    }
    
    /// Create an edge connecting the given handles in the given order and orientations.
    /// Ignores existing edges.
    virtual void create_edge(const handle_t& left, const handle_t& right) {
        this->get()->create_edge(left, right);
    }
    
    /// Alter the node that the given handle corresponds to so the orientation
    /// indicated by the handle becomes the node's local forward orientation.
    /// Rewrites all edges pointing to the node and the node's sequence to
    /// reflect this. Invalidates all handles to the node (including the one
    /// passed). Returns a new, valid handle to the node in its new forward
    /// orientation. Note that it is possible for the node's ID to change.
    /// Does not update any stored paths. May change the ordering of the underlying
    /// graph.
    virtual handle_t apply_orientation(const handle_t& handle) {
        return this->get()->apply_orientation(handle);
    }
    
    /// Split a handle's underlying node at the given offsets in the handle's
    /// orientation. Returns all of the handles to the parts. Other handles to
    /// the node being split may be invalidated. The split pieces stay in the
    /// same local forward orientation as the original node, but the returned
    /// handles come in the order and orientation appropriate for the handle
    /// passed in.
    /// Updates stored paths.
    virtual std::vector<handle_t> divide_handle(const handle_t& handle, const std::vector<size_t>& offsets) {
        return this->get()->divide_handle(handle, offsets);
    }
    
    /// Adjust the representation of the graph in memory to improve performance.
    /// Optionally, allow the node IDs to be reassigned to further improve
    /// performance.
    /// Note: Ideally, this method is called one time once there is expected to be
    /// few graph modifications in the future.
    virtual void optimize(bool allow_id_reassignment = true) {
        this->get()->optimize(allow_id_reassignment);
    }

    /// Reorder the graph's internal structure to match that given.
    /// This sets the order that is used for iteration in functions like for_each_handle.
    /// Optionally may compact the id space of the graph to match the ordering, from 1->|ordering|.
    /// This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.
    virtual void apply_ordering(const std::vector<handle_t>& order, bool compact_ids = false) {
        this->get()->apply_ordering(order, compact_ids);
    }
    
    /// Set a minimum id to increment the id space by, used as a hint during construction.
    /// May have no effect on a backing implementation.
    virtual void set_id_increment(const nid_t& min_id) {
        this->get()->set_id_increment(min_id);
    }
    
    /// Add the given value to all node IDs.
    /// Has a default implementation in terms of reassign_node_ids, but can be
    /// implemented more efficiently in some graphs.
    virtual void increment_node_ids(nid_t increment){
        this->get()->increment_node_ids(increment);
    }
    
    /// This specialization for long appears to be needed to avoid confusion about nid_t
    virtual void increment_node_ids(long increment) {
        this->get()->increment_node_ids(increment);
    }
    
    /// Renumber all node IDs using the given function, which, given an old ID, returns the new ID.
    /// Modifies the graph in place. Invalidates all outstanding handles.
    /// If the graph supports paths, they also must be updated.
    /// The mapping function may return 0. In this case, the input ID will
    /// remain unchanged. The mapping function should not return any ID for
    /// which it would return 0.
    virtual void reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id) {
        this->get()->reassign_node_ids(get_new_id);
    }
    
    /**
     * Destroy the given path. Invalidates handles to the path and its steps.
     */
    virtual void destroy_path(const path_handle_t& path) {
        this->get()->destroy_path(path);
    }

    /**
     * Create a path with the given name. The caller must ensure that no path
     * with the given name exists already, or the behavior is undefined.
     * Returns a handle to the created empty path. Handles to other paths must
     * remain valid.
     */
    virtual path_handle_t create_path_handle(const std::string& name,
                                             bool is_circular = false) {
        return this->get()->create_path_handle(name, is_circular);
    }
    
    /**
     * Append a visit to a node to the given path. Returns a handle to the new
     * final step on the path which is appended. If the path is cirular, the new
     * step is placed between the steps considered "last" and "first" by the
     * method path_begin. Handles to prior steps on the path, and to other paths,
     * must remain valid.
     */
    virtual step_handle_t append_step(const path_handle_t& path, const handle_t& to_append) {
        return this->get()->append_step(path, to_append);
    }
    
    /**
     * Prepend a visit to a node to the given path. Returns a handle to the new
     * first step on the path which is appended. If the path is cirular, the new
     * step is placed between the steps considered "last" and "first" by the
     * method path_begin. Handles to later steps on the path, and to other paths,
     * must remain valid.
     */
    virtual step_handle_t prepend_step(const path_handle_t& path, const handle_t& to_prepend) {
        return this->get()->prepend_step(path, to_prepend);
    }
    
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
    virtual std::pair<step_handle_t, step_handle_t> rewrite_segment(const step_handle_t& segment_begin,
                                                                    const step_handle_t& segment_end,
                                                                    const std::vector<handle_t>& new_segment) {
        return this->get()->rewrite_segment(segment_begin, segment_end, new_segment);
    }
    
    /**
     * Make a path circular or non-circular. If the path is becoming circular, the
     * last step is joined to the first step. If the path is becoming linear, the
     * step considered "last" is unjoined from the step considered "first" according
     * to the method path_begin.
     */
    virtual void set_circularity(const path_handle_t& path, bool circular) {
        this->get()->set_circularity(path, circular);
    }
    
    /// Remove the node belonging to the given handle and all of its edges.
    /// Either destroys any paths in which the node participates, or leaves a
    /// "hidden", un-iterateable handle in the path to represent the sequence
    /// of the removed node.
    /// Invalidates the destroyed handle.
    /// May be called during serial for_each_handle iteration **ONLY** on the node being iterated.
    /// May **NOT** be called during parallel for_each_handle iteration.
    /// May **NOT** be called on the node from which edges are being followed during follow_edges.
    /// May **NOT** be called during iteration over paths, if it could destroy a path.
    /// May **NOT** be called during iteration along a path, if it could destroy that path.
    virtual void destroy_handle(const handle_t& handle) {
        this->get()->destroy_handle(handle);
    }
    
    /// Remove the edge connecting the given handles in the given order and orientations.
    /// Ignores nonexistent edges.
    /// Does not update any stored paths.
    virtual void destroy_edge(const handle_t& left, const handle_t& right) {
        this->get()->destroy_edge(left, right);
    }
    
    /// Shorten a node by truncating either the left or right side of the node, relative to the orientation
    /// of the handle, starting from a given offset along the nodes sequence. Any edges on the truncated
    /// end of the node are deleted. Returns a (possibly altered) handle to the truncated node.
    /// May invalid stored paths.
    virtual handle_t truncate_handle(const handle_t& handle, bool trunc_left, size_t offset) {
        return this->get()->truncate_handle(handle, trunc_left, offset);
    }
    
    /// Remove all nodes and edges. May also remove all paths, if applicable.
    virtual void clear() {
        this->get()->clear();
    }
};

/**
 * Defines a fully-featured GraphProxy that you can inherit from to implement
 * MutablePathDeletableHandleGraph and SerializableHandleGraph from one backing
 * object that need not implement either, as long as it satisfies the concept.
 */
template<typename BackingGraph>
struct GraphProxy : virtual public MutablePathDeletableHandleGraphProxy<BackingGraph>, virtual public SerializableHandleGraphProxy<BackingGraph> {
};

/**
 * A GraphProxy over an object at a specific address. Must not outlive the
 * backing object, and does not own it.
 */
template<typename BackingGraph>
class NonOwningGraphProxy : virtual public GraphProxy<BackingGraph> {
public:
    NonOwningGraphProxy(BackingGraph* implementation) : implementation(implementation) {
        // Nothing to do!
    }
    
    BackingGraph* get() {
        return implementation;
    }
    
    const BackingGraph* get() const {
        return implementation;
    }
    
protected:
    BackingGraph* implementation;
};


}

#endif

