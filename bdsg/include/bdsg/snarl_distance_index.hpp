#ifndef BDSG_SNARL_DISTANCE_HPP_INCLUDED
#define BDSG_SNARL_DISTANCE_HPP_INCLUDED

//#define debug_indexing

#include <handlegraph/snarl_decomposition.hpp>
#include <handlegraph/algorithms/dijkstra.hpp>
#include <handlegraph/util.hpp>
#include <handlegraph/trivially_serializable.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <string>



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
 * The distance index. Stores minimum distances among nodes in each 
 * snarl's netgraph and each chain.
 * Also used to store the snarl tree
 */


class SnarlDistanceIndex : public SnarlDecomposition, public TriviallySerializable {

public:
    ~SnarlDistanceIndex();
    SnarlDistanceIndex();
public:


////////////////// SnarlDecomposition methods

    /**
     * Get a net handle referring to a tip-to-tip traversal of the contents of the root snarl.
     * TODO: Special handling for circular things in the root snarl? Circular traversal type?
     */
    net_handle_t get_root() const ;
    
    /**
     * Return true if the given handle refers to (a traversal of) the root
     * snarl, and false otherwise.
     */
    bool is_root(const net_handle_t& net) const;
    
    /**
     * Returns true if the given net handle refers to (a traversal of) a snarl.
     */
    bool is_snarl(const net_handle_t& net) const;
    /**
     * Returns true if the given net handle refers to (a traversal of) a chain.
     */
    bool is_chain(const net_handle_t& net) const;
    /**
     * Returns true if the given net handle refers to (a traversal of) a chain that loops (a chain where the first and last node are the same).
     */
    bool is_looping_chain(const net_handle_t& net) const;
    /**
     * Returns true if the given net handle refers to (a traversal of) a trivial chain that represents a single node.
     */
    bool is_trivial_chain(const net_handle_t& net) const;
    /**
     * Returns true if the given net handle refers to (a traversal of) a single node, and thus has a corresponding handle_t.
     */
    bool is_node(const net_handle_t& net) const;
    /**
     * Return true if the given net handle is a snarl bound sentinel (in either
     * inward or outward orientation), and false otherwise.
     */
    bool is_sentinel(const net_handle_t& net) const;
    
    /**
     * Turn a handle to an oriented node into a net handle for a start-to-end or end-to-start traversal of the node, as appropriate.
     */
    net_handle_t get_net(const handle_t& handle, const handlegraph::HandleGraph* graph) const;
    
    /**
     * For a net handle to a traversal of a single node, get the handle for that node in the orientation it is traversed.
     * May not be called for other net handles.
     */
    handle_t get_handle(const net_handle_t& net, const handlegraph::HandleGraph* graph) const;
    
    /**
     * Get the parent snarl of a chain, or the parent chain of a snarl or node.
     * If the child is start-to-end or end-to-start, and the parent is a chain,
     * the chain comes out facing the same way, accounting for the relative
     * orientation of the child snarl or node in the chain. Otherwise,
     * everything is produced as start-to-end, even if that is not actually a
     * realizable traversal of a snarl or chain. May not be called on the root
     * snarl.
     *
     * Also works on snarl boundary sentinels.
     */
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
    
    /**
     * Get the bounding handle for the snarl or chain referenced by the given
     * net handle, getting the start or end facing in or out as appropriate.
     *
     * For snarls, returns the bounding sentinel net handles. For chains,
     * returns net handles for traversals of the bounding nodes of the chain.
     *
     * Ignores traversal type.
     *
     * May not be called on traversals of individual nodes.
     */
    net_handle_t get_bound(const net_handle_t& snarl, bool get_end, bool face_in) const;
    
    /**
     * Return a net handle to the same snarl/chain/node in the opposite orientation.
     * No effect on tip-to-tip, start-to-start, or end-to-end net handles. Flips all the others.
     */
    net_handle_t flip(const net_handle_t& net) const;
    
    /**
     * Get a canonical traversal handle from any net handle. All handles to the
     * same net graph element have the same canonical traversal. That canonical
     * traversal must be realizable, and might not always be start-to-end or
     * even consistently be the same kind of traversal for different snarls,
     * chains, or nodes. Mostly useful to normalize for equality comparisons.
     */
    net_handle_t canonical(const net_handle_t& net) const;

    /**
     * Return the kind of location at which the given traversal starts.
     */
    endpoint_t starts_at(const net_handle_t& traversal) const;
    
    /**
     * Return the kind of location at which the given traversal ends.
     */
    endpoint_t ends_at(const net_handle_t& traversal) const;

protected:
    /**
     * Internal implementation for for_each_child.
     */
    bool for_each_child_impl(const net_handle_t& traversal, const std::function<bool(const net_handle_t&)>& iteratee) const;

    /**
     * Internal implementation for for_each_traversal.
     */
    bool for_each_traversal_impl(const net_handle_t& item, const std::function<bool(const net_handle_t&)>& iteratee) const;

    /**
     * Internal implementation for follow_net_edges.
     */
    bool follow_net_edges_impl(const net_handle_t& here, const handlegraph::HandleGraph* graph, bool go_left, const std::function<bool(const net_handle_t&)>& iteratee) const;

public:


        /**
     * Get a net handle for traversals of a snarl or chain that contains
     * the given oriented bounding node traversals or sentinels. Given two
     * sentinels for a snarl, produces a net handle to a start-to-end,
     * end-to-end, end-to-start, or start-to-start traversal of that snarl.
     * Given handles to traversals of the bounding nodes of a chain, similarly
     * produces a net handle to a traversal of the chain.
     *
     * For a chain, either or both handles can also be a snarl containing tips,
     * for a tip-to-start, tip-to-end, start-to-tip, end-to-tip, or tip-to-tip
     * traversal. Similarly, for a snarl, either or both handles can be a chain
     * in the snarl that contains internal tips, or that has no edges on the
     * appropriate end.
     *
     * May only be called if a path actually exists between the given start
     * and end.
     */
    net_handle_t get_parent_traversal(const net_handle_t& traversal_start, const net_handle_t& traversal_end) const;

public:

