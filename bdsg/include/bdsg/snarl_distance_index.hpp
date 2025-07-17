#ifndef BDSG_SNARL_DISTANCE_HPP_INCLUDED
#define BDSG_SNARL_DISTANCE_HPP_INCLUDED

//#define debug_distance_indexing
//#define count_allocations

#include <handlegraph/snarl_decomposition.hpp>
#include <handlegraph/algorithms/dijkstra.hpp>
#include <handlegraph/util.hpp>
#include <handlegraph/trivially_serializable.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <string>
#include <numeric>
#include <atomic>
#include <arpa/inet.h>

/*
TODO
For making a new method for finding distances in a snarl:

The distance index itself is just a vector of integers. The vector is split up into "records" that store information for each snarl, node, and chain.
There are different types of records depending on what information they store. Each record starts with a record_t to define what type of record it is.
The SnarlTreeRecord class can interpret a record given its index in the distance index and its record type.

To make a new method for storing distances in a snarl, you'll need to create a new type of record_t that defines your new structure
There are three components that I use to define a record 

- The static variables defining the size of the record and the offsets of each value in the record. eg. SNARL_RECORD_SIZE, SNARL_NODE_COUNT_OFFSET
- The SnarlTreeRecord(Writer) struct that can interpret the static variables to write the record and get information out of it
- The TemporaryRecord that gets filled in with all the values in the distance index before writing it in the final format

The easiest way to incorporate a new snarl record with the existing code is probably to add code to SnarlRecord and SnarlRecordWriter to be able to
write and get distances from a new type of record_t.
The difference between the current snarl record and the new one will probably only be the distance vector at the end, so you can probably just keep the 
existing static variables defining the record and rely on the record_t it stores to know how to interpret the distances.

You'll also need to add to or make a new TemporarySnarlRecord
I didn't optimize the TemporaryRecords at all so they are just structs with all possible values that might be needed by the real distance index.
They get filled by the "populate_snarl_index" function in vg: vg/src/snarl_distance_index.hpp/cpp

I currently have an "oversized snarl" implementation where the snarl just doesn't store the distances if it has more than a user-specified maximum
number of children. You can just use the same limit and build the new type of snarl instead of the oversized snarl. You might need to make sure the
distance index doesn't throw any errors when making the index though 

Be careful with the get_max_record_length function. This predicts the length of the record in the distance index for reserving memory for building
the final index. It shouldn't change the accuracy if you get it wrong but it does affect the memory use and runtime of index building 
*/

 /**
  * This defines the distance index, which also serves as a snarl tree that implements libhandlegraph's 
  * SnarlDecomposition interface
  *
  * The distance index is used to find the minimum distance between two positions on the graph
  * The structure of the distance index is based on the snarl tree. 
  * Variation graphs can be decomposed into nested substructures called "snarls" and "chains, and the 
  * decomposition is described by the snarl tree
  * The distance index provides an interface to traverse the snarl tree and to find minimum distances
  * between two sibling nodes in the snarl tree (eg between two chains that are children of the same snarl)
  * The distance index can also be built without distances and used just to traverse the snarl tree
  *
  * A chain is comprised of at least one node and may also contain snarls. 
  * Each snarl is comprised of child chains and is bounded by two boundary nodes. 
  * The boundary nodes are not part of the snarl, but are represented in the snarl as sentinels
  * Trivial snarls (with only one edge between the boundary nodes) are not represented, but a snarl may 
  * have no nodes if it has other edges
  *
  * The root of a snarl tree contains child chains. The children of the root are not necessarily connected
  * components in the graph- there may be connectivity between chains that are children of the root
  * The depth of the root is 0, the depth of its child chains is 1, the depth of the nodes and snarls that are 
  * children of those chains is also 1, and the chains that are children of those snarls have depth 2
  *
  * All structures in the snarl tree have two sides that can define the orientation of a traversal 
  * A forward traversal of a node is always based on the node's orientation 
  * A forward traversal of a snarl or chain goes from its start boundary node to its end boundary node
  * Nodes and snarls can also be oriented backwards relative to the orientation of the parent chain- 
  * if a traversal of a chain going start to end traverses a child end to start. Then the child would 
  * be considered to be reversed in its parent
  *
  */



//How to hash pairs, copied from vg
// http://stackoverflow.com/questions/4870437/pairint-int-pair-as-key-of-unordered-map-issue#comment5439557_4870467
// https://github.com/Revolutionary-Games/Thrive/blob/fd8ab943dd4ced59a8e7d1e4a7b725468b7c2557/src/util/pair_hash.h
// taken from boost
#ifndef OVERLOAD_PAIR_HASH
#define OVERLOAD_PAIR_HASH
namespace std {
namespace
{

    // Code from boost
    // Reciprocal of the golden ratio helps spread entropy
    //     and handles duplicates.
    // See Mike Seymour in magic-numbers-in-boosthash-combine:
    //     http://stackoverflow.com/questions/4948780

    template <class T>
    inline void hash_combine(size_t& seed, T const& v)
    {
        seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    // Recursive template code derived from Matthieu M.
    template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
    struct HashValueImpl
    {
        static void apply(size_t& seed, Tuple const& tuple)
        {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, std::get<Index>(tuple));
        }
    };

    template <class Tuple>
    struct HashValueImpl<Tuple,0>
    {
        static void apply(size_t& seed, Tuple const& tuple)
        {
            hash_combine(seed, std::get<0>(tuple));
        }
    };
}

template <typename A, typename B>
struct hash<pair<A,B> > {
    size_t operator()(const pair<A,B>& x) const {
        size_t hash_val = std::hash<A>()(x.first);
        hash_combine(hash_val, x.second);
        return hash_val;
    }
};

// from http://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
template <typename ... TT>
struct hash<std::tuple<TT...>>
{
    size_t
    operator()(std::tuple<TT...> const& tt) const
    {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
        return seed;
    }

};
}
#endif  // OVERLOAD_PAIR_HASH


namespace bdsg { 


using namespace sdsl;
using namespace std;
using namespace handlegraph;

/**
  * The distance index, which also acts as a snarl decomposition.
  *
  * The distance index provides an interface to traverse the snarl tree and to
  * find minimum distances between two sibling nodes in the snarl tree (eg
  * between two chains that are children of the same snarl).
  *
  * It also provides a method for quickly calculating the minimum distance
  * between two positions on the graph.
  *
  * The implementation here is tightly coupled with the filling-in code in vg
  * (see vg::fill_in_distance_index()). To make a SnarlDistanceIndex that
  * actually works, you have to construct the object, and then call
  * get_snarl_tree_records() with zero or more TemporaryDistanceIndex objects
  * for connected components, and a graph.
  *
  * The TemporaryDistanceIndex needs to have a variety of TemporaryRecord
  * implementation classes (TemporaryChainRecord, TemporarySnarlRecord,
  * TemporaryNodeRecord) set up and added to it; this all has to be done "by
  * hand", as it were, because no code is in this library to help you do it.
  *
  */
class SnarlDistanceIndex : public SnarlDecomposition, public TriviallySerializable {

public:
    ~SnarlDistanceIndex();
    SnarlDistanceIndex();

public:

    //Serialize and deserialize from TriviallySerializable

    using TriviallySerializable::serialize;
    using TriviallySerializable::deserialize;
    void dissociate();

    void serialize(const std::function<void(const void*, size_t)>& iteratee) const;
    void serialize(int fd);
    void deserialize(int fd);

    void serialize_members(std::ostream& out) const;
    void deserialize_members(std::istream& in);


    virtual uint32_t get_magic_number() const;
    std::string get_prefix() const;
    
    /// Allow for preloading the index for more accurate timing of algorithms
    /// that use it, if it fits in memory. If blocking is true, waits for the
    /// index to be paged in. Otherwise, just tells the OS that we will want to
    /// use it.
    void preload(bool blocking = false) const;


////////////////////////////////////  How we define different properties of a net handle

    public:

    ///The connectivity of a net_handle- this defines the direction that the net_handle is traversed
    enum connectivity_t { START_START=1, START_END, START_TIP, 
                            END_START, END_END, END_TIP, 
                            TIP_START, TIP_END, TIP_TIP};

    ///Type of a net_handle_t, which may not be the type of the record
    ///This is to allow a node record to be seen as a chain from the perspective of a handle
    enum net_handle_record_t {ROOT_HANDLE=0, NODE_HANDLE, SNARL_HANDLE, CHAIN_HANDLE, SENTINEL_HANDLE};

/////////////////////////////  functions for distance calculations using net_handle_t's 

public:


    ///Get the minimum distance between two positions in the graph.
    ///If unoriented_distance is true, then ignore the orientations of the positions.
    ///Otherwise, distance is calculated from the first position going forward to the second position going forward.
    ///The distance includes one of the positions; the distance from one position to itself is 0.
    ///Returns std::numeric_limits<size_t>::max() if there is no path between the two positions.
    ///
    ///distance_traceback is helping to find actual distance path. For each of the nodes, keep a vector of the ancestors of the node
    ///and the distance and direction (as + or - values of the distance) taken in the minimum distance path to get to the start and end of the parent.
    ///
    ///For example, if the first node is traversed forward and only reaches the end node of its parent chain, then the values stored will be <inf, +distance>.
    ///If it were traversed backwards to reach the start node and couldn't reach the end node, then the values stored would be <-distance, inf>.
    ///The distance value is the distance to the end of the node to the and of the chain/snarl, and distances stored are distances within the net_handle associated with it.
    ///So the first value in the vector will always be node itself and the offset in the node, and the second value will be the parent chain and the distance from the.
    ///ends of the node to the ends of the parent ( or the distance between nodes in the chain).
    ///The last value of the two nodes should match (in the absolute value at least). It will be the common ancestor node and the distances will be the distance to the 
    ///start/end of the other node with the same meaning for the sign of the distance.
    ///The hints will go up to the lowest common ancestor needed for finding the shortest distance, which may or may not be the LCA or the root.
    size_t minimum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, const handlegraph::nid_t id2, 
                            const bool rev2, const size_t offset2, bool unoriented_distance = false, const HandleGraph* graph=nullptr, 
                            pair<vector<tuple<net_handle_t, int32_t, int32_t>>, vector<tuple<net_handle_t, int32_t, int32_t>>>* distance_traceback=nullptr) const ;