    //Serialize and deserialize from TriviallySerializable
    //
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

///////////////////////////// More public functions for distance calculations using net_handle_t's 

public:
    /**
     * Find the distance between the two child node sides in the parent, facing each other, not 
     * including the lengths of the nodes
     * This only takes into account the endpoint of the net_handle_t traversal, it does not care if the traversal
     * was possible. Doesn't allow you to find the distance from a traversal ending/starting in a tip
     * requires that the children are children of the parent
     * For chains, the distance to/from a snarl is really the distance from the outer node of the snarl
     * Returns std::numeric_limits<size_t>::max() if there is not path between them in the parent 
     * or if they are not children of the parent
     */
    size_t distance_in_parent(const net_handle_t& parent, const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph=nullptr) const;
    
    //Return true if child1 comes before child2 in the chain. 
    bool is_ordered_in_chain(const net_handle_t& child1, const net_handle_t& child2) const;
    //Return the offset of the child in the chain
    //TODO: I don't like exposing this because it doesn't really 
    //mean anything but I need it
    size_t get_record_offset_in_chain(const net_handle_t& child) const;


    /** 
     * For two net handles, get a net handle lowest common ancestor
     * If the lowest common ancestor is the root, then the two handles may be in 
     * different connected components. In this case, return false
     */
    pair<net_handle_t, bool> lowest_common_ancestor(const net_handle_t& net1, const net_handle_t& net2) const;


    /**
     * Return the length of the net, which must represent a node (or sentinel of a snarl)
     */
    size_t node_length(const net_handle_t& net) const ;


    /**
     * This is also the length of a net, but it can also be a snarl or chain. 
     * The length of a chain includes the boundary nodes, a snarl does not
     * A looping chain only includes the start/end node once
     */
    size_t minimum_length(const net_handle_t& net) const;

    /**
     * What is the node id of the node represented by this net handle
     * net must be a node or a sentinel
     */
    nid_t node_id(const net_handle_t& net) const ;

    /**
     * Does the graph have this node?
     */
    bool has_node(const nid_t id) const;
    /**
     * Get a net handle from a node
     */
    net_handle_t get_node_net_handle(const nid_t id) const;

    /**
     * How deep is the snarl tree? The root is 0, top-level chain is 1, etc
     * Only counts chains
     */
    size_t get_max_tree_depth() const;

    /**
     * What is the depth of this net handle. Nodes and snarls get the depth of their parent, the epth of the root is 0
     */
    size_t get_depth(const net_handle_t& net) const;

    bool has_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const ;
    bool has_external_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const ; 


    /**
     * Get the minimum distance between two positions in the graph
     * If unoriented_distance is true, then ignore the orientations of the positions
     * Otherwise, distance is calculated from the first position going forward to the second position going forward
     * The distance includes one of the positions; the distance from one position to itself is 0
     * Returns std::numeric_limits<size_t>::max() if there is no path between the two positions
     */
    //TODO: The positions can't be const?
    size_t minimum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, const handlegraph::nid_t id2, const bool rev2, const size_t offset2, bool unoriented_distance = false, const HandleGraph* graph=nullptr) const ;

    
//////////////////////////////////////////  The actual distance index
private:
    
    /**
     *
     * This stores all records for the root, nodes, chains/snarls, and snarls' children
     * 
     * It's really made up of five types of vectors: 
     * 
     * - The (single) root vector has the format:
     *   [root tag, # connected components, # nodes, min_node_id, [pointer to node/snarl/chain record] x N]]
     *   The root vector stores the root of every connected component, which can be a 
     *   node, snarl, or chain
     * 
     * - The (single) node vector stores a record for each node and has the format:
     *   [[node tag, pointer to parent, node length, rank in parent]] x N
     *   Rank in a chain is actually the offset in the chain, so it points to that node in the chain
     * 
     * - A chain record for each chain, which is interspersed with snarl records:
     *   [chain tag, #nodes, pointer to parent, min length, max length, rank in parent, start, end, pointer to last child
     *       [node id, prefix sum, fd loop, rev loop, snarl record size, (snarl record, snarl record size)] x N] 
     *          (plus an extra node id, prefix sum, fd loop, rev loop at the end for the last node), 0, 0
     *    The two 0's at the end indicates that it's the end of the chain
     *    snarl_record_size is the number of things in the snarl record (so 0 if there is no snarl there)
     *    start/end include the orientations
     * 
     * - A snarl record for each snarl, which are stuck in chains
     *   [snarl tag, # nodes, pointer to parent, min length, max length, rank in parent, 
     *      pointer to children (in child vector), start, end, distance vector]
     *   Trivial snarls will still be contained within a chain
     *   Single nodes will technically be contained in a chain, but not stored as a chain
     *   The rank of the start node will always be 0 and the end node rank will be 1
     *   start/end are the start and end nodes, include the orientations
     *   For the first and last nodes, we only care about the node sides pointing in
     *   Each node side in the actual distance matrix will be 2*(rank-1) for the left side, and 
     *   2rank+1 for the right side, and 0 for the start, 1 for the end, where we only keep the 
     *   inner node side of the start and end
     *   Node count is the number of nodes, not including boundary nodes
     * 
     * - The (single) child vector, listing children in snarls
     *   [child vector tag, (pointer to records) x N
     *   Each snarl will have a pointer into here, and will also know how many children it has
     * 
     * 
     *   For each of the "rank_in_parent" fields, the last bit is 1 if it is reversed in the parent
     *
     */
    
public:
//TODO: This should definitely not be public
    bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector> snarl_tree_records;

    /* If this is 0, then don't store distances
     * Otherwise, for snarls with more children than snarl_size_limit, only store the distances
     * that include boundary nodes (OVERSIZED_SNARL)
     */
    size_t snarl_size_limit = 1000;

/////////////////  Construction methods
public:

    void set_size_limit (size_t size) {snarl_size_limit=size;}




private:

    std::string tag = "DistanceIndex3.0";


/////// These are used to interpret the distance index, which is just a vector of ints
/////// Uses a SnarlTreeRecord as the main class for defining and interpreting the records
/////// SnarlTreeRecords are given a base pointer, which points to the start of the record
/////// These tell the record how big it is and the offsets of each of its values
    
    //Root record
    const static size_t ROOT_RECORD_SIZE = 5;
    const static size_t COMPONENT_COUNT_OFFSET = 1;
    const static size_t NODE_COUNT_OFFSET = 2;
    const static size_t MIN_NODE_ID_OFFSET = 3;
    const static size_t MAX_TREE_DEPTH_OFFSET = 4;

    //Node record
    const static size_t NODE_RECORD_SIZE = 4;
    const static size_t NODE_PARENT_OFFSET = 1;
    const static size_t NODE_LENGTH_OFFSET = 2;
    const static size_t NODE_RANK_OFFSET = 3;
 
    //Chain record
    const static size_t CHAIN_RECORD_SIZE = 10;
    const static size_t CHAIN_NODE_COUNT_OFFSET = 1;
    const static size_t CHAIN_PARENT_OFFSET = 2;
    const static size_t CHAIN_MIN_LENGTH_OFFSET = 3; //If this is a multicomponent chain, then the actual min length is 0, but this will be the length of the first component since it is the only length that matters when looping around the outside of the chain
    const static size_t CHAIN_MAX_LENGTH_OFFSET = 4;
    const static size_t CHAIN_RANK_OFFSET = 5;
    const static size_t CHAIN_START_NODE_OFFSET = 6;
    const static size_t CHAIN_END_NODE_OFFSET = 7;
    const static size_t CHAIN_LAST_CHILD_OFFSET = 8; //The offset of the last thing in the chain - node or (if looping chain) snarl
    const static size_t CHAIN_DEPTH_OFFSET = 9;

   
    //Snarl record (which occurs within a chain)
    const static size_t SNARL_RECORD_SIZE = 9;
    const static size_t SNARL_NODE_COUNT_OFFSET = 1;
    const static size_t SNARL_PARENT_OFFSET = 2;
    const static size_t SNARL_MIN_LENGTH_OFFSET = 3;
    const static size_t SNARL_MAX_LENGTH_OFFSET = 4;
    const static size_t SNARL_CHILD_RECORD_OFFSET = 5;
    //TODO: This could also be found from the list of the snarl's children, but probably better here, even if it's duplicative
    const static size_t SNARL_START_NODE_OFFSET = 6;
    const static size_t SNARL_END_NODE_OFFSET = 7;
    const static size_t SNARL_BIT_WIDTH_OFFSET = 8;

    
    //TODO: Try moving the entire node record here and just have the nodes point to where they are in the chain
    //Node record within a chain
    const static size_t CHAIN_NODE_RECORD_SIZE = 5; //# things for a node (not including snarl record)
    const static size_t CHAIN_NODE_MULTICOMPONENT_RECORD_SIZE = 6;//A multicomponent chain node is exactly the same but with an additional value - the component offset
    const static size_t CHAIN_NODE_ID_OFFSET = 0; //node id of this node
    const static size_t CHAIN_NODE_PREFIX_SUM_OFFSET = 1;
    const static size_t CHAIN_NODE_FORWARD_LOOP_OFFSET = 2;
    const static size_t CHAIN_NODE_REVERSE_LOOP_OFFSET = 3;
    //This is only for multicomponent chains
    const static size_t CHAIN_NODE_COMPONENT_OFFSET = 4;
    //The snarl size is repeated immediately after a chain node record (so the offset is CHAIN_NODE(_MULTICOMPONENT)_RECORD_SIZE -1)
    //If there is a snarl, the snarl size is repeated after the snarl record

////////////////////////////////////  How we define different properties of the distance index

    public:

    /*
     *
     * The "tags" for defining what kind of record we're looking at will be a record_t and a 
     * bit vector indicating connectivity. The bit vector will be the last 6 bits of the tag
     * 
     * Each bit represents one type of connectivity:
     * start-start, start-end, start-tip, end-end, end-tip, tip-tip
     * 
     * The remainder of the tag will be the record_t of the record
     * NODE, SNARL, and CHAIN indicate that they don't store distances.
     * SIMPLE_SNARL is a snarl with all children connecting only to the boundary nodes
     * OVERSIZED_SNARL only stores distances to the boundaries
     * ROOT_SNARL represents a connected component of the root. It has no start or end node so 
     *    its children actually belong to the root
     * MULTICOMPONENT_CHAIN can represent a chain with snarls that are not start-end connected
     *     The chain is split up into components between these snarls, each node is tagged with
     *     which component it belongs to
     */
    //TODO: Make simple snarls work
    //TODO: Maybe also add a tag for trivial snarls/chains
    //TODO: Unary snarls? Looping chains?
    enum record_t {ROOT=1, 
                   NODE, DISTANCED_NODE, 
                   SNARL, DISTANCED_SNARL, SIMPLE_SNARL, OVERSIZED_SNARL, ROOT_SNARL, DISTANCED_ROOT_SNARL,
                   CHAIN, DISTANCED_CHAIN, MULTICOMPONENT_CHAIN,
                   CHILDREN};

    enum connectivity_t { START_START=1, START_END, START_TIP, 
                            END_START, END_END, END_TIP, 
                            TIP_START, TIP_END, TIP_TIP};
    //Type of a net_handle_t. This is to allow a node record to be seen as a chain from the 
    //perspective of a handle
    enum net_handle_record_t {ROOT_HANDLE=0, NODE_HANDLE, SNARL_HANDLE, CHAIN_HANDLE, SENTINEL_HANDLE};

private:
    /*Give each of the enum types a name for debugging */
    vector<std::string> record_t_as_string = {"ROOT", "NODE", "DISTANCED_NODE", 
                     "SNARL", "DISTANCED_SNARL", "SIMPLE_SNARL", "OVERSIZED_SNARL", 
                     "ROOT_SNARL", "DISTANCED_ROOT_SNARL",
                     "CHAIN", "DISTANCED_CHAIN", "MULTICOMPONENT_CHAIN",
                     "CHILDREN"};
    vector<std::string> connectivity_t_as_string = { "START_START", "START_END", "START_TIP", 
                            "END_START", "END_END", "END_TIP", 
                            "TIP_START", "TIP_END", "TIP_TIP"};
    vector<std::string> net_handle_record_t_string = {"ROOT_HANDLE", "NODE_HANDLE", "SNARL_HANDLE", 
                                                "CHAIN_HANDLE", "SENTINEL_HANDLE"};