    ///Find an approximation of the maximum distance between two positions. 
    ///This isn't a true maximum- the only guarantee is that it's greater than or equal to the minimum distance.
    size_t maximum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, const handlegraph::nid_t id2, 
                            const bool rev2, const size_t offset2, bool unoriented_distance = false, const HandleGraph* graph=nullptr) const ;

    //Find the distance between the two child node sides in the parent, facing each other, not 
    //including the lengths of the nodes.
    //This only takes into account the endpoint of the net_handle_t traversal, it does not care if the traversal
    //was possible. Doesn't allow you to find the distance from a traversal ending/starting in a tip.
    //requires that the children are children of the parent.
    //For chains, the distance to/from a snarl is really the distance from the outer node of the snarl.
    //Returns std::numeric_limits<size_t>::max() if there is not path between them in the parent 
    //or if they are not children of the parent.
    //Distance limit is the distance after which we give up if we're doing a traversal.
    size_t distance_in_parent(const net_handle_t& parent, const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph=nullptr, size_t distance_limit = std::numeric_limits<size_t>::max()) const;

    //Distance_in_parent for distances in a snarl given the rank and orientation instead of a handle
    //You should use distance in parent unless you're sure the ranks are correct - this shouldn't
    //be exposed to the public interface but I needed it
    size_t distance_in_snarl(const net_handle_t& parent, const size_t& rank1, const bool& right_side1, 
            const size_t& rank2, const bool& right_side2, const HandleGraph* graph=nullptr, 
            size_t distance_limit = std::numeric_limits<size_t>::max()) const;

    ///Find the maximum distance between two children in the parent. 
    ///This is the same as distance_in_parent for everything except children of chains
    size_t max_distance_in_parent(const net_handle_t& parent, const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph=nullptr, size_t distance_limit = std::numeric_limits<size_t>::max()) const;

    ///Get the distance from the child to the start or end bound of the parent.
    ///parent_and_child_types are hints to figure out the type of snarl/chain records the parent and child are.
    ///tuple of parent record type, parent handle type, child record type, child handle type.
    ///This is really just used to see if the parent and child are trivial chains, so it might not be exactly what the actual record is.
    size_t distance_to_parent_bound(const net_handle_t& parent, bool to_start, net_handle_t child,
                                    tuple<net_handle_record_t, net_handle_record_t, net_handle_record_t, net_handle_record_t> parent_and_child_types = make_tuple(ROOT_HANDLE, ROOT_HANDLE, ROOT_HANDLE, ROOT_HANDLE)) const;

    ///If this node id and orientation is pointing into a snarl, then return the start.
    ///node id and orientation pointing into the snarl, and if the snarl is trivial.
    ///Returns <0, false, false> if it doesn't point into a snarl.
    tuple<nid_t, bool, bool> into_which_snarl(const nid_t& id, const bool& reverse) const;

    
    ///Return true if child1 comes before child2 in the chain. 
    bool is_ordered_in_chain(const net_handle_t& child1, const net_handle_t& child2) const;


    bool is_externally_start_end_connected(const net_handle_t net) const {return is_externally_start_end_connected(snarl_tree_records->at(get_record_offset(net)));}
    bool is_externally_start_start_connected(const net_handle_t net) const {return is_externally_start_start_connected(snarl_tree_records->at(get_record_offset(net)));}
    bool is_externally_end_end_connected(const net_handle_t net) const {return is_externally_end_end_connected(snarl_tree_records->at(get_record_offset(net)));}


    ///For two net handles, get a net handle lowest common ancestor.
    ///If the lowest common ancestor is the root, then the two handles may be in
    ///different connected components. In this case, return false.
    pair<net_handle_t, bool> lowest_common_ancestor(const net_handle_t& net1, const net_handle_t& net2) const;


    ///Return the length of the net, which must represent a node (or sentinel of a snarl)
    size_t node_length(const net_handle_t& net) const ;


    ///This is also the length of a net, but it can also be a snarl or chain. 
    ///The length of a chain includes the boundary nodes, a snarl does not.
    ///A looping chain only includes the start/end node once
    size_t minimum_length(const net_handle_t& net) const;
    size_t maximum_length(const net_handle_t& net) const;

    ///The length of a chain. If it is a multicomponent chain, then the length of 
    ///the last component, which is used for calculating distance, instead of inf 
    size_t chain_minimum_length(const net_handle_t& net) const;

    ///What is the node id of the node represented by this net handle.
    ///net must be a node or a sentinel
    nid_t node_id(const net_handle_t& net) const ;

    ///Does the graph have this node?
    bool has_node(const nid_t id) const;

    ///Only really relevant for nodes in chains, is the node
    ///traversed backwards relative to the orientation of the chain
    bool is_reversed_in_parent(const net_handle_t& net) const;

    ///Get a net handle from a node and, optionally, an orientation
    net_handle_t get_node_net_handle(const nid_t id, bool rev = false) const;

    ///How deep is the snarl tree? The root is 0, top-level chain is 1, etc
    ///Only counts chains
    size_t get_max_tree_depth() const;

    ///What is the depth of this net handle? Nodes and snarls get the depth of their parent.
    ///The depth of the root is 0, the depth of its child chains is 1, the depth of the nodes and snarls that are 
    ///children of those chains is also 1, and the chains that are children of those snarls have depth 2
    size_t get_depth(const net_handle_t& net) const;


    //Given a handle, return a unique identifier for the connected component that it's on.
    //Connected components are based on the connectivity of the graph, so there may be fewer
    //connected components than there are root-level structures. For example, if two root-level
    //chains are connected to each other in the root, then they will be considered one connected
    //component but two separate root-level chains.
    size_t get_connected_component_number(const net_handle_t& net) const;

    ///Given the connected component number (from get_connected_component_number), get the
    ///root-level handle pointing to it.
    ///If the connected component is a root-level snarl, then this may return a "root" handle,
    ///but it will actually point to the snarl
    net_handle_t get_handle_from_connected_component(size_t num) const;


    ///Is there a path between the start and end endpoints within the net handle?
    bool has_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const ;

    ///Is there a path between the start and end endpoints outside the net handle?
    ///This is used for children of the root
    bool has_external_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const ; 


    ///Get the prefix sum value for a node in a chain.
    ///Fails if the parent of net is not a chain
    size_t get_prefix_sum_value(const net_handle_t& net) const;

    ///Get the maximum prefix sum value for a node in a chain.
    ///Fails if the parent of net is not a chain
    size_t get_max_prefix_sum_value(const net_handle_t& net) const;

    ///Get the forward loop value for a node in a chain.
    ///Fails if the parent of net is not a chain
    size_t get_forward_loop_value(const net_handle_t& net) const;

    ///Get the reverse value for a node in a chain.
    ///Fails if the parent of net is not a chain
    size_t get_reverse_loop_value(const net_handle_t& net) const;

    //If get_end is true, then get the second component of the last node in a looping chain.
    //If the chain loops, then the first and last node are the same.
    //If it is also a multicomponent, chain, then it is in two different components.
    //If get_end is true, then get the larger of the two components.
    size_t get_chain_component(const net_handle_t& net, bool get_end = false) const;



public:


////////////////// SnarlDecomposition methods

    ///Get a net handle referring to a tip-to-tip traversal of the contents of the root snarl.
    net_handle_t get_root() const ;
    
    ///Return true if the given handle refers to (a traversal of) the root
    ///snarl, and false otherwise.
    bool is_root(const net_handle_t& net) const;

    ///Return true if the given handle refers to (a traversal of) a snarl of the root,
    ///which is considered to be the root but actually refers to a subset of the children 
    ///of the root that are connected
    bool is_root_snarl(const net_handle_t& net) const;
    
    ///Returns true if the given net handle refers to (a traversal of) a snarl.
    bool is_snarl(const net_handle_t& net) const;

    ///Return true if the given snarl is a DAG and false otherwise
    ///Returns true if the given net_handle_t is not a snarl
    bool is_dag(const net_handle_t& snarl) const;

    ///Returns true if the given net handle refers to (a traversal of) a simple snarl
    ///A simple snarl is a bubble where each child node can only reach the boundary nodes,
    ///and each side of a node reaches a different boundary node
    ///There may also be an edge connecting the two boundary nodes but no additional 
    ///edges are allowed
    bool is_simple_snarl(const net_handle_t& net) const;

    ///Returns true if the given net handle refers to (a traversal of) a regular snarl
    ///A regular snarl is the same as a simple snarl, except that the children may be
    ///nested chains, rather than being restricted to nodes 
    bool is_regular_snarl(const net_handle_t& net) const;

    ///Returns true if the given net handle refers to (a traversal of) a chain.
    bool is_chain(const net_handle_t& net) const;
    ///Returns true if the given net handle refers to (a traversal of) a chain that is not start-end connected
    bool is_multicomponent_chain(const net_handle_t& net) const;

    ///Returns true if the given net handle refers to (a traversal of) a chain that loops (a chain where the first and last node are the same).
    bool is_looping_chain(const net_handle_t& net) const;

    ///Returns true if the given net handle refers to (a traversal of) a trivial chain that represents a single node.
    bool is_trivial_chain(const net_handle_t& net) const;

    ///Returns true if the given net handle refers to (a traversal of) a single node, and thus has a corresponding handle_t.
    bool is_node(const net_handle_t& net) const;

    ///Return true if the given net handle is a snarl bound sentinel (in either
    ///inward or outward orientation), and false otherwise.
    bool is_sentinel(const net_handle_t& net) const;
    
    ///Turn a handle to an oriented node into a net handle for a start-to-end or end-to-start traversal of the node, as appropriate.
    net_handle_t get_net(const handle_t& handle, const handlegraph::HandleGraph* graph) const;
    
    ///For a net handle to a traversal of a single node, get the handle for that node in the orientation it is traversed.
    ///May not be called for other net handles.
    handle_t get_handle(const net_handle_t& net, const handlegraph::HandleGraph* graph) const;
    
    ///Get the parent snarl of a chain, or the parent chain of a snarl or node.
    ///If the child is start-to-end or end-to-start, and the parent is a chain,
    ///the chain comes out facing the same way, accounting for the relative
    ///orientation of the child snarl or node in the chain. Otherwise,
    ///everything is produced as start-to-end, even if that is not actually a
    ///realizable traversal of a snarl or chain. May not be called on the root
    ///snarl.
    ///
    ///Also works on snarl boundary sentinels.
    net_handle_t get_parent(const net_handle_t& child) const;
    
    
    // We have sentinel net_handle_t values for the start/end of each snarl, so
    // that we can tell which last edge a traversal of the contents of a snarl
    // takes when we represent it as a list of net_handle_t items. We also use
    // these to query what's attached to the snarl start/end when traversing,
    // and to see self loops immediately inside the snarl. These may actually
    // just be the handles for the nodes at the start'end of the snarl with
    // special flags set.
    //
    // For chains, we use the net handles to the appropriate first/last nodes
    // in the appropriate orientation.
    // The chain bound of a looping chain will have connectivity with the same
    // start/end point when it is for the end of the chain; the connectivity
    // will be start-start if it is going backwards in the node, and end-end
    // if it is going forwards
    
    ///Get the bounding handle for the snarl or chain referenced by the given
    ///net handle, getting the start or end facing in or out as appropriate.
    ///
    ///For snarls, returns the bounding sentinel net handles. For chains,
    ///returns net handles for traversals of the bounding nodes of the chain.
    ///If the chain is a looping chain, then the start and end of the chain
    ///are the same, so the connectivity of the bound indicates which we're
    ///looking at; the connectivity will be start-start if it is going 
    ///backwards in the node, and end-end if it is going forwards.
    ///
    ///Ignores traversal type.
    ///
    ///May not be called on traversals of individual nodes.
    net_handle_t get_bound(const net_handle_t& snarl, bool get_end, bool face_in) const;

    ///Given the sentinel of a snarl, return a handle to the node representing it
    net_handle_t get_node_from_sentinel(const net_handle_t& sentinel) const;

    
    ///Return a net handle to the same snarl/chain/node in the opposite orientation.
    ///No effect on tip-to-tip, start-to-start, or end-to-end net handles. Flips all the others.
    net_handle_t flip(const net_handle_t& net) const;
    
    ///Get a canonical traversal handle from any net handle. All handles to the
    ///same net graph element have the same canonical traversal. That canonical
    ///traversal must be realizable, and might not always be start-to-end or
    ///even consistently be the same kind of traversal for different snarls,
    ///chains, or nodes. Mostly useful to normalize for equality comparisons.
    ///
    ///Any root snarl will become just the root
    ///Anything without connectivity will get START_END
    net_handle_t canonical(const net_handle_t& net) const;

    ///Makes a start-end traversal of the net.
    ///Faster than canonical because it doesn't check the index for anything 
    net_handle_t start_end_traversal_of(const net_handle_t& net) const;

    ///Return the kind of location at which the given traversal starts.
    endpoint_t starts_at(const net_handle_t& traversal) const;
    
    ///Return the kind of location at which the given traversal ends.
    endpoint_t ends_at(const net_handle_t& traversal) const;

    ///For a child of a snarl, the rank is used to calculate the distance
    size_t get_rank_in_parent(const net_handle_t& net) const;

    ///How many connected components are in this graph?
    ///This returns the number of topological connected components, not necessarily the 
    ///number of nodes in the top-level snarl 
    size_t connected_component_count() const;

    ///Get the child of a snarl from its rank. This shouldn't be exposed to the public interface but I need it
    ///Please don't use it
    ///For 0 or 1, returns the sentinel facing in. Otherwise return the child as a chain going START_END
    net_handle_t get_snarl_child_from_rank(const net_handle_t& snarl, const size_t& rank) const;

    /// Does this net handle store distances?
    bool has_distances(const net_handle_t& net) const;
    /// Does the distance index in general store distances?
    bool has_distances() const;

protected:
    ///Internal implementation for for_each_child.
    bool for_each_child_impl(const net_handle_t& traversal, const std::function<bool(const net_handle_t&)>& iteratee) const;

    ///Internal implementation for for_each_traversal.
    bool for_each_traversal_impl(const net_handle_t& item, const std::function<bool(const net_handle_t&)>& iteratee) const;

    ///Internal implementation for follow_net_edges.
    ///if go_left is true, then go out  from the start of the traversal of here, otherwise go out from
    ///the end of the traversal 
    bool follow_net_edges_impl(const net_handle_t& here, const handlegraph::HandleGraph* graph, bool go_left, const std::function<bool(const net_handle_t&)>& iteratee) const;

public:


    ///Get a net handle for traversals of a snarl or chain that contains
    ///the given oriented bounding node traversals or sentinels. Given two
    ///sentinels for a snarl, produces a net handle to a start-to-end,
    ///end-to-end, end-to-start, or start-to-start traversal of that snarl.
    ///Given handles to traversals of the bounding nodes of a chain, similarly
    ///produces a net handle to a traversal of the chain.
    ///
    ///For a chain, either or both handles can also be a snarl containing tips,
    ///for a tip-to-start, tip-to-end, start-to-tip, end-to-tip, or tip-to-tip
    ///traversal. Similarly, for a snarl, either or both handles can be a chain
    ///in the snarl that contains internal tips, or that has no edges on the
    ///appropriate end.
    ///
    ///May only be called if a path actually exists between the given start
    ///and end.
    net_handle_t get_parent_traversal(const net_handle_t& traversal_start, const net_handle_t& traversal_end) const;


private:

    ///Function to walk through the shortest path between the two nodes+orientations. Orientation is the same as for minimum_distance - 
    ///traverses from the first node going forward to the second node going forward.
    ///Calls iteratee on each node of the shortest path between the nodes and the distance to the start of that node
    void for_each_handle_in_shortest_path(const handlegraph::nid_t id1, const bool rev1, const handlegraph::nid_t id2, const bool rev2, 
                                          const HandleGraph* graph, const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee) const;

    ///Helper function for recursively traversing the shortest path in a snarl.
    ///start and end must be children (or sentinels) of the snarl.
    ///The distance found will traverse start going forward and reach end going forward.
    ///doesn't call iteratee on start or end.
    ///If to_duplicate isn't the nullptr, then keep a list of everything traversed (flipped) and its length
    void for_each_handle_in_shortest_path_in_snarl(const net_handle_t& snarl_handle, net_handle_t start, net_handle_t end,
                                          size_t distance_to_traverse, size_t& distance_traversed, const HandleGraph* graph, 
                                          const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee,
                                          vector<pair<net_handle_t, size_t>>* to_duplicate) const;
    ///Helper function for recursively traversing the shortest path in a chain.
    ///start and end must be children of the chain.
    ///iteratee is not run on start or end.
    void for_each_handle_in_shortest_path_in_chain(const net_handle_t& chain_handle, net_handle_t start, net_handle_t end,
                                          size_t distance_to_traverse, size_t& distance_traversed, const HandleGraph* graph, 
                                          const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee,
                                          vector<pair<net_handle_t, size_t>>* to_duplicate) const;