////////////////////////////// How to interpret net_handle_ts
//TODO: Does this depend on endianness???
//Last 3 bits are the net_handle_record_t, next four are the connectivity_t, last are the offset into snarl_tree_records
//
//For sentinel net_handle_t's, which represent the boundaries of snarls (just inside of the boundary nodes),
//The record points to the snarl containing them, and the connectivity indicates which bound 
//we're looking at (START_END for start in, START_START for start out, etc)

public:
//TODO: This should not be public

    ///The offset into records that this handle points to
    const static size_t get_record_offset (const handlegraph::net_handle_t& net_handle) {
        return handlegraph::as_integer(net_handle) >> 7;
    }
    const static connectivity_t get_connectivity (const handlegraph::net_handle_t& net_handle){
        size_t connectivity_as_int = (handlegraph::as_integer(net_handle)>>3) & 15; //Get 4 bits after last 3
        assert (connectivity_as_int <= 9);
        return static_cast<connectivity_t>(connectivity_as_int);
    }
    const static net_handle_record_t get_handle_type (const handlegraph::net_handle_t& net_handle) {
        size_t connectivity_as_int = handlegraph::as_integer(net_handle) & 7; //Get last 3 bits
        assert (connectivity_as_int <= 4);
        return static_cast<net_handle_record_t>(connectivity_as_int);
    }

    const static handlegraph::net_handle_t get_net_handle(size_t pointer, connectivity_t connectivity, net_handle_record_t type) {
        net_handle_t handle =  as_net_handle(((((pointer << 4) | connectivity)<<3) | type)); 
        return handle;
    
    }
    handlegraph::net_handle_t get_net_handle(size_t pointer, connectivity_t connectivity) const  {
        net_handle_record_t type = SnarlTreeRecord(pointer, &snarl_tree_records).get_record_handle_type(); 
        return get_net_handle(pointer, connectivity, type); 
    
    }
    handlegraph::net_handle_t get_net_handle(size_t pointer) const  {
        net_handle_record_t type = SnarlTreeRecord(pointer, &snarl_tree_records).get_record_handle_type(); 
        return canonical(get_net_handle(pointer, START_END, type)); 
    
    }



    //Get the offset into snarl_tree_records for a node record
    size_t get_offset_from_node_id (const handlegraph::nid_t& id) const {
        size_t node_records_offset = snarl_tree_records->at(COMPONENT_COUNT_OFFSET) + ROOT_RECORD_SIZE; 
        size_t offset = (id-snarl_tree_records->at(MIN_NODE_ID_OFFSET)) * NODE_RECORD_SIZE;
        return node_records_offset + offset; 
    }
    //And its inverse, get the id from the offset of the node record
    //TODO: Do I want to add the min_node_id?
    handlegraph::nid_t get_node_id_from_offset(size_t offset) const {
        size_t min_node_id = snarl_tree_records->at(MIN_NODE_ID_OFFSET);
        size_t node_records_offset = snarl_tree_records->at(COMPONENT_COUNT_OFFSET) + ROOT_RECORD_SIZE; 
        return ((offset-node_records_offset) / NODE_RECORD_SIZE) + min_node_id;
    }

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


    ////////////////////////////////  SnarlTreeRecord class for interpreting the records in a distance index

    //Define a struct for interpreting each type of snarl tree node record (For node, snarl, chain)
    //These will be used with net_handle_t's, which store a pointer into snarl_tree_records 
    //as an offset. The last 4 bits of the net handle will be the connectivity of the handle,
    //as a connectivity_t
    //TODO: This might be overkill but I want it to be easy to change what gets stored in the index
    //
    struct SnarlTreeRecord {


        //The offset of the start of this record in snarl_tree_records
        size_t record_offset;
        const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records;

        //Constructors assuming that this record already exists
        SnarlTreeRecord(){};
        SnarlTreeRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;

            record_t type = get_record_type();
            assert(type >= 1 && type <= 13 );
        }
        SnarlTreeRecord (const net_handle_t& net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = get_record_offset(net);
            records = tree_records;
            record_t type = get_record_type();
            assert(type >= 1 && type <= 13 );
        }

        virtual size_t get_offset() { return record_offset; }

        //What type of snarl tree node is this?
        //This will be the first value of any record
        virtual record_t get_record_type() const {return static_cast<record_t>((*records)->at(record_offset) >> 9);}

        //The name is a bit misleading, it is the handle type that the record thinks it is, 
        //not necessarily the record type of the net_handle_t that was used to produce it
        virtual net_handle_record_t get_record_handle_type() const {
            record_t type= get_record_type();
            //TODO: I"m not sure if a root snarl should be a root or a snarl
            if (type == ROOT || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
                return ROOT_HANDLE;
            } else if (type == NODE || type == DISTANCED_NODE) {
                return NODE_HANDLE;
            } else if (type == SNARL || type == DISTANCED_SNARL || type ==  SIMPLE_SNARL ||type ==  OVERSIZED_SNARL){
                return SNARL_HANDLE;
            } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN) {
                return CHAIN_HANDLE;
            } else {
                throw runtime_error("error: trying to get the handle type of a list of children");
            }
        }

        //Get the internal connectivity of the structure
        virtual bool is_start_start_connected() const {return (*records)->at(record_offset) & 32;}
        virtual bool is_start_end_connected() const {return (*records)->at(record_offset) & 16;}
        virtual bool is_start_tip_connected() const {return (*records)->at(record_offset) & 8;}
        virtual bool is_end_end_connected() const {return (*records)->at(record_offset) & 4;}
        virtual bool is_end_tip_connected() const {return (*records)->at(record_offset) & 2;}
        virtual bool is_tip_tip_connected() const {return (*records)->at(record_offset) & 1;}

        //And the external connectivity. This is only relevant for root-level structures
        //since it would otherwise be captured by the containing snarl
        virtual bool is_externally_start_end_connected() const {return (*records)->at(record_offset) & 64;}
        virtual bool is_externally_start_start_connected() const {return (*records)->at(record_offset) & 128;}
        virtual bool is_externally_end_end_connected() const {return (*records)->at(record_offset) & 256;}

        virtual bool has_connectivity(connectivity_t connectivity) const;
        virtual bool has_connectivity(endpoint_t start, endpoint_t end);
        virtual bool has_external_connectivity(endpoint_t start, endpoint_t end);

        //Get and set a pointer to this node's parent, including its orientation
        //TODO: I don't think it matters if a chain is reversed or not, also it might not matter if a snarl is
        virtual size_t get_parent_record_offset() const;

        //Get and set the minimum length (distance from start to end, including boundaries for 
        // chains but not snarls, just node length for nodes)
        virtual size_t get_min_length() const;
        
        //Get and set this node's maximum length
        //This isn't actually a maximum, it's the maximum among minimum distance paths 
        //through each node in the snarl/chain
        virtual size_t get_max_length() const;

        //Get and set this structure's rank in its parent
        //For children of snarls, this means the actual rank
        //For children of chains, it points to the node in the chain
        virtual size_t get_rank_in_parent() const;

        //Is this node reversed in its parent
        //TODO: I think a node is the only thing that can be reversed in its parent, and only if it is in a chain
        virtual bool get_is_reversed_in_parent() const;

        //Get the node id of the start/end of this structure (start node of a snarl/chain)
        //TODO: Also need to add min node id
        virtual handlegraph::nid_t get_start_id() const; 
        //True if the node is traversed backwards to enter the structure
        virtual bool get_start_orientation() const;
        virtual handlegraph::nid_t get_end_id() const;
        //Return true if the end node is traversed backwards to leave the snarl
        virtual handlegraph::nid_t get_end_orientation() const;

        //TODO: These are redeclared so that I don't need to pass the SnarlTreeRecord the actual distance index
        //Get the offset into snarl_tree_records for a node record
        virtual size_t get_offset_from_id (const handlegraph::nid_t id) const ;
        //And its inverse, get the id from the offset of the node record
        //TODO: Do I want to add the min_node_id?
        virtual handlegraph::nid_t get_id_from_offset(size_t offset) const ;
    };

    //Record interpreter that has a non-const reference to snarl_tree_records, so it can
    //also add things
    struct SnarlTreeRecordConstructor {

        //TODO: This might be bad but this has the same members as SnarlTreeRecord
        //and does basically the same thing but doesn't inherit from it
        size_t record_offset;
        bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records;

        //Constructors assuming that this record already exists
        SnarlTreeRecordConstructor() {};
        SnarlTreeRecordConstructor (size_t pointer, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;

            record_t type = get_record_type();
            assert(type == ROOT || type == NODE || type == DISTANCED_NODE || type == SNARL || 
                    type == DISTANCED_SNARL || type == OVERSIZED_SNARL || 
                    type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == CHAIN || 
                    type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN);
        }
        SnarlTreeRecordConstructor (const net_handle_t& net, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = get_record_offset(net);
            records = tree_records;
            record_t type = get_record_type();
            assert(type == ROOT || type == NODE || type == DISTANCED_NODE || type == SNARL || 
                    type == DISTANCED_SNARL || type == OVERSIZED_SNARL || 
                    type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == CHAIN || 
                    type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN);
        }
        //What type of snarl tree node is this?
        //This will be the first value of any record
        //TODO: This copies from SnarlTreeRecord
        virtual record_t get_record_type() const;
        virtual void set_start_start_connected();
        virtual void set_start_end_connected();
        virtual void set_start_tip_connected();
        virtual void set_end_end_connected();
        virtual void set_end_tip_connected();
        virtual void set_tip_tip_connected();
        virtual void set_externally_start_end_connected();
        virtual void set_externally_start_start_connected() const;
        virtual void set_externally_end_end_connected() const;
        virtual void set_record_type(record_t type);
        virtual void set_min_length(size_t length);
        virtual void set_max_length(size_t length);
        virtual void set_rank_in_parent(size_t rank);
        virtual void set_is_reversed_in_parent(bool rev);
        virtual void set_parent_record_offset(size_t pointer);
        //Rev is true if the node is traversed backwards to enter the snarl
        virtual void set_start_node(handlegraph::nid_t id, bool rev); 
        //Rev is true if the node is traversed backwards to leave the snarl
        virtual void set_end_node(handlegraph::nid_t id, bool rev) const;
    };

    struct RootRecord : SnarlTreeRecord {

        RootRecord (){};
        RootRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;
            assert(get_record_type() == ROOT);
        }
        RootRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = get_record_offset(net);
            records = tree_records;
            assert(get_record_type() == ROOT);
        }
        virtual size_t get_connected_component_count() const {return (*records)->at(record_offset+COMPONENT_COUNT_OFFSET);}
        virtual size_t get_node_count() const {return (*records)->at(record_offset+NODE_COUNT_OFFSET);}
        virtual size_t get_max_tree_depth() const {return (*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET);}
        virtual size_t get_min_node_id() const {return (*records)->at(record_offset+MIN_NODE_ID_OFFSET);}
        virtual SnarlTreeRecord get_component_record(size_t component_number) const {return SnarlTreeRecord((*records)->at(record_offset+2+component_number), records);}
        virtual bool for_each_child(const std::function<bool(const handlegraph::net_handle_t&)>& iteratee) const;

    };

    struct RootRecordConstructor : RootRecord, SnarlTreeRecordConstructor {

        //Constructor meant for creating a new record, at the end of snarl_tree_records
        //TODO: The way I wrote this pointer should be 0
        RootRecordConstructor (size_t pointer, size_t connected_component_count, size_t node_count, size_t max_tree_depth, 
                    handlegraph::nid_t min_node_id, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records){
            RootRecord::record_offset = pointer;
            SnarlTreeRecordConstructor::record_offset = pointer;
            SnarlTreeRecordConstructor::records = records;
            RootRecord::records = records;
            //Allocate memory for the root vector and for all of the nodes
#ifdef debug_indexing
            cerr << " Resizing array to add root: length " << (*SnarlTreeRecordConstructor::records)->size() << " -> " 
                 << (*SnarlTreeRecordConstructor::records)->size() + ROOT_RECORD_SIZE + connected_component_count + (NODE_RECORD_SIZE * node_count) << endl;
#endif
            (*SnarlTreeRecordConstructor::records)->resize((*SnarlTreeRecordConstructor::records)->size() + ROOT_RECORD_SIZE + connected_component_count + (NODE_RECORD_SIZE * node_count));
            set_record_type(ROOT);
            set_min_node_id(min_node_id);
            set_node_count(node_count);
            set_max_tree_depth(max_tree_depth);
            set_connected_component_count(connected_component_count);
        }
        virtual void set_connected_component_count(size_t connected_component_count);
        virtual void set_node_count(size_t node_count);
        virtual void set_max_tree_depth(size_t tree_depth);
        virtual void set_min_node_id(handlegraph::nid_t node_id);
        virtual void add_component(size_t index, size_t offset);
    };
    struct NodeRecord : SnarlTreeRecord {


        NodeRecord() {};
        NodeRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;

            assert(get_record_type() == NODE || get_record_type() == DISTANCED_NODE);
        }
        NodeRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            records = tree_records;
            record_offset = get_record_offset(net);

            assert(get_handle_type(net) == NODE_HANDLE);
            assert(get_record_type() == NODE || get_record_type() == DISTANCED_NODE);
            assert(get_connectivity(net) == START_END || get_connectivity(net) == END_START
                  || get_connectivity(net) == START_START || get_connectivity(net) == END_END);
        }

        virtual handlegraph::nid_t get_node_id() const {return get_id_from_offset(record_offset);}

        //TODO: This one is a bit redundant but fine I think
        virtual size_t get_node_length() const {return (*records)->at(record_offset + NODE_LENGTH_OFFSET);}

        virtual bool in_chain() const {return SnarlTreeRecord(get_parent_record_offset(), records).get_record_handle_type() == CHAIN_HANDLE;}

    };

    struct NodeRecordConstructor : NodeRecord , SnarlTreeRecordConstructor {


        //Constructor meant for creating a new record, at the end of snarl_tree_records
        //The memory for all nodes has already been allocated by the root
        NodeRecordConstructor (size_t pointer, record_t type, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records){
            SnarlTreeRecordConstructor::records = records;
            NodeRecord::records = records;
            //TODO: Only one get_offset_from_id?
            NodeRecord::record_offset =  pointer;
            SnarlTreeRecordConstructor::record_offset =  pointer;
            set_record_type(type);
            set_start_end_connected();
        }
        virtual void set_node_length(size_t length) {
#ifdef debug_indexing
            cerr <<NodeRecord::record_offset + NODE_LENGTH_OFFSET << " set node length " << length << endl;
            assert((*SnarlTreeRecordConstructor::records)->at(NodeRecord::record_offset + NODE_LENGTH_OFFSET) == 0);
#endif
            (*SnarlTreeRecordConstructor::records)->at(NodeRecord::record_offset + NODE_LENGTH_OFFSET) = length;
        }
    };

    struct SnarlRecord : SnarlTreeRecord {


        SnarlRecord() {};
        SnarlRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;
            record_t type = get_record_type();
            assert(type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || type == ROOT_SNARL
                || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL);
        }

        SnarlRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = get_record_offset(net);
            records = tree_records;
            net_handle_record_t type = get_handle_type(net);
            assert(type == SNARL_HANDLE || type == SENTINEL_HANDLE || type == ROOT_HANDLE);
        }

        //How big is the entire snarl record?
        static size_t distance_vector_size(record_t type, size_t node_count, size_t distance_values_per_vector_element, size_t snarl_tree_record_bit_width);
        static size_t record_size (record_t type, size_t node_count, size_t distance_vectors_per_element, size_t snarl_tree_record_bit_width) ;
        virtual size_t record_size() ;

        //We're going to store multiple distance values in each vector slot to save bits
        //Get the bit width used for storing distances
        size_t get_distance_bit_width() const ;
        //And how many distance values are we keeping in each slot in the snarl_tree_records vector
        size_t get_distance_values_per_vector_element() const;

        //Helper function to get the offsets for packing distance values into a vector by adjusting the bit width
        //returns pair<offset of the element in snarl_tree_records, bits to the right of the distance value in the vector element>
        pair<size_t, size_t> get_offset_of_distance_entry_in_vector_and_element(size_t distance_vector_offset) const;  


        //Get the index into the distance vector for the calculating distance between the given node sides
        static size_t get_distance_vector_offset(size_t rank1, bool right_side1, size_t rank2, 
                bool right_side2, size_t node_count, record_t type); 

        virtual size_t get_distance_vector_offset(size_t rank1, bool right_side1, 
                size_t rank2, bool right_side2) const;

        //TODO: I want to also add a function to do this given a node id or net_handle_t instead of rank
        //Get and set the distances between two node sides in the graph
        //Ranks identify which node, sides indicate node side: false for left, true for right
        virtual size_t get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const;

        virtual size_t get_node_count() const;

        virtual size_t get_child_record_pointer() const;

        virtual bool for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const;

    };

    struct SnarlRecordConstructor : SnarlRecord , SnarlTreeRecordConstructor {


        SnarlRecordConstructor();

        SnarlRecordConstructor (size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, record_t type, size_t max_distance){
            //Constructor for making a new record, including allocating memory.
            //Assumes that this is the latest record being made, so pointer will be the end of
            //the array and we need to allocate extra memory past it
            //TODO:I'm not sure yet how memory will actually be allocated
            SnarlTreeRecordConstructor::records = records;
            SnarlRecord::records = records;

            SnarlRecord::record_offset = (*records)->size();
            SnarlTreeRecordConstructor::record_offset = (*records)->size();
            size_t distance_values_per_vector_element = (*records)->width() / bit_width(max_distance);

            size_t extra_size = record_size(type, node_count, distance_values_per_vector_element, (*records)->width());
#ifdef debug_indexing
            cerr << " Resizing array to add snarl: length " << (*SnarlTreeRecordConstructor::records)->size() << " -> "  << (*SnarlTreeRecordConstructor::records)->size() + extra_size << endl;
#endif
            (*SnarlTreeRecordConstructor::records)->resize((*SnarlTreeRecordConstructor::records)->size() + extra_size);
            set_node_count(node_count);
            set_record_type(type);
        }
        SnarlRecordConstructor(bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, size_t pointer) {
            //Make a constructor for a snarl record that has already been allocated.
            //For adding children to an existing snarl record
            SnarlRecord::record_offset = pointer;
            SnarlTreeRecordConstructor::record_offset = pointer;
            SnarlTreeRecordConstructor::records = records;
            SnarlRecord::records = records;
        }

        //Set the bit width used for storing distances 
        void set_distance_bit_width(size_t max_distance_value);
        void set_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2, size_t distance);

        //Node count is the number of nodes in the snarl, not including boundary nodes
        void set_node_count(size_t node_count);

        void set_child_record_pointer(size_t pointer) ;

        //Add a reference to a child of this snarl. Assumes that the index is completed up
        //to here
        void add_child(size_t pointer);
    };

    struct ChainRecord : SnarlTreeRecord {

        ChainRecord() {};
        ChainRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = pointer;
            records = tree_records;
            net_handle_record_t record_type= get_record_handle_type();
            if (record_type == NODE_HANDLE) {
                net_handle_record_t parent_type = SnarlTreeRecord(
                    NodeRecord(pointer, records).get_parent_record_offset(), records
                ).get_record_handle_type();
                assert(parent_type == ROOT_HANDLE || parent_type == SNARL_HANDLE);
            } else {
                assert(get_record_handle_type() == CHAIN_HANDLE);
            }
        }
        ChainRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
            record_offset = get_record_offset(net);
            records = tree_records;

            net_handle_record_t record_type = get_record_handle_type();
            if (record_type == NODE_HANDLE) {
                net_handle_record_t parent_type = SnarlTreeRecord(
                    NodeRecord(record_offset, records).get_parent_record_offset(), records
                ).get_record_handle_type();
                assert(parent_type == ROOT_HANDLE || parent_type == SNARL_HANDLE);
            } else {
                assert(get_record_handle_type() == CHAIN_HANDLE);
            }
        }

        virtual size_t get_node_count() const;

        ///Returns (offset, is_snarl)
        virtual pair<size_t, bool> get_last_child_offset() const;

        ///Returns true if it is a looping chain and the last node is connected to the rest of the chain by going backwards
        virtual bool get_is_looping_chain_connected_backwards() const;

        //Get the values this node (boundary node of a snarl in the chain)
        //pointer is a pointer into snarl_tree_records, to the beginning of the record for this node
        //So it'll point to the node id of the node we're looking at
        virtual size_t get_chain_node_id(size_t pointer) const;
        virtual size_t get_prefix_sum_value(size_t pointer) const;
        virtual size_t get_forward_loop_value(size_t pointer) const;
        virtual size_t get_reverse_loop_value(size_t pointer) const;
        virtual size_t get_chain_component(size_t pointer, bool get_end=false) const;
        virtual size_t get_depth() const;

        //Get the distance between the given node sides (relative to the orientation of the chain)
        //Nodes represent a tuple of <pointer, right_side, and length of the node>
        //This is the distance between the node sides, leaving the first and entering the second,
        //not including node lengths
        //TODO: Double check finding the distance for the same node
        virtual size_t get_distance(tuple<size_t, bool, size_t> node1, 
                             tuple<size_t, bool, size_t> node2) const;

        ///For a chain that loops (when the start and end node are the same), find the 
        //distance walking around the back of the loop
        virtual size_t get_distance_taking_chain_loop(tuple<size_t, bool, size_t> node1, 
                             tuple<size_t, bool, size_t> node2) const;

        ////////////////////////// methods for navigating the snarl tree from this chain

        //Get the offset into snarl_tree_records of the first node in the chain
        virtual size_t get_first_node_offset() const; 

        //Given a pointer to a child node, return the next child. This includes nodes and snarls.
        //The node pointer points to the node id, snarl pointer points to the start of the snarl record
        //bool is true if it is a snarl, false if it is a node
        //go_left is true if we are traversing the chain right to left
        //returns 0 if this was the last (/first) node in the chain
        virtual pair<size_t, bool> get_next_child(const pair<size_t, bool> pointer, bool go_left) const;
        //The same thing but take and return a net_handle_t
        //return the same net_handle_t if this is the end of the chain
        //For looping chains, this one has to know whether the boundary node is representing the start or end
        //This means that you only see the boundary node as the start or the end
        virtual net_handle_t get_next_child(const net_handle_t& net_handle, bool go_left) const;

        virtual bool for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const;
    };

    struct ChainRecordConstructor : ChainRecord , SnarlTreeRecordConstructor {
        //Constructor for a chain record
        //Since the size of the vector will be unknown (since we don't know how big the snarls are),
        //Add nodes and snarls as they come up. Assumes that the memory has already been reserved but
        //not allocated yet

        //TODO: I don't think I even need node count
        ChainRecordConstructor() {}
        ChainRecordConstructor (size_t pointer, record_t type, size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records){
            assert(type == CHAIN || 
                   type == DISTANCED_CHAIN ||
                   type == MULTICOMPONENT_CHAIN);
            ChainRecord::record_offset = pointer;
            SnarlTreeRecordConstructor::record_offset = pointer;
            SnarlTreeRecordConstructor::records = records;
            ChainRecord::records = records;
#ifdef debug_indexing
            cerr << " Resizing array to add chain: length " << (*SnarlTreeRecordConstructor::records)->size() << " -> "  << (*SnarlTreeRecordConstructor::records)->size() + CHAIN_RECORD_SIZE << endl;
#endif
            (*SnarlTreeRecordConstructor::records)->resize((*SnarlTreeRecordConstructor::records)->size() + CHAIN_RECORD_SIZE);
            set_node_count(node_count);
            set_record_type(type);
        }


        /* Functions to add children to a chain. Assumes that the chain is well formed up to here
         * These will always be called in order going forward in the chain, starting and ending 
         * with add_node, with either add_snarl or add_trivial_snarl in between them. 
         * Nodes will be [node id, prefix sum, fd loop, rev loop], and snarls will be flanked by
         * the record size. A trivial snarl will only be a record size of 0. 
         * Two 0's where the snarl size and next node id should be indicates the end of the chain
         */

        void add_node(handlegraph::nid_t id, size_t prefix_sum, size_t forward_loop, size_t reverse_loop);
        void add_node(handlegraph::nid_t id, size_t prefix_sum, size_t forward_loop, size_t reverse_loop, size_t component);
        void set_node_count(size_t node_count);
        void set_depth(size_t depth);

        //The offset of the last child, if it is a snarl, and if it can loop 
        void set_last_child_offset(size_t offset, bool is_snarl, bool loopable);

        void add_trivial_snarl();

        //Add a snarl to the end of the chain and return a SnarlRecordConstructor pointing to it
        SnarlRecordConstructor add_snarl(size_t snarl_size, record_t type, size_t max_snarl_distance); 

        //This gets called when we've added all of the chain's children
        void finish_chain();
    };