////////////////////////////// How to interpret net_handle_ts
//
public:

    ///A record_t is the type of structure that a record can be.
    /// The actual distance index is stored as a series of "records" for each snarl/node/chain. 
    /// The record type defines what is stored in a record
    ///
    ///NODE, SNARL, and CHAIN indicate that they don't store distances.
    ///SIMPLE_SNARL is a snarl with all children connecting only to the boundary nodes in one direction (ie, a bubble).
    ///TRIVIAL_SNARL represents consecutive nodes in a chain. 
    ///NODE represents a node that is a trivial chain. A node can only be the child of a snarl.
    ///OVERSIZED_SNARL only stores distances to the boundaries.
    ///ROOT_SNARL represents a connected component of the root. It has no start or end node so 
    ///   its children technically belong to the root.
    ///MULTICOMPONENT_CHAIN can represent a chain with snarls that are not start-end connected.
    ///    The chain is split up into components between these snarls, each node is tagged with
    ///    which component it belongs to.
    ///
    enum record_t {ROOT=1, 
                   NODE, DISTANCED_NODE, 
                   TRIVIAL_SNARL, DISTANCED_TRIVIAL_SNARL,
                   SIMPLE_SNARL, DISTANCED_SIMPLE_SNARL,
                   SNARL, DISTANCED_SNARL,  OVERSIZED_SNARL, 
                   ROOT_SNARL, DISTANCED_ROOT_SNARL,
                   CHAIN, DISTANCED_CHAIN, MULTICOMPONENT_CHAIN,
                   CHILDREN};
    const static bool has_distances(record_t type) {
        return type == DISTANCED_NODE || type == DISTANCED_TRIVIAL_SNARL || type == DISTANCED_SIMPLE_SNARL
            || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || type == DISTANCED_ROOT_SNARL 
            || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN;
    }


    
    ///Given the type of the record, return the handle type. Some record types can represent multiple things,
    ///for example a simple snarl record is used to represent a snarl, and the nodes/trivial chains in it.
    ///This will return whatever is higher on the snarl tree. A simple snarl will be considered a snarl,
    ///a root snarl will be considered a root, etc
    const static net_handle_record_t get_record_handle_type(record_t type) {
        if (type == ROOT || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
            return ROOT_HANDLE;
        } else if (type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
            return NODE_HANDLE;
        } else if (type == SNARL || type == DISTANCED_SNARL || type ==  SIMPLE_SNARL ||type ==  OVERSIZED_SNARL 
                 || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL){
            return SNARL_HANDLE;
        } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN) {
            return CHAIN_HANDLE;
        } else {
            throw runtime_error("error: trying to get the handle type of a list of children");
        }
    }

public:
/*
 * These are used to actually get values from a net_handle_t, which is just an int.
 * Last 3 bits are the net_handle_record_t, next 4 are the connectivity_t, next 8 are the node offset, 
 * last are the offset into snarl_tree_records.
 * 
 * For sentinel net_handle_t's, which represent the boundaries of snarls (just inside of the boundary nodes),
 * The record points to the snarl containing them, and the connectivity indicates which bound 
 * we're looking at (START_END for start in, START_START for start out, etc)
 */


    ///The offset into records that this handle points to
    const static size_t get_record_offset (const handlegraph::net_handle_t& net_handle) {
        return handlegraph::as_integer(net_handle) >> 15;
    }
    ///The offset of a node in a trivial snarl (0 if it isn't a node in a trivial snarl)
    const static size_t get_node_record_offset (const handlegraph::net_handle_t& net_handle) {
        return (handlegraph::as_integer(net_handle) >> 7 ) & MAX_TRIVIAL_SNARL_NODE_COUNT; //Get 8 bits after last 7
    }
    const static connectivity_t get_connectivity (const handlegraph::net_handle_t& net_handle){
        size_t connectivity_as_int = (handlegraph::as_integer(net_handle)>>3) & 15; //Get 4 bits after last 3
        assert (connectivity_as_int <= 9);
        return static_cast<connectivity_t>(connectivity_as_int);
    }
    const static net_handle_record_t get_handle_type (const handlegraph::net_handle_t& net_handle) {
        size_t record_type = handlegraph::as_integer(net_handle) & 7; //Get last 3 bits
        assert (record_type <= 4);
        return static_cast<net_handle_record_t>(record_type);
    }

    const static handlegraph::net_handle_t get_net_handle_from_values(size_t pointer, connectivity_t connectivity, 
                                                                      net_handle_record_t type, size_t node_offset=0) {
        if (pointer > ((size_t)1 << (64-BITS_FOR_TRIVIAL_NODE_OFFSET-3-4))-1) {
            throw runtime_error("error: don't have space in net handle for record offset");
        }
        net_handle_t handle =  as_net_handle((((((pointer << BITS_FOR_TRIVIAL_NODE_OFFSET) | node_offset) << 4) 
                                                  | connectivity)<<3) | type); 
        return handle;
    
    }
    handlegraph::net_handle_t get_net_handle(size_t pointer, connectivity_t connectivity) const  {
        net_handle_record_t type = SnarlTreeRecord(pointer, &snarl_tree_records).get_record_handle_type(); 
        size_t node_record_offset = SnarlTreeRecord(pointer, &snarl_tree_records).get_record_type() == SIMPLE_SNARL || 
                                    SnarlTreeRecord(pointer, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ? 1 : 0;
        return get_net_handle_from_values(pointer, connectivity, type, node_record_offset); 
    
    }
    handlegraph::net_handle_t get_net_handle(size_t pointer) const  {
        net_handle_record_t type = SnarlTreeRecord(pointer, &snarl_tree_records).get_record_handle_type(); 
        return get_net_handle_from_values(pointer, START_END, type); 
    
    }


    ///Get the offset into snarl_tree_records for the pointer to a node record.
    const static size_t get_node_pointer_offset (const handlegraph::nid_t& id, const handlegraph::nid_t& min_node_id, size_t component_count) {
        size_t node_records_offset = component_count + ROOT_RECORD_SIZE; 
        size_t offset = (id-min_node_id)*2;
        return node_records_offset + offset; 
    }
    //handlegraph::nid_t get_node_id_from_offset(size_t offset) const {
    //    size_t min_node_id = snarl_tree_records->at(MIN_NODE_ID_OFFSET);
    //    size_t node_records_offset = snarl_tree_records->at(COMPONENT_COUNT_OFFSET) + ROOT_RECORD_SIZE; 
    //    return ((offset-node_records_offset) / NODE_RECORD_SIZE) + min_node_id;
    //}

    const static connectivity_t endpoints_to_connectivity(endpoint_t start, endpoint_t end) {
        if (start == START && end == START) {
            return START_START;
        } else if (start == START && end == END) {
            return START_END;
        } else if (start == START && end == TIP) {
            return START_TIP;
        } else if (start == END && end == START) {
            return END_START;
        } else if (start == END && end == END) {
            return END_END;
        } else if (start == END && end == TIP) {
            return END_TIP;
        } else if (start == TIP && end == START) {
            return TIP_START;
        } else if (start == TIP && end == END) {
            return TIP_END;
        } else if (start == TIP && end == TIP) {
            return TIP_TIP;
        } else {
            throw runtime_error("error: invalid endpoints");
        }
    }
    const static endpoint_t get_start_endpoint(connectivity_t connectivity) {
        endpoint_t start_endpoint;
        if (connectivity == START_START || connectivity == START_END || connectivity == START_TIP){
            start_endpoint = START;
        } else if (connectivity == END_START || connectivity == END_END || connectivity == END_TIP){
            start_endpoint = END;
        } else if (connectivity == TIP_START || connectivity == TIP_END || connectivity == TIP_TIP){
            start_endpoint = TIP;
        } else {
            throw runtime_error("error: invalid connectivity");
        }
        return start_endpoint;
    }
    const static endpoint_t get_start_endpoint(net_handle_t net) {
        return get_start_endpoint(get_connectivity(net));
    }
    const static endpoint_t get_end_endpoint(connectivity_t connectivity) {
        endpoint_t end_endpoint;
        if (connectivity == START_START || connectivity == END_START || connectivity == TIP_START){
            end_endpoint = START;
        } else if (connectivity == START_END || connectivity == END_END || connectivity == TIP_END){
            end_endpoint = END;
        } else if (connectivity == START_TIP || connectivity == END_TIP || connectivity == TIP_TIP){
            end_endpoint = TIP;
        } else {
            throw runtime_error("error: invalid connectivity");
        }
        return end_endpoint;
    }
    const static endpoint_t get_end_endpoint(const net_handle_t& net) {
        return get_end_endpoint(get_connectivity(net));
    }
    const static pair<endpoint_t, endpoint_t> connectivity_to_endpoints(const connectivity_t& connectivity) {
        return make_pair(get_start_endpoint(connectivity), get_end_endpoint(connectivity));
    }




    
//////////////////////////////////////////  The actual distance index
    
private:

    ///This vector is the entire distance index. It is split up into "records" that are defined below
    bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector> snarl_tree_records;

/*
 * These are used to interpret snarl_tree_records, which is just a vector of ints.
 * Uses a SnarlTreeRecord as the main class for defining and interpreting the records.
 * SnarlTreeRecords are given a base pointer, which points to the start of the record
 * Each record starts with a "tag", which defines which type of record it is. The 
 * contents of the record are defined below: 
*/
    
    /*Root record
    
     * - The (single) root vector has the format:
     *   [root tag, # connected components (N), # nodes (M), min_node_id, max depth [pointer to node/snarl/chain record] x N], [pointer to node+ node offset] x M]
     *   The root vector stores the root of every connected component, which can be a 
     *   node, snarl, or chain
     */
    const static size_t ROOT_RECORD_SIZE = 5;
    const static size_t COMPONENT_COUNT_OFFSET = 1;
    const static size_t NODE_COUNT_OFFSET = 2;
    const static size_t MIN_NODE_ID_OFFSET = 3;
    const static size_t MAX_TREE_DEPTH_OFFSET = 4;

    /*Node record
     * - A node record for nodes in snarls/roots. These are interpreted as either trivial chains or nodes.
     *   These will be interspersed between chains more or less based on where they are in the snarl tree
     *   [node tag, node id, pointer to parent, node length, rank in parent, distances to snarl bounds(x4)]
     */
    const static size_t NODE_RECORD_SIZE = 9;
    const static size_t NODE_ID_OFFSET = 1;
    const static size_t NODE_PARENT_OFFSET = 2;
    const static size_t NODE_LENGTH_OFFSET = 3;
    const static size_t NODE_RANK_OFFSET = 4;
    //The distances from the left or right of the node to the left or right of its parent snarl (or 0 if its parent is a root)
    //As with other distances, store 0 for inf, value +1 for everything else
    const static size_t NODE_DISTANCE_LEFT_START_OFFSET = 5;
    const static size_t NODE_DISTANCE_RIGHT_START_OFFSET = 6;
    const static size_t NODE_DISTANCE_LEFT_END_OFFSET = 7;
    const static size_t NODE_DISTANCE_RIGHT_END_OFFSET = 8;
 
    /*Chain record

     * - A chain record for each chain, which contains interspersed node and snarl records:
     *   The nodes are all stored in a "TrivialSnarl", which is a bunch of nodes with no snarls between them
     *   [chain tag, #nodes, pointer to parent, min length, max length, rank in parent, start, end, pointer to last child, depth, distances to snarl bounds (x4),
     *       [ (trivial nodes), trivial_nodes_size, snarl_record_size, (snarl record),] x N] 
     *    snarl_record_size is the number of things in the snarl record (so 0 if there is no snarl there)
     *    It is necessary for traversing the chain, so we know how many entries to skip when looking for the next child
     *    start/end include the orientations
     *
     * If this is a multicomponent chain, then the actual min length is 0, but this will store the length of the last component 
     * since it is the only length that matters when looping around the outside of the chain
     */ 
    const static size_t CHAIN_RECORD_SIZE = 14;
    const static size_t CHAIN_NODE_COUNT_OFFSET = 1;
    const static size_t CHAIN_PARENT_OFFSET = 2;
    const static size_t CHAIN_MIN_LENGTH_OFFSET = 3;     
    const static size_t CHAIN_MAX_LENGTH_OFFSET = 4;
    const static size_t CHAIN_RANK_OFFSET = 5;
    const static size_t CHAIN_START_NODE_OFFSET = 6;
    const static size_t CHAIN_END_NODE_OFFSET = 7;
    const static size_t CHAIN_LAST_CHILD_OFFSET = 8; //The offset of the last thing in the chain - node or (if looping chain) snarl
    const static size_t CHAIN_DEPTH_OFFSET = 9;
    //DIstances from the left/right of the chain to the left/right of its parent
    const static size_t CHAIN_DISTANCE_LEFT_START_OFFSET = 10;
    const static size_t CHAIN_DISTANCE_RIGHT_START_OFFSET = 11;
    const static size_t CHAIN_DISTANCE_LEFT_END_OFFSET = 12;
    const static size_t CHAIN_DISTANCE_RIGHT_END_OFFSET = 13;

    /*Trivial snarl record (which occurs within a chain) representing nodes in a chain
     * These contain up to 128 nodes with nothing between them
     * TODO: There isn't really a good reason why they only contain up to 128 nodes- it's just because I was having unrelated problems when I wrote it and I thought it might help and never undid it

     *   [trivial snarl tag, pointer to parent, node count, prefix sum, fd loop, rev loop, component]

     * The record is followed by [node id+orientation, right prefix sum] for each node in the trivial snarl
     * So the total length of the distanced trivial snarl is 8+2*#nodes, and the length of a distanceless
     * trivial snarl is 8+#nodes
     * The right prefix sum is the sum from the start of the trivial chain to the right side of the node (relative to the chain)
     * The node_record_offset in a net_handle_t to a trivial snarl points to a node in the trivial snarl
 
     */
    const static size_t BITS_FOR_TRIVIAL_NODE_OFFSET = 8;
    const static size_t MAX_TRIVIAL_SNARL_NODE_COUNT =  (1 << BITS_FOR_TRIVIAL_NODE_OFFSET) -1;
    const static size_t DISTANCED_TRIVIAL_SNARL_RECORD_SIZE = 8;
    const static size_t DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE = 3;
    const static size_t TRIVIAL_SNARL_PARENT_OFFSET = 1;
    const static size_t TRIVIAL_SNARL_NODE_COUNT_OFFSET = 2;
    const static size_t TRIVIAL_SNARL_PREFIX_SUM_OFFSET = 3;
    const static size_t TRIVIAL_SNARL_MAX_PREFIX_SUM_OFFSET = 4;
    const static size_t TRIVIAL_SNARL_FORWARD_LOOP_OFFSET = 5;
    const static size_t TRIVIAL_SNARL_REVERSE_LOOP_OFFSET = 6;
    const static size_t TRIVIAL_SNARL_COMPONENT_OFFSET = 7;
   
    /*Snarl record (which occurs within a chain)
     * 
     * - A snarl record for each snarl, which are stuck in chains
     *   [snarl tag, # nodes, pointer to parent, min length, max length, rank in parent, 
     *      pointer to children (in child vector), distance vector]
     *   The rank of the start node will always be 0 and the end node rank will be 1
     *   start/end are the start and end nodes, include the orientations
     *   For the first and last nodes, we only care about the node sides pointing in
     *   Each node side in the actual distance matrix will be 2*(rank-1) for the left side, and 
     *   2rank+1 for the right side, and 0 for the start, 1 for the end, where we only keep the 
     *   inner node side of the start and end
     *   Distances to the bounds of the snarl are stored by the children themselves, not the snarl.
     *   Node count is the number of nodes, not including boundary nodes
     */
    const static size_t SNARL_RECORD_SIZE = 8;
    const static size_t SNARL_NODE_COUNT_OFFSET = 1;
    const static size_t SNARL_PARENT_OFFSET = 2;
    const static size_t SNARL_MIN_LENGTH_OFFSET = 3;
    const static size_t SNARL_MAX_LENGTH_OFFSET = 4;
    const static size_t SNARL_DISTANCE_START_START_OFFSET = 5;
    const static size_t SNARL_DISTANCE_END_END_OFFSET = 6;
    const static size_t SNARL_CHILD_RECORD_OFFSET = 7;

    /*A simple snarl for bubbles with only nodes with two edges, one to each bound
     * [simple snarl tag, node count+length, parent, [node id, node length]xN
    */
    const static size_t SIMPLE_SNARL_RECORD_SIZE = 3;
    //This one stores the node count and min and max lengths of the snarl
    //It'll take 26 bits: 11 for each length
    //TODO: This is only for nodes, so it assumes a maximum node length of 1024
    const static size_t SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET = 1;
    const static size_t SIMPLE_SNARL_PARENT_OFFSET = 2;

     /*  At the end is the (single) child vector, listing children in snarls
     *   [child vector tag, (pointer to records) x N
     *   Each snarl will have a pointer into here, and will also know how many children it has
     *   This is at the end of the index because it is only really used when looking for all children
     *   of a snarl. This isn't done that often, and will probably be pretty slow anyways,
     *   so I didn't want to waste space in the actual snarl record.
     */ 

private:
    /*Give each of the enum types a name for printing */
    vector<std::string> record_t_as_string = {"ROOT", "NODE", "DISTANCED_NODE", 
                     "TRIVIAL_SNARL", "DISTANCED_TRIVIAL_SNARL",
                     "SNARL", "DISTANCED_SNARL", "SIMPLE_SNARL", "OVERSIZED_SNARL", 
                     "ROOT_SNARL", "DISTANCED_ROOT_SNARL",
                     "CHAIN", "DISTANCED_CHAIN", "MULTICOMPONENT_CHAIN",
                     "CHILDREN"};
    vector<std::string> connectivity_t_as_string = { "START_START", "START_END", "START_TIP", 
                            "END_START", "END_END", "END_TIP", 
                            "TIP_START", "TIP_END", "TIP_TIP"};
    vector<std::string> net_handle_record_t_string = {"ROOT_HANDLE", "NODE_HANDLE", "SNARL_HANDLE", 
                                                "CHAIN_HANDLE", "SENTINEL_HANDLE"};


    /* If this is 0, then don't store distances.
     * Otherwise, for snarls with more children than snarl_size_limit, only store the distances
     * that include boundary nodes (OVERSIZED_SNARL)
     */
    size_t snarl_size_limit = 5000;

    //If this is true, then only store distance along top-level chains. Everything still needs its minimum lengths to get
    //the distances along top-level chains but don't store internal distances in snarls or in nested chains
    //This overrides snarl_size_limit
    bool only_top_level_chain_distances=false;
    static const int max_num_size_limit_warnings = 100;
    std::atomic<int> size_limit_warnings{0};
    static const uint32_t magic_number = 1738636486;
    
public:
    void set_snarl_size_limit (size_t size) {snarl_size_limit=size;}
    void set_only_top_level_chain_distances (bool only_chain) {only_top_level_chain_distances=only_chain;}




private:
    /*
     *
     * The "tags" for defining what kind of record we're looking at. These are the first entry in any 
     * record. They will be a record_t and a bit vector indicating connectivity.
     * The bit vector will be the last 6 bits of the tag
     * 
     * Each bit represents one type of connectivity:
     * start-start, start-end, start-tip, end-end, end-tip, tip-tip
     * 
     * The remainder of the tag will be the record_t of the record
     */
    /////////// Methods for interpreting the tags for each snarl tree record

    const static record_t get_record_type(const size_t tag) {return static_cast<record_t>(tag >> 9);}

    const static bool is_start_start_connected(const size_t tag) {return tag & 32;}
    const static bool is_start_end_connected(const size_t tag)   {return tag & 16;}
    const static bool is_start_tip_connected(const size_t tag)   {return tag & 8;}
    const static bool is_end_end_connected(const size_t tag)     {return tag & 4;}
    const static bool is_end_tip_connected(const size_t tag)     {return tag & 2;}
    const static bool is_tip_tip_connected(const size_t tag)     {return tag & 1;}

    //And the external connectivity. This is only relevant for root-level structures
    //since it would otherwise be captured by the containing snarl
    const static bool is_externally_start_end_connected(const size_t tag) {return tag & 64;}
    const static bool is_externally_start_start_connected(const size_t tag) {return tag & 128;}
    const static bool is_externally_end_end_connected(const size_t tag) {return tag & 256;}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  SnarlTreeRecord class for interpreting the records in a distance index
//
/* Define a struct for interpreting each type of snarl tree node record (For node, snarl, chain)
 *
 * This is meant to be a layer in between snarl_tree_records and the public interface.
 * Each net_handle_t has a pointer to the start of a record in snarl_tree_records. 
 * SnarlTreeRecord keeps the pointer and interprets the values stored in the record .
 *
 * SnarlTreeRecordWriter does the same thing but for writing values to the index.
 *
 */
    struct SnarlTreeRecord {


        //The offset of the start of this record in snarl_tree_records
        size_t record_offset;
        const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records;

        //Constructors assuming that this record already exists
        SnarlTreeRecord(){};
        SnarlTreeRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        SnarlTreeRecord (const net_handle_t& net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        size_t get_offset() { return record_offset; }

        //What type of snarl tree node is this?
        //This will be the first value of any record
        record_t get_record_type() const {return SnarlDistanceIndex::get_record_type((*records)->at(record_offset));}

        //The name is a bit misleading, it is the handle type that the record thinks it is, 
        //not necessarily the record type of the net_handle_t that was used to produce it
        net_handle_record_t get_record_handle_type() const {
            return SnarlDistanceIndex::get_record_handle_type(get_record_type());
        }


        //Get the internal connectivity of the structure
        bool is_start_start_connected() const {return SnarlDistanceIndex::is_start_start_connected((*records)->at(record_offset));}
        bool is_start_end_connected() const {return SnarlDistanceIndex::is_start_end_connected((*records)->at(record_offset));}
        bool is_start_tip_connected() const {return SnarlDistanceIndex::is_start_tip_connected((*records)->at(record_offset));}
        bool is_end_end_connected() const {return SnarlDistanceIndex::is_end_end_connected((*records)->at(record_offset));}
        bool is_end_tip_connected() const {return SnarlDistanceIndex::is_end_tip_connected((*records)->at(record_offset));}
        bool is_tip_tip_connected() const {return SnarlDistanceIndex::is_tip_tip_connected((*records)->at(record_offset));}

        //And the external connectivity. This is only relevant for root-level structures
        //since it would otherwise be captured by the containing snarl
        bool is_externally_start_end_connected() const {return SnarlDistanceIndex::is_externally_start_end_connected((*records)->at(record_offset));}
        bool is_externally_start_start_connected() const {return SnarlDistanceIndex::is_externally_start_start_connected((*records)->at(record_offset));}
        bool is_externally_end_end_connected() const {return SnarlDistanceIndex::is_externally_end_end_connected((*records)->at(record_offset));}

        bool has_connectivity(connectivity_t connectivity) const;
        bool has_connectivity(endpoint_t start, endpoint_t end);

        //Get a pointer to this node's parent, including its orientation
        size_t get_parent_record_offset() const;

        //Get the minimum length (distance from start to end, including boundaries for 
        // chains but not snarls, just node length for nodes)
        size_t get_min_length() const;
        
        //Get this node's maximum length.
        //This isn't actually a maximum, it's the maximum among minimum distance paths 
        //through each node in the snarl/chain
        size_t get_max_length() const;

        //Get this structure's rank in its parent.
        //For children of snarls, this means the actual rank.
        //For children of chains, it points to the node in the chain
        size_t get_rank_in_parent() const;

        //Is this node reversed in its parent
        bool get_is_reversed_in_parent() const;

        //Get the node id of the start/end of this structure (start node of a snarl/chain)
        handlegraph::nid_t get_start_id() const; 
        //True if the node is traversed backwards to enter the structure
        bool get_start_orientation() const;
        handlegraph::nid_t get_end_id() const;
        //Return true if the end node is traversed backwards to leave the snarl
        handlegraph::nid_t get_end_orientation() const;

    };

    //Record interpreter that has a non-const reference to snarl_tree_records, so it can
    //also add things
    struct SnarlTreeRecordWriter {

        //and does basically the same thing but doesn't inherit from it
        size_t record_offset;
        bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records;

        //Constructors assuming that this record already exists
        SnarlTreeRecordWriter() {};
        SnarlTreeRecordWriter (size_t pointer, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        SnarlTreeRecordWriter (const net_handle_t& net, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        //What type of snarl tree node is this?
        //This will be the first value of any record
        record_t get_record_type() const;
        void set_start_start_connected();
        void set_start_end_connected();
        void set_start_tip_connected();
        void set_end_end_connected();
        void set_end_tip_connected();
        void set_tip_tip_connected();
        void set_externally_start_end_connected();
        void set_externally_start_start_connected() const;
        void set_externally_end_end_connected() const;
        void set_record_type(record_t type);
        void set_min_length(size_t length);
        void set_max_length(size_t length);
        void set_rank_in_parent(size_t rank);
        void set_is_reversed_in_parent(bool rev);
        void set_parent_record_offset(size_t pointer);
        //Rev is true if the node is traversed backwards to enter the snarl
        void set_start_node(handlegraph::nid_t id, bool rev); 
        //Rev is true if the node is traversed backwards to leave the snarl
        void set_end_node(handlegraph::nid_t id, bool rev) const;
    };

    struct RootRecord : SnarlTreeRecord {

        RootRecord (){};
        RootRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        RootRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        size_t get_connected_component_count() const {return (*records)->at(record_offset+COMPONENT_COUNT_OFFSET);}
        size_t get_node_count() const {return (*records)->at(record_offset+NODE_COUNT_OFFSET);}
        size_t get_max_tree_depth() const {return (*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET);}
        size_t get_min_node_id() const {return (*records)->at(record_offset+MIN_NODE_ID_OFFSET);}
        SnarlTreeRecord get_component_record(size_t component_number) const {return SnarlTreeRecord((*records)->at(record_offset+2+component_number), records);}
        bool for_each_child(const std::function<bool(const handlegraph::net_handle_t&)>& iteratee) const;

    };

    struct RootRecordWriter : RootRecord, SnarlTreeRecordWriter {
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::records;
        using SnarlTreeRecordWriter::get_record_type;

        //Constructor meant for creating a new record, at the end of snarl_tree_records
        RootRecordWriter (size_t pointer, size_t connected_component_count, size_t node_count, size_t max_tree_depth, 
                    handlegraph::nid_t min_node_id, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records);

        void set_connected_component_count(size_t connected_component_count);
        void set_node_count(size_t node_count);
        void set_max_tree_depth(size_t tree_depth);
        void set_min_node_id(handlegraph::nid_t node_id);
        void add_component(size_t index, size_t offset);
    };
    struct NodeRecord : SnarlTreeRecord {

        NodeRecord() {};
        NodeRecord (size_t pointer, size_t node_offset, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        NodeRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        handlegraph::nid_t get_node_id() const;

        size_t get_node_length() const;

        //Get the distance from left/right of the node to start/end of the parent
        size_t get_distance_left_start();
        size_t get_distance_right_start();
        size_t get_distance_left_end();
        size_t get_distance_right_end();

    };

    struct NodeRecordWriter : NodeRecord , SnarlTreeRecordWriter {
        using SnarlTreeRecordWriter::get_record_type;
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::records;


        //Constructor meant for creating a new record, at the end of snarl_tree_records
        //The memory for all nodes has already been allocated by the root
        NodeRecordWriter (size_t pointer, size_t node_offset, record_t type, 
            bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* snarl_records, nid_t node_id);

        void set_node_id(nid_t value);
        void set_rank_in_parent(size_t value);
        void set_node_length(size_t value);

        //Set the distance from left/right of the node to start/end of the parent
        void set_distance_left_start(size_t distance);
        void set_distance_right_start(size_t distance);
        void set_distance_left_end(size_t distance);
        void set_distance_right_end(size_t distance);
    };

    struct TrivialSnarlRecord : SnarlTreeRecord {

        TrivialSnarlRecord() {};

        size_t get_node_count() const;

        //Returns the prefix sum, forward loop, reverse loop, and component.
        //The component will be 0 for the first/last node of a looping chain
        //Node ranks are from get_node_record_offset(net_handle_T)
        tuple<size_t, size_t, size_t, size_t> get_chain_values(size_t node_rank) const;
        size_t get_prefix_sum(size_t node_rank) const;
        size_t get_max_prefix_sum(size_t node_rank) const;
        size_t get_forward_loop(size_t node_rank) const ;
        size_t get_reverse_loop(size_t node_rank) const;

        //If the chain loops, then the first and last node are the same.
        //If it is also a multicomponent, chain, then it is in two different components.
        //If get_end is true, then get the larger of the two components.
        size_t get_chain_component(size_t node_rank, bool get_end=false) const;
        nid_t get_node_id(size_t node_rank) const; 
        size_t get_node_length(size_t node_rank) const; 
        size_t get_rank_in_parent(size_t node_rank) const; 
        bool get_is_reversed_in_parent(size_t node_rank) const; //is the node_rank-th node reversed

        size_t get_record_size() { 
            return get_record_type() == DISTANCED_TRIVIAL_SNARL
                   ? DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (get_node_count() * 2)
                   : DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE + get_node_count();
        }
        TrivialSnarlRecord (size_t offset, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
    };

    //For constructing a trivial snarl, we only build the snarl itself, and 
    //we don't initially know how many nodes it contains
    //
   
    struct TrivialSnarlRecordWriter : TrivialSnarlRecord, SnarlTreeRecordWriter {
        using SnarlTreeRecordWriter::get_record_type;
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::records;

        void set_prefix_sum(size_t value) const;
        void set_max_prefix_sum(size_t value) const;
        void set_forward_loop(size_t value) const;
        void set_reverse_loop(size_t value) const;
        void set_chain_component(size_t value) const;
        void set_node_count(size_t value) const;


        //Constructor meant for creating a new record, at the end of snarl_tree_records
        //New record is true if this is the first time we're making this trivial snarl, false if we
        //just need the constructor to add a new node
        TrivialSnarlRecordWriter (size_t pointer, record_t type, 
            bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* snarl_records, bool new_record);
    };

    struct SnarlRecord : SnarlTreeRecord {


        SnarlRecord() {};
        SnarlRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        SnarlRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        //How big is the entire snarl record?
        static size_t distance_vector_size(record_t type, size_t node_count);
        static size_t record_size (record_t type, size_t node_count) ;
        size_t record_size() ;

        //Get the index into the distance vector for the calculating distance between the given node sides
        static size_t get_distance_vector_offset(size_t rank1, bool right_side1, size_t rank2, 
                bool right_side2, size_t node_count, record_t type); 

        size_t get_distance_vector_offset(size_t rank1, bool right_side1, 
                size_t rank2, bool right_side2) const;

        //Get the distances between two node sides in the graph
        //Ranks identify which node, sides indicate node side: false for left, true for right
        size_t get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const;

        size_t get_distance_start_start() const;
        size_t get_distance_end_end() const;

        size_t get_node_count() const;

        //Get the offset of the list of children
        size_t get_child_record_pointer() const;

        bool for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const;

    };

    struct SnarlRecordWriter : SnarlRecord , SnarlTreeRecordWriter {
        using SnarlTreeRecordWriter::records;
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::get_record_type;


        SnarlRecordWriter();

        SnarlRecordWriter (size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, record_t type);
        SnarlRecordWriter(bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, size_t pointer);

        void set_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2, size_t distance);

        void set_distance_start_start(size_t value);
        void set_distance_end_end(size_t value) ;

        //Node count is the number of nodes in the snarl, not including boundary nodes
        void set_node_count(size_t node_count);

        void set_child_record_pointer(size_t pointer) ;

        //Add a reference to a child of this snarl. Assumes that the index is completed up
        //to here
        void add_child(size_t pointer);
    };

    struct SimpleSnarlRecord : SnarlTreeRecord {

        //The node offset is set if we're considering this to be a node, not a snarl
        //If we're looking it as a snarl, then it's inf
        //Node rank must be >=2 because 0 and 1 are the start and end node
        size_t node_rank;

        SimpleSnarlRecord() {};
        SimpleSnarlRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records, size_t node = std::numeric_limits<size_t>::max());

        SimpleSnarlRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);

        //How big is the entire snarl record?
        const static size_t record_size(size_t node_count, bool include_distances) {return SIMPLE_SNARL_RECORD_SIZE + (node_count*2);}
        size_t record_size() ;

        //Get the distances between two node sides in the graph
        //Ranks identify which node, sides indicate node side: false for left, true for right
        size_t get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const;

        size_t get_node_count() const;

        nid_t get_node_id(size_t rank = std::numeric_limits<size_t>::max()) const;
        size_t get_node_length(size_t rank = std::numeric_limits<size_t>::max()) const;
        bool get_node_is_reversed(size_t rank = std::numeric_limits<size_t>::max()) const;


        net_handle_t get_child_from_rank(const size_t& rank) const;
        bool for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const;

    };
    struct SimpleSnarlRecordWriter : SimpleSnarlRecord , SnarlTreeRecordWriter {
        using SnarlTreeRecordWriter::records;
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::get_record_type;


        SimpleSnarlRecordWriter();

        SimpleSnarlRecordWriter (size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, record_t type);
        SimpleSnarlRecordWriter(bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, size_t pointer);


        //Node count is the number of nodes in the snarl, not including boundary nodes
        void set_node_count(size_t node_count);

        //Add a reference to a child of this snarl. Assumes that the index is completed up
        //to here
        void add_child(size_t rank, nid_t node_id, size_t node_length, bool is_reversed);
    };

    struct ChainRecord : SnarlTreeRecord {

        ChainRecord() {};
        ChainRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        ChainRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records);
        ChainRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records, size_t tag);

        size_t get_node_count() const;

        ///Returns (offset, is_snarl, node_offset).
        //node_offset is 0 if it is a snarl
        tuple<size_t, bool, size_t> get_last_child_offset() const;

        ///Returns true if it is a looping chain and the last node is connected to the rest of the chain by going backwards
        bool get_is_looping_chain_connected_backwards() const;

        size_t get_depth() const;

        //Get the distance between the given node sides (relative to the orientation of the chain).
        //The ranks are the offsets of the nodes in the chain (points to the record tag).
        //This is the distance between the node sides, leaving the first and entering the second,
        //not including node lengths.
        //is_looping_chain indicates whether this chain loops 
        //and last_chain_component is the component of the last thing in the chain.
        //component is the default component of the chain and end_component is the component if it is a looping chain and we want the second component of the first/last node.
        size_t get_distance(size_t rank1, bool right_side1, size_t node_length1, 
                                    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1, size_t end_component1,
                                    size_t rank2, bool right_side2, size_t node_length2,
                                    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2, size_t end_component2) const;

        ///For a chain that loops (when the start and end node are the same), find the 
        //distance walking around the back of the loop
        size_t get_distance_taking_chain_loop(size_t rank1, bool right_side1, size_t node_length1, 
                                    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1,
                                    size_t rank2, bool right_side2, size_t node_length2,
                                    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2) const;

        //Get the distance from left/right of the chain to start/end of the parent
        size_t get_distance_left_start();
        size_t get_distance_right_start();
        size_t get_distance_left_end();
        size_t get_distance_right_end();

        ////////////////////////// methods for navigating the snarl tree from this chain

        //Get the offset into snarl_tree_records of the first node in the chain
        size_t get_first_node_offset() const; 

        //Given a net_Handle_t to a child node, return the next child.
        //go_left is true if we are traversing the chain right to left.
        //
        //return the same net_handle_t if this is trying to walk out the end of the chain.
        //
        //For looping chains, this one has to know whether the boundary node is representing the start or end.
        //This means that you only see the boundary node as the start or the end.
        //
        //The next handle will be pointing in the direction that we just moved.
        net_handle_t get_next_child(const net_handle_t& net_handle, bool go_left) const;

        bool for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const;

    };

    struct ChainRecordWriter : ChainRecord , SnarlTreeRecordWriter {
        //Constructor for a chain record.
        //Since the size of the vector will be unknown (since we don't know how big the snarls are),
        //Add nodes and snarls as they come up. Assumes that the memory has already been reserved but
        //not allocated yet.
        using SnarlTreeRecordWriter::records;
        using SnarlTreeRecordWriter::record_offset;
        using SnarlTreeRecordWriter::get_record_type;

        ChainRecordWriter() {}
        ChainRecordWriter (size_t pointer, record_t type, size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records);



        void set_node_count(size_t node_count);
        void set_depth(size_t depth);

        //The offset of the last child, if it is a snarl, and if it can loop 
        void set_last_child_offset(size_t offset, bool is_snarl, bool loopable);

        //Set the distance from left/right of the chain to start/end of the parent
        void set_distance_left_start(size_t distance);
        void set_distance_right_start(size_t distance);
        void set_distance_left_end(size_t distance);
        void set_distance_right_end(size_t distance);

        /* Functions to add children to a chain. Assumes that the chain is well formed up to here.
         * These will always be called in order going forward in the chain.
         * The chain is actually composed of snarl records and trivial snarl records, but we 
         * add things by node and snarl.
         * We need to keep a SnarlTreeRecordWriter to the last thing that we added (snarl or trivial snarl),
         * so that when we add nodes we either add them to the end of the last trivial snarl or 
         * (if we have too many nodes in the last trivial snarl or if the last thing on the chain is a snarl) 
         * create a new trivial snarl .
         * Each (trivial/simple) snarl record is flanked by the size of the record, so it will be 
         * [chain info] ts size | ts record | s size | s record | s size | ts size | ts | ts size ....
         *
         */

        //Add a snarl to the end of the chain and return a SnarlRecordWriter pointing to it
        SnarlRecordWriter add_snarl(size_t snarl_size, record_t type, size_t previous_child_offset); 
        SimpleSnarlRecordWriter add_simple_snarl(size_t snarl_size, record_t type, size_t previous_child_offset); 
        //Add a node to the end of a chain and return the offset of the record it got added to
        //If new_record is true, make a new trivial snarl record for the node
        size_t add_node(nid_t node_id, size_t node_length, bool is_reversed_in_parent,
                size_t prefix_sum, size_t forward_loop, size_t reverse_loop, size_t component, 
                size_t max_prefix_sum, size_t previous_child_offset, bool new_record, bool include_distances);

    };


private:
    ////////////////////// More methods for dealing with net_handle_ts
    SnarlTreeRecord get_snarl_tree_record(const handlegraph::net_handle_t& net_handle) const {
        return SnarlTreeRecord(get_record_offset(net_handle), &snarl_tree_records);
    }
    SnarlTreeRecord get_node_record(const handlegraph::net_handle_t& net_handle) const {
        return NodeRecord(get_record_offset(net_handle), get_node_record_offset(net_handle), &snarl_tree_records); 
    }
    SnarlTreeRecord get_snarl_record(const handlegraph::net_handle_t& net_handle) const {
        return SnarlRecord(get_record_offset(net_handle), &snarl_tree_records); 
    }
    SnarlTreeRecord get_chain_record(const handlegraph::net_handle_t& net_handle) const {
        return ChainRecord(get_record_offset(net_handle), &snarl_tree_records); 
    }

public:

    //Return a string of what the handle is
    std::string net_handle_as_string(const net_handle_t& net) const;

    //Traverse the decomposition and run snarl/chain/node iteratee for ever snarl/chain/node.
    //No guarantees are made about the order of the traversal.
    //Each iteratee returns false to stop iterating and true to continue.
    //Returns false if it was stopped early, true if it finished.
    bool traverse_decomposition(const std::function<bool(const net_handle_t&)>& snarl_iteratee,
                                const std::function<bool(const net_handle_t&)>& chain_iteratee,
                                const std::function<bool(const net_handle_t&)>& node_iteratee) const;
    bool traverse_decomposition_helper(const net_handle_t& net,
                                const std::function<bool(const net_handle_t&)>& snarl_iteratee,
                                const std::function<bool(const net_handle_t&)>& chain_iteratee,
                                const std::function<bool(const net_handle_t&)>& node_iteratee) const;
        

    //Print the entire index to cout.
    //Prints each snarl, chain, and node in the index, top down, one per line as a csv: 
    // self, parent, # children, depth
    void print_self() const;
    //Helper function for print self that recursively prints a net handle
    //and all its descendants.
    void print_descendants_of(const net_handle_t net) const;

    //Print stats about every snarl to stdout.
    //tab separated file of:
    //start_id  end_id  node count  depth
    void print_snarl_stats() const;

    //Write a json file of every snarl, chain, and node to stderr
    void write_snarls_to_json() const;

    //Validate the distance index. Without debug turned on, this will only
    //assert a bunch of stuff and try to write the thing that fails to cerr
    //With debug turned on, write the whole index the same as print_self.
    void validate_index() const;
    void validate_descendants_of(const net_handle_t net) const;
    void validate_ancestors_of(const net_handle_t net) const;

    std::tuple<size_t, size_t, size_t> get_usage() ;


public:

    /*
     * A structure to store everything in the distance index, but not in just one vector to make it easier to construct.
     * This can also be used to combine distance indexes
     */
    enum temp_record_t {TEMP_CHAIN=0, TEMP_SNARL, TEMP_NODE, TEMP_ROOT};

    class TemporaryDistanceIndex{
    public:
        TemporaryDistanceIndex();
        ~TemporaryDistanceIndex();

        //Get a string of the start and end of a structure
        std::string structure_start_end_as_string(pair<temp_record_t, size_t> index) const;
    
        handlegraph::nid_t min_node_id=0;
        handlegraph::nid_t max_node_id=0;
        size_t root_structure_count=0; //How many things are in the root
        size_t max_tree_depth = 0;
        size_t max_index_size= 0;
        size_t max_distance = 0;

        //How long is the record going to be in the distance index?
        size_t get_max_record_length() const; 

        //This will actually store each individual record separately, and each 
        //will have real pointers to their parents/children (as opposed to offsets)
        struct TemporaryRecord {
        };
        struct TemporaryChainRecord : TemporaryRecord {
            handlegraph::nid_t start_node_id;
            handlegraph::nid_t end_node_id;
            size_t end_node_length=0;
            size_t tree_depth=0; //TODO: This isn't used but I left it because I couldn't get the python bindings to build when I changed it
            //Type of the parent and offset into the appropriate vector
            //(TEMP_ROOT, 0) if this is a root level chain
            pair<temp_record_t, size_t> parent;
            size_t min_length=0;//Including boundary nodes
            size_t max_length = 0;

            //Distances from the left/right of the node to the start/end of the parent snarl
            size_t distance_left_start = std::numeric_limits<size_t>::max();
            size_t distance_right_start = std::numeric_limits<size_t>::max();
            size_t distance_left_end = std::numeric_limits<size_t>::max();
            size_t distance_right_end = std::numeric_limits<size_t>::max();

            size_t rank_in_parent=0;

            //What is the index of this record in root_snarl_components
            size_t root_snarl_index = std::numeric_limits<size_t>::max();

            bool start_node_rev;
            bool end_node_rev;
            bool reversed_in_parent;
            bool is_trivial;
            bool is_tip = false;
            bool loopable = true; //If this is a looping snarl, this is false if the last snarl is not start-end connected

            vector<pair<temp_record_t, size_t>> children; //All children, both nodes and snarls, in order
            //Distances for the chain, one entry per node
            //TODO This would probably be more efficient as a vector of a struct of five ints
            vector<size_t> prefix_sum;
            vector<size_t> max_prefix_sum;
            vector<size_t> forward_loops;
            vector<size_t> backward_loops;
            vector<size_t> chain_components;//Which component does each node belong to, usually all 0s


            //How long is the record going to be in the distance index?
            size_t get_max_record_length(bool include_distances) const;
        };
        struct TemporarySnarlRecord : TemporaryRecord{
            pair<temp_record_t, size_t> parent;
            handlegraph::nid_t start_node_id;
            size_t start_node_length=0;
            handlegraph::nid_t end_node_id;
            size_t end_node_length=0;
            size_t node_count=0;
            size_t min_length = std::numeric_limits<size_t>::max(); //Not including boundary nodes
            size_t max_length = 0;
            size_t max_distance = 0;
            size_t tree_depth = 0; //TODO: This isn't used but I left it because I couldn't get the python bindings to build when I changed it

            size_t distance_start_start = std::numeric_limits<size_t>::max();
            size_t distance_end_end = std::numeric_limits<size_t>::max();

            size_t rank_in_parent=0;

            bool reversed_in_parent;
            bool start_node_rev;
            bool end_node_rev;
            bool is_trivial;
            bool is_simple;
            bool is_tip = false;
            bool is_root_snarl = false;
            bool include_distances = true;
            vector<pair<temp_record_t, size_t>> children; //All children, nodes and chains, in arbitrary order
            unordered_set<size_t> tippy_child_ranks; //The ranks of children that are tips
            //vector<tuple<pair<size_t, bool>, pair<size_t, bool>, size_t>> distances;
            unordered_map<pair<pair<size_t, bool>, pair<size_t, bool>>, size_t> distances;
                     
            //How long is the record going to be in the distance index?
            size_t get_max_record_length() const ;
        };
        struct TemporaryNodeRecord : TemporaryRecord{
            TemporaryNodeRecord() :
            node_id(0), parent(make_pair(TEMP_ROOT, 0)), node_length(0), 
            rank_in_parent(0), reversed_in_parent(false){
            }
            handlegraph::nid_t node_id;
            pair<temp_record_t, size_t> parent;
            size_t node_length=0;
            size_t rank_in_parent=0;
            size_t root_snarl_index = std::numeric_limits<size_t>::max();
            //Distances from the left/right of the node to the start/end of the parent snarl
            size_t distance_left_start = std::numeric_limits<size_t>::max();
            size_t distance_right_start = std::numeric_limits<size_t>::max();
            size_t distance_left_end = std::numeric_limits<size_t>::max();
            size_t distance_right_end = std::numeric_limits<size_t>::max();

            bool reversed_in_parent;
            bool is_tip = false;


            //How long is the record going to be in the distance index?
            const static size_t get_max_record_length() {
                return NODE_RECORD_SIZE;} 
        };


        vector<pair<temp_record_t, size_t>> components;
        vector<pair<temp_record_t, size_t>> root_snarl_components;
        vector<TemporaryChainRecord> temp_chain_records;
        vector<TemporarySnarlRecord> temp_snarl_records;
        vector<TemporaryNodeRecord> temp_node_records;
        bool use_oversized_snarls = false;
        friend class SnarlDistanceIndex;

    };

    friend class TemporaryDistanceIndex;

public:
    ///Add integers, returning max() if any of them are max()
    static size_t sum(const size_t& val1, const size_t& val2) {
        if (val1 == std::numeric_limits<size_t>::max() || val2 == std::numeric_limits<size_t>::max()) {
            return std::numeric_limits<size_t>::max();
        } else {
            return val1 + val2;
        }
    }
    static size_t minus(size_t x, size_t y) {
        if (x == std::numeric_limits<size_t>::max()) {
            return numeric_limits<size_t>::max();
        } else if (y == std::numeric_limits<size_t>::max()) {
            return numeric_limits<size_t>::max();
        } else {
            return x - y;
        }
    }
    //The maximum that isn't infinite
    static size_t maximum(size_t x, size_t y) {
        if (x == std::numeric_limits<size_t>::max()) {
            return y;
        } else if (y == std::numeric_limits<size_t>::max()) {
            return x;
        } else {
            return std::max(x, y);
        }
    }
    //How many bits are needed to represent this value 
    static size_t bit_width(size_t value) {
        return std::ceil(std::log2(value+1));
    }
public:
    //Given an arbitrary number of temporary indexes, produce the final one
    //Each temporary index must be a separate connected component
    void get_snarl_tree_records(const vector<const TemporaryDistanceIndex*>& temporary_indexes, const HandleGraph* graph);

    void time_accesses();

};


}
#endif