private:
    ////////////////////// More methods for dealing with net_handle_ts
    SnarlTreeRecord get_snarl_tree_record(const handlegraph::net_handle_t& net_handle) const {
        return SnarlTreeRecord(get_record_offset(net_handle), &snarl_tree_records);
    }
    SnarlTreeRecord get_node_record(const handlegraph::net_handle_t& net_handle) const {
        return NodeRecord(get_record_offset(net_handle), &snarl_tree_records); 
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

    //Print the entire index to cout
    //Prints each snarl, chain, and node in the index, top down, one per line as a csv: 
    // self, parent, # children, depth
    void print_self() const;
    //Helper function for print self that recursively prints a net handle
    //and all its descendants
    void print_descendants_of(const net_handle_t net) const;

    //Validate the distance index. Without debug turned on, this will only
    //assert a bunch of stuff and try to write the thing that fails to cerr
    //With debug turned on, write the whole index the same as print_self

    void validate_index() const;
    void validate_descendants_of(const net_handle_t net) const;
    void validate_ancestors_of(const net_handle_t net) const;

    std::tuple<size_t, size_t, size_t> get_usage() ;


public:

    /*
     * A structure to store everything in the distance index, but not in just one vector to make it easier to construct
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
        size_t max_index_size= 0;//TODO: This will change depending on how the actual index is represented
        size_t max_distance = 0;
        size_t get_max_record_length() const {return ROOT_RECORD_SIZE + root_structure_count + (max_node_id-min_node_id+1) * NODE_RECORD_SIZE + max_index_size;} 

        //This will actually store each individual record separately, and each 
        //will have real pointers to their parents/children (as opposed to offsets)
        struct TemporaryRecord {
        };
        struct TemporaryChainRecord : TemporaryRecord {
            handlegraph::nid_t start_node_id;
            bool start_node_rev;
            handlegraph::nid_t end_node_id;
            bool end_node_rev;
            size_t end_node_length;
            size_t tree_depth = 0;
            //Type of the parent and offset into the appropriate vector
            //(TEMP_ROOT, 0) if this is a root level chain
            pair<temp_record_t, size_t> parent;
            size_t min_length;//Including boundary nodes
            size_t max_length;
            vector<pair<temp_record_t, size_t>> children; //All children, both nodes and snarls, in order
            //Distances for the chain, one entry per node
            vector<size_t> prefix_sum;
            vector<size_t> forward_loops;
            vector<size_t> backward_loops;
            vector<size_t> chain_components;//Which component does each node belong to, usually all 0s
            size_t rank_in_parent;
            bool reversed_in_parent;
            bool is_trivial;
            bool is_tip = false;
            //What is the index of this record in root_snarl_components
            size_t root_snarl_index = std::numeric_limits<size_t>::max();
            bool loopable = true; //If this is a looping snarl, this is false if the last snarl is not start-end connected
            size_t get_max_record_length() const {return CHAIN_RECORD_SIZE + (CHAIN_NODE_MULTICOMPONENT_RECORD_SIZE+1)*prefix_sum.size() + 3;} 
        };
        struct TemporarySnarlRecord : TemporaryRecord{
            handlegraph::nid_t start_node_id;
            bool start_node_rev;
            size_t start_node_length;
            handlegraph::nid_t end_node_id;
            bool end_node_rev;
            size_t end_node_length;
            size_t node_count;
            size_t min_length; //Not including boundary nodes
            size_t max_length;
            size_t max_distance = 0;
            size_t tree_depth = 0;
            pair<temp_record_t, size_t> parent;
            vector<pair<temp_record_t, size_t>> children; //All children, nodes and chains, in arbitrary order
            unordered_set<size_t> tippy_child_ranks; //The ranks of children that are tips
            unordered_map<pair<pair<size_t, bool>, pair<size_t, bool>>, size_t> distances;
            size_t rank_in_parent;
            bool reversed_in_parent;
            //The minimum distances to go into and out of the snarl from the start or end nodes
            size_t loop_start;
            size_t loop_end;
            bool is_trivial;
            bool is_tip = false;
            bool is_root_snarl = false;
            size_t get_max_record_length() {return SNARL_RECORD_SIZE + node_count * node_count + node_count;}
        };
        struct TemporaryNodeRecord : TemporaryRecord{
            TemporaryNodeRecord() :
            node_id(0), parent(make_pair(TEMP_ROOT, 0)), node_length(0), 
            rank_in_parent(0), reversed_in_parent(false){
            }
            handlegraph::nid_t node_id;
            pair<temp_record_t, size_t> parent;
            size_t node_length;
            size_t rank_in_parent;
            bool reversed_in_parent;
            bool is_tip = false;
            size_t get_max_record_length() {return NODE_RECORD_SIZE;} 
        };


        vector<pair<temp_record_t, size_t>> components;
        vector<pair<temp_record_t, size_t>> root_snarl_components;
        vector<TemporaryChainRecord> temp_chain_records;
        vector<TemporarySnarlRecord> temp_snarl_records;
        vector<TemporaryNodeRecord> temp_node_records;
        friend class SnarlDistanceIndex;

    };

    friend class TemporaryDistanceIndex;

public:
    ///Add integers, returning max() if any of them are max()
    static size_t sum(const vector<size_t> vals) {
        size_t sum = 0;
        for (const size_t& x : vals) {
            if (x ==  std::numeric_limits<size_t>::max()) {
                return std::numeric_limits<size_t>::max();
            } else {
                sum += x;
            }
        }
        return sum;
    }
    static size_t minus(size_t x, size_t y) {
        if (x == std::numeric_limits<size_t>::max()) {
            return numeric_limits<size_t>::max();
        } else if (y == std::numeric_limits<size_t>::max()) {
            return -numeric_limits<size_t>::max();
        } else {
            return x - y;
        }
    }
    //How many bits are needed to represent this value (with some wiggle room)
    static size_t bit_width(size_t value) {
        return log2(value+1) + 3;

    }
public:
    //Given an arbitrary number of temporary indexes, produce the final one
    //Each temporary index must be a separate connected component
    void get_snarl_tree_records(const vector<const TemporaryDistanceIndex*>& temporary_indexes, const     HandleGraph* graph);
    void time_accesses();

};


}
#endif
