//#define debug_distance_indexing
//#define debug_snarl_traversal
//#define debug_distances
//#define debug_distance_paths

#include "bdsg/snarl_distance_index.hpp"
#include <jansson.h>
#include <arpa/inet.h>

using namespace std;
using namespace handlegraph;
namespace bdsg {


///////////////////////////////////////////////////////////////////////////////////////////////////
//Constructor
SnarlDistanceIndex::SnarlDistanceIndex() {
    snarl_tree_records.construct(get_prefix());
    snarl_tree_records->width(64);
}
SnarlDistanceIndex::~SnarlDistanceIndex() {
}


/*Temporary distance index for constructing the index from the graph
 */
SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryDistanceIndex(){}

SnarlDistanceIndex::TemporaryDistanceIndex::~TemporaryDistanceIndex(){}

string SnarlDistanceIndex::TemporaryDistanceIndex::structure_start_end_as_string(pair<temp_record_t, size_t> index) const {
    if (index.first == TEMP_NODE) {
        assert(index.second == temp_node_records[index.second-min_node_id].node_id);
        return "node " + std::to_string(temp_node_records[index.second-min_node_id].node_id);
    } else if (index.first == TEMP_SNARL) {
        const TemporarySnarlRecord& temp_snarl_record =  temp_snarl_records[index.second];
        return "snarl " + std::to_string( temp_snarl_record.start_node_id) 
                + (temp_snarl_record.start_node_rev ? " rev" : " fd") 
                + " -> " + std::to_string( temp_snarl_record.end_node_id) 
                + (temp_snarl_record.end_node_rev ? " rev" : " fd");
    } else if (index.first == TEMP_CHAIN) {
        const TemporaryChainRecord& temp_chain_record = temp_chain_records[index.second];
        return "chain " + std::to_string( temp_chain_record.start_node_id) 
                + (temp_chain_record.start_node_rev ? " rev" : " fd") 
                + " -> "  + std::to_string( temp_chain_record.end_node_id) 
                + (temp_chain_record.end_node_rev ? " rev" : " fd");
    } else if (index.first == TEMP_ROOT) {
        return (string) "root";
    } else {
        return (string)"???" + std::to_string(index.first) + "???";
    }
}
//The max record length of this chain
size_t SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length(bool include_distances) const {
    if (is_trivial) {
        return NODE_RECORD_SIZE;
    } else { 
        //Count how many trivial snarls are in this chain
        size_t trivial_snarl_count = 0;
        size_t last_node_count = 0; // How many nodes have we seen in a row?
        size_t nontrivial_snarl_count = 0;
        size_t total_node_count = 0;
        for (const pair<temp_record_t, size_t>& child : children) {
            if (child.first == TEMP_NODE) {
                if (total_node_count==0 || child != children.front()) {
                    //If this is the last node in the chain, don't do anything
                    total_node_count++;
                    if (last_node_count == 0 || last_node_count == MAX_TRIVIAL_SNARL_NODE_COUNT) {
                        //New trivial snarl
                        trivial_snarl_count ++;
                        last_node_count = 1;
                    } else {
                        last_node_count ++;
                    }
                }
            } else {
                //IF this is a snarl
                last_node_count = 0;
                nontrivial_snarl_count++;
            }
        }
        //The size of the chain record + the size of all the trivial snarls in the chain
        // + the size of the nodes in the trivial snarls + the sizes of all
        // the snarls in the chain
        return CHAIN_RECORD_SIZE + ((include_distances ? DISTANCED_TRIVIAL_SNARL_RECORD_SIZE : DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE) *trivial_snarl_count) 
                                 + (total_node_count * (include_distances ? 2 : 1)) 
                                 + ((trivial_snarl_count + nontrivial_snarl_count) * 2) - 1;
    }
}
//The max record length of the root
size_t SnarlDistanceIndex::TemporaryDistanceIndex::get_max_record_length() const {
    return ROOT_RECORD_SIZE + root_structure_count + (max_node_id-min_node_id+1) * 2 + max_index_size;
}

//The max record length of this snarl
size_t SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::get_max_record_length() const {
    if (is_trivial) {
        return 0;
    } else if (is_simple) {
        return SimpleSnarlRecord::record_size(node_count, include_distances); 
    } else {
         if (parent.first == TEMP_ROOT) {
             return SnarlRecord::record_size(include_distances ? DISTANCED_ROOT_SNARL : ROOT_SNARL, node_count) + node_count;
         } else {
            return SnarlRecord::record_size(include_distances ? DISTANCED_SNARL : SNARL, node_count) + node_count;
         }
    }
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//Implement the SnarlDecomposition's functions for moving around the snarl tree
//


net_handle_t SnarlDistanceIndex::get_root() const {
    // The root is the first thing in the index, the traversal is tip to tip
    return get_net_handle_from_values(0, TIP_TIP, ROOT_HANDLE);
}

bool SnarlDistanceIndex::is_root(const net_handle_t& net) const {
    return get_handle_type(net) == ROOT_HANDLE;
}

bool SnarlDistanceIndex::is_root_snarl(const net_handle_t& net) const {
#ifdef debug_distances
    if (get_handle_type(net) == ROOT_HANDLE && get_record_offset(net) != 0) {
        assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == ROOT_SNARL ||
               SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL);
    }
#endif

    return get_handle_type(net) == ROOT_HANDLE && get_record_offset(net) != 0;
}


bool SnarlDistanceIndex::is_snarl(const net_handle_t& net) const {
#ifdef debug_distances
if(get_handle_type(net) == SNARL_HANDLE){
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == ROOT_SNARL ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL);
    assert(get_node_record_offset(net) == 0 || get_node_record_offset(net) == 1);
}
#endif
    return get_handle_type(net) == SNARL_HANDLE;
}

bool SnarlDistanceIndex::is_dag(const net_handle_t& snarl) const {
    record_t record_type = SnarlTreeRecord(snarl, &snarl_tree_records).get_record_type();
    if ( record_type == SNARL || record_type == ROOT_SNARL ) {
        //If this is a snarl but didn't store distances
        cerr << "warning: checking if a snarl is a dag in an index without distances. Returning true" << endl;
        return true; 
    } else if (record_type == DISTANCED_SNARL || record_type == OVERSIZED_SNARL || record_type == DISTANCED_ROOT_SNARL) {
        //If this is any kind of non-simple snarl

        if (record_type != DISTANCED_ROOT_SNARL) {
            //If there were boundary nodes, check for loops on the bounds

            //The bounds of the snarl facing in
            net_handle_t snarl_start = get_bound(snarl, false, true);
            net_handle_t snarl_end = get_bound(snarl, true, true);

            //If there are loops on the bounds, then this is not a dag
            if (distance_in_parent(snarl, snarl_start, snarl_start) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(snarl, snarl_end, snarl_end) != std::numeric_limits<size_t>::max()){
                return false;
            }
        }

        //Otherwise, go through each child of the snarl
        //This returns false if it stopped early - if the iterator returned false - if it had non-dag edges
        return for_each_child(snarl, [&] (const net_handle_t& child) {
            //Check if there are any self-loops on the child
            if (distance_in_parent(snarl, child, child) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(snarl, child, flip(child)) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(snarl, flip(child), child) != std::numeric_limits<size_t>::max()){

                //If there are self-loops
                return false;
            } else {
                return true;
            }
        });
    } else {
        //If this is a simple snarl or not a snarl, then return false
        if (!is_snarl(snarl)) {
            cerr << "Warning: checking if a non-snarl is a dag" << endl;
        }
        return true;
    }
}

bool SnarlDistanceIndex::is_simple_snarl(const net_handle_t& net) const {
#ifdef debug_distances
if(get_handle_type(net) == SNARL_HANDLE){
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == ROOT_SNARL ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL);
}
#endif
    return get_handle_type(net) == SNARL_HANDLE && get_node_record_offset(net) == 1;
}

bool SnarlDistanceIndex::is_regular_snarl(const net_handle_t& net) const {
#ifdef debug_distances
if(get_handle_type(net) == SNARL_HANDLE){
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == ROOT_SNARL ||
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL);
}
#endif

    //If there is any edge from the boundary nodes to themselves, then it cannot be regular
    net_handle_t start_in = get_bound(net, false, true);
    net_handle_t end_in = get_bound(net, true, true);
    if (distance_in_parent(net, start_in, start_in) != std::numeric_limits<size_t>::max()) {
        return false;
    }
    if (distance_in_parent(net, end_in, end_in) != std::numeric_limits<size_t>::max()) {
        return false;
    }
    bool is_regular = true;

    for_each_child(net, [&](const net_handle_t& child) {
        //If there isn't a path through the snarl that passes through the child 
        //or there's an extra path through the child then it is irregular
        bool start_right = distance_in_parent(net, start_in, child) != std::numeric_limits<size_t>::max();
        bool start_left = distance_in_parent(net, start_in, flip(child)) != std::numeric_limits<size_t>::max();
        bool end_right = distance_in_parent(net, end_in, child) != std::numeric_limits<size_t>::max();
        bool end_left = distance_in_parent(net, end_in, flip(child)) != std::numeric_limits<size_t>::max();

        if (start_right && end_left) {
            if (start_left || end_right) {
                is_regular = false;
                return false;
            }
        } else if (start_left && end_right) {
            if (start_right || end_left) {
                is_regular = false;
                return false;
            }
        } else {
            //There wasn't a path through this node so it is irregular
            is_regular = false;
            return false;
        }

        //If there is an edge to any other child, then it is irregular 
        for_each_child(net, [&](const net_handle_t& child2) {
            if (distance_in_parent(net, child, child2) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(net, child, flip(child2)) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(net, flip(child), child2) != std::numeric_limits<size_t>::max() ||
                distance_in_parent(net, flip(child), flip(child2)) != std::numeric_limits<size_t>::max()) {
                is_regular = false;
                return false;
            }
            //Return true to continue traversing
            return true;
        });
        //Return true to continue traversing
        return true;
    });
    return is_regular;
}

bool SnarlDistanceIndex::is_chain(const net_handle_t& net) const {
#ifdef debug_distances
if (get_handle_type(net) ==CHAIN_HANDLE) {
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == CHAIN_HANDLE ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL);
}
#endif
    return get_handle_type(net) == CHAIN_HANDLE;
}
bool SnarlDistanceIndex::is_multicomponent_chain(const net_handle_t& net) const {
#ifdef debug_distances
if (get_handle_type(net) ==CHAIN_HANDLE) {
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == CHAIN_HANDLE ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
    SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL);
}
#endif
    return get_handle_type(net) == CHAIN_HANDLE 
        && SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == MULTICOMPONENT_CHAIN;
}
bool SnarlDistanceIndex::is_looping_chain(const net_handle_t& net) const {
    if (!is_chain(net) || is_trivial_chain(net)) {
        return false;
    }
    ChainRecord chain_record(net, &snarl_tree_records);
    return chain_record.get_start_id() == chain_record.get_end_id();
}
bool SnarlDistanceIndex::is_ordered_in_chain(const net_handle_t& child1, const net_handle_t& child2) const {
#ifdef debug_distances
    cerr << "are children " << net_handle_as_string(child1) << " and " << net_handle_as_string(child2) << " ordered in parent " << net_handle_as_string(get_parent(child1)) << "?" << endl;
    if (!(is_chain(get_parent(child1)) && canonical(get_parent(child1)) == canonical(get_parent(child2)))) {
        throw runtime_error("error: Trying to see if two children are ordered in a non-chain parent or different parents");
    }
    /*This is the proper way to do it since it doesn't depend on how I'm storing the children of the chain,
     * but it takes too long so just double check this if anything changes
    size_t rank1 = is_node(child1) ? TrivialSnarlRecord(get_record_offset(child1), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(child1))
                                   : SnarlTreeRecord(child1, &snarl_tree_records).get_rank_in_parent();
    size_t rank2 = is_node(child2) ? TrivialSnarlRecord(get_record_offset(child2), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(child2))
                                   : SnarlTreeRecord(child2, &snarl_tree_records).get_rank_in_parent();
     */
#endif
    size_t rank1 = get_record_offset(child1) + get_node_record_offset(child1);
    size_t rank2 = get_record_offset(child2) + get_node_record_offset(child2);

    return  rank1 < rank2;
}


bool SnarlDistanceIndex::is_trivial_chain(const net_handle_t& net) const {
    bool handle_is_chain =get_handle_type(net) == CHAIN_HANDLE; 
    bool record_is_node = SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE;
    bool record_is_simple_snarl = SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
                    SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL ;
    bool handle_has_node_offset = get_node_record_offset(net) >= 2;
    
    return handle_is_chain && (record_is_node
                || (record_is_simple_snarl && handle_has_node_offset));
}

bool SnarlDistanceIndex::is_node(const net_handle_t& net) const {
#ifdef debug_distances 
if(get_handle_type(net) == NODE_HANDLE){
    assert( SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE 
           || SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL
           || SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL );
}
#endif
    return get_handle_type(net) == NODE_HANDLE;
}
bool SnarlDistanceIndex::is_sentinel(const net_handle_t& net) const {
#ifdef debug_distances
if(get_handle_type(net) == SENTINEL_HANDLE){
    assert(SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE
           || SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL
           || SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL);
}
#endif
    return get_handle_type(net) == SENTINEL_HANDLE;

}

net_handle_t SnarlDistanceIndex::get_net(const handle_t& handle, const handlegraph::HandleGraph* graph) const{
    net_handle_t net = get_node_net_handle(graph->get_id(handle));
    return graph->get_is_reverse(handle) ? flip(net) : net;

}
handle_t SnarlDistanceIndex::get_handle(const net_handle_t& net, const handlegraph::HandleGraph* graph) const{
    if (get_handle_type(net) == SENTINEL_HANDLE) {
        SnarlRecord snarl_record(net, &snarl_tree_records);
        if (starts_at(net) == START) {
            return graph->get_handle(snarl_record.get_start_id(), 
                       ends_at(net) == START ? !snarl_record.get_start_orientation()   //Going out
                                             : snarl_record.get_start_orientation());  //Going in
        } else {
            assert (starts_at(net) == END);
            return graph->get_handle(snarl_record.get_end_id(), 
                       ends_at(net) == END ? snarl_record.get_end_orientation()   //Going out
                                           : !snarl_record.get_end_orientation());  //Going in
        }

    } else if (get_handle_type(net) == NODE_HANDLE || get_handle_type(net) == CHAIN_HANDLE ) {
        return graph->get_handle(node_id(net), ends_at(net) == END ? false : true);
    } else {
        throw runtime_error("error: trying to get a handle from a snarl, chain, or root");
    }
}

net_handle_t SnarlDistanceIndex::get_parent(const net_handle_t& child) const {

    connectivity_t child_connectivity = get_connectivity(child);

    //If the child is the sentinel of a snarl, just return the snarl
    if (get_handle_type(child) == SENTINEL_HANDLE) {
        connectivity_t parent_connectivity = START_END;
        if (child_connectivity == START_END || child_connectivity == END_START) {
            //If the child sentinel is pointing through the snarl, then the snarl should point
            //in the same direction
            parent_connectivity = child_connectivity;
        } else if (child_connectivity == START_START) {
            //If the sentinel is the start node pointing out, then the parent should point out the start
            parent_connectivity = END_START;
        } else if (child_connectivity == END_END) {
            //If the sentinel is the end node pointing out, then the parent should point out the end
            parent_connectivity = START_END;
        }
        return get_net_handle_from_values(get_record_offset(child), parent_connectivity, 
                                          SNARL_HANDLE, get_node_record_offset(child)); 
    } else if (get_handle_type(child) == ROOT_HANDLE) {
        throw runtime_error("error: trying to find the parent of the root");
    } else if (get_record_type(snarl_tree_records->at(get_record_offset(child))) == SIMPLE_SNARL ||
               get_record_type(snarl_tree_records->at(get_record_offset(child))) == DISTANCED_SIMPLE_SNARL) {
        //If this is a simple snarl and a node or chain, then the parent offset doesn't change
        if (get_handle_type(child) == NODE_HANDLE) {
            //If this is a node, then return it as a chain
            return get_net_handle_from_values(get_record_offset(child), child_connectivity, 
                                              CHAIN_HANDLE, get_node_record_offset(child));
        } else if (get_handle_type(child) == CHAIN_HANDLE) {
            //If this is a chain, then return the same thing as a snarl
            return get_net_handle_from_values(get_record_offset(child), START_END, SNARL_HANDLE, 1);
        }
    }

    //Otherwise, we need to move up one level in the snarl tree

    //Get the pointer to the parent to find its type
    size_t parent_pointer = SnarlTreeRecord(child, &snarl_tree_records).get_parent_record_offset();
    net_handle_record_t parent_type = SnarlTreeRecord(parent_pointer, &snarl_tree_records).get_record_handle_type();

    
    //The connectivity of the parent defaults to start-end
    connectivity_t parent_connectivity = START_END;
    //If the parent is going to be a chain, then it has the connectivity of the child, flipped if the child is 
    //reversed in the parent
    if ((get_handle_type(child) == NODE_HANDLE || get_handle_type(child) == SNARL_HANDLE) 
            && (child_connectivity == START_END || child_connectivity == END_START)) {
        if (is_reversed_in_parent(child)) {
            parent_connectivity = child_connectivity == START_END ? END_START : START_END;

        } else {
            parent_connectivity = child_connectivity;
        }
    } else if (parent_type == ROOT_HANDLE) {
        parent_connectivity = TIP_TIP;
    }
    if (get_handle_type(child) == NODE_HANDLE && parent_type != CHAIN_HANDLE) {
        //If this is a node and it's parent is not a chain, we want to pretend that its 
        //parent is a chain
        return get_net_handle_from_values(get_record_offset(child), child_connectivity, CHAIN_HANDLE, get_node_record_offset(child));
    } 

    return get_net_handle(parent_pointer, parent_connectivity);
}

net_handle_t SnarlDistanceIndex::get_bound(const net_handle_t& snarl, bool get_end, bool face_in) const {
    if (get_handle_type(snarl) == CHAIN_HANDLE) {
        ChainRecord chain_record(snarl, &snarl_tree_records);
        size_t offset;
        size_t node_offset;
        bool is_looping_chain=false;
        bool rev_in_parent;
        if (is_trivial_chain(snarl)) {
            offset = get_record_offset(snarl);
            rev_in_parent = false;
            node_offset = get_node_record_offset(snarl);
        } else {
            if (get_end) {
                tuple<size_t, bool, size_t> last_child = chain_record.get_last_child_offset();
                is_looping_chain = chain_record.get_start_id() == chain_record.get_end_id();
                offset = is_looping_chain ? chain_record.get_first_node_offset() : std::get<0>(last_child);
                node_offset = is_looping_chain ? 0 : std::get<2>(last_child); 

            } else {
                offset = chain_record.get_first_node_offset(); 
                node_offset = 0;
                
            }
            rev_in_parent = TrivialSnarlRecord(offset, &snarl_tree_records).get_is_reversed_in_parent(node_offset);
        }
        if (get_end) {
            rev_in_parent = !rev_in_parent;
        }
        if (!face_in){
            rev_in_parent = !rev_in_parent;
        }
        connectivity_t connectivity = rev_in_parent ? END_START : START_END;

        if (is_looping_chain && get_end) {
            //If this is a looping chain and we're getting the end, then the traversal of the node will be the 
            //end endpoint repeated, instead of the actual traversal
            connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), get_end_endpoint(connectivity));
        }
        return get_net_handle_from_values(offset, connectivity,  NODE_HANDLE, node_offset);

    } else if (is_root(snarl)) {
        throw runtime_error("Trying to get the bounds of a root");
    } else {
        assert(get_handle_type(snarl) == SNARL_HANDLE);
        endpoint_t start = get_end ? END : START;
        endpoint_t end = face_in ? (start == END ? START : END) : start;
        return get_net_handle_from_values(get_record_offset(snarl), endpoints_to_connectivity(start, end), SENTINEL_HANDLE, get_node_record_offset(snarl));
    }
}

net_handle_t SnarlDistanceIndex::get_node_from_sentinel(const net_handle_t& sentinel) const {
    assert(is_sentinel(sentinel));
    if (is_root(get_parent(get_parent(sentinel)))) {
        throw runtime_error("Trying to get the bounds of a root");
    }
    net_handle_t snarl = get_parent(sentinel);
    net_handle_t parent_chain = get_parent(snarl);
    assert(is_chain(parent_chain));
    //The snarl must be in a chain, so the boundary nodes will be the next things on the chain 
    ChainRecord chain_record(parent_chain, &snarl_tree_records);

    net_handle_t next_handle = chain_record.get_next_child(snarl, starts_at(sentinel) == START);

    //If the sentinel is facing into the snarl, then flip the node after getting it from the chain
    return ends_at(sentinel) != starts_at(sentinel) ? flip(next_handle) : next_handle ;


}

net_handle_t SnarlDistanceIndex::flip(const net_handle_t& net) const {
    connectivity_t old_connectivity = get_connectivity(net);
    connectivity_t new_connectivity;
    if (is_sentinel(net)) {
        //If this is the sentinel of a snarl, then the first endpoint indicates the node
        // and the second indicates the orientation, so only flip the second
        new_connectivity = endpoints_to_connectivity(get_start_endpoint(old_connectivity),
                get_end_endpoint(old_connectivity) == START ? END : START);
    } else {
        new_connectivity = endpoints_to_connectivity(get_end_endpoint(old_connectivity), 
                                                                get_start_endpoint(old_connectivity));
    }
    return get_net_handle_from_values(get_record_offset(net), new_connectivity, get_handle_type(net), get_node_record_offset(net));
}

net_handle_t SnarlDistanceIndex::canonical(const net_handle_t& net) const {
    SnarlTreeRecord record(net, &snarl_tree_records);
    record_t type = record.get_record_type();
    if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        return get_root();
    }


    connectivity_t connectivity;
    if (record.is_start_end_connected()) {
        connectivity = START_END;
    } else if (record.is_start_tip_connected()) {
        connectivity = START_TIP;
    } else if (record.is_end_tip_connected()) {
        connectivity = END_TIP;
    } else if (record.is_start_start_connected()) {
        connectivity = START_START;
    } else if (record.is_end_end_connected()) {
        connectivity = END_END;
    } else if (record.is_tip_tip_connected()) {
        connectivity = TIP_TIP;
    } else {
        connectivity = START_END; 
    }
    return get_net_handle_from_values(get_record_offset(net), connectivity, get_handle_type(net), get_node_record_offset(net));
}

net_handle_t SnarlDistanceIndex::start_end_traversal_of(const net_handle_t& net) const {
    return get_net_handle_from_values(get_record_offset(net), START_END, get_handle_type(net), get_node_record_offset(net));
}

SnarlDecomposition::endpoint_t SnarlDistanceIndex::starts_at(const net_handle_t& traversal) const {
    return get_start_endpoint(get_connectivity(traversal));

}
SnarlDecomposition::endpoint_t SnarlDistanceIndex::ends_at(const net_handle_t& traversal) const {
    return get_end_endpoint( get_connectivity(traversal));
}

size_t SnarlDistanceIndex::get_rank_in_parent(const net_handle_t& net) const {
    size_t tag = snarl_tree_records->at(get_record_offset(net));
    if (get_record_type(tag) == TRIVIAL_SNARL ||
        get_record_type(tag) == DISTANCED_TRIVIAL_SNARL) {
        return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(net));
    } else if (get_record_type(tag) == SIMPLE_SNARL ||
        get_record_type(tag) == DISTANCED_SIMPLE_SNARL) {
        if (is_snarl(net)) {
            return get_record_offset(net);
        } else {
            return get_node_record_offset(net);
        }
    } else {
        return SnarlTreeRecord(net, &snarl_tree_records).get_rank_in_parent();
    }
}

size_t SnarlDistanceIndex::connected_component_count() const {
    return RootRecord (get_root(), &snarl_tree_records).get_connected_component_count();
}

net_handle_t SnarlDistanceIndex::get_snarl_child_from_rank(const net_handle_t& snarl, const size_t& rank) const {
#ifdef debug_distances
    assert(is_snarl(snarl));
#endif
    if (rank == 0) {
        return get_bound(snarl, false, true);
    } else if (rank == 1) {
        return get_bound(snarl, true, true);
    } else if (is_simple_snarl(snarl) ){
        return SimpleSnarlRecord(snarl, &snarl_tree_records).get_child_from_rank(rank);
    } else {
        //Ranks for children of snarls start from 2 since 0 and 1 are reserved for the bounds
#ifdef debug_distances
        assert(rank-2 < get_node_count());
#endif
        if (rank == 0) {
            //The boundary node. This technically shouldn't be here because boundary nodes aren't children
            //of the snarl but I'll leave it anyway because I will probably use it
            //Returns the bound facing in
            return get_bound(snarl, false, true);
        } else if ( rank == 1) {
            //The end bound
            return get_bound(snarl, true, true);
        } else {
            //TODO: Since I changed the ranks of snarls to be in a topological order, this is now slow
            net_handle_t child_handle;
            for_each_child(snarl, [&](const net_handle_t& child) {
                if (get_rank_in_parent(child) == rank) {
                    child_handle = child;
                    return false;
                }
                return true;
            });
            return child_handle;
        }
    }
}

bool SnarlDistanceIndex::has_distances(const net_handle_t& net) const {
    return has_distances(SnarlTreeRecord(net, &snarl_tree_records).get_record_type()); 
}

bool SnarlDistanceIndex::has_distances() const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    return has_distances(get_node_net_handle(root_record.get_min_node_id())); 
}

bool SnarlDistanceIndex::for_each_child_impl(const net_handle_t& traversal, const std::function<bool(const net_handle_t&)>& iteratee) const {
#ifdef debug_snarl_traversal
    cerr << "Go through children of " << net_handle_as_string(traversal) << endl;
#endif
    //What is this according to the snarl tree
    net_handle_record_t record_type = SnarlTreeRecord(traversal, &snarl_tree_records).get_record_handle_type();
    //What is this according to the handle 
    //(could be a trivial chain but actually a node according to the snarl tree)
    net_handle_record_t handle_type = get_handle_type(traversal);
    if (record_type == ROOT_HANDLE) {
        RootRecord root_record(get_root(), &snarl_tree_records);
        return root_record.for_each_child(iteratee);
    } else if (SnarlTreeRecord(traversal, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
        SnarlTreeRecord(traversal, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL ) {
        //If this is a simple snarl then it is a bit different
        if (handle_type == CHAIN_HANDLE) {
            //If the handle thinks it's a chain, then it is a trivial chain in the snarl so we do
            //iteratee for the node
            return iteratee(get_net_handle_from_values(get_record_offset(traversal), get_connectivity(traversal), 
                                           NODE_HANDLE, get_node_record_offset(traversal)));
        } else if (handle_type == SNARL_HANDLE) {
            return SimpleSnarlRecord(traversal, &snarl_tree_records).for_each_child(iteratee);
        } else { 
            throw runtime_error("error: Looking for children of a node or sentinel in a simple snarl");
        }
    } else if (record_type == SNARL_HANDLE) {
        SnarlRecord snarl_record(traversal, &snarl_tree_records);
        return snarl_record.for_each_child(iteratee);
    } else if (record_type == CHAIN_HANDLE) {
        ChainRecord chain_record(traversal, &snarl_tree_records);
        return chain_record.for_each_child(iteratee);
    } else  if (record_type == NODE_HANDLE && handle_type == CHAIN_HANDLE) {
        //This is actually a node but we're pretending it's a chain
#ifdef debug_snarl_traversal
        cerr << "     which is actually a node pretending to be a chain" << endl;
#endif
        return iteratee(get_net_handle_from_values(get_record_offset(traversal), get_connectivity(traversal), NODE_HANDLE));
    } else {
        throw runtime_error("error: Looking for children of a node or sentinel");
    }
   
}

bool SnarlDistanceIndex::for_each_traversal_impl(const net_handle_t& item, const std::function<bool(const net_handle_t&)>& iteratee) const {
    if (get_handle_type(item) == SENTINEL_HANDLE) {
        if (!iteratee(get_net_handle_from_values(get_record_offset(item), START_END, get_handle_type(item), get_node_record_offset(item)))) {
            return false;
        }
        if (!iteratee(get_net_handle_from_values(get_record_offset(item), END_START, get_handle_type(item), get_node_record_offset(item)))) {
            return false;
        }
        return true;
    }
    SnarlTreeRecord record(item, &snarl_tree_records);
    for ( size_t type = 1 ; type <= 9 ; type ++ ){
        connectivity_t connectivity = static_cast<connectivity_t>(type);
        if (record.has_connectivity(connectivity)) {
            if (!iteratee(get_net_handle_from_values(get_record_offset(item), connectivity, get_handle_type(item), get_node_record_offset(item)))) {
                return false;
            }
        }
    }
    return true;
}

bool SnarlDistanceIndex::follow_net_edges_impl(const net_handle_t& here, const handlegraph::HandleGraph* graph, bool go_left, const std::function<bool(const net_handle_t&)>& iteratee) const {
#ifdef debug_snarl_traversal
    cerr << "following edges from " << net_handle_as_string(here) << " going " << (go_left ? "rev" : "fd") << endl;
    cerr << "        that is a child of " << net_handle_as_string(get_parent(here)) << endl;
#endif

    SnarlTreeRecord this_record(here, &snarl_tree_records);
    SnarlTreeRecord parent_record (get_parent(here), &snarl_tree_records);

    if (parent_record.get_record_handle_type() == ROOT_HANDLE &&
        parent_record.get_record_type() != ROOT_SNARL &&
        parent_record.get_record_type() != DISTANCED_ROOT_SNARL) {
#ifdef debug_snarl_traversal
        cerr << "The parent is a root so just check self connectivity" << endl;
#endif
        //If this is a root-level chain or node
        if ((go_left && starts_at(here) == END) || (!go_left && ends_at(here) == END)) {
            //Follow edges leaving the root structure at the end
            if (this_record.is_externally_start_end_connected()) {
                //Follow edge from end to start
                if (!iteratee(get_net_handle_from_values(get_record_offset(here), START_END, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from end back to the end
                if (!iteratee(get_net_handle_from_values(get_record_offset(here), END_START, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
        } else {
            //Follow edges leaving the root structure at the end
            if (this_record.is_externally_start_end_connected()) {
                //Follow edge from start to end
                if (!iteratee(get_net_handle_from_values(get_record_offset(here), END_START, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from the start back to the start
                if (!iteratee(get_net_handle_from_values(get_record_offset(here), START_END, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }

        }
        return true;

    } else if (get_handle_type(here) == CHAIN_HANDLE || get_handle_type(here) == SENTINEL_HANDLE) {
#ifdef debug_distances
        cerr << "Parent is a snarl (or root snarl)" << endl;
        assert(parent_record.get_record_handle_type() == SNARL_HANDLE ||
               parent_record.get_record_handle_type() == ROOT_HANDLE);
        //It could also be the root
#endif
        //If this is a chain (or a node pretending to be a chain) and it is the child of a snarl
        //Or if it is the sentinel of a snarl, then we walk through edges in the snarl
        //It can either run into another chain (or node) or the boundary node
        bool is_root_snarl = parent_record.get_record_type() == ROOT_SNARL
                           || parent_record.get_record_type() == DISTANCED_ROOT_SNARL;


        //Get the graph handle for the end node of whatever this is, pointing in the right direction
        handle_t graph_handle;
        if (get_handle_type(here) == SENTINEL_HANDLE) {
            if ((get_connectivity(here) == START_END && !go_left) ||
                (get_connectivity(here) == START_START && go_left)) {
                //If it's going into the snarl from the start node
                graph_handle = graph->get_handle(parent_record.get_start_id(), parent_record.get_start_orientation());
            } else if ((get_connectivity(here) == END_START && !go_left) ||
                       (get_connectivity(here) == END_END && go_left)) {
                //If it's going into the snarl from the end node
                graph_handle = graph->get_handle(parent_record.get_end_id(), !parent_record.get_end_orientation());
            } else {
                //This is facing out, so don't do anything 
                return true;
            }
        } else if (get_handle_type(here) == NODE_HANDLE ||is_trivial_chain(here)) {
            nid_t id = node_id(here);
            graph_handle = graph->get_handle(id, go_left ? starts_at(here) == START : ends_at(here) == START);
        } else {
            //Get the bound of the snarl or chain. If the traversal ends (or, if go_left is true, starts) at the
            //start, then go left from here, otherwise go right (forwards)
            graph_handle = get_handle(get_bound(here, go_left ? starts_at(here) != START : ends_at(here) != START, false), graph);
        }
#ifdef debug_snarl_traversal
        cerr << "        traversing graph from actual node " << graph->get_id(graph_handle) << (graph->get_is_reverse(graph_handle) ? "rev" : "fd") << endl;
#endif
        graph->follow_edges(graph_handle, false, [&](const handle_t& h) {
#ifdef debug_snarl_traversal
            cerr << "  reached actual node " << graph->get_id(h) << (graph->get_is_reverse(h) ? "rev" : "fd") << endl;
#endif

            if (!is_root_snarl && graph->get_id(h) == parent_record.get_start_id()) {
                //If this is the start boundary node of the parent snarl, then do this on the sentinel
#ifdef debug_snarl_traversal
                cerr << "    -> start of parent " << endl;
                assert(graph->get_is_reverse(h) == !parent_record.get_start_orientation());
#endif
                return iteratee(get_bound(get_parent(here), false, false));
            } else if (!is_root_snarl && graph->get_id(h) == parent_record.get_end_id()) {
#ifdef debug_snarl_traversal
                assert(graph->get_is_reverse(h) == parent_record.get_end_orientation());
                cerr << "    -> end of parent " << endl;
#endif
                return iteratee(get_bound(get_parent(here), true, false));
            } else {
                //It is either another chain or a node, but the node needs to pretend to be a chain
                net_handle_t node_net_handle = get_node_net_handle(graph->get_id(h));
                if (graph->get_is_reverse(h)) {
                    node_net_handle = flip(node_net_handle);
                }

                if (get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == NODE || 
                    get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == DISTANCED_NODE   ) {
                    //If this is a node make a net_handle_t of a node pretending to be a chain
                    net_handle_t next_net = get_net_handle_from_values(get_record_offset(node_net_handle), 
                                                           graph->get_is_reverse(h) ? END_START : START_END, 
                                                           CHAIN_HANDLE);
#ifdef debug_snarl_traversal
                cerr << "    -> actual child node " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                } else if (get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == SIMPLE_SNARL || 
                           get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == DISTANCED_SIMPLE_SNARL ) {
                    //If the node is a node in a simple snarl
                    net_handle_t next_net = get_net_handle_from_values(get_record_offset(node_net_handle), 
                                                           graph->get_is_reverse(h) ? END_START : START_END, 
                                                           CHAIN_HANDLE,
                                                           get_node_record_offset(node_net_handle));
#ifdef debug_snarl_traversal
                cerr << "    -> actual child node " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                } else {
                    //next_node_record is also the start of a chain
                    bool rev = node_net_handle != get_bound(get_parent(node_net_handle), false, true);
                    net_handle_t next_net = get_net_handle_from_values(get_record_offset(get_parent(node_net_handle)), 
                                                           rev ? END_START : START_END, 
                                                           CHAIN_HANDLE);
#ifdef debug_snarl_traversal
                    assert(SnarlTreeRecord(get_parent(node_net_handle), &snarl_tree_records).get_record_handle_type() == CHAIN_HANDLE);
                   // assert(get_node_id_from_offset(next_node_record.record_offset) 
                   //     == SnarlTreeRecord(next_node_record.get_parent_record_offset(), &snarl_tree_records).get_start_id() || 
                   //     get_node_id_from_offset(next_node_record.record_offset) 
                   //     == SnarlTreeRecord(next_node_record.get_parent_record_offset(), &snarl_tree_records).get_end_id());
                cerr << "    -> child chain " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                }
            }
            return false;
        });

        
    } else if (get_handle_type(here) == SNARL_HANDLE || get_handle_type(here) == NODE_HANDLE) {
#ifdef debug_snarl_traveral
        assert(parent_record.get_record_handle_type() == CHAIN_HANDLE);
#endif
        if (get_record_offset(here) == parent_record.record_offset) {
            //If this is a trivial chain
            return true;
        }
        //If this is a snarl or node, then it is the component of a (possibly pretend) chain
        ChainRecord parent_chain(this_record.get_parent_record_offset(), &snarl_tree_records);
        bool go_left_in_chain = go_left ? starts_at(here) == START : ends_at(here) == START;
        bool is_rev = is_node(here) ? TrivialSnarlRecord(get_record_offset(here), &snarl_tree_records).get_is_reversed_in_parent(get_node_record_offset(here))
                                    : false;
        if (is_rev) {
            go_left_in_chain = !go_left_in_chain;
        }
        net_handle_t next_net = parent_chain.get_next_child(here, go_left_in_chain);
        if (next_net == here ) {
            //If this is the end of the chain
            return true;
        }
        return iteratee(next_net);
        
    }
    return true;
}


net_handle_t SnarlDistanceIndex::get_parent_traversal(const net_handle_t& traversal_start, const net_handle_t& traversal_end) const {
    
    net_handle_record_t start_handle_type = get_handle_type(traversal_start);
    net_handle_record_t end_handle_type = get_handle_type(traversal_end);

    if (start_handle_type == SENTINEL_HANDLE) {
        //these are the sentinels of a snarl
        assert(end_handle_type == SENTINEL_HANDLE);
        endpoint_t start_endpoint = get_start_endpoint(get_connectivity(traversal_start));
        endpoint_t end_endpoint = get_start_endpoint(get_connectivity(traversal_end));
        return get_net_handle_from_values(get_record_offset(get_parent(traversal_start)), 
                              endpoints_to_connectivity(start_endpoint, end_endpoint),
                              SNARL_HANDLE, get_node_record_offset(traversal_start));
    } else {
        //These are the endpoints or tips in a chain
        SnarlTreeRecord start_record = get_snarl_tree_record(traversal_start);
        SnarlTreeRecord end_record = get_snarl_tree_record(traversal_end);
#ifdef debug_snarl_traversal
        if (start_record.get_parent_record_offset() != end_record.get_parent_record_offset()) {
            throw runtime_error("error: Looking for parent traversal of two non-siblings");
        }
#endif
        SnarlTreeRecord parent_record (start_record.get_parent_record_offset(), &snarl_tree_records);
#ifdef debug_snarl_traversal
        assert(parent_record.get_record_handle_type() == CHAIN_HANDLE);
#endif

        //Figure out what the start and end of the traversal are
        endpoint_t start_endpoint;
        if (start_handle_type == NODE_HANDLE && 
            node_id(traversal_start) == parent_record.get_start_id() &&
            (get_start_endpoint(traversal_start) == START && !parent_record.get_start_orientation() ||
             get_start_endpoint(traversal_start) == END && parent_record.get_start_orientation()) ){
            //If traversal_start is a node and is also the start node oriented into the parent
            start_endpoint = START;
    
        } else if (start_handle_type == NODE_HANDLE && 
            node_id(traversal_start) == parent_record.get_end_id() &&
            (get_start_endpoint(traversal_start) == START && parent_record.get_end_orientation() ||
             get_start_endpoint(traversal_start) == END && !parent_record.get_end_orientation()) ){
            //If traversal_start is a node and also the end node and oriented going into the parent
            start_endpoint = END;
    
        } else if (start_handle_type == SNARL_HANDLE) {
            start_endpoint = TIP;
        } else {
            throw runtime_error("error: trying to get an invalid traversal of a chain");
        }
    
        endpoint_t end_endpoint;
        if (end_handle_type == NODE_HANDLE && 
            node_id(traversal_end) == parent_record.get_start_id() &&
            (get_start_endpoint(traversal_end) == START && parent_record.get_start_orientation() ||
             get_start_endpoint(traversal_end) == END && !parent_record.get_start_orientation())){
            //If traversal_end is a node and also the start node oriented out of the parent
            end_endpoint = START;
        } else if (end_handle_type == NODE_HANDLE && 
            node_id(traversal_end) == parent_record.get_end_id() &&
            (get_start_endpoint(traversal_end) == START && !parent_record.get_end_orientation() ||
             get_start_endpoint(traversal_end) == END && parent_record.get_end_orientation()) ){
            //If traversal_end is a node and also the end node oriented out of the parent
            end_endpoint = END;
        } else if (end_handle_type == SNARL_HANDLE) {
            end_endpoint = TIP;
        } else {
            throw runtime_error("error: trying to get an invalid traversal of a chain");
        }
#ifdef debug_snarl_traversal

        if (!parent_record.has_connectivity(start_endpoint, end_endpoint)) {
            throw runtime_error("error: Trying to get parent traversal that is not connected");
        }
#endif

        return get_net_handle_from_values(parent_record.record_offset, 
                              endpoints_to_connectivity(start_endpoint, end_endpoint), 
                              CHAIN_HANDLE);
    }


}


//Serialize and deserialize from TriviallySerializable will just call save and load

void SnarlDistanceIndex::dissociate() {
    snarl_tree_records.dissociate();
    snarl_tree_records.reset();
}
void SnarlDistanceIndex::serialize(const std::function<void(const void*, size_t)>& iteratee)  const {
    snarl_tree_records.save(iteratee);
}
void SnarlDistanceIndex::serialize(int fd) {
    //This gets called by TriviallySerializable::serialize(filename), which doesn't
    //write the prefix so it must be written here
    snarl_tree_records.save(fd);
}
void SnarlDistanceIndex::deserialize(int fd) {
    //This gets called by TriviallySerializable::deserialize(filename), which
    //doesn't check for the prefix, so this should expect it
    snarl_tree_records.load(fd, get_prefix());
}

void SnarlDistanceIndex::serialize_members(std::ostream& out) const {
    //This gets called by Serializable::serialize(ostream), which writes the prefix
    //so don't write the prefix here
    snarl_tree_records.save_after_prefix(out, get_prefix());
}
void SnarlDistanceIndex::deserialize_members(std::istream& in){
    //This gets called by Serializable::deserialize(istream), which has already
    //read the prefix, so don't expect the prefix
    snarl_tree_records.load_after_prefix(in, get_prefix());
}

uint32_t SnarlDistanceIndex::get_magic_number()const {
    //random number?
    return magic_number;
}
//Copied from MappedPackedGraph
std::string SnarlDistanceIndex::get_prefix() const {
    //return the magic number as a string

    //Reinterpret the magic number as a char array (with 4 chars) 
    uint32_t number = htonl(magic_number);
    const char* as_chars = reinterpret_cast<const char*> (&number);
    std::string as_string(as_chars, as_chars+4);
    return as_string;
}

void SnarlDistanceIndex::preload(bool blocking) const {
    snarl_tree_records.preload(blocking);
}

size_t SnarlDistanceIndex::distance_in_parent(const net_handle_t& parent, 
        const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph, size_t distance_limit) const {

#ifdef debug_distances
    cerr << "\t\tFind distance between " << net_handle_as_string(child1) 
         << " and " << net_handle_as_string(child2) 
         << "\tin parent " << net_handle_as_string(parent) << endl;
    assert(canonical(parent) == canonical(get_parent(child1)));
    assert(canonical(parent) == canonical(get_parent(child2)));
#endif

    //Get the orientation of the children. This only cares about the end endpoint, and assumes that things that end
    //in tips are start-end, meaning that they don't end at the start
    bool child_ends_at_start1 = ends_at(child1) == START;
    bool child_ends_at_start2 = ends_at(child2) == START;

    if (is_root(parent)) {
        //If the parent is the root, then the children must be in the same root snarl for them to be
        //connected
        size_t parent_record_offset1 = get_record_offset(get_parent(child1));
        size_t parent_record_offset2 = get_record_offset(get_parent(child2));

        if (parent_record_offset1 != parent_record_offset2) {
            //If the children are in different connected components
#ifdef debug_distances
            cerr << "=>The children are in different root components" << endl;
#endif
            return std::numeric_limits<size_t>::max();
        } else if (get_record_type(snarl_tree_records->at(parent_record_offset1)) != DISTANCED_ROOT_SNARL){
            //If they are in the same connected component, but it is not a root snarl
#ifdef debug_distances
            cerr << "=>They are in a connected component of the root; checking external connectivity" << endl;
#endif
            if (get_record_offset(child1) == get_record_offset(child2)) {
                //If they are the same child of the root but not in a snarl, then check the external connectivity
                if (child_ends_at_start1 && child_ends_at_start2) {
                    return has_external_connectivity(child1, START, START) ? 0 : std::numeric_limits<size_t>::max();
                } else if (!child_ends_at_start1 && !child_ends_at_start2) {
                    return has_external_connectivity(child1, END, END) ? 0 : std::numeric_limits<size_t>::max();
                } else if ((child_ends_at_start1 && !child_ends_at_start2) ||
                            (!child_ends_at_start1 && child_ends_at_start2)) {
                    if (has_external_connectivity(child1, START, END)) {
                        //If we can take an edge around the snarl
                        return 0;
                    } else if (has_external_connectivity(child1, START, START) && has_external_connectivity(child1, END, END)) {
                        //If we can take the loops on the two ends of the snarl, walk through the snarl
                        return minimum_length(child1);
                    }
                }             
            }
            return std::numeric_limits<size_t>::max();
        } else {
#ifdef debug_distances
            cerr << "=>They are in a snarl, check distance in snarl" << endl;
            cerr << "\tsnarl at offset " << parent_record_offset1 << " with ranks " << get_rank_in_parent(child1) << " " << get_rank_in_parent(child2) << endl;
#endif                                                                                 
            //They are in the same root snarl, so find the distance between them
            SnarlRecord snarl_record(parent_record_offset1, &snarl_tree_records);

            return snarl_record.get_distance(get_rank_in_parent(child1), !child_ends_at_start1, 
                                             get_rank_in_parent(child2), !child_ends_at_start2);
        }


    } else if (is_chain(parent)) {
        if (get_record_handle_type(get_record_type(snarl_tree_records->at(get_record_offset(parent)))) == NODE_HANDLE ||
            get_record_handle_type(get_record_type(snarl_tree_records->at(get_record_offset(parent)))) == SNARL_HANDLE) {
            return std::numeric_limits<size_t>::max();
        }
        ChainRecord chain_record(parent, &snarl_tree_records);
#ifdef debug_distances
        assert(is_node(child1) || is_snarl(child1));
        assert(is_node(child2) || is_snarl(child2));
#endif

        //These are the values needed for calculating the distance in the chain
        //They are:
        //rank in chain, go_left, node_length, prefix_sum, forward_loop, reverse_loop, component
        size_t rank_in_chain1; size_t rank_in_chain2;
        bool go_left1; bool go_left2;
        size_t node_length1; size_t node_length2;
        size_t prefix_sum1; size_t prefix_sum2;
        size_t forward_loop1 ; size_t forward_loop2;
        size_t reverse_loop1 ; size_t reverse_loop2;
        size_t component1; size_t component2;
        size_t end_component1; size_t end_component2;

        //The node lengths of snarl boundaries, to be added to the total distance
        size_t node_lengths_to_add = 0;

        //Get all the relevant chain values
        for (size_t i = 0 ; i <=1 ; i ++ ) {
            const net_handle_t& child = i == 0 ? child1 : child2;
            size_t& rank_in_chain= i == 0 ? rank_in_chain1: rank_in_chain2;
            bool& go_left_in_chain = i == 0 ? go_left1  : go_left2;
            size_t& node_length  = i == 0 ? node_length1  : node_length2;
            size_t& prefix_sum   = i == 0 ? prefix_sum1   : prefix_sum2;
            size_t& forward_loop = i == 0 ? forward_loop1 : forward_loop2;
            size_t& reverse_loop = i == 0 ? reverse_loop1 : reverse_loop2;
            size_t& component    = i == 0 ? component1    : component2;
            size_t& end_component    = i == 0 ? end_component1    : end_component2;
            bool& child_ends_at_start = i == 0 ? child_ends_at_start1 : child_ends_at_start2;

            if (is_node(child)) {

                bool go_left = is_reversed_in_parent(child);
                if (child_ends_at_start){
                    go_left = !go_left;
                }
                rank_in_chain=get_rank_in_parent(child);
                go_left_in_chain = go_left;
                node_length = minimum_length(child);
                std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                    TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_chain_values(get_node_record_offset(child));
                end_component = TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_chain_component(get_node_record_offset(child), true);

                
            } else {
                //If this is a snarl, then find the appropriate boundary node and fill it in
                assert(is_snarl(child));
                if (child_ends_at_start) {
                    //If we're going backwards in the chain, then we want the start node
                    //And turn it into a node, since it will be a sentinel
                    net_handle_t start_bound = get_node_from_sentinel(get_bound(child, false, false));

                    size_t start_length = minimum_length(start_bound);

                        //set values for first
                    rank_in_chain=get_record_offset(start_bound) + get_node_record_offset(start_bound);
                    go_left_in_chain = true;
                    node_length = start_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                        TrivialSnarlRecord (get_record_offset(start_bound), &snarl_tree_records).get_chain_values(get_node_record_offset(start_bound));

                    node_lengths_to_add += start_length;
                } else {
                    //Do the same thing for the snarl end node if we're going forwards
                    net_handle_t end_bound = get_node_from_sentinel(get_bound(child, true, false));
                    size_t end_length = minimum_length(end_bound);
                    size_t end_rank = get_record_offset(end_bound) + get_node_record_offset(end_bound);

                    //set values for first
                    rank_in_chain=end_rank;
                    go_left_in_chain = false;
                    node_length = end_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) =
                            TrivialSnarlRecord (get_record_offset(end_bound), &snarl_tree_records).get_chain_values(get_node_record_offset(end_bound));

                    node_lengths_to_add += end_length;
                }
            }
        }

        if (is_node(child1) != is_node(child2) && rank_in_chain1 == rank_in_chain2 &&
               go_left1 != go_left2) {
            //If one child is a node and the other is a snarl, and the node is the boundary of the snarl
            //pointing into the snarl, then the distance is 0
            return 0;
        } else if (!is_node(child1) && !is_node(child2) && rank_in_chain1 == rank_in_chain2 &&
                go_left1 != go_left2) {
                //If the snarls are adjacent (and not the same snarl)
            return node_length2;//return the node length
        }

        return sum(chain_record.get_distance(rank_in_chain1, go_left1,
                                              node_length1, prefix_sum1,
                                              forward_loop1, reverse_loop1,
                                              component1, component1, 
                                              rank_in_chain2, go_left2,
                                              node_length2, prefix_sum2,
                                              forward_loop2, reverse_loop2,
                                              component2, component2),
                    node_lengths_to_add);

    } else if (is_snarl(parent)) {
        bool snarl_is_root = is_root_snarl(parent);
        size_t rank1, rank2; bool rev1, rev2;
        if (is_sentinel(child1)) {
            rank1 = starts_at(child1) == START ? 0 : 1;
            rev1 = false;
        } else {
            rank1 = get_rank_in_parent(child1);
            rev1 = !child_ends_at_start1;
        }
        if (is_sentinel(child2)) {
            rank2 = starts_at(child2) == START ? 0 : 1;
            rev2 = false;
        } else {
            rank2 = get_rank_in_parent(child2);
            rev2 = !child_ends_at_start2;
        }
        if ((is_sentinel(child1) && starts_at(child1) == ends_at(child1)) ||
            (is_sentinel(child2) && starts_at(child2) == ends_at(child2)) ) {
            //If this is a sentinel pointing out of the snarl
#ifdef debug_distances
            cerr << "            => " << std::numeric_limits<size_t>::max() << endl;
#endif
            return std::numeric_limits<size_t>::max();
        }

#ifdef debug_distances
            cerr << "             between ranks " << rank1 << " " << rev1 << " " << rank2 << " " << rev2 << endl;
#endif

        if (get_record_type(snarl_tree_records->at(get_record_offset(parent))) == DISTANCED_SIMPLE_SNARL) {
            return SimpleSnarlRecord(parent, &snarl_tree_records).get_distance(rank1, rev1, rank2, rev2);
        } else if (get_record_type(snarl_tree_records->at(get_record_offset(parent))) == OVERSIZED_SNARL 
            && !(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) ) {
            //If this is an oversized snarl and we're looking for internal distances, then we didn't store the
            //distance and we have to find it using dijkstra's algorithm
            if (graph == nullptr) {
                if (size_limit_warnings.load() < max_num_size_limit_warnings) {
                    int warning_num = const_cast<SnarlDistanceIndex*>(this)->size_limit_warnings++;
                    if (warning_num < max_num_size_limit_warnings) {
                        std::string msg = "warning: trying to find the distance in an oversized snarl without a graph. Returning inf\n";
                        if (warning_num + 1 == max_num_size_limit_warnings) {
                            msg += "suppressing further warnings\n";
                        }
                        std::cerr << msg;
                    }
                }
                return std::numeric_limits<size_t>::max();
            }
            handle_t handle1 = is_node(child1) ? get_handle(child1, graph) : get_handle(get_bound(child1, !child_ends_at_start1, false), graph); 
            handle_t handle2 = is_node(child2) ? get_handle(child2, graph) : get_handle(get_bound(child2, !child_ends_at_start2, false), graph);
            handle2 = graph->flip(handle2);

            size_t distance = std::numeric_limits<size_t>::max();
            handlegraph::algorithms::dijkstra(graph, handle1, [&](const handle_t& reached, size_t dist) {
                if (reached == handle2) {
                    distance = dist;
                    return false;
                } else if (dist > distance_limit) {
                    distance = std::numeric_limits<size_t>::max();
                    return false;
                }
                return true;
            }, false);
            return distance;

            
        } else if (rank1 == 0 && rank2 == 0 && !snarl_is_root) {
            //Start to start is stored in the snarl
            return SnarlRecord(parent, &snarl_tree_records).get_distance_start_start();
        } else if ((rank1 == 0 && rank2 == 1) || (rank1 == 1 && rank2 == 0) && !snarl_is_root) {
            //start to end / end to start is stored in the snarl
            return SnarlRecord(parent, &snarl_tree_records).get_min_length();
        } else if (rank1 == 1 && rank2 == 1 && !snarl_is_root) {
            //end to end is stored in the snarl
            return SnarlRecord(parent, &snarl_tree_records).get_distance_end_end();
        } else if ((rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) && !snarl_is_root) {
            //If one node is a boundary and the other is a child
            size_t boundary_rank = (rank1 == 0 || rank1 == 1) ? rank1 : rank2;
            const net_handle_t& internal_child = (rank1 == 0 || rank1 == 1) ? child2 : child1;
            bool internal_is_reversed = (rank1 == 0 || rank1 == 1) ? rev2 : rev1;
            if (is_trivial_chain( internal_child) ) {
                //Child is just a node pretending to be a chain
                if (boundary_rank == 0 && !internal_is_reversed) {
                    //Start to left of child
                    return NodeRecord(internal_child, &snarl_tree_records).get_distance_left_start();
                } else if (boundary_rank == 0 && internal_is_reversed) {
                    //Start to right of child
                    return NodeRecord(internal_child, &snarl_tree_records).get_distance_right_start();
                } else if (boundary_rank == 1 && !internal_is_reversed) {
                    //End to left of child
                    return NodeRecord(internal_child, &snarl_tree_records).get_distance_left_end();
                } else {
                    //End to right of child
                    return NodeRecord(internal_child, &snarl_tree_records).get_distance_right_end();
                }
            } else {
                //Child is an actual chain
                if (boundary_rank == 0 && !internal_is_reversed) {
                    //Start to left of child
                    return ChainRecord(internal_child, &snarl_tree_records).get_distance_left_start();
                } else if (boundary_rank == 0 && internal_is_reversed) {
                    //Start to right of child
                    return ChainRecord(internal_child, &snarl_tree_records).get_distance_right_start();
                } else if (boundary_rank == 1 && !internal_is_reversed) {
                    //End to left of child
                    return ChainRecord(internal_child, &snarl_tree_records).get_distance_left_end();
                } else {
                    //End to right of child
                    return ChainRecord(internal_child, &snarl_tree_records).get_distance_right_end();
                }
            }
        } else {
           return SnarlRecord(parent, &snarl_tree_records).get_distance(rank1, rev1, rank2, rev2);
        }
    } else {
        throw runtime_error("error: Trying to find distance in the wrong type of handle");
    }
}

size_t SnarlDistanceIndex::distance_in_snarl(const net_handle_t& parent, 
        const size_t& rank1, const bool& right_side1, const size_t& rank2, const bool& right_side2, 
        const HandleGraph* graph, size_t distance_limit) const {

    bool snarl_is_root = is_root_snarl(parent);
    
    if (get_record_type(snarl_tree_records->at(get_record_offset(parent))) == DISTANCED_SIMPLE_SNARL) {
        return SimpleSnarlRecord(parent, &snarl_tree_records).get_distance(rank1, right_side1, rank2, right_side2);
    } else if (get_record_type(snarl_tree_records->at(get_record_offset(parent))) == OVERSIZED_SNARL 
        && !(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) ) {
        //If this is an oversized snarl and we're looking for internal distances, then we didn't store the
        //distance and we have to find it using dijkstra's algorithm
        if (graph == nullptr) {
            if (size_limit_warnings.load() < max_num_size_limit_warnings) {
                int warning_num = const_cast<SnarlDistanceIndex*>(this)->size_limit_warnings++;
                if (warning_num < max_num_size_limit_warnings) {
                    std::string msg = "warning: Trying to find the distance in an oversized snarl with zip codes. Returning inf\n";
                    if (warning_num + 1 == max_num_size_limit_warnings) {
                        msg += "suppressing further warnings\n";
                    }
                    std::cerr << msg;
                }
            }
            return std::numeric_limits<size_t>::max();
        } else {
            net_handle_t net1 = get_snarl_child_from_rank(parent, rank1);  
            if (!right_side1) {
                net1 = flip(net1);
            }
            net_handle_t net2 = get_snarl_child_from_rank(parent, rank2);  
            if (right_side2) {
                net2 = flip(net2);
            }
            handle_t handle1 = get_handle(net1, graph); 
            handle_t handle2 = get_handle(net2, graph);

            size_t distance = std::numeric_limits<size_t>::max();
            handlegraph::algorithms::dijkstra(graph, handle1, [&](const handle_t& reached, size_t dist) {
                if (reached == handle2) {
                    distance = dist;
                    return false;
                } else if (dist > distance_limit) {
                    distance = std::numeric_limits<size_t>::max();
                    return false;
                }
                return true;
            }, false);
            return distance;
        }
        
    } else if (rank1 == 0 && rank2 == 0 && !snarl_is_root) {
        //Start to start is stored in the snarl
        return SnarlRecord(parent, &snarl_tree_records).get_distance_start_start();
    } else if (((rank1 == 0 && rank2 == 1) || (rank1 == 1 && rank2 == 0)) && !snarl_is_root) {
        //start to end / end to start is stored in the snarl
        return SnarlRecord(parent, &snarl_tree_records).get_min_length();
    } else if (rank1 == 1 && rank2 == 1 && !snarl_is_root) {
        //end to end is stored in the snarl
        return SnarlRecord(parent, &snarl_tree_records).get_distance_end_end();
    } else if ((rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) && !snarl_is_root) {
        //If one node is a boundary and the other is a child
        size_t boundary_rank = (rank1 == 0 || rank1 == 1) ? rank1 : rank2;
        const net_handle_t& internal_child = (rank1 == 0 || rank1 == 1) ? get_snarl_child_from_rank(parent, rank2) 
                                                                        : get_snarl_child_from_rank(parent, rank1);
        bool internal_is_reversed = (rank1 == 0 || rank1 == 1) ? right_side2 : right_side1;
        if (is_trivial_chain( internal_child) ) {
            //Child is just a node pretending to be a chain
            if (boundary_rank == 0 && !internal_is_reversed) {
                //Start to left of child
                return NodeRecord(internal_child, &snarl_tree_records).get_distance_left_start();
            } else if (boundary_rank == 0 && internal_is_reversed) {
                //Start to right of child
                return NodeRecord(internal_child, &snarl_tree_records).get_distance_right_start();
            } else if (boundary_rank == 1 && !internal_is_reversed) {
                //End to left of child
                return NodeRecord(internal_child, &snarl_tree_records).get_distance_left_end();
            } else {
                //End to right of child
                return NodeRecord(internal_child, &snarl_tree_records).get_distance_right_end();
            }
        } else {
            //Child is an actual chain
            if (boundary_rank == 0 && !internal_is_reversed) {
                //Start to left of child
                return ChainRecord(internal_child, &snarl_tree_records).get_distance_left_start();
            } else if (boundary_rank == 0 && internal_is_reversed) {
                //Start to right of child
                return ChainRecord(internal_child, &snarl_tree_records).get_distance_right_start();
            } else if (boundary_rank == 1 && !internal_is_reversed) {
                //End to left of child
                return ChainRecord(internal_child, &snarl_tree_records).get_distance_left_end();
            } else {
                //End to right of child
                return ChainRecord(internal_child, &snarl_tree_records).get_distance_right_end();
            }
        }
    } else {
       return SnarlRecord(get_record_offset(parent), &snarl_tree_records).get_distance(rank1, right_side1, rank2, right_side2);
    }

}
size_t SnarlDistanceIndex::max_distance_in_parent(const net_handle_t& parent, 
        const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph, size_t distance_limit) const {

#ifdef debug_distances
    cerr << "\t\tFind maximum distance between " << net_handle_as_string(child1) 
         << " and " << net_handle_as_string(child2) 
         << "\tin parent " << net_handle_as_string(parent) << endl;
    assert(canonical(parent) == canonical(get_parent(child1)));
    assert(canonical(parent) == canonical(get_parent(child2)));
#endif

    if (is_chain(parent)) {

        //Get the orientation of the children. This only cares about the end endpoint, and assumes that things that end
        //in tips are start-end, meaning that they don't end at the start
        bool child_ends_at_start1 = ends_at(child1) == START;
        bool child_ends_at_start2 = ends_at(child2) == START;

        if (get_record_handle_type(get_record_type(snarl_tree_records->at(get_record_offset(parent)))) == NODE_HANDLE ||
            get_record_handle_type(get_record_type(snarl_tree_records->at(get_record_offset(parent)))) == SNARL_HANDLE) {
            return std::numeric_limits<size_t>::max();
        }
        ChainRecord chain_record(parent, &snarl_tree_records);
#ifdef debug_distances
        assert(is_node(child1) || is_snarl(child1));
        assert(is_node(child2) || is_snarl(child2));
#endif

        //These are the values needed for calculating the distance in the chain
        //They are:
        //rank in chain, go_left, node_length, prefix_sum, forward_loop, reverse_loop, component
        size_t rank_in_chain1; size_t rank_in_chain2;
        bool go_left1; bool go_left2;
        size_t node_length1; size_t node_length2;
        size_t prefix_sum1; size_t prefix_sum2;
        size_t forward_loop1 ; size_t forward_loop2;
        size_t reverse_loop1 ; size_t reverse_loop2;
        size_t component1; size_t component2;
        size_t end_component1; size_t end_component2;

        //The node lengths of snarl boundaries, to be added to the total distance
        size_t node_lengths_to_add = 0;

        //Get all the relevant chain values
        for (size_t i = 0 ; i <=1 ; i ++ ) {
            const net_handle_t& child = i == 0 ? child1 : child2;
            size_t& rank_in_chain= i == 0 ? rank_in_chain1: rank_in_chain2;
            bool& go_left_in_chain = i == 0 ? go_left1  : go_left2;
            size_t& node_length  = i == 0 ? node_length1  : node_length2;
            size_t& prefix_sum   = i == 0 ? prefix_sum1   : prefix_sum2;
            size_t& forward_loop = i == 0 ? forward_loop1 : forward_loop2;
            size_t& reverse_loop = i == 0 ? reverse_loop1 : reverse_loop2;
            size_t& component    = i == 0 ? component1    : component2;
            size_t& end_component    = i == 0 ? end_component1    : end_component2;
            bool& child_ends_at_start = i == 0 ? child_ends_at_start1 : child_ends_at_start2;

            if (is_node(child)) {

                bool go_left = is_reversed_in_parent(child);
                if (child_ends_at_start){
                    go_left = !go_left;
                }
                rank_in_chain=get_rank_in_parent(child);
                go_left_in_chain = go_left;
                node_length = minimum_length(child);
                std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                    TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_chain_values(get_node_record_offset(child));
                prefix_sum = TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_max_prefix_sum(get_node_record_offset(child));
                end_component = TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_chain_component(get_node_record_offset(child), true);

                
            } else {
                //If this is a snarl, then find the appropriate boundary node and fill it in
                assert(is_snarl(child));
                if (child_ends_at_start) {
                    //If we're going backwards in the chain, then we want the start node
                    //And turn it into a node, since it will be a sentinel
                    net_handle_t start_bound = get_node_from_sentinel(get_bound(child, false, false));

                    size_t start_length = minimum_length(start_bound);

                        //set values for first
                    rank_in_chain=get_record_offset(start_bound) + get_node_record_offset(start_bound);
                    go_left_in_chain = true;
                    node_length = start_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                        TrivialSnarlRecord (get_record_offset(start_bound), &snarl_tree_records).get_chain_values(get_node_record_offset(start_bound));

                    prefix_sum = TrivialSnarlRecord (get_record_offset(start_bound), &snarl_tree_records).get_max_prefix_sum(get_node_record_offset(start_bound));
                    node_lengths_to_add += start_length;
                } else {
                    //Do the same thing for the snarl end node if we're going forwards
                    net_handle_t end_bound = get_node_from_sentinel(get_bound(child, true, false));
                    size_t end_length = minimum_length(end_bound);
                    size_t end_rank = get_record_offset(end_bound) + get_node_record_offset(end_bound);

                    //set values for first
                    rank_in_chain=end_rank;
                    go_left_in_chain = false;
                    node_length = end_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) =
                            TrivialSnarlRecord (get_record_offset(end_bound), &snarl_tree_records).get_chain_values(get_node_record_offset(end_bound));

                    prefix_sum = TrivialSnarlRecord (get_record_offset(end_bound), &snarl_tree_records).get_max_prefix_sum(get_node_record_offset(end_bound));

                    node_lengths_to_add += end_length;
                }
            }
        }

        if (is_node(child1) != is_node(child2) && rank_in_chain1 == rank_in_chain2 &&
               go_left1 != go_left2) {
            //If one child is a node and the other is a snarl, and the node is the boundary of the snarl
            //pointing into the snarl, then the distance is 0
            return 0;
        } else if (!is_node(child1) && !is_node(child2) && rank_in_chain1 == rank_in_chain2 &&
                go_left1 != go_left2) {
                //If the snarls are adjacent (and not the same snarl)
            return node_length2;//return the node length
        }

        return sum(chain_record.get_distance(rank_in_chain1, go_left1,
                                              node_length1, prefix_sum1,
                                              forward_loop1, reverse_loop1,
                                              component1, component1, 
                                              rank_in_chain2, go_left2,
                                              node_length2, prefix_sum2,
                                              forward_loop2, reverse_loop2,
                                              component2, component2),
                    node_lengths_to_add);

    } else {
        //If the parent isn't a chain, then just return the minimum
        return distance_in_parent(parent, child1, child2, graph, distance_limit);
    }
}

size_t SnarlDistanceIndex::distance_to_parent_bound(const net_handle_t& parent, bool to_start, net_handle_t child,
    tuple<net_handle_record_t, net_handle_record_t, net_handle_record_t, net_handle_record_t> parent_and_child_types) const {
    /* parent_and_child_types is a tuple of parent handle type, parent record type, child handle type, child record type
      * This is really just used to see if the parent and child are trivial chains, so it might not be exactly what 
      * the actual record is
      * */



    //Get the record and handle types of the parent and child
    bool has_handle_types = (parent_and_child_types != make_tuple(ROOT_HANDLE, ROOT_HANDLE, ROOT_HANDLE, ROOT_HANDLE));
    bool parent_is_chain = has_handle_types ? std::get<0>(parent_and_child_types) == CHAIN_HANDLE 
                                            : is_chain(parent);
    bool parent_is_trivial_chain = has_handle_types ? (std::get<0>(parent_and_child_types) == NODE_HANDLE && std::get<1>(parent_and_child_types) == CHAIN_HANDLE)
                                                    : is_trivial_chain(parent);
    bool child_is_trivial_chain = has_handle_types ? (std::get<2>(parent_and_child_types) == NODE_HANDLE && std::get<3>(parent_and_child_types) == CHAIN_HANDLE)
                                                    : is_trivial_chain(child);
    bool parent_is_snarl = has_handle_types ? std::get<0>(parent_and_child_types) == SNARL_HANDLE
                                            : is_snarl(parent);

    //Get the orientation of the child. This only cares about the end endpoint, and assumes that things that end
    //in tips are start-end, meaning that they don't end at the start
    bool child_ends_at_start = ends_at(child) == START;
    if (parent_is_trivial_chain) {
        //If this is a node pretending to be a chain and the parent is the trivial chain, 
        //then the child is always oriented forwards in the parent (because it is the parent) and the distance is 0 or inf
        if ((child_ends_at_start && to_start) ||
            (!child_ends_at_start && !to_start)) {
            //If the child is traversed backwards and we're going to the start
            //Or the child is traversed forwards and we're going to the end
            return 0;
        } else {
            return std::numeric_limits<size_t>::max();
        } 
    } else if (parent_is_snarl) {
        if (is_simple_snarl(parent)) {
            if (to_start) {
                if (child_ends_at_start != is_reversed_in_parent(child)) {
                    //If the child is going backwards, and it isn't reversed in the parent,
                    //or if the child is going forwards and it is reversed in the parent,
                    //then the distances to the start will be 0
                    return 0;
                } else {
                    return std::numeric_limits<size_t>::max();
                }
            } else {
                //to end of snarl
                if (child_ends_at_start == is_reversed_in_parent(child)) {
                    //If the child is going backwards and it is reversed in the parent
                    //or the child is going forwards and it isn't reversed in the parent
                    return 0;
                } else {
                    return std::numeric_limits<size_t>::max();
                }
            }
        } else {
            //Otherwise, the parent is a normal snarl and the child nodes/chains know the distance
            //to the ends of the snarl
    
            if (child_is_trivial_chain) {
                NodeRecord child_record (child, &snarl_tree_records);
                if (child_ends_at_start && to_start) {
                    return child_record.get_distance_left_start();
                } else if (!child_ends_at_start && to_start) {
                    return child_record.get_distance_right_start();
                } else if (child_ends_at_start && !to_start) { 
                    return child_record.get_distance_left_end();
                } else {
                    return child_record.get_distance_right_end();
                }
            } else {
                ChainRecord child_record (child, &snarl_tree_records);
                if (child_ends_at_start && to_start) {
                    return child_record.get_distance_left_start();
                } else if (!child_ends_at_start && to_start) {
                    return child_record.get_distance_right_start();
                } else if (child_ends_at_start && !to_start) { 
                    return child_record.get_distance_left_end();
                } else {
                    return child_record.get_distance_right_end();
                }
            }
        }


    } else {
        //Otherwise, the parent is a chain so get the bound and use distance_in_parent


        //The parent bound we want, facing in
        net_handle_t parent_bound = get_bound(parent, !to_start, true);

        if (parent_bound == flip(child)) {
            //If the child is the bound we want and pointing into it (out of the chain)
            return 0;
        }
        //The node length of the boundary node, only set for chains 
        size_t bound_length = !parent_is_chain ? 0 : minimum_length(parent_bound);

        if (parent_is_chain && !to_start){
            if (is_looping_chain(parent)){
                //If this is a looping chain and we want the end 
                bound_length = 0;
            }
        }
        
        return sum(bound_length, 
                    distance_in_parent(parent, parent_bound, child));
    }

}

tuple<nid_t, bool, bool> SnarlDistanceIndex::into_which_snarl(const nid_t& id, const bool& reverse) const {
    net_handle_t node = get_node_net_handle(id, reverse);
    net_handle_t parent = get_parent(node);

    if (is_trivial_chain(parent) || is_root(parent)) {
        //If the parent is a trivial chain, then it is a node child of a snarl
        return {0, false, false};
    } else {
        //Otherwise, the parent is an actual chain and this is the boundary node of a (possibly trivial) snarl

        //If the node is traversed forwards in the parent and we are going backwards (or the opposite),
        //then we are going backwards in the chain
        bool go_left_in_chain = is_reversed_in_parent(node) != reverse;
        ChainRecord chain_record (parent, &snarl_tree_records);
        net_handle_t next_child = chain_record.get_next_child(node, go_left_in_chain);
        if (is_snarl(next_child)) {
            //If the node points into a snarl
            net_handle_t start = get_node_from_sentinel(get_bound(next_child, false, true));
            return {node_id(start), ends_at(start) == START, false}; 
        } else if (next_child == node) {
            //If the node points out of the chain
            return {0, false, false};
        } else {
            //If the node points to another node (trivial snarl)
            //Assume that the trivial snarl is oriented forwards in the chain, so the start node
            //is whichever comes first in the chain
            if (go_left_in_chain) {
                //If we went left in the chain, then the start node of the snarl is the next node
                return {node_id(next_child), is_reversed_in_parent(next_child), true};
            } else {
                //If we went right in the chain, then the original node is the start node
                return {id, reverse, true};
            }
        }
    }
}




pair<net_handle_t, bool> SnarlDistanceIndex::lowest_common_ancestor(const net_handle_t& net1, const net_handle_t& net2) const {
    net_handle_t parent1 = net1;
    net_handle_t parent2 = net2;

    std::unordered_set<net_handle_t> net1_ancestors;
    while (!is_root(parent1)){
        net1_ancestors.insert(canonical(parent1));
        parent1 = get_parent(parent1);
    }

    while (net1_ancestors.count(canonical(parent2)) == 0 && !is_root(parent2)){
        //Go up until the parent2 matches something in the ancestors of net1
        //This loop will end because everything is in the same root eventually
        parent2 = get_parent(parent2);
    }

    bool is_connected = true;
    if (is_root(parent2) && is_root(parent1)){
        size_t parent_record_offset1 = SnarlTreeRecord(parent1, &snarl_tree_records).get_parent_record_offset();
        size_t parent_record_offset2 = SnarlTreeRecord(parent2, &snarl_tree_records).get_parent_record_offset();
        if (parent_record_offset1 != parent_record_offset2) {
            is_connected = false;
        }
    }
    return make_pair(canonical(parent2), is_connected);
}

size_t SnarlDistanceIndex::minimum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, 
                                            const handlegraph::nid_t id2, const bool rev2, const size_t offset2, 
                                            bool unoriented_distance, const HandleGraph* graph, 
                                            pair<vector<tuple<net_handle_t, int32_t, int32_t>>, vector<tuple<net_handle_t, int32_t, int32_t>>>* distance_traceback) const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    size_t max_node_id = root_record.get_min_node_id() + root_record.get_node_count();
    if (id1 < root_record.get_min_node_id() || id2 < root_record.get_min_node_id() ||
        id1 > max_node_id || id2 > max_node_id) {
        throw runtime_error("error: Looking for the minimum distance of a node that does not exist");
    }
    if (distance_traceback != nullptr) {
        //Clear the traceback
        distance_traceback->first.clear();
        distance_traceback->second.clear();
    }


#ifdef debug_distances
        cerr << endl;
        cerr << "Find the minimum distance between " << id1 << " " <<rev1 <<" " << offset1 << " and " << id2 << " " << rev2 << " " << offset2 << endl;
#endif


    /*Helper function to walk up the snarl tree
     * Given a net handle, its parent,  and the distances to the start and end of the handle, 
     * update the distances to reach the ends of the parent and update the handle and its parent
     * If the parent is a chain, then the new distances include the boundary nodes of the chain.
     * If it is a snarl, it does not*/
    auto update_distances = [&](net_handle_t& net, net_handle_t& parent, size_t& dist_start, size_t& dist_end, bool first_node) {
#ifdef debug_distances
        cerr << "     Updating distance from node " << net_handle_as_string(net) << " at parent " << net_handle_as_string(parent) << endl;
#endif

        if (is_trivial_chain(parent)) {
            //Don't update distances for the trivial chain
            return;
        } else if (is_simple_snarl(parent)) {
            //If it's a simple snarl just check if they should be reversed
            if (is_reversed_in_parent (net)) {
                size_t tmp = dist_start;
                dist_start = dist_end;
                dist_end = tmp;
            } 

            if (distance_traceback != nullptr) {
                //Add an entry for this node
                tuple<net_handle_t, int32_t, int32_t>* current_traceback;
                if (first_node) {
                    distance_traceback->first.emplace_back(parent, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                    current_traceback = &distance_traceback->first.back();
                } else {
                    distance_traceback->second.emplace_back(parent, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                    current_traceback = &distance_traceback->second.back();
                }


                //If we're keeping track of the traceback, then remember the distance to the start/end of the parent
                if (is_reversed_in_parent(net)) {
                    std::get<1>(*current_traceback) = 0; 
                    std::get<2>(*current_traceback) = std::numeric_limits<int32_t>::min();
                } else {
                    std::get<1>(*current_traceback) = std::numeric_limits<int32_t>::min();
                    std::get<2>(*current_traceback) = 0;
                }
            }
            return;
        }


        net_handle_t start_bound = get_bound(parent, false, true);
        net_handle_t end_bound = get_bound(parent, true, true);

        //The lengths of the start and end nodes of net
        //This is only needed if net is a snarl, since the boundary nodes are not technically part of the snarl
        size_t start_length = is_chain(parent) ? node_length(start_bound) : 0;
        size_t end_length = is_chain(parent) ? node_length(end_bound) : 0;

        //Get the distances from the bounds of the parent to the node we're looking at
        size_t distance_start_start = start_bound == net ? 0 
                : sum(start_length, distance_in_parent(parent, start_bound, flip(net), graph));
        size_t distance_start_end = start_bound == flip(net) ? 0 
                : sum(start_length, distance_in_parent(parent, start_bound, net, graph));
        size_t distance_end_start = end_bound == net ? 0 
                : sum(end_length, distance_in_parent(parent, end_bound, flip(net), graph));
        size_t distance_end_end = end_bound == flip(net) ? 0 
                : sum(end_length, distance_in_parent(parent, end_bound, net, graph));

        size_t distance_start = dist_start;
        size_t distance_end = dist_end; 

        if (distance_traceback != nullptr) {
            //Add an entry for this node
            tuple<net_handle_t, int32_t, int32_t>* current_traceback;
            if (first_node) {
                distance_traceback->first.emplace_back(parent, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                current_traceback = &distance_traceback->first.back();
            } else {
                distance_traceback->second.emplace_back(parent, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                current_traceback = &distance_traceback->second.back();
            }


            //If we're keeping track of the traceback, then remember the distance to the start/end of the parent

            if (sum(distance_start_start, distance_start) != std::numeric_limits<size_t>::max() &&
                sum(distance_start_start, distance_start) < sum(distance_start_end , distance_end) ) {
                //If the distance to the start of the parent comes form the start of the node
                std::get<1>(*current_traceback) = distance_start_start == 0 ? std::numeric_limits<int32_t>::min() : -distance_start_start; 
            } else if (sum(distance_start_end , distance_end) != std::numeric_limits<size_t>::max()) {
                //If the distance to the start of the parent comes from the end of the node
                std::get<1>(*current_traceback) = distance_start_end; 
                
            }

            if (sum(distance_end_start , distance_start) != std::numeric_limits<size_t>::max() &&
                sum(distance_end_start , distance_start) < sum(distance_end_end , distance_end)) {
                //If the distance to the end of the parent comes from the distance to the start of the node
                std::get<2>(*current_traceback) = distance_end_start == 0 ? std::numeric_limits<int32_t>::min() : -distance_end_start;
            } else if (sum(distance_end_end , distance_end)) {
                //If the distance to the end of the parent comes from the distance to the end of the node
                std::get<2>(*current_traceback) = distance_end_end;
            }
        }


        dist_start = std::min( sum(distance_start_start, distance_start), 
                                    sum(distance_start_end , distance_end));
        dist_end = std::min(sum(distance_end_start , distance_start), 
                            sum(distance_end_end , distance_end));
#ifdef debug_distances
        cerr << "        ...new distances to start and end: " << dist_start << " " << dist_end << endl;
#endif
        return;
    };

    /*
     * Get net handles for the two nodes and the distances from each position to the ends of the handles
     */
    net_handle_t net1 = get_node_net_handle(id1);
    net_handle_t net2 = get_node_net_handle(id2);
    pair<net_handle_t, bool> lowest_ancestor = lowest_common_ancestor(net1, net2);
    if (!lowest_ancestor.second) {
        //If these are not in the same connected component
#ifdef debug_distances
        cerr << "These are in different connected components" << endl;
#endif
        return std::numeric_limits<size_t>::max();
    }

    //The lowest common ancestor of the two positions
    net_handle_t common_ancestor = start_end_traversal_of(lowest_ancestor.first);

#ifdef debug_distances
        cerr << "Found the lowest common ancestor " << net_handle_as_string(common_ancestor) << endl;
#endif
    //These are the distances to the ends of the node, including the position
    size_t distance_to_start1 = rev1 ? node_length(net1) - offset1 : offset1 + 1;
    size_t distance_to_end1 = rev1 ? offset1 + 1 : node_length(net1) - offset1;
    size_t distance_to_start2 = rev2 ? node_length(net2) - offset2 : offset2 + 1;
    size_t distance_to_end2 = rev2 ? offset2 + 1 : node_length(net2) - offset2;

    if (!unoriented_distance) {
        //If we care about the oriented distance, one of the distances will be infinite
        if (rev1) {
            distance_to_end1 = std::numeric_limits<size_t>::max();
        } else {
            distance_to_start1 = std::numeric_limits<size_t>::max();
        }
        if (rev2) {
            distance_to_start2 = std::numeric_limits<size_t>::max();
        } else {
            distance_to_end2 = std::numeric_limits<size_t>::max();
        }
    }

#ifdef debug_distances
        cerr << "Starting with distances " << distance_to_start1 << " " << distance_to_end1 << " and " << distance_to_start2 << " " << distance_to_end2 << endl;
#endif

    size_t minimum_distance = std::numeric_limits<size_t>::max();

    //Remember which common ancestor the minimum distance actually comes from
    //tuple of <the ancestor, distance between the children, connectivity between the two children
    tuple<net_handle_t, size_t, connectivity_t> common_ancestor_connectivity (get_root(), std::numeric_limits<size_t>::max(), START_END); 


    /*
     * Walk up the snarl tree until net1 and net2 are children of the lowest common ancestor
     * Keep track of the distances to the ends of the net handles as we go
     */
 
    if (start_end_traversal_of(net1) == start_end_traversal_of(net2)){
        if (sum(distance_to_end1 , distance_to_start2) > node_length(net1) && 
            sum(distance_to_end1 , distance_to_start2) != std::numeric_limits<size_t>::max()) {
            //If the positions are on the same node and are pointing towards each other, then
            //check the distance between them in the node
            minimum_distance = minus(sum(distance_to_end1 , distance_to_start2), node_length(net1));
            if (distance_traceback != nullptr) {
                //If we're recording the traceback, remember the common node as the common ancestor
                common_ancestor_connectivity = std::make_tuple(net1, minimum_distance-1, END_START); 
            }
        }
        if (sum(distance_to_start1 , distance_to_end2) > node_length(net1) && 
            sum(distance_to_start1 , distance_to_end2) != std::numeric_limits<size_t>::max()) {
            minimum_distance = std::min(minus(sum(distance_to_start1 , distance_to_end2), node_length(net1)), minimum_distance);
            if (distance_traceback != nullptr && minimum_distance < std::get<1>(common_ancestor_connectivity)) {
                //If we're recording the traceback, remember the common node as the common ancestor
                common_ancestor_connectivity = std::make_tuple(net1, minimum_distance-1, START_END); 
            }
        }
        if (distance_traceback != nullptr) {
            //If we're recording the traceback, add the nodes
            distance_traceback->first.emplace_back(net1, 
                                                   distance_to_start1 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_start1, 
                                                   distance_to_end1 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_end1);
            distance_traceback->second.emplace_back(net2, 
                                                   distance_to_start2 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_start2, 
                                                   distance_to_end2 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_end2);        
        }
        common_ancestor = start_end_traversal_of(get_parent(net1));
    } else {

        //Get the distance from position 1 up to the ends of a child of the common ancestor
#ifdef debug_distances
        cerr << "Reaching the children of the lowest common ancestor for first position..." << endl;
#endif   
        if (distance_traceback != nullptr) {
            //Start the traceback with the nodes themselves and their offsets
            distance_traceback->first.emplace_back(net1, 
                                                   distance_to_start1 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_start1, 
                                                   distance_to_end1 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_end1);
            distance_traceback->second.emplace_back(net2, 
                                                   distance_to_start2 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_start2, 
                                                   distance_to_end2 == std::numeric_limits<size_t>::max() ? std::numeric_limits<int32_t>::max() : distance_to_end2);        
        }
        while (start_end_traversal_of(get_parent(net1)) != common_ancestor && !is_root(get_parent(net1))) {
            net_handle_t parent = start_end_traversal_of(get_parent(net1));
            update_distances(net1, parent, distance_to_start1, distance_to_end1, true);
            net1 = parent;
        }
#ifdef debug_distances
        cerr << "Reached node " << net_handle_as_string(net1) << " for position 1" << endl;
        cerr << "   with distances to ends " << distance_to_start1 << " and " << distance_to_end1 << endl;
        cerr << "Reaching the children of the lowest common ancestor for position 2..." << endl;
#endif   
        //And the same for position 2
        while (start_end_traversal_of(get_parent(net2)) != start_end_traversal_of(common_ancestor) && !is_root(get_parent(net2))) {
            net_handle_t parent = start_end_traversal_of(get_parent(net2));
            update_distances(net2, parent, distance_to_start2, distance_to_end2, false);
            net2 = parent;
        }
#ifdef debug_distances
        cerr << "Reached node " << net_handle_as_string(net2) << " for position 2" << endl;
        cerr << "   with distances to ends " << distance_to_start2 << " and " << distance_to_end2 << endl;
#endif
    }


    /* 
     * common_ancestor is now the lowest common ancestor of both net handles, and 
     * net1 and net2 are both children of common_ancestor
     * Walk up to the root and check for distances between the positions within each
     * ancestor
     */

    while (!is_root(net1)){
#ifdef debug_distances
            cerr << "At common ancestor " << net_handle_as_string(common_ancestor) <<  endl;
            cerr << "  with distances for child 1 (" << net_handle_as_string(net1) << "): " << distance_to_start1 << " "  << distance_to_end1 << endl;
            cerr << "                     child 2 (" << net_handle_as_string(net2) << "): " << distance_to_start2 << " " <<  distance_to_end2 << endl;
#endif

        //Find the minimum distance between the two children (net1 and net2)
        size_t distance_start_start = distance_in_parent(common_ancestor, flip(net1), flip(net2), graph);
        size_t distance_start_end = distance_in_parent(common_ancestor, flip(net1), net2, graph);
        size_t distance_end_start = distance_in_parent(common_ancestor, net1, flip(net2), graph);
        size_t distance_end_end = distance_in_parent(common_ancestor, net1, net2, graph);

        size_t old_minimum = minimum_distance;

        //And add those to the distances we've found to get the minimum distance between the positions
        minimum_distance = std::min(minimum_distance, 
                           std::min(sum(sum(distance_start_start , distance_to_start1), distance_to_start2),
                           std::min(sum(sum(distance_start_end , distance_to_start1), distance_to_end2),
                           std::min(sum(sum(distance_end_start , distance_to_end1), distance_to_start2),
                                    sum(sum(distance_end_end , distance_to_end1), distance_to_end2)))));

        if (distance_traceback != nullptr && minimum_distance != std::numeric_limits<size_t>::max()
            && minimum_distance != old_minimum) {
            //If we want to do traceback and the distance was updated here

            if (minimum_distance == sum(sum(distance_start_start , distance_to_start1), distance_to_start2)) {
                //If the distance was the start of 1 to the start of 2
                common_ancestor_connectivity = std::make_tuple(common_ancestor, distance_start_start, START_START);
            } else if (minimum_distance == sum(sum(distance_start_end , distance_to_start1), distance_to_end2)) {
                common_ancestor_connectivity = std::make_tuple(common_ancestor, distance_start_end, START_END);
            } else if (minimum_distance == sum(sum(distance_end_start , distance_to_end1), distance_to_start2)) {
                common_ancestor_connectivity = std::make_tuple(common_ancestor, distance_end_start, END_START);
            } else {
                assert(minimum_distance == sum(sum(distance_end_end , distance_to_end1), distance_to_end2));
                common_ancestor_connectivity = std::make_tuple(common_ancestor, distance_end_end, END_END);
            } 
        }

#ifdef debug_distances
            cerr << "    Found distances between nodes: " << distance_start_start << " " << distance_start_end << " " << distance_end_start << " " << distance_end_end << endl;
            cerr << "  best distance is " << minimum_distance << endl;
#endif
        if (!is_root(common_ancestor)) {
            //Update the distances to reach the ends of the common ancestor
            update_distances(net1, common_ancestor, distance_to_start1, distance_to_end1, true);
            update_distances(net2, common_ancestor, distance_to_start2, distance_to_end2, false);

            //Update which net handles we're looking at
            net1 = common_ancestor;
            net2 = common_ancestor;
            common_ancestor = start_end_traversal_of(get_parent(common_ancestor));
        } else {
            //Just update this one to break out of the loop
            net1 = common_ancestor;
            if (distance_traceback != nullptr) {
                distance_traceback->first.emplace_back(common_ancestor, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                distance_traceback->second.emplace_back(common_ancestor, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
            }

        }

#ifdef debug_distances
            cerr << "  new common ancestor " << net_handle_as_string(common_ancestor) <<  endl;
            cerr << "  new distances are " << distance_to_start1 << " "  << distance_to_end1 << 
                    " " << distance_to_start2 << " " <<  distance_to_end2 << endl;
#endif
    }

    if (distance_traceback != nullptr) {
        //If we care about the traceback, then traceback and remove values that aren't used
        if (minimum_distance == std::numeric_limits<size_t>::max()) {
            //If there was no minimum distance, clear the traceback
            distance_traceback->first.clear();
            distance_traceback->second.clear();
        } else {
            //Otherwise, do the traceback

            for ( bool first_node : {true, false}) {
                vector<tuple<net_handle_t, int32_t, int32_t>>& current_vector = first_node ? distance_traceback->first : distance_traceback->second;
                //Cut back the traceback to the common ancestor where the children were connected
                while (std::get<0>(current_vector.back()) != std::get<0>(common_ancestor_connectivity)) {
                    current_vector.pop_back();
                } 
#ifdef debug_distances
                cerr << "At " << (first_node ? "first " : "second ") << "node, stripped back to " << current_vector.size() << " ancestors" << endl;
                cerr << "\treplacing last ancestor with common ancestor and distances: " << net_handle_as_string(std::get<0>(common_ancestor_connectivity))
                     << " " << std::get<1>(common_ancestor_connectivity) << " " << std::get<2>(common_ancestor_connectivity) << endl;
#endif

                /*Now update the common ancestor in the traceback
                 */

                //Is the path going out the start of this node?
                bool from_start = (first_node && get_start_endpoint(std::get<2>(common_ancestor_connectivity)) == START) ||
                                  (!first_node && get_end_endpoint(std::get<2>(common_ancestor_connectivity)) == START);
                //Is the path going to the start of the other node?
                bool to_start = (first_node && get_end_endpoint(std::get<2>(common_ancestor_connectivity)) == START) ||
                                  (!first_node && get_start_endpoint(std::get<2>(common_ancestor_connectivity)) == START);
                if (to_start) { 
                    std::get<1>(current_vector.back()) = from_start ? (std::get<1>(common_ancestor_connectivity) == 0 ? std::numeric_limits<int32_t>::min()
                                                                                                                      : -std::get<1>(common_ancestor_connectivity))
                                                                    : std::get<1>(common_ancestor_connectivity);
                    std::get<2>(current_vector.back()) = std::numeric_limits<int32_t>::max();
                } else {
                    std::get<1>(current_vector.back()) = std::numeric_limits<int32_t>::max();
                    std::get<2>(current_vector.back()) = from_start ? (std::get<1>(common_ancestor_connectivity) == 0 ? std::numeric_limits<int32_t>::min()
                                                                                                                      : -std::get<1>(common_ancestor_connectivity))
                                                                    : std::get<1>(common_ancestor_connectivity);
                }

                /*
                 * Walk through the traceback and keep only the relevant value
                 */
                for (int i = current_vector.size() - 2 ; i > 0 ; i--) {
                    assert(i >= 0 && i < current_vector.size());
                    if (from_start) {
                        //If the parent of the current child has distance out the start, clear the distance to this child's parent's end
                        std::get<2>(current_vector[i]) = std::numeric_limits<int32_t>::max();

                        //If the current child's distance to it's parent's start is negative, then it's distance went out the start so
                        //we will want it's child distance to the start
                        from_start = std::get<1>(current_vector[i]) < 0;
#ifdef debug_distances
                        assert(std::get<1>(current_vector[i]) != std::numeric_limits<int32_t>::max());
#endif
                    } else {
                        //If the parent of the current child has distance out the end, clear the distance to this child's parent's start
                        std::get<1>(current_vector[i]) = std::numeric_limits<int32_t>::max();

                        //If the current child's distance to it's parent's end is negative, then it's distance went out the start so
                        //we will want it's child distance to the start
                        from_start = std::get<2>(current_vector[i]) < 0;
#ifdef debug_distances
                        assert(std::get<2>(current_vector[i]) != std::numeric_limits<int32_t>::max());
#endif
                    }
                }
            }
#ifdef debug_distance_paths
            cerr << "Traced back to common ancestor " << net_handle_as_string(std::get<0>(common_ancestor_connectivity)) <<  endl;
            cerr << "Node1: " << endl;
            for (auto x : distance_traceback->first) {
                cerr << "\t" << net_handle_as_string(std::get<0>(x)) << " " << std::get<1>(x) << " " << std::get<2>(x) << endl;
            }
            cerr << "Node2: " << endl;
            for (auto x : distance_traceback->second) {
                cerr << "\t" << net_handle_as_string(std::get<0>(x)) << " " << std::get<1>(x) << " " << std::get<2>(x) << endl;
            }

#endif
        }
    }

    //minimum distance currently includes both positions
    return minimum_distance == std::numeric_limits<size_t>::max() ? std::numeric_limits<size_t>::max() : minimum_distance-1;



}
size_t SnarlDistanceIndex::maximum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, 
                                            const handlegraph::nid_t id2, const bool rev2, const size_t offset2, 
                                            bool unoriented_distance, const HandleGraph* graph) const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    size_t max_node_id = root_record.get_min_node_id() + root_record.get_node_count();
    if (id1 < root_record.get_min_node_id() || id2 < root_record.get_min_node_id() ||
        id1 > max_node_id || id2 > max_node_id) {
        throw runtime_error("error: Looking for the maximum distance of a node that does not exist");
    }


#ifdef debug_distances
        cerr << endl;
        cerr << "Find the maximum distance between " << id1 << " " <<rev1 <<" " << offset1 << " and " << id2 << " " << rev2 << " " << offset2 << endl;
#endif


    /*Helper function to walk up the snarl tree
     * Given a net handle, its parent,  and the distances to the start and end of the handle, 
     * update the distances to reach the ends of the parent and update the handle and its parent
     * If the parent is a chain, then the new distances include the boundary nodes of the chain.
     * If it is a snarl, it does not*/
    //TODO: This should really be an actual function and it doesn't consider the lengths of the 
    //boundary nodes. I think chains might need to know the lengths of their boundary nodes,
    //or it could find it from the snarl. Really, since the snarl is storing it anyway, I should
    //probaby rearrange things so that either the snarl or the chain can access boundary node lengths
    auto update_distances = [&](net_handle_t& net, net_handle_t& parent, size_t& dist_start, size_t& dist_end, bool first_node) {
#ifdef debug_distances
        cerr << "     Updating distance from node " << net_handle_as_string(net) << " at parent " << net_handle_as_string(parent) << endl;
#endif

        if (is_trivial_chain(parent)) {
            //Don't update distances for the trivial chain
            return;
        } else if (is_simple_snarl(parent)) {
            //If it's a simple snarl just check if they should be reversed
            if (is_reversed_in_parent (net)) {
                size_t tmp = dist_start;
                dist_start = dist_end;
                dist_end = tmp;
            } 

            return;
        }


        net_handle_t start_bound = get_bound(parent, false, true);
        net_handle_t end_bound = get_bound(parent, true, true);

        //The lengths of the start and end nodes of net
        //This is only needed if net is a snarl, since the boundary nodes are not technically part of the snarl
        size_t start_length = is_chain(parent) ? node_length(start_bound) : 0;
        size_t end_length = is_chain(parent) ? node_length(end_bound) : 0;

        //Get the distances from the bounds of the parent to the node we're looking at
        size_t distance_start_start = start_bound == net ? 0 
                : sum(start_length, max_distance_in_parent(parent, start_bound, flip(net), graph));
        size_t distance_start_end = start_bound == flip(net) ? 0 
                : sum(start_length, max_distance_in_parent(parent, start_bound, net, graph));
        size_t distance_end_start = end_bound == net ? 0 
                : sum(end_length, max_distance_in_parent(parent, end_bound, flip(net), graph));
        size_t distance_end_end = end_bound == flip(net) ? 0 
                : sum(end_length, max_distance_in_parent(parent, end_bound, net, graph));

        size_t distance_start = dist_start;
        size_t distance_end = dist_end; 

        dist_start = maximum( sum(distance_start_start, distance_start), 
                              sum(distance_start_end , distance_end));
        dist_end = maximum(sum(distance_end_start , distance_start), 
                           sum(distance_end_end , distance_end));
#ifdef debug_distances
        cerr << "        ...new distances to start and end: " << dist_start << " " << dist_end << endl;
#endif
        return;
    };

    /*
     * Get net handles for the two nodes and the distances from each position to the ends of the handles
     * TODO: net2 is pointing in the opposite direction of the position. The final 
     * distance will be between the two nets pointing towards each other
     */
    net_handle_t net1 = get_node_net_handle(id1);
    net_handle_t net2 = get_node_net_handle(id2);
    pair<net_handle_t, bool> lowest_ancestor = lowest_common_ancestor(net1, net2);
    if (!lowest_ancestor.second) {
        //If these are not in the same connected component
#ifdef debug_distances
        cerr << "These are in different connected components" << endl;
#endif
        return std::numeric_limits<size_t>::max();
    }

    //The lowest common ancestor of the two positions
    net_handle_t common_ancestor = start_end_traversal_of(lowest_ancestor.first);

#ifdef debug_distances
        cerr << "Found the lowest common ancestor " << net_handle_as_string(common_ancestor) << endl;
#endif
    //These are the distances to the ends of the node, including the position
    size_t distance_to_start1 = rev1 ? node_length(net1) - offset1 : offset1 + 1;
    size_t distance_to_end1 = rev1 ? offset1 + 1 : node_length(net1) - offset1;
    size_t distance_to_start2 = rev2 ? node_length(net2) - offset2 : offset2 + 1;
    size_t distance_to_end2 = rev2 ? offset2 + 1 : node_length(net2) - offset2;

    if (!unoriented_distance) {
        //If we care about the oriented distance, one of the distances will be infinite
        if (rev1) {
            distance_to_end1 = std::numeric_limits<size_t>::max();
        } else {
            distance_to_start1 = std::numeric_limits<size_t>::max();
        }
        if (rev2) {
            distance_to_start2 = std::numeric_limits<size_t>::max();
        } else {
            distance_to_end2 = std::numeric_limits<size_t>::max();
        }
    }

#ifdef debug_distances
        cerr << "Starting with distances " << distance_to_start1 << " " << distance_to_end1 << " and " << distance_to_start2 << " " << distance_to_end2 << endl;
#endif

    size_t maximum_distance = std::numeric_limits<size_t>::max();

    //Remember which common ancestor the maximum distance actually comes from
    //tuple of <the ancestor, distance between the children, connectivity between the two children
    tuple<net_handle_t, size_t, connectivity_t> common_ancestor_connectivity (get_root(), std::numeric_limits<size_t>::max(), START_END); 


    /*
     * Walk up the snarl tree until net1 and net2 are children of the lowest common ancestor
     * Keep track of the distances to the ends of the net handles as we go
     */
 
    if (start_end_traversal_of(net1) == start_end_traversal_of(net2)){
        if (sum(distance_to_end1 , distance_to_start2) > node_length(net1) && 
            sum(distance_to_end1 , distance_to_start2) != std::numeric_limits<size_t>::max()) {
            //If the positions are on the same node and are pointing towards each other, then
            //check the distance between them in the node
            maximum_distance = minus(sum(distance_to_end1 , distance_to_start2), node_length(net1));
        }
        if (sum(distance_to_start1 , distance_to_end2) > node_length(net1) && 
            sum(distance_to_start1 , distance_to_end2) != std::numeric_limits<size_t>::max()) {
            size_t new_max = minus(sum(distance_to_start1 , distance_to_end2), node_length(net1));
            if (new_max != std::numeric_limits<size_t>::max()) {
                maximum_distance = maximum_distance == std::numeric_limits<size_t>::max() ? new_max 
                                 : std::max(new_max, maximum_distance);
            }
        }
        common_ancestor = start_end_traversal_of(get_parent(net1));
    } else {

        //Get the distance from position 1 up to the ends of a child of the common ancestor
#ifdef debug_distances
        cerr << "Reaching the children of the lowest common ancestor for first position..." << endl;
#endif   
        while (start_end_traversal_of(get_parent(net1)) != common_ancestor && !is_root(get_parent(net1))) {
            net_handle_t parent = start_end_traversal_of(get_parent(net1));
            update_distances(net1, parent, distance_to_start1, distance_to_end1, true);
            net1 = parent;
        }
#ifdef debug_distances
        cerr << "Reached node " << net_handle_as_string(net1) << " for position 1" << endl;
        cerr << "   with distances to ends " << distance_to_start1 << " and " << distance_to_end1 << endl;
        cerr << "Reaching the children of the lowest common ancestor for position 2..." << endl;
#endif   
        //And the same for position 2
        while (start_end_traversal_of(get_parent(net2)) != start_end_traversal_of(common_ancestor) && !is_root(get_parent(net2))) {
            net_handle_t parent = start_end_traversal_of(get_parent(net2));
            update_distances(net2, parent, distance_to_start2, distance_to_end2, false);
            net2 = parent;
        }
#ifdef debug_distances
        cerr << "Reached node " << net_handle_as_string(net2) << " for position 2" << endl;
        cerr << "   with distances to ends " << distance_to_start2 << " and " << distance_to_end2 << endl;
#endif
    }
    //TODO: I'm taking this out because it should really be start-end connected, but this
    //won't do this if that traversal isn't possible. Really it should just be setting the 
    //connectivity instead
    //net1 = canonical(net1);
    //net2 = canonical(net2);


    /* 
     * common_ancestor is now the lowest common ancestor of both net handles, and 
     * net1 and net2 are both children of common_ancestor
     * Walk up to the root and check for distances between the positions within each
     * ancestor
     */

    while (!is_root(net1)){
        //TODO: Actually checking distance in a chain is between the nodes, not the snarl
        //and neither include the lengths of the nodes
#ifdef debug_distances
            cerr << "At common ancestor " << net_handle_as_string(common_ancestor) <<  endl;
            cerr << "  with distances for child 1 (" << net_handle_as_string(net1) << "): " << distance_to_start1 << " "  << distance_to_end1 << endl;
            cerr << "                     child 2 (" << net_handle_as_string(net2) << "): " << distance_to_start2 << " " <<  distance_to_end2 << endl;
#endif

        //Find the maximum distance between the two children (net1 and net2)
        size_t distance_start_start = max_distance_in_parent(common_ancestor, flip(net1), flip(net2), graph);
        size_t distance_start_end = max_distance_in_parent(common_ancestor, flip(net1), net2, graph);
        size_t distance_end_start = max_distance_in_parent(common_ancestor, net1, flip(net2), graph);
        size_t distance_end_end = max_distance_in_parent(common_ancestor, net1, net2, graph);

        size_t old_maximum = maximum_distance;

        //And add those to the distances we've found to get the maximum distance between the positions
        maximum_distance = maximum(maximum_distance, 
                           maximum(sum(sum(distance_start_start , distance_to_start1), distance_to_start2),
                           maximum(sum(sum(distance_start_end , distance_to_start1), distance_to_end2),
                           maximum(sum(sum(distance_end_start , distance_to_end1), distance_to_start2),
                                    sum(sum(distance_end_end , distance_to_end1), distance_to_end2)))));


#ifdef debug_distances
            cerr << "    Found distances between nodes: " << distance_start_start << " " << distance_start_end << " " << distance_end_start << " " << distance_end_end << endl;
            cerr << "  best distance is " << maximum_distance << endl;
#endif
        if (!is_root(common_ancestor)) {
            //Update the distances to reach the ends of the common ancestor
            update_distances(net1, common_ancestor, distance_to_start1, distance_to_end1, true);
            update_distances(net2, common_ancestor, distance_to_start2, distance_to_end2, false);

            //Update which net handles we're looking at
            net1 = common_ancestor;
            net2 = common_ancestor;
            common_ancestor = start_end_traversal_of(get_parent(common_ancestor));
        } else {
            //Just update this one to break out of the loop
            net1 = common_ancestor;

        }

#ifdef debug_distances
            cerr << "  new common ancestor " << net_handle_as_string(common_ancestor) <<  endl;
            cerr << "  new distances are " << distance_to_start1 << " "  << distance_to_end1 << 
                    " " << distance_to_start2 << " " <<  distance_to_end2 << endl;
#endif
    }


    //maximum distance currently includes both positions
    return maximum_distance == std::numeric_limits<size_t>::max() ? std::numeric_limits<size_t>::max() : maximum_distance-1;

}
void SnarlDistanceIndex::for_each_handle_in_shortest_path(const handlegraph::nid_t id1, const bool rev1, const handlegraph::nid_t id2, const bool rev2, 
                                      const HandleGraph* graph, const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee) const {
    cerr << "This isn't working, use handlegraph::algorithms::for_each_handle_in_shortest_path instead until I fix it" << endl;
    return;

    /* First, run minimum distance to get the traceback.
     * The traceback will be a list of ancestors for each node. They will take the format 
        <net_handle, distance from last child to to start of net, distance to end of net>
       The distances will be signed, + meaning that the child (the child of the net handle in the tuple) is traversed forward, - for backwards
       The last entry in both lists will be for the lowest common ancestor that must be traversed to find the minimum distance (not
       necessarily the actual lowest common ancestor), and the distances will be from either end of the 
    */
    pair<vector<tuple<net_handle_t, int32_t, int32_t>>, vector<tuple<net_handle_t, int32_t, int32_t>>> distance_traceback;
    size_t distance = minimum_distance(id1, rev1, 0, id2, rev2, 0, false,graph, &distance_traceback);
#ifdef debug_distance_paths
    cerr << endl << "Find the minimum distance path between " << id1 << (rev1 ? "rev" : "fd") << " and " << id2 << (rev2 ? "rev" : "fd")
         << " which has distance between them " << distance << endl;
#endif


    /* We can stop now if the minimum_distance was infinite or if the minimum_distance was within one node
    */

    if (distance_traceback.first.size() == 0 && distance_traceback.second.size() == 0) {
        //If there was no path between them
#ifdef debug_distance_paths
        assert(distance == std::numeric_limits<size_t>::max());
#endif
        return;
    } else if (distance_traceback.first.size() == 1 && distance_traceback.second.size() == 1) {
        //If there is only one thing in the traceback, then the two positions must be on the same node
#ifdef debug_distance_paths
        assert(is_node(std::get<0>(distance_traceback.first.back())));
        assert(is_node(std::get<0>(distance_traceback.second.back())));
        assert(std::get<0>(distance_traceback.first.back()) == std::get<0>(distance_traceback.second.back()));
        assert(id1 == id2);
        assert(node_id(std::get<0>(distance_traceback.first.back())) == id1);
#endif
        return;
    }
#ifdef debug_distance_paths
        assert(distance_traceback.first.size() > 1);
        assert(distance_traceback.second.size() > 1);
#endif


    /* Otherwise, walk up the snarl tree from the first node (traversing its traceback vector forwards), and at each
     * ancestor, call for_each_handle_in_shortest_path_in_snarl/chain to get to the boundary nodes, 
     * then at the common ancestor, traverse between the second to last ancestor in both lists,
     * then go through the second node's traceback vector backwards, calling for_each_handle_in_shortest_path_in_snarl/chain
     * to get from the boundary nodes to each child
    */

    // Start with the first node
    net_handle_t& current_node = std::get<0>(distance_traceback.first[0]);
    net_handle_t& current_parent = std::get<0>(distance_traceback.first[1]);

    size_t distance_traversed = 0;


    for (size_t i = 1 ; i < distance_traceback.first.size()-1 ; i++) {
        //Walking up the snarl tree, walk from the current node to the current parent in the given distance
        //i always points to the current parent that we're finding distances in
        if (is_trivial_chain(get_parent(current_node))) {
            current_node = get_parent(current_node);
        }
#ifdef debug_distance_paths
        cerr << "At traceback for first node: " << net_handle_as_string(std::get<0>(distance_traceback.first[i])) << " " << std::get<1>(distance_traceback.first[i]) << " " << std::get<2>(distance_traceback.first[i]) << endl;
        cerr << "At current node " << net_handle_as_string(current_node) << endl;
#endif

        //The (signed) distance from the current node to the end of the current parent
        int32_t distance_to_traverse = std::get<2>(distance_traceback.first[i]) == std::numeric_limits<int32_t>::max() 
                            ? std::get<1>(distance_traceback.first[i])
                            :  std::get<2>(distance_traceback.first[i]);
#ifdef debug_distance_paths
        assert(distance_to_traverse != std::numeric_limits<int32_t>::max());                                
#endif

        //The boundary node/sentinel of the parent pointing out of the parent
        net_handle_t boundary;
        if (std::get<1>(distance_traceback.first[i]) == std::numeric_limits<int32_t>::max()) {
            //If we're traversing to the end of the parent
            boundary = get_bound(current_parent, true, false);
        } else {
            //If we're traversing to the beginning of the parent
            boundary = get_bound(current_parent, false, false);
        }

        //Make sure the current node is oriented correctly
        if ((ends_at(current_node) != START && distance_to_traverse < 0) 
            || (ends_at(current_node) == START && distance_to_traverse >= 0)) {
            current_node = flip(current_node);
        }
        //We don't care about the orientation for the distance anymore so get the actual value
        distance_to_traverse = distance_to_traverse == std::numeric_limits<int32_t>::min() ? 0 : std::abs(distance_to_traverse);


        if (is_chain(current_parent)) {
            if (current_node != boundary) {
                //Go from current node to boundary, not including the length of boundary in the distance_to_traverse
                for_each_handle_in_shortest_path_in_chain(current_parent, current_node, boundary, distance_to_traverse - node_length(boundary), distance_traversed, graph, iteratee, nullptr);

                //Now iteratee() on boundary
                iteratee(get_handle(boundary, graph), distance_traversed);
                distance_traversed += minimum_length(boundary);
                distance_to_traverse -= minimum_length(boundary);
            }
        } else {
            for_each_handle_in_shortest_path_in_snarl(current_parent, current_node, boundary, distance_to_traverse, distance_traversed, graph, iteratee, nullptr);
        }
        current_node = current_parent;
        current_parent = std::get<0>(distance_traceback.first[i+1]);
    }

    /* Now walk between the two children of the common ancestor */

#ifdef debug_distance_paths
    assert(current_parent == std::get<0>(distance_traceback.second.back()));
#endif

    int32_t distance_between = std::get<1>(distance_traceback.first[distance_traceback.first.size()-1]) == std::numeric_limits<int32_t>::max() 
                             ? std::get<2>(distance_traceback.first[distance_traceback.first.size()-1])
                             : std::get<1>(distance_traceback.first[distance_traceback.first.size()-1]);

    //Child of the common ancestor for the second node
    net_handle_t next_node = std::get<0>(distance_traceback.second[distance_traceback.second.size()-2]);
    if (is_trivial_chain(get_parent(next_node))) {
        next_node = get_parent(next_node);
    }
    if (is_trivial_chain(get_parent(current_node))) {
        current_node = get_parent(current_node);
    }
#ifdef debug_distance_paths
    cerr << "Finding distance between two children " << net_handle_as_string(current_node) << " and " << net_handle_as_string (next_node) << endl;
    cerr << "\tdistance should be " << distance_between << endl;
#endif

    //Make sure the current node is oriented correctly
    if ((ends_at(current_node) != START && distance_between < 0) 
        || (ends_at(current_node) == START && distance_between > 0)) {
        current_node = flip(current_node);
    } 
    distance_between = distance_between == std::numeric_limits<int32_t>::min() ? 0 : std::abs(distance_between);

    //And the next node. Based on the last entry in the first node's traceback. The second or third value will 
    //be inf depending on whether the second node's ancestor is traversed forward or backwards 
    if ((ends_at(next_node) != START && std::get<1>(distance_traceback.first.back()) == std::numeric_limits<int32_t>::max()) 
        || (ends_at(next_node) == START && std::get<2>(distance_traceback.first.back()) == std::numeric_limits<int32_t>::max())) {
#ifdef debug_distance_paths
        cerr << "Flip next node which is currently: " << net_handle_as_string(next_node) << " because the last traceback for the first node is " 
             << net_handle_as_string(std::get<0>(distance_traceback.first.back())) << " " 
             << std::get<1>(distance_traceback.first.back()) << " " << std::get<2>(distance_traceback.first.back()) << endl; 
#endif

        next_node = flip(next_node);
    }

    if (is_chain(current_parent)) {
        for_each_handle_in_shortest_path_in_chain(current_parent, current_node, next_node, distance_between, distance_traversed, graph, iteratee, nullptr);
    } else {
        for_each_handle_in_shortest_path_in_snarl(current_parent, current_node, next_node, distance_between, distance_traversed, graph, iteratee, nullptr);
    }

    /* Now walk down the snarl tree for the second node, finding the distance_to_traverse from the bound of each parent to its child
     * Start with the parent one below the common ancestor  
    */
    current_parent = next_node;
    if (distance_traceback.second.size() >= 3) {
        next_node = std::get<0>(distance_traceback.second[distance_traceback.second.size() - 3]);
    }
#ifdef debug_distance_paths
    cerr << "Now walk down node 2 from parent " << net_handle_as_string(current_parent) << " to node " << net_handle_as_string(next_node) << endl;
#endif
    for (int i = distance_traceback.second.size() - 2 ; i > 0 ; i--) {
        assert(i >= 1 && i < distance_traceback.second.size());
        //Walk down the snarl tree. i points to the current parent, next_node is one below the parent
        //
        current_parent = std::get<0>(distance_traceback.second[i]);
        next_node = std::get<0>(distance_traceback.second[i-1]);
        if (is_trivial_chain(get_parent(next_node))) {
            next_node = get_parent(next_node);
        }

#ifdef debug_distance_paths
        cerr << "At traceback for second node: " << net_handle_as_string(std::get<0>(distance_traceback.second[i])) << " " << std::get<1>(distance_traceback.second[i]) << " " << std::get<2>(distance_traceback.second[i]) << endl;
        cerr << "At next node " << net_handle_as_string(next_node) << " with parent " << net_handle_as_string(current_parent) << endl;
#endif

        //The (signed) distance_to_traverse from the current node to the end of the current parent
        int32_t distance_to_traverse = std::get<2>(distance_traceback.second[i]) == std::numeric_limits<int32_t>::max() 
                            ? std::get<1>(distance_traceback.second[i])
                            :  std::get<2>(distance_traceback.second[i]);

        //The boundary node/sentinel of the parent pointing into the parent
        net_handle_t boundary;
        if (std::get<1>(distance_traceback.second[i]) == std::numeric_limits<int32_t>::max()) {
            //If we're traversing to the end of the parent
            boundary = get_bound(current_parent, true, true);
        } else {
            //If we're traversing to the beginning of the parent
            boundary = get_bound(current_parent, false, true);
        }

        //Make sure the current node is oriented correctly
        if ((ends_at(next_node) == START && distance_to_traverse < 0) 
            || (ends_at(next_node) != START && distance_to_traverse >= 0)) {
            next_node = flip(next_node);
        } 

        //We don't care about the orientation for the distance anymore so get the actual value
        distance_to_traverse = distance_to_traverse == std::numeric_limits<int32_t>::min() ? 0 : std::abs(distance_to_traverse);


        if (is_chain(current_parent)) {

            if (next_node != boundary) {
                iteratee(get_handle(boundary, graph), distance_traversed);
                distance_traversed += minimum_length(boundary);
                distance_to_traverse -= minimum_length(boundary);

                for_each_handle_in_shortest_path_in_chain(current_parent, boundary, next_node, distance_to_traverse, distance_traversed, graph, iteratee, nullptr);
            }
        } else {
            for_each_handle_in_shortest_path_in_snarl(current_parent, boundary, next_node, distance_to_traverse, distance_traversed, graph, iteratee, nullptr);
        }

    }
}
void SnarlDistanceIndex::for_each_handle_in_shortest_path_in_snarl(const net_handle_t& snarl_handle, net_handle_t start, net_handle_t end,
                                      size_t distance_to_traverse, size_t& distance_traversed, const HandleGraph* graph,
                                      const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee,
                                      vector<pair<net_handle_t, size_t>>* to_duplicate) const {
#ifdef debug_distance_paths
    size_t target_distance = distance_to_traverse;
    size_t starting_distance = distance_traversed;
    cerr << "Find shortest path in " << net_handle_as_string(snarl_handle) << " from " << net_handle_as_string(start) << " to " << net_handle_as_string(end) << " with distance " << distance_to_traverse << endl;
    if (SnarlRecord(snarl_handle, &snarl_tree_records).get_record_type() != OVERSIZED_SNARL) {
        cerr << "\tactual distance is " << distance_in_parent(snarl_handle, start, flip(end)) << endl;
        assert(distance_in_parent(snarl_handle, start, flip(end)) == distance_to_traverse);
    }
#endif

    /* Traverse the shortest path from start to end using the A* algorithm
     * For any child that isn't a trivial chain, recurse with for_each_handle_in_shortest_path_in_chain
     * Since the heuristic is the exact minimum distance, we don't need to have a priority queue of next nodes,
     * there will always be only one that is on the minimum distance path.  
    */

    SnarlRecord snarl_record (snarl_handle, &snarl_tree_records);
    if (snarl_record.get_record_type() == OVERSIZED_SNARL) {
        //IF this is an oversized snarl, then we don't have any distance information so use the handlgraph algorithm
        //for traversing the shortest path

        //Get the start going out of the child chain (or just the node)
        handlegraph::handle_t start_handle = (is_trivial_chain(start) || is_sentinel(start)) ? get_handle(start, graph) :
            get_handle((ends_at(start) != START ? get_bound(start, true, false) : get_bound(start, false, false)), graph); 
        //Get the end going into the child chain (or just the node)
        handlegraph::handle_t end_handle = (is_trivial_chain(end) || is_sentinel(end)) ? get_handle(end, graph) :
            get_handle((ends_at(end) != START ?  get_bound(end, false, true) : get_bound(end, true, true) ), graph); 
#ifdef debug_distance_paths
        cerr << "Traversing oversized snarl from " << graph->get_id(start_handle) << (graph->get_is_reverse(start_handle) ? "rev" : "fd")
             << " to " << graph->get_id(end_handle) << (graph->get_is_reverse(end_handle) ? "rev" : "fd") << endl;
#endif

        //Add distance traversed through the snarl to distance_traversed
        size_t distance_to_add = 0;
        handlegraph::algorithms::for_each_handle_in_shortest_path(graph, start_handle, end_handle, [&](handlegraph::handle_t next_handle, size_t distance) {
            distance_to_add += graph->get_length(next_handle);
            if (to_duplicate != nullptr) {
                net_handle_t duplicated_node = get_net(next_handle, graph);
                to_duplicate->emplace_back(flip(duplicated_node), minimum_length(duplicated_node));
            }
            return iteratee(next_handle, distance + distance_traversed);
        });
        distance_traversed += distance_to_add;
#ifdef debug_distance_paths
        cerr << "Finished traversing oversized snarl, we've now traversed " << distance_traversed << endl;
#endif
        return;

    }
#ifdef debug_distance_paths
    unordered_set<net_handle_t> seen;
#endif

    net_handle_t next = start;

    //We need to make sure we actually try to find a path when start == end instead of stopping immediately
    bool started = start != end;
    while (next != end || !started) {
#ifdef debug_distance_paths
        cerr << "Walk out from " << net_handle_as_string(next) << endl;;
        if (seen.count(next) > 0) {
            cerr << "WE've already seen " << net_handle_as_string(next) << endl;
            throw runtime_error("error: Retraversing node");
        }
        seen.emplace(next);
#endif
        follow_net_edges(next, graph, false, [&] (const net_handle_t& next_net) {
#ifdef debug_distance_paths
            cerr << "Following net edge from " << net_handle_as_string(next) << endl;
            cerr << "Checking next net " << net_handle_as_string(next_net) << " find distance to " << net_handle_as_string(flip(end)) << endl;
            cerr << "Traversed " << distance_traversed << " so far, looking for " << distance_to_traverse << endl;
            bool snarl_is_root = is_root(snarl_handle) || is_root_snarl(snarl_handle) ||
                           SnarlTreeRecord(snarl_handle, &snarl_tree_records).get_record_type() == ROOT_SNARL ||
                            SnarlTreeRecord(snarl_handle, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL;
            if (!is_root(snarl_handle)) {
                if( (end != get_bound(snarl_handle, true, false) && next ==  get_bound(snarl_handle, true, false))
                || (end != get_bound(snarl_handle, false, false) && next ==  get_bound(snarl_handle, false, false))) {
                    throw runtime_error("error: Trying to leave the snarl without finding the thing we're looking for");
                }
            }
#endif
            if (next_net == end) {
                //If we reach the end, stop because we want the minimum distance path and we must be done
#ifdef debug_distance_paths
                cerr << "\tFinished traversal" << endl;
                assert(distance_to_traverse == 0);
#endif
                next = next_net;
                started = true;
                return false;
            } else if (is_sentinel(next_net)) { 
                //If we've reached a sentinel and are trying to leave the snarl but haven't hit the thing we're looking for yet
#ifdef debug_distance_paths
                cerr << "\tTrying to leave the snarl so continue looking" << endl;
#endif
               started = true;
                return true; //Continue
            } else if (sum(distance_in_parent(snarl_handle, next_net, flip(end)), minimum_length(next_net)) == distance_to_traverse) {
#ifdef debug_distance_paths
                cerr << "\tThis can reach the end going across in " << distance_to_traverse << " so iterate on it and continue from " << net_handle_as_string(next_net) << endl;
#endif
                //If the next node can reach the end in the correct distances going across the node

                if (is_trivial_chain(next_net) || is_node(next_net)) {
                    //If this is just a node, run iteratee on it
                    iteratee(get_handle(next_net, graph), distance_traversed);
                    distance_traversed += minimum_length(next_net);
                    distance_to_traverse -= minimum_length(next_net);
                    if (to_duplicate != nullptr) {
                        to_duplicate->emplace_back(flip(next_net), minimum_length(next_net));
                    }
                    
                } else if (is_chain(next_net)) {
#ifdef debug_distance_paths
                    size_t starting_distance_traversed = distance_traversed;
#endif
                    //Recurse on the chain
                    net_handle_t chain_start_bound = get_bound(next_net, ends_at(next_net) == START, true);
                    net_handle_t chain_end_bound = get_bound(next_net, ends_at(next_net) != START, false);

                    //iteratee on the start bound
                    iteratee(get_handle(chain_start_bound, graph), distance_traversed);
                    distance_traversed += minimum_length(chain_start_bound);
                    if (to_duplicate != nullptr) {
                        to_duplicate->emplace_back(flip(chain_start_bound), minimum_length(chain_start_bound));
                    }

                    //Do the chain
                    for_each_handle_in_shortest_path_in_chain(next_net, chain_start_bound, 
                                                              chain_end_bound,
                                                              minimum_length(next_net) - minimum_length(chain_start_bound) - minimum_length(chain_end_bound), 
                                                              distance_traversed, graph, iteratee, to_duplicate);

                    //iteratee on the end bound
                    iteratee(get_handle(chain_end_bound, graph), distance_traversed);
                    distance_traversed += minimum_length(chain_end_bound);
#ifdef debug_distance_paths
                    cerr << "Just traversed " << net_handle_as_string(next_net) << " with length " << minimum_length(next_net) << " started at distance " << starting_distance_traversed << " and now at " << distance_traversed << endl;
                    assert((distance_traversed - starting_distance_traversed) == minimum_length(next_net));
#endif

                    //This includes the bounds
                    distance_to_traverse -= minimum_length(next_net);
                    if (to_duplicate != nullptr) {
                        to_duplicate->emplace_back(flip(chain_end_bound), minimum_length(chain_end_bound));
                    }
                }
                next = next_net;
                started = true;

                return false;//Return false to stop iterating
            } else if (is_chain(next_net) && sum(sum(distance_in_parent(snarl_handle, flip(next_net), flip(end)), 
                             distance_in_parent(next_net, get_bound(next_net, ends_at(next_net) == START, true),
                                                          get_bound(next_net, ends_at(next_net) == START, true))),
                            2*minimum_length(get_bound(next_net, ends_at(next_net) == START, true))) == distance_to_traverse) {
#ifdef debug_distance_paths
                cerr << "\tThis can reach the end by looping in " << distance_to_traverse << " so iterate on it and continue from " << net_handle_as_string(flip(next_net)) << endl;
#endif
                //If the next node can reach the end in the correct distances by reversing in the node

                //Recurse on the chain
                net_handle_t chain_start_bound = get_bound(next_net, ends_at(next_net) == START, true);

                //Iteratee on the bound of the chain going in
                iteratee(get_handle(chain_start_bound, graph), distance_traversed);
                distance_traversed += minimum_length(chain_start_bound);
                distance_to_traverse -= minimum_length(chain_start_bound);
                if (to_duplicate != nullptr) {
                    to_duplicate->emplace_back(flip(chain_start_bound), minimum_length(chain_start_bound));
                }

                //Recurse on the chain finding a path from start to start
                for_each_handle_in_shortest_path_in_chain(next_net, chain_start_bound, 
                                                          flip(chain_start_bound),
                                                          distance_in_parent(next_net, chain_start_bound, chain_start_bound), 
                                                          distance_traversed, graph, iteratee, to_duplicate);
                distance_to_traverse -= distance_in_parent(next_net, chain_start_bound, chain_start_bound);


                //Iteratee on the bound of the chain going out
                iteratee(get_handle(flip(chain_start_bound), graph), distance_traversed);
                distance_traversed += minimum_length(chain_start_bound);
                distance_to_traverse -= minimum_length(chain_start_bound);
                if (to_duplicate != nullptr) {
                    to_duplicate->emplace_back(flip(chain_start_bound), minimum_length(chain_start_bound));
                }

                //Flip the chain so it's pointing backwards
                next = flip(next_net);
#ifdef debug_distance_paths
                cerr << "back to traversing snarl " << net_handle_as_string(snarl_handle) << " after looping through chain " << net_handle_as_string(next_net) << " now start from " << net_handle_as_string(next) << endl;
#endif
                started = false; //Just in case this is the thing we want to reach that we just looped on and now we need to reach it from the other side

                return false;//Return false to stop iterating
            } else {
#ifdef debug_distance_paths
                cerr << "\tThis is nothing interesting so continue looking" << endl;
                cerr << "\t\tdistance across is " << distance_in_parent(snarl_handle, next_net, flip(end)) << " + " <<  minimum_length(next_net)<< endl;
                cerr << "\t\tdistance to loop is " << distance_in_parent(snarl_handle, flip(next_net), flip(end)) << " + " <<  
                             distance_in_parent(next_net, get_bound(next_net, ends_at(next_net) == START, true),
                                                          get_bound(next_net, ends_at(next_net) == START, true)) << " + " << 
                            minimum_length(get_bound(next_net, ends_at(next_net) == START, true)) << endl;
#endif
                return true;//Return true to continue iterating
            }
        });
    }
#ifdef debug_distance_paths
    cerr << "Finished traversing " << net_handle_as_string(snarl_handle) << " we were looking for distance " << target_distance << " starting at " << starting_distance << " and now we've traversed " << distance_traversed << endl;
    assert(target_distance == (distance_traversed - starting_distance));
#endif
}
void SnarlDistanceIndex::for_each_handle_in_shortest_path_in_chain(const net_handle_t& chain_handle, net_handle_t start, net_handle_t end,
                                      size_t distance_to_traverse, size_t& distance_traversed, const HandleGraph* graph,
                                      const std::function<bool(const handlegraph::handle_t, size_t)>& iteratee,
                                      vector<pair<net_handle_t, size_t>>* to_duplicate_recursed) const {
#ifdef debug_distance_paths
    size_t target_distance = distance_to_traverse;
    size_t starting_distance = distance_traversed;
    cerr << "Find shortest path in " << net_handle_as_string(chain_handle) << " from " << net_handle_as_string(start) << " to " << net_handle_as_string(end) << " with distance " << distance_to_traverse << endl;
    cerr << "\tactual distance is " << distance_in_parent(chain_handle, start, flip(end))<< endl;
    assert(distance_in_parent(chain_handle, start, flip(end)) == distance_to_traverse);
    assert(canonical(chain_handle) == canonical(get_parent(start)));
    assert(canonical(chain_handle) == canonical(get_parent(end)));

#endif


    /* Walk through the chain and run iteratee for every node, recurse on every snarl
     * The shortest path may loop up to twice in the chain - (eg if it goes from node 2 fd to node 1 fd) 
     * In this case, keep track of everything that will be duplicated, and run through this list backwards,
     * reversing the orientations of nodes when going in the opposite direction 
     * There are three cases of the path being reversed:
     *   1) If the two children are oriented towards each other ( -1-> <-2- ) 
     *      The traversal goes forward from 1, reaches 2 going forward, reaches a loop, then duplicates everything between 2 and the loop
     *   2) If the two children are oriented away from each other ( <-2-  -1->  ) 
     *      The traversal goes forward from 1, finds a loop and duplicates everything until 1, then continues backwards until it reaches 2 
     *   3) If the first node comes after the second, both oriented in the same direction ( -2->   -1-> ) 
     *      The traversal must loop twice: traverse forward from 1, loop, duplicate everything from 1 to loop, traverse back to 2, 
     *      traverse until loop, duplicate everything between 2 and loop
     *
     * Since I made this recursive and there can be two to_duplicates, only fill in one of them at a time. Prioritize the to_duplicate
     * just created, and when to_duplicate gets duplicated, add the forward and reverse to to_duplicate_recursed
    */
    ChainRecord chain_record (chain_handle, &snarl_tree_records);

    //Are we going left in the chain?
    bool go_left_start = (ends_at(start) != START) == is_reversed_in_parent(start);
    bool go_left_end = (ends_at(end) != START) == is_reversed_in_parent(end);
    bool go_left = go_left_start;

    // We need to keep track of if we're looking for a loop immediately or after traversing a specific node, and the start and end
    // of the portion of the path that needs to be duplicated
    // If loop_asap is true, then keep adding everything found (flipped) to to_duplicate. At each snarl, only go forwards if it can't loop
    //   When the loop is found, call iteratee on everything in to_duplicate, in reverse order and continue from
    //   the first thing in the list
    // If loop_asap is false and loop_eventually is true (loop_after_this must not be the root), then traverse normally but check each
    //   child to see if it's loop_after_this. When loop_after_this is found, loop_after this becomes root, loop_asap becomes true, and
    //   we start looking for a loop
    //
    // to_duplicate will start with the net_handle_t that we start duplicating on (loop_after_this if we didn't start immediately)
    // Everything but the first will be a node and gets copied and re-traversed, the first one becomes start in the next iteration  
    // If the first thing is a node, then it goes in to_duplicate twice
    bool loop_asap = false; 
    vector<pair<net_handle_t, size_t>> to_duplicate;
    bool loop_eventually = false;
    net_handle_t loop_after_this = get_root();
    size_t last_loop = is_node(start) ? (go_left ? get_reverse_loop_value(start) : get_forward_loop_value(start))
                                      : std::numeric_limits<size_t>::max();

    bool ordered = is_ordered_in_chain(start, end);
#ifdef debug_distance_paths
    cerr << "Children are " << (ordered ? " ordered in chain " : " not ordered in chain") << " node 1 is going " 
         << (go_left_start ? " reverse " : " forward ") << " and node 2 is going " << (go_left_end ? " reverse " : " forward")
         << " in the chain" << endl;
#endif
    if ((ordered && !go_left_start && go_left_end) || 
        (!ordered && go_left_start && !go_left_end)) {
#ifdef debug_distance_paths
        cerr << " Children are oriented towards each other so loop at " << net_handle_as_string(flip(end)) << endl;
#endif
        //Case 1 - oriented towards each other- go forwards from 1, pass 2, and then loop
        if (flip(end) == start) {
#ifdef debug_distance_paths
            cerr << "\tbut this is really the start so just loop immediately" << endl;
#endif
            loop_asap = true;

        } else {
            loop_eventually = true;
            loop_after_this = flip(end);
        }
    } else if ((ordered && go_left_start && !go_left_end) ||
               (!ordered && !go_left_start && go_left_end)) {
#ifdef debug_distance_paths
        cerr << " Children are oriented away from each other so loop at asap" << endl;
#endif
        //Case 2 - oriented away from each other - go forwards from 1, try to loop immediately to find 2
        loop_asap = true;

    } else if ((ordered && go_left_start && go_left_end) ||
               (!ordered && !go_left_start && !go_left_end) ||
               start == end) {
        //Case 3 - loop twice - go forwards from 1 and loop immediately, then pass 2 and loop
#ifdef debug_distance_paths
        cerr << " Children are oriented backwards so loop at asap and then at " << net_handle_as_string(flip(end)) << endl;
#endif
        loop_asap = true;
        loop_eventually = true;
        loop_after_this = flip(end);
    }

    //If the start and end are the same, need to make sure that we actually start traversing
    bool started = start != end;
    
    while (start != end || !started) {
#ifdef debug_distance_paths
        cerr << "at chain child " << net_handle_as_string(start) << (go_left ? " going left" : " going right") << endl;
        cerr << "\t traversed " << distance_traversed << ", reach end (" << net_handle_as_string(end) << " in " << distance_to_traverse << endl;
        cerr << "\t distance to end " << net_handle_as_string(end) << distance_in_parent(chain_handle, start, flip(end)) << endl;
        if (loop_asap) {
            cerr << "\tTrying to loop" << endl;
        }
        assert(start != get_bound(chain_handle, false, false));
        assert(start != get_bound(chain_handle, true, false));
        assert(distance_to_traverse == distance_in_parent(chain_handle, start, flip(end)));
#endif
        net_handle_t next = chain_record.get_next_child(start, go_left); 
#ifdef debug_distance_paths
        cerr << "\treached next node " << net_handle_as_string(next) << endl;
#endif
        bool need_to_flip = go_left;
        if (ends_at(next) == START) {
            need_to_flip = !need_to_flip;
        }
        if (is_reversed_in_parent(next)) {
            need_to_flip = !need_to_flip;
        }

        if (need_to_flip) {
            next = flip(next);
#ifdef debug_distance_paths
            cerr << "Flipping next node "  << endl;
#endif
        }
        if (next == end && started) {
#ifdef debug_distance_paths
            cerr << "\tthis is the end so we're done" << endl;
#endif
            start = next;
            break;
        } else if (is_node(next)) {
            //If this is a node, then run iteratee on it 
#ifdef debug_distance_paths
            cerr << "\titerate on node " << net_handle_as_string(next) << " with length " << minimum_length(next) << endl;
#endif
            iteratee(get_handle(next, graph), distance_traversed);
            size_t length = minimum_length(next);
            distance_traversed += length;
            distance_to_traverse -= length;

            last_loop = go_left ? get_reverse_loop_value(next) : get_forward_loop_value(next);
            if (loop_asap) {
                //If we want to loop as soon as possible, remember the distance to loop from this node (in the next snarl)
#ifdef debug_distance_paths
                cerr << "We want to loop, the next loop distance after " << net_handle_as_string(next) << " will be " << last_loop << endl;
#endif
                if (last_loop == 0) {
                    //If we want to loop on this node
#ifdef debug_distance_paths
                    cerr << "Loop on this node" << endl;
#endif

                    //Flip the node
                    next = flip(next);

                    //Before we loop, copy everything we've saved so far to the higher recursion to_duplicate
                    if (to_duplicate_recursed != nullptr) {
                        for (size_t i = 1 ; i < to_duplicate.size() ; i++) {
                            to_duplicate_recursed->emplace_back(to_duplicate[i]);
                        }
                    }

                    //Iteratee() on the flipped node
                    iteratee(get_handle(next, graph), distance_traversed);
                    distance_traversed += length;
                    distance_to_traverse -= length;

#ifdef debug_distance_paths
                cerr << "Finished looping on the node, now go back and repeat the duplicated portion (" << to_duplicate.size() << " things to duplicate)" << endl;
                for (auto x : to_duplicate) {
                    cerr << "\t" << net_handle_as_string(x.first) << ": " << x.second << endl;
                }
#endif

                    //Now backtrack to when we started tracking the duplicated part
                    //Skip the first thing in to_duplicate, which might be in to_duplicate twice
                    if (to_duplicate.size() > 1) {
                        for (int i = to_duplicate.size() - 1 ; i >= 1 ; i--) {
                            iteratee(get_handle(to_duplicate[i].first, graph), distance_traversed);
                            distance_traversed += to_duplicate[i].second;
                            distance_to_traverse -= to_duplicate[i].second;
                            if (to_duplicate_recursed != nullptr) {
                                to_duplicate_recursed->emplace_back(flip(to_duplicate[i].first), to_duplicate[i].second);
                            }
                        }
                    }

                    //And continue traversing from the first thing in to_duplicate (or the current node)
                    next = to_duplicate.size() == 0 ? next : to_duplicate[0].first;
                    to_duplicate.clear();
                    loop_asap = false;
                    go_left = !go_left;
#ifdef debug_distance_paths
                    cerr << "WE looped and now the new current node is " << net_handle_as_string(next) << 
                            " and we're going " << (go_left ? "left " : "right ") << "in the chain" << endl;
#endif
                    
                } else {
                    if (to_duplicate.size() == 0) 
                        to_duplicate.emplace_back(flip(next), 0);{
                    }
                    to_duplicate.emplace_back(flip(next), length);
                }
            } else if (to_duplicate_recursed != nullptr) {
                to_duplicate_recursed->emplace_back(flip(next), length);
            }
        } else {
            net_handle_t start_in = go_left ? get_bound(next, true, true) : get_bound(next, false, true);
            //If this is a snarl and it must be traversed, then recurse on the snarl
            //If the chain is being traversed backwards, traverse the snarl end->start, otherwise start->end
            if (loop_asap && last_loop != std::numeric_limits<size_t>::max() &&
                last_loop == distance_in_parent(next, start_in,start_in, graph, std::numeric_limits<size_t>::max())) {
#ifdef debug_distance_paths
                cerr << "This snarl is the best place to loop so loop and then duplicate the last " << to_duplicate.size() << " traversals" << endl;
#endif
                //If we're trying to loop and the shortest loop is in this snarl, then traverse the snarl in a loop 
                // and go through to_duplicate in reverse
                // If we're also tracking to_duplicate_recursed, then we haven't added anything since we started adding to 
                // to_duplicate, so first add everything there, then add everything duplicated as well
                if (to_duplicate_recursed != nullptr) {
                    for (size_t i = 1 ; i < to_duplicate.size() ; i++) {
                        to_duplicate_recursed->emplace_back(to_duplicate[i]);
                    }
                }
                for_each_handle_in_shortest_path_in_snarl(next, start_in, flip(start_in),
                                                      last_loop, distance_traversed, graph, iteratee, to_duplicate_recursed);
                distance_to_traverse -= last_loop;

#ifdef debug_distance_paths
                cerr << "Finished looping in the snarl, now go back and repeat the duplicated portion (" << to_duplicate.size() << " things to duplicate)" << endl;
                for (auto x : to_duplicate) {
                    cerr << "\t" << net_handle_as_string(x.first) << ": " << x.second << endl;
                }
#endif
                //Now backtrack to when we started tracking the duplicated part
                if (to_duplicate.size() > 1) {
                    for (int i = to_duplicate.size() - 1 ; i >= 1 ; i--) {
                        iteratee(get_handle(to_duplicate[i].first, graph), distance_traversed);
                        distance_traversed += to_duplicate[i].second;
                        distance_to_traverse -= to_duplicate[i].second;
                        if (to_duplicate_recursed != nullptr) {
                            to_duplicate_recursed->emplace_back(flip(to_duplicate[i].first), to_duplicate[i].second);
                        }
                    }
                }

                //And continue traversing from the first thing in to_duplicate
                next = to_duplicate.size() == 0 ? flip(next) : to_duplicate[0].first;
                to_duplicate.clear();
                loop_asap = false;
                go_left = !go_left;
#ifdef debug_distance_paths
                cerr << "Finished looping and duplicating, now continue from " << net_handle_as_string(next) << endl;
#endif
            } else if (minimum_length(next) != 0) {
                //Otherwise traverse across snarl (if there is path straight across)
#ifdef debug_distance_paths
                if (loop_asap) {
                    cerr << "this snarl didn't have the loop distance " << last_loop << " so traverse it forward" << endl;
                }
#endif

                if (loop_asap && to_duplicate.size() == 0) 
                    //If we're keeping track of what to duplicate and this is the first thing, then remember that when we finish
                    //duplicating, we go backwards from here
                    to_duplicate.emplace_back(flip(next), 0);{
                }
                for_each_handle_in_shortest_path_in_snarl(next, start_in,
                                                            go_left ? get_bound(next, false, false) : get_bound(next, true, false),
                                                      minimum_length(next), distance_traversed, graph, iteratee,
                                                      loop_asap ? &to_duplicate : to_duplicate_recursed);
                distance_to_traverse -= minimum_length(next);
            }
            last_loop = std::numeric_limits<size_t>::max();
        }
        if (!loop_asap && loop_eventually && next == loop_after_this) {
            //If we want to find a loop after the current node/snarl
#ifdef debug_distance_paths
            cerr << "Found the next thing we want to loop after, so now look for a loop asap" << endl;
#endif
            loop_eventually = false;
            loop_asap = true;
            loop_after_this = get_root();
        }
        start = next;
        started = true;
    }
#ifdef debug_distance_paths
    cerr << "Finished traversing " << net_handle_as_string( chain_handle) << " we were looking for distance " << target_distance << " starting at " << starting_distance << " and now we've traversed " << distance_traversed << endl;
    assert(target_distance == (distance_traversed - starting_distance));
#endif
}


size_t SnarlDistanceIndex::node_length(const net_handle_t& net) const {
    if (is_node(net)) {
        if (get_record_type(snarl_tree_records->at(get_record_offset(net))) == DISTANCED_NODE) {
            return NodeRecord(net, &snarl_tree_records).get_node_length();
        } else if (get_record_type(snarl_tree_records->at(get_record_offset(net))) == DISTANCED_SIMPLE_SNARL) {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_node_length();
        } else {
            assert(get_record_type(snarl_tree_records->at(get_record_offset(net))) == DISTANCED_TRIVIAL_SNARL);
            return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_length(get_node_record_offset(net));
        }
    } else if (is_sentinel(net)) {
        return node_length(get_node_from_sentinel(net));
    } else {
        throw runtime_error("error: Looking for the node length of a non-node net_handle_t");
    }

}


size_t SnarlDistanceIndex::minimum_length(const net_handle_t& net) const {
    auto record_type = SnarlTreeRecord(net, &snarl_tree_records).get_record_type();
    if (!has_distances(record_type)) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    } else if (record_type == DISTANCED_TRIVIAL_SNARL) {
        return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_length(get_node_record_offset(net));
    } else if (record_type == DISTANCED_SIMPLE_SNARL) {
        if (is_snarl(net)) {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_min_length();
        } else {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_node_length();
        }
    } else if (record_type == MULTICOMPONENT_CHAIN) {
        return std::numeric_limits<size_t>::max();
    } else {
        return SnarlTreeRecord(net, &snarl_tree_records).get_min_length();
    }
}

size_t SnarlDistanceIndex::chain_minimum_length(const net_handle_t& net) const {
#ifdef debug_distances
    assert(is_chain(net));
#endif
    if (is_trivial_chain(net)) {
        return minimum_length(net);
    } else {
        return ChainRecord(net, &snarl_tree_records).get_min_length();
    }
}
size_t SnarlDistanceIndex::maximum_length(const net_handle_t& net) const {
    auto record_type = SnarlTreeRecord(net, &snarl_tree_records).get_record_type();
    if (!has_distances(record_type)) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    } else if (record_type == DISTANCED_TRIVIAL_SNARL &&
        get_node_record_offset(net) != 0) {
        return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_length(get_node_record_offset(net));
    } else if (record_type == DISTANCED_SIMPLE_SNARL) {
        if (is_snarl(net)) {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_max_length();
        } else {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_node_length();
        }
    } else {
        return SnarlTreeRecord(net, &snarl_tree_records).get_max_length();
    }
}
nid_t SnarlDistanceIndex::node_id(const net_handle_t& net) const {
    if (is_node(net) || is_trivial_chain(net)) {
        if (get_record_type(snarl_tree_records->at(get_record_offset(net))) == NODE 
            || get_record_type(snarl_tree_records->at(get_record_offset(net))) == DISTANCED_NODE) {
            return NodeRecord(net, &snarl_tree_records).get_node_id();
        }  else if (get_record_type(snarl_tree_records->at(get_record_offset(net))) == SIMPLE_SNARL
                 || get_record_type(snarl_tree_records->at(get_record_offset(net))) == DISTANCED_SIMPLE_SNARL) {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_node_id();
        } else {
            return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_id(get_node_record_offset(net));
        }
    } else if (is_sentinel(net)) {
        SnarlRecord snarl_record(net, &snarl_tree_records);
        NodeRecord node_record;
        if (get_start_endpoint(net) == START) {
            return snarl_record.get_start_id();
        } else {
            return snarl_record.get_end_id();
        }
    } else {
        throw runtime_error("error: Looking for the node id of a non-node net_handle_t");
    }

}
bool SnarlDistanceIndex::has_node(const nid_t id) const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    size_t node_pointer_offset = get_node_pointer_offset(id, root_record.get_min_node_id(), root_record.get_connected_component_count());
    return snarl_tree_records->at(node_pointer_offset) != 0;
}

bool SnarlDistanceIndex::is_reversed_in_parent(const net_handle_t& net) const {
    SnarlTreeRecord record(net, &snarl_tree_records);
    if (record.get_record_type() == TRIVIAL_SNARL || record.get_record_type() == DISTANCED_TRIVIAL_SNARL) {
        return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_is_reversed_in_parent(get_node_record_offset(net));
    } else if ((record.get_record_type() == SIMPLE_SNARL || record.get_record_type() == DISTANCED_SIMPLE_SNARL) && is_chain(net)) {
        return SimpleSnarlRecord(net, &snarl_tree_records).get_node_is_reversed();
    } else {
        return record.get_is_reversed_in_parent();
    }
}
net_handle_t SnarlDistanceIndex::get_node_net_handle(const nid_t id, bool rev) const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    size_t node_pointer_offset = get_node_pointer_offset(id, root_record.get_min_node_id(), root_record.get_connected_component_count());
    size_t record_offset = snarl_tree_records->at(node_pointer_offset);
    size_t node_record_offset = snarl_tree_records->at(node_pointer_offset+1);
    return get_net_handle_from_values(record_offset, rev ? END_START : START_END, NODE_HANDLE, node_record_offset); 
}

size_t SnarlDistanceIndex::get_max_tree_depth() const {
    return RootRecord(get_root(), &snarl_tree_records).get_max_tree_depth();
}

size_t SnarlDistanceIndex::get_depth(const net_handle_t& net) const {
    if (is_root(net)) {
        return 0;
    } else if (SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
               SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL ){
        //If this is a simple snarl, then it can be a node, snarl, or chain

        //The depth of the snarl's parent chain
        size_t snarl_depth = get_depth(get_parent(get_net_handle_from_values(get_record_offset(net), START_END, SNARL_HANDLE, 1)));
        if (is_snarl(net) || is_sentinel(net)) {
            //If this is a snarl or sentinel of the snarl, then it has the depth of its parent
            return snarl_depth;
        } else {
            //If it is the snarl's child node or chain, then add 1
            return snarl_depth + 1;
        }
    } else if (is_snarl(net) || is_sentinel(net) || is_node(net)) {
        return get_depth(get_parent(net));
    } else if (is_trivial_chain(net)) {
        return get_depth(get_parent(net)) + 1;
    } else if (is_chain(net)) {
        return ChainRecord(net, &snarl_tree_records).get_depth();
    } else {
        throw runtime_error("error: Unknown handle type");
    }
        
}

size_t SnarlDistanceIndex::get_connected_component_number(const net_handle_t& net) const {
    if (get_record_offset(net) == 0) {
        throw runtime_error("error: trying to get the connected component number of the root");
    }
    net_handle_t child = net;
    net_handle_t parent = is_root(net) ? net : get_parent(net);
    while (!is_root(parent)) {
        child = parent;
        parent = get_parent(child);
    }
    if (get_record_offset(parent) == 0) {
        //If the parent is actually the root
        return SnarlTreeRecord(child, &snarl_tree_records).get_rank_in_parent();
    } else {
        //Otherwise, it must be a root-level snarl pretending to be a root
        return SnarlTreeRecord(parent, &snarl_tree_records).get_rank_in_parent();
    }
}


net_handle_t SnarlDistanceIndex::get_handle_from_connected_component(size_t num) const {
    size_t child_offset = snarl_tree_records->at(ROOT_RECORD_SIZE + num);
    net_handle_record_t type = SnarlTreeRecord(child_offset, &snarl_tree_records).get_record_handle_type();
    if (type == NODE_HANDLE) {
        //If this child is a node, then pretend it's a chain
        return get_net_handle_from_values(child_offset, START_END, CHAIN_HANDLE);
    } else if (type == SNARL_HANDLE) {
        return get_net_handle_from_values(child_offset, START_END, ROOT_HANDLE);
    } else {
        return get_net_handle_from_values(child_offset, START_END, type);
    }
}



bool SnarlDistanceIndex::has_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const {
    SnarlTreeRecord record(net, &snarl_tree_records);
    return record.has_connectivity(start, end);
}

SnarlDistanceIndex::SnarlTreeRecord::SnarlTreeRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = pointer;
    records = tree_records;
 
 #ifdef debug_distance_indexing
    record_t type = get_record_type();
    assert(type >= 1 && type <= 17 );
 #endif
}

SnarlDistanceIndex::SnarlTreeRecord::SnarlTreeRecord (const net_handle_t& net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
 #ifdef debug_distance_indexing
    record_t type = get_record_type();
    assert(type >= 1 && type <= 17 );
 #endif
}


bool SnarlDistanceIndex::SnarlTreeRecord::has_connectivity(connectivity_t connectivity) const {
    if (connectivity == START_START) {
        return is_start_start_connected();
    } else if (connectivity == START_END || connectivity == END_START) {
        return is_start_end_connected();
    } else if (connectivity == START_TIP || connectivity == TIP_START) {
        return is_start_tip_connected();
    } else if (connectivity == END_END) {
        return is_end_end_connected();
    } else if (connectivity == END_TIP || connectivity == TIP_END) {
        return is_end_tip_connected();
    } else if (connectivity == TIP_TIP) {
        return is_tip_tip_connected();
    } else {
        throw runtime_error("error: Invalid connectivity");
    }
}

size_t SnarlDistanceIndex::get_prefix_sum_value(const net_handle_t& net) const {
#ifdef debug_distances
    net_handle_t parent = get_parent(net);
    if (!is_node(net) || !is_chain(parent) || is_trivial_chain(parent)) {
        throw runtime_error("error: Trying to get chain values from a net_handle_t that isn't a node in a chain");
    }
    if (!is_chain(get_parent(net)) || is_trivial_chain(get_parent(net))) {
        return std::numeric_limits<size_t>::max();
    }
#endif
    return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_prefix_sum(get_node_record_offset(net));
}
size_t SnarlDistanceIndex::get_max_prefix_sum_value(const net_handle_t& net) const {
#ifdef debug_distances
    net_handle_t parent = get_parent(net);
    if (!is_node(net) || !is_chain(parent) || is_trivial_chain(parent)) {
        throw runtime_error("error: Trying to get chain values from a net_handle_t that isn't a node in a chain");
    }
    if (!is_chain(get_parent(net)) || is_trivial_chain(get_parent(net))) {
        return std::numeric_limits<size_t>::max();
    }
#endif
    return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_max_prefix_sum(get_node_record_offset(net));
}

size_t SnarlDistanceIndex::get_forward_loop_value(const net_handle_t& net) const {
#ifdef debug_distances
    net_handle_t parent = get_parent(net);
    if (!is_node(net) || !is_chain(parent) || is_trivial_chain(parent)) {
        throw runtime_error("error: Trying to get chain values from a net_handle_t that isn't a node in a chain");
    }
    if (!is_chain(get_parent(net)) || is_trivial_chain(get_parent(net))) {
        return std::numeric_limits<size_t>::max();
    }
#endif
    return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_forward_loop(get_node_record_offset(net));
}

size_t SnarlDistanceIndex::get_reverse_loop_value(const net_handle_t& net) const {
#ifdef debug_distances
    net_handle_t parent = get_parent(net);
    if (!is_node(net) || !is_chain(parent) || is_trivial_chain(parent)) {
        throw runtime_error("error: Trying to get chain values from a net_handle_t that isn't a node in a chain");
    }
    if (!is_chain(get_parent(net)) || is_trivial_chain(get_parent(net))) {
        return std::numeric_limits<size_t>::max();
    }
#endif
    return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_reverse_loop(get_node_record_offset(net));
}

size_t SnarlDistanceIndex::get_chain_component(const net_handle_t& net, bool get_end) const {
#ifdef debug_distances
    net_handle_t parent = get_parent(net);
    if (!is_node(net) || !is_chain(parent) || is_trivial_chain(parent)) {
        throw runtime_error("error: Trying to get chain values from a net_handle_t that isn't a node in a chain");
    }
    if (!is_chain(get_parent(net)) || is_trivial_chain(get_parent(net))) {
        return std::numeric_limits<size_t>::max();
    }
#endif
    return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_chain_component(get_node_record_offset(net), get_end);
}


size_t SnarlDistanceIndex::SnarlTreeRecord::get_min_length() const {
    record_t type = get_record_type();
    size_t val;
    if (type == DISTANCED_NODE ) {
        return (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    } else if (type == DISTANCED_TRIVIAL_SNARL) {
        size_t last_node_offset = TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (last_node_offset*2) + 1);
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        val =  (*records)->at(record_offset + SNARL_MIN_LENGTH_OFFSET);
    } else if (type == DISTANCED_SIMPLE_SNARL)  {
        size_t raw_val =  (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET);
        return (raw_val >> 11) & ((1 << 11) - 1);
    } else if (type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        val = (*records)->at(record_offset + CHAIN_MIN_LENGTH_OFFSET);
    } else if (type == NODE || type == SNARL || type == CHAIN) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    } else if (type == ROOT || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the length of the root");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
    return val == 0 ? std::numeric_limits<size_t>::max() : val - 1;
};

size_t SnarlDistanceIndex::SnarlTreeRecord::get_max_length() const {
    record_t type = get_record_type();
    size_t val;
    if (type == DISTANCED_NODE ) {
        return (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    } else if (type == DISTANCED_TRIVIAL_SNARL) {
        size_t last_node_offset = TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (last_node_offset*2) + 1);
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        val = (*records)->at(record_offset + SNARL_MAX_LENGTH_OFFSET);
    } else if (type == DISTANCED_SIMPLE_SNARL)  {
        size_t raw_val =  (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET);
        return raw_val & ((1 << 11) - 1);
    } else if (type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        val = (*records)->at(record_offset + CHAIN_MAX_LENGTH_OFFSET);
    } else if (type == NODE || type == SNARL || type == CHAIN) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }  else if (type == ROOT || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the length of the root");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }

    return val == 0 ? std::numeric_limits<size_t>::max() : val - 1;
};

size_t SnarlDistanceIndex::SnarlTreeRecord::get_rank_in_parent() const {
    record_t type = get_record_type();
    if (type == NODE || type == DISTANCED_NODE) {
        return (*records)->at(record_offset + NODE_RANK_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL)  {
        throw runtime_error("error: node ranks need the node offsets");
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        //For root snarls, the rank gets stored in the length slot
        return (*records)->at( record_offset + SNARL_MIN_LENGTH_OFFSET);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            ||  type == SIMPLE_SNARL
            || type == DISTANCED_SIMPLE_SNARL)  {
        return record_offset;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return (*records)->at(record_offset + CHAIN_RANK_OFFSET) >> 1;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
};
bool SnarlDistanceIndex::SnarlTreeRecord::get_is_reversed_in_parent() const {
    record_t type = get_record_type();
    if (type == NODE || type == DISTANCED_NODE) {
        return false;
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        cerr << "warning: Getting orientation of a trivial snarl" << endl;
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL 
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        return false;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return (*records)->at(record_offset + CHAIN_RANK_OFFSET) & 1;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
};

handlegraph::nid_t SnarlDistanceIndex::SnarlTreeRecord::get_start_id() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        throw runtime_error("error: trying to get the start node of the root");
    } else if (type == NODE || type == DISTANCED_NODE) {
        //cerr << "warning: Looking for the start of a node" << endl;
        return (*records)->at(record_offset + NODE_ID_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        TrivialSnarlRecord trivial_snarl_record(record_offset, records);
        return trivial_snarl_record.get_node_id(0);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
             || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        //To get the start node of a snarl, get the thing to the left of it in the chain
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle_from_values(record_offset, START_END, SNARL_HANDLE, node_offset), true);
        return TrivialSnarlRecord(get_record_offset(next_node_in_chain), records).get_node_id(get_node_record_offset(next_node_in_chain));
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_START_NODE_OFFSET)) >> 1;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the start node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
}
bool SnarlDistanceIndex::SnarlTreeRecord::get_start_orientation() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        throw runtime_error("error: trying to get the start node of the root");
    } else if (type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        //cerr << "warning: Looking for the start of a node" << endl;
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
             || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        //Get the next node in the chain (going left)
        //The handle will be pointing in the direction we just moved, so if it is going START_END, then it is reversed
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle_from_values(record_offset, START_END, SNARL_HANDLE, node_offset), true);
        return get_end_endpoint( get_connectivity(next_node_in_chain)) == END;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_START_NODE_OFFSET)) & 1;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the start node of a root snarl");
    }else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
}
handlegraph::nid_t SnarlDistanceIndex::SnarlTreeRecord::get_end_id() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        throw runtime_error("error: trying to get the end node of the root");
    } else if (type == NODE || type == DISTANCED_NODE ) {
        //cerr << "warning: Looking for the end of a node" << endl;
        //Offset of the start of the node vector
        return (*records)->at(record_offset + NODE_ID_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        TrivialSnarlRecord trivial_snarl_record(record_offset, records);
        return trivial_snarl_record.get_node_id(trivial_snarl_record.get_node_count()-1);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        //For a snarl, walk right in the chain
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle_from_values(record_offset, START_END, SNARL_HANDLE, node_offset), false);
        return TrivialSnarlRecord(get_record_offset(next_node_in_chain), records).get_node_id(get_node_record_offset(next_node_in_chain));
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_END_NODE_OFFSET)) >> 1;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the end node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
}

handlegraph::nid_t SnarlDistanceIndex::SnarlTreeRecord::get_end_orientation() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        throw runtime_error("error: trying to get the end node of the root");
    } else if (type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        //cerr << "warning: Looking for the end of a node" << endl;
        //Offset of the start of the node vector
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        //Get the next node in the chain (going right)
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle_from_values(record_offset, START_END, SNARL_HANDLE, node_offset), false);
        //The handle will be pointing in the direction we just moved, so if it is going END_START, then it is reversed
        return get_end_endpoint( get_connectivity(next_node_in_chain)) == START;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_END_NODE_OFFSET)) & 1;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the end node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
}



bool SnarlDistanceIndex::SnarlTreeRecord::has_connectivity(endpoint_t start, endpoint_t end){
    return has_connectivity(endpoints_to_connectivity(start, end));
}

bool SnarlDistanceIndex::has_external_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const {
    size_t tag = snarl_tree_records->at(get_record_offset(net));
    if (start == START && end == START) {
        return is_externally_start_start_connected(tag);
    } else if (start == END && end == END) {
        return is_externally_end_end_connected(tag);
    } else if ( (start == START && end == END ) || (start == END && end == START)) {
        return is_externally_start_end_connected(tag);
    } else {
        return false;
    }
}

size_t SnarlDistanceIndex::SnarlTreeRecord::get_parent_record_offset() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        return 0;
    } else if (type == NODE || type == DISTANCED_NODE) {
        return ((*records)->at(record_offset + NODE_PARENT_OFFSET));
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        return (*records)->at(record_offset + TRIVIAL_SNARL_PARENT_OFFSET);
    } else if (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) {
        return (*records)->at(record_offset + SIMPLE_SNARL_PARENT_OFFSET);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
        return ((*records)->at(record_offset + SNARL_PARENT_OFFSET));
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_PARENT_OFFSET));
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
};

SnarlDistanceIndex::SnarlTreeRecordWriter::SnarlTreeRecordWriter (size_t pointer, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = pointer;
    records = tree_records;
 
#ifdef debug_distance_indexing
    record_t type = get_record_type();
    assert(type == ROOT || type == NODE || type == DISTANCED_NODE ||
            type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL ||
            type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL ||
            type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL ||
            type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == CHAIN ||
            type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN);
#endif
}

SnarlDistanceIndex::SnarlTreeRecordWriter::SnarlTreeRecordWriter (const net_handle_t& net, 
                                                          bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
#ifdef debug_distance_indexing
    record_t type = get_record_type();
    assert(type == ROOT || type == NODE || type == DISTANCED_NODE ||  
            type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL || 
            type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL || 
            type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || 
            type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == CHAIN ||
            type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN);
#endif
}




SnarlDistanceIndex::record_t SnarlDistanceIndex::SnarlTreeRecordWriter::get_record_type() const {
    return static_cast<record_t>((*records)->at(record_offset) >> 9);
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_start_start_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_start_connected" << endl;
#endif    

    (*records)->at(record_offset) = (*records)->at(record_offset) | 32;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_start_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 16;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_start_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 8;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_end_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set end_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 4;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_end_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set end_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 2;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_tip_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set tpi_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 1;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_externally_start_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally start_end connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 64;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_externally_start_start_connected() const {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally start_start connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 128;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_externally_end_end_connected() const {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally end_end connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 256;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_record_type(record_t type) {
    assert((*records)->at(record_offset) == 0);
    (*records)->at(record_offset) = ((static_cast<size_t>(type) << 9) | ((*records)->at(record_offset) & 511));
}


void SnarlDistanceIndex::SnarlTreeRecordWriter::set_min_length(size_t length) {
    record_t type = get_record_type();
    size_t offset;
    if (type == DISTANCED_NODE) {
        offset = record_offset + NODE_LENGTH_OFFSET;
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        offset = record_offset + SNARL_MIN_LENGTH_OFFSET;
    } else if (type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_MIN_LENGTH_OFFSET;
    } else if (type == DISTANCED_SIMPLE_SNARL) {
        if (length > (1 << 11)-1) {
            throw runtime_error("error: node length is too large");
        }
        size_t old_val =  (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET);
        size_t new_val = old_val | (length << 11);
        (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET)= new_val;
         return;  
    } else if (type == NODE || type == SNARL || type == CHAIN ) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the length of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set min length to be " << length << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = (length == std::numeric_limits<size_t>::max() ? 0 : length + 1);
};
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_max_length(size_t length) {
    record_t type = get_record_type();
    size_t offset;
    if (type == DISTANCED_NODE) {
        throw runtime_error("error: set the max length of a node");
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        offset = record_offset + SNARL_MAX_LENGTH_OFFSET;
    } else if (type == DISTANCED_SIMPLE_SNARL) {
        if (length > (1 << 11)-1) {
            throw runtime_error("error: node length is too large");
        }
        size_t old_val =  (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET);
        size_t new_val = old_val | length;
        (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET) = new_val;
        return;
    } else if (type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_MAX_LENGTH_OFFSET;
    } else if (type == DISTANCED_NODE || type == SNARL || type == CHAIN) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the length of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set max length to be " << length << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = (length == std::numeric_limits<size_t>::max() ? 0 : length + 1);
};

void SnarlDistanceIndex::SnarlTreeRecordWriter::set_rank_in_parent(size_t rank) {
    record_t type = get_record_type();
    size_t offset;
    if (type == NODE || type == DISTANCED_NODE) {
        offset = record_offset + NODE_RANK_OFFSET;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        (*records)->at(record_offset + SNARL_MIN_LENGTH_OFFSET) = rank;
        return;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            ||  type == SIMPLE_SNARL
            || type == DISTANCED_SIMPLE_SNARL)  {
        cerr << "SETTING THE RANK OF A SNARL WHICH I'M PRETTY SURE DOESN'T MEAN ANYTHING" << endl;
        return;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_RANK_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set rank in parent to be " << rank << endl;
    assert((*records)->at(offset) >> 1 == 0);
#endif

    bool rev = (*records)->at(offset) & 1;
    (*records)->at(offset) = (rank << 1) | rev;
};
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_is_reversed_in_parent(bool rev) {
    record_t type = get_record_type();
    size_t offset;
    if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        return;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_RANK_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set rev in parent to be " << rev << endl;
#endif

    (*records)->at(offset) =  (((*records)->at(offset)>>1)<<1) | rev;
};
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_parent_record_offset(size_t pointer){
    record_t type = get_record_type();
    size_t offset;
    if (type == NODE || type == DISTANCED_NODE) {
        offset = record_offset + NODE_PARENT_OFFSET;
    }  else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        offset = record_offset + TRIVIAL_SNARL_PARENT_OFFSET;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
#ifdef debug_distance_indexing
        if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
            assert(pointer == 0);
        }
#endif

        offset = record_offset + SNARL_PARENT_OFFSET;
    } else if (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) {
        offset = record_offset + SIMPLE_SNARL_PARENT_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_PARENT_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set parent offset to be " << pointer << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = pointer;
};
//Rev is true if the node is traversed backwards to enter the snarl
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_start_node(handlegraph::nid_t id, bool rev) {
    record_t type = get_record_type();
    size_t offset;
    if (type == ROOT || type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        throw runtime_error("error: trying to set the start node id of a node or root");
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        throw runtime_error("error: trying to set the start node id of a snarl");
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_START_NODE_OFFSET;
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the start node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set start node to be " << id << " facing " << (rev ? "rev" : "fd") << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = (id << 1) | rev;
}
void SnarlDistanceIndex::SnarlTreeRecordWriter::set_end_node(handlegraph::nid_t id, bool rev) const {
    record_t type = get_record_type();
    size_t offset;
    if (type == ROOT || type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        throw runtime_error("error: trying to set the node id of a node or root");
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        throw runtime_error("error: trying to set the end node id of a snarl");
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_END_NODE_OFFSET;
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the end node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_distance_indexing
    cerr << offset << " set end node to be " << id << " facing " << (rev ? "rev" : "fd") << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = (id << 1) | rev;
}

SnarlDistanceIndex::RootRecord::RootRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = pointer;
    records = tree_records;
#ifdef debug_distance_indexing
    assert(get_record_type() == ROOT);
#endif
}


SnarlDistanceIndex::RootRecord::RootRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
#ifdef debug_distance_indexing
    assert(get_record_type() == ROOT);
#endif
}



bool SnarlDistanceIndex::RootRecord::for_each_child(const std::function<bool(const handlegraph::net_handle_t&)>& iteratee) const {
    assert(record_offset == 0);
    size_t connected_component_count = get_connected_component_count();
    for (size_t i = 0 ; i < connected_component_count ; i++) {

        size_t child_offset = (*records)->at(record_offset + ROOT_RECORD_SIZE + i);
        net_handle_record_t type = SnarlTreeRecord(child_offset, records).get_record_handle_type();
        record_t record_type = SnarlTreeRecord(child_offset, records).get_record_type();


        if (record_type == ROOT_SNARL || record_type == DISTANCED_ROOT_SNARL) {
            //This is a bunch of root components that are connected, so go through each
            SnarlRecord snarl_record(child_offset, records);
            if (! snarl_record.for_each_child(iteratee)) {
                return false;
            }
        } else {
            //Otherwise, it is a separate connected component
            net_handle_t child_handle;
            if (type == NODE_HANDLE) {
                //If this child is a node, then pretend it's a chain
                child_handle = get_net_handle_from_values(child_offset, START_END, CHAIN_HANDLE);
            } else {
                child_handle =  get_net_handle_from_values(child_offset, START_END, type);
            }
            if (!iteratee(child_handle)) {
                return false;
            }
        }
    }
    return true;
}

SnarlDistanceIndex::RootRecordWriter::RootRecordWriter (size_t pointer, size_t connected_component_count, size_t node_count, 
        size_t max_tree_depth, handlegraph::nid_t min_node_id, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records){

    record_offset = pointer;
    records = records;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlTreeRecordWriter::records = records;
    RootRecord::record_offset = pointer;
    RootRecord::records = records;
    //Allocate memory for the root vector and for all of the nodes
#ifdef debug_distance_indexing

    //The root record will always start at 0
    assert(pointer == 0);
    cerr << " Resizing array to add root: length " << (*records)->size() << " -> "
         << (*records)->size() + ROOT_RECORD_SIZE + connected_component_count + (node_count*2) << endl;
#endif
    (*records)->resize((*records)->size() + ROOT_RECORD_SIZE + connected_component_count + (node_count*2));
    set_record_type(ROOT);
    set_min_node_id(min_node_id);
    set_node_count(node_count);
    set_max_tree_depth(max_tree_depth);
    set_connected_component_count(connected_component_count);
#ifdef count_allocations
    cerr << "new_root\t" <<  (ROOT_RECORD_SIZE + connected_component_count + (node_count*2)) << "\t" << (*records)->siz     e() << endl;
#endif
}


void SnarlDistanceIndex::RootRecordWriter::set_connected_component_count(size_t connected_component_count) {
#ifdef debug_distance_indexing
    cerr << record_offset+COMPONENT_COUNT_OFFSET << " set connected component to be " << connected_component_count << endl;
    assert((*records)->at(record_offset+COMPONENT_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset+COMPONENT_COUNT_OFFSET)=connected_component_count;
}
void SnarlDistanceIndex::RootRecordWriter::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset+NODE_COUNT_OFFSET << " set node count to be " << node_count << endl;
    assert((*records)->at(record_offset+NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset+NODE_COUNT_OFFSET)=node_count;
}
void SnarlDistanceIndex::RootRecordWriter::set_max_tree_depth(size_t tree_depth) {
#ifdef debug_distance_indexing
    cerr << record_offset+MAX_TREE_DEPTH_OFFSET << " set max tree depth to be " << tree_depth << endl;
    assert((*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET) == 0);
#endif

    (*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET)=tree_depth;
}
void SnarlDistanceIndex::RootRecordWriter::set_min_node_id(handlegraph::nid_t node_id) {
#ifdef debug_distance_indexing
    cerr << record_offset+MIN_NODE_ID_OFFSET << " set min node id to be " << node_id << endl;
    assert((*records)->at(record_offset+MIN_NODE_ID_OFFSET) == 0);
#endif

    (*records)->at(record_offset+MIN_NODE_ID_OFFSET)=node_id;
}
void SnarlDistanceIndex::RootRecordWriter::add_component(size_t index, size_t offset) {
#ifdef debug_distance_indexing
    cerr << record_offset+ROOT_RECORD_SIZE+index << " set new component " << offset << endl;
    cerr << "Old value " << (*records)->at(record_offset+ROOT_RECORD_SIZE+index) << endl;
    assert((*records)->at(record_offset+ROOT_RECORD_SIZE+index) == 0);
    assert(index < get_connected_component_count());
#endif

    (*records)->at(record_offset+ROOT_RECORD_SIZE+index) = offset;

}

SnarlDistanceIndex::SnarlRecord::SnarlRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = pointer;
    records = tree_records;
#ifdef debug_distance_indexing
    record_t type = get_record_type();
    assert(type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL || type == ROOT_SNARL
        || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL);
#endif
}

SnarlDistanceIndex::SnarlRecord::SnarlRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
#ifdef debug_distance_indexing
    net_handle_record_t type = get_handle_type(net);
    assert(type == SNARL_HANDLE || type == SENTINEL_HANDLE || type == ROOT_HANDLE);
#endif
}


size_t SnarlDistanceIndex::SnarlRecord::distance_vector_size(record_t type, size_t node_count) {
    if (type == SNARL || type == ROOT_SNARL){
        //For a normal snarl, its just the record size and the pointers to children
        return 0;
    } else if (type == DISTANCED_SNARL || type == DISTANCED_ROOT_SNARL) {
        //For a normal min distance snarl just the distances between internal node sides
        size_t node_side_count = node_count * 2;
        size_t vector_size =  (((node_side_count+1)*node_side_count) / 2);
        return vector_size;
    } else if (type ==  OVERSIZED_SNARL){
        //For a large min_distance snarl, all distances get stored in the children
        return 0;
    } else {
        throw runtime_error ("error: this is not a snarl");
    }
}

size_t SnarlDistanceIndex::SnarlRecord::record_size (record_t type, size_t node_count) {
    return SNARL_RECORD_SIZE + distance_vector_size(type, node_count);
}
size_t SnarlDistanceIndex::SnarlRecord::record_size() {
    record_t type = get_record_type();
   return record_size(type, get_node_count());
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance_start_start() const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + SNARL_DISTANCE_START_START_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::SnarlRecord::get_distance_end_end() const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + SNARL_DISTANCE_END_END_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}

SnarlDistanceIndex::SnarlRecordWriter::SnarlRecordWriter (size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, record_t type){
    //Constructor for making a new record, including allocating memory.
    //Assumes that this is the latest record being made, so pointer will be the end of
    //the array and we need to allocate extra memory past it

    records = records;
    record_offset = (*records)->size();
    SnarlTreeRecordWriter::record_offset = (*records)->size();
    SnarlTreeRecordWriter::records = records;
    SnarlRecord::record_offset = (*records)->size();
    SnarlRecord::records = records;
    
    size_t extra_size = record_size(type, node_count);
#ifdef debug_distance_indexing
    if (type == OVERSIZED_SNARL) {
            cerr << "oversized" << endl;
    }
    cerr << " Resizing array to add snarl: length " << (*records)->size() << " -> "  << (*records)->size() + extra_size      << endl;
#endif
    (*records)->resize((*records)->size() + extra_size);
    set_node_count(node_count);
    set_record_type(type);

#ifdef count_allocations
    cerr << "new_snarl\t" << extra_size << "\t" << (*records)->size() << endl;
#endif
}

SnarlDistanceIndex::SnarlRecordWriter::SnarlRecordWriter(bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, size_t pointer) {
    //Make a constructor for a snarl record that has already been allocated.
    //For adding children to an existing snarl record
    record_offset = pointer;
    records = records;
    SnarlTreeRecordWriter::records = records;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlRecord::records = records;
    SnarlRecord::record_offset = pointer;
}


void SnarlDistanceIndex::SnarlRecordWriter::set_distance_start_start(size_t value) {
    (*records)->at(record_offset + SNARL_DISTANCE_START_START_OFFSET) = (value == std::numeric_limits<size_t>::max() ? 0 : value + 1);
}
void SnarlDistanceIndex::SnarlRecordWriter::set_distance_end_end(size_t value) {
    (*records)->at(record_offset + SNARL_DISTANCE_END_END_OFFSET) = (value == std::numeric_limits<size_t>::max() ? 0 : value + 1);
}


size_t SnarlDistanceIndex::SnarlRecord::get_distance_vector_offset(size_t rank1, bool right_side1, size_t rank2,
        bool right_side2, size_t node_count, record_t type) {
    //how many node sides in this snarl
    size_t node_side_count = node_count * 2;

    if (type == DISTANCED_SNARL) {
        //For distances snarls, the ranks 0 and 1 are for the start and end nodes. The distance
        //matrix in the snarl record is only for distances between internal nodes, so 0 and 1 are
        //never stored and we decrement the ranks by 2
        rank1 -= 2;
        rank2 -= 2;
    }

    //make sure we're looking at the correct node side
    rank1 = rank1 * 2;
    if (right_side1) {
        rank1 += 1;
    }
    rank2 = rank2 * 2;
    if (right_side2) {
        rank2 += 1;
    }
    

    //reverse order of ranks if necessary
    if (rank1 > rank2) {
        size_t tmp = rank1;
        rank1 = rank2;
        rank2 = tmp;
    }

    if (type == SNARL || type == ROOT_SNARL || type == OVERSIZED_SNARL) {
        throw runtime_error("error: trying to access distance in a distanceless snarl tree");
    } else if (type == DISTANCED_SNARL || type == DISTANCED_ROOT_SNARL) {
        //normal distance index
        size_t k = node_side_count-rank1;
        return (((node_side_count+1) * node_side_count)/2) - (((k+1)*k) / 2) + rank2 - rank1;
    } else {
        throw runtime_error("error: trying to distance from something that isn't a snarl");
    }
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance_vector_offset(size_t rank1, bool right_side1,
        size_t rank2, bool right_side2) const {
    return get_distance_vector_offset(rank1, right_side1, rank2, right_side2,
        get_node_count(), get_record_type());
}


bool SnarlDistanceIndex::SnarlRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {
    size_t child_count = get_node_count();
    size_t child_record_offset = get_child_record_pointer();
    for (size_t i = 0 ; i < child_count ; i++) {
        size_t child_offset =  (*records)->at(child_record_offset + i);
#ifdef debug_distances
        net_handle_record_t type = SnarlTreeRecord(child_offset, records).get_record_handle_type();
        assert(type == NODE_HANDLE || type == CHAIN_HANDLE);
#endif
        net_handle_t child_handle =  get_net_handle_from_values (child_offset, START_END, CHAIN_HANDLE);
        bool result = iteratee(child_handle);
        if (result == false) {
            return false;
        }
    }
    return true;
}

void SnarlDistanceIndex::SnarlRecordWriter::set_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2, size_t distance) {

#ifdef debug_distance_indexing 
    //This distance hasn't been set yet (or it's equal to what we're setting)
    assert((get_distance(rank1, right_side1, rank2, right_side2) == std::numeric_limits<size_t>::max() ||
           get_distance(rank1, right_side1, rank2, right_side2) == distance));
#endif
    //Don't save internal distances for oversized snarls
    if (get_record_type() == OVERSIZED_SNARL && !(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1)){
        return;
    }

    //Offset of this particular distance in the distance vector
    size_t distance_vector_offset = get_distance_vector_offset(rank1, right_side1, rank2, right_side2);
    assert(distance_vector_offset <= distance_vector_size(get_record_type(), get_node_count())); 

    //Value we actually want to save
    size_t val = distance == std::numeric_limits<size_t>::max() ? 0 : distance+1;

    (*records)->at(distance_vector_offset+record_offset+SNARL_RECORD_SIZE) = val;
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    if (get_record_type() == OVERSIZED_SNARL) {
        throw runtime_error("error: trying to distance from an oversized snarl");
    }

    //Offset of this particular distance in the distance vector
    size_t distance_vector_offset = get_distance_vector_offset(rank1, right_side1, rank2, right_side2);

    size_t val = (*records)->at(distance_vector_offset+record_offset+SNARL_RECORD_SIZE);

    return  val == 0 ? std::numeric_limits<size_t>::max() : val-1;

}
size_t SnarlDistanceIndex::SnarlRecord::get_node_count() const {
    return (*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET);
}

//Node count is the number of nodes in the snarl, not including boundary nodes
void SnarlDistanceIndex::SnarlRecordWriter::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset + SNARL_NODE_COUNT_OFFSET << " set snarl node count " << node_count << endl;
    assert(node_count > 0);
    assert((*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET) = node_count;
}

size_t SnarlDistanceIndex::SnarlRecord::get_child_record_pointer() const {
    return (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) ;
}

void SnarlDistanceIndex::SnarlRecordWriter::set_child_record_pointer(size_t pointer) {
#ifdef debug_distance_indexing
    cerr << (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) << endl;
    assert((*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) == 0);
#endif

    (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) = pointer;
}

//Add a reference to a child of this snarl. Assumes that the index is completed up
//to here
void SnarlDistanceIndex::SnarlRecordWriter::add_child(size_t pointer){
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child pointer to the end of the array " << endl;
#endif
    size_t start_i = (*records)->size();
    (*records)->resize(start_i+1);
    (*records)->at(start_i) = pointer;


#ifdef count_allocations
    cerr << "snarl_child\t1\t" << (*records)->size() <<  endl;
#endif
}

SnarlDistanceIndex::SimpleSnarlRecord::SimpleSnarlRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records, size_t node){
    record_offset = pointer;
    records = tree_records;
    node_rank = node;
#ifdef debug_distance_indexing
    assert (node_rank >=2);
    assert(get_record_type() == SIMPLE_SNARL || get_record_type() == DISTANCED_SIMPLE_SNARL);
#endif
}

SnarlDistanceIndex::SimpleSnarlRecord::SimpleSnarlRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
    node_rank = get_node_record_offset(net) == 1 ? std::numeric_limits<size_t>::max() : get_node_record_offset(net);
    
#ifdef debug_distance_indexing
    assert (node_rank >=2);
    assert(get_record_type() == SIMPLE_SNARL || get_record_type() == DISTANCED_SIMPLE_SNARL);
#endif
}



size_t SnarlDistanceIndex::SimpleSnarlRecord::record_size() {
    
    return SIMPLE_SNARL_RECORD_SIZE + get_node_count()*2;
}
size_t SnarlDistanceIndex::SimpleSnarlRecord::get_node_count() const {

    return (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET) >> 22;
}

SnarlDistanceIndex::SimpleSnarlRecordWriter::SimpleSnarlRecordWriter (size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, record_t type){

    //Constructor for making a new record, including allocating memory.
    //Assumes that this is the latest record being made, so pointer will be the end of
    //the array and we need to allocate extra memory past it
    records = records;
    record_offset = (*records)->size();
    SnarlTreeRecordWriter::record_offset = (*records)->size();
    SnarlTreeRecordWriter::records = records;
    SimpleSnarlRecord::record_offset = (*records)->size();
    SimpleSnarlRecord::records = records;
    
#ifdef debug_distance_indexing
    cerr << " Resizing array to add simple snarl: length " << (*records)->size() << " -> "  << (*records)->size() + SIMPLE_SNARL_RECORD_SIZE + 2*node_count << endl;
    cerr << "\t simple snarl has " << node_count << " nodes " << endl;
#endif
    (*records)->resize((*records)->size() + SIMPLE_SNARL_RECORD_SIZE + 2*node_count);
    set_node_count(node_count);
    set_record_type(type);
    set_node_count(node_count);
    set_start_end_connected();
#ifdef count_allocations
    cerr << "new_simple_snarl\t" << SIMPLE_SNARL_RECORD_SIZE << "\t" << (*records)->size() << endl;
    cerr << "simple_snarl_nodes\t" << (node_count*2) << "\t" << (*records)->size() << endl;
#endif
}

SnarlDistanceIndex::SimpleSnarlRecordWriter::SimpleSnarlRecordWriter(bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records, size_t pointer) {
    //Make a constructor for a snarl record that has already been allocated.
    //For adding children to an existing snarl record
    record_offset = pointer;
    records = records;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlTreeRecordWriter::records = records;
    SimpleSnarlRecord::record_offset = pointer;
    SimpleSnarlRecord::records = records;

}


void SnarlDistanceIndex::SimpleSnarlRecordWriter::set_node_count(size_t node_count) {
    size_t old_value = (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET);
    size_t new_value = old_value | (node_count << 22);
    (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET) = new_value;
}
nid_t SnarlDistanceIndex::SimpleSnarlRecord::get_node_id(size_t rank) const {
    rank = rank == std::numeric_limits<size_t>::max() ? node_rank : rank;
    assert(rank >=2);
    assert(rank != std::numeric_limits<size_t>::max());
    return (*records)->at(record_offset + SIMPLE_SNARL_RECORD_SIZE + ((rank-2)*2));
}
size_t SnarlDistanceIndex::SimpleSnarlRecord::get_node_length(size_t rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    rank = rank == std::numeric_limits<size_t>::max() ? node_rank : rank;
    assert(rank >=2);
    assert(rank != std::numeric_limits<size_t>::max());
    return (*records)->at(record_offset + SIMPLE_SNARL_RECORD_SIZE + ((rank-2)*2) + 1) >> 1;
}
bool SnarlDistanceIndex::SimpleSnarlRecord::get_node_is_reversed(size_t rank) const {
    rank = rank == std::numeric_limits<size_t>::max() ? node_rank : rank;
    assert(rank>=2);
    assert(rank != std::numeric_limits<size_t>::max());
    return (*records)->at(record_offset + SIMPLE_SNARL_RECORD_SIZE + ((rank-2)*2) + 1) & 1;
}
size_t SnarlDistanceIndex::SimpleSnarlRecord::get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    if (rank1 > rank2) {
        //Order the nodes
        size_t tmp = rank1; bool tmp_rev = right_side1;
        rank1 = rank2; right_side1 = right_side2;
        rank2 = tmp; right_side2 = tmp_rev;
    }
    if (rank1 == 0 && rank2 == 1) {
        return get_min_length();
    } else if (rank1 == rank2) {
        //There are no loops
        return std::numeric_limits<size_t>::max();
    } else if (rank1 == 0) {
        //If the first node is the start node, then the distance is 0 if the node is not reversed 
        //and we are looking at the left side (or is reversed and right side), inf otherwise 
        if( get_node_is_reversed(rank2) == right_side2) {
            return 0;
        } else {
            return std::numeric_limits<size_t>::max();
        }
    } else if (rank1 == 1) {
        //If the first node is the end node, then the opposite of above
        if( get_node_is_reversed(rank2) == right_side2) {
            return std::numeric_limits<size_t>::max();
        } else {
            return 0;
        }
    } else {
        //Otherwise there is no path between them
        return std::numeric_limits<size_t>::max();
    }
}
net_handle_t SnarlDistanceIndex::SimpleSnarlRecord::get_child_from_rank(const size_t& rank) const {
    return get_net_handle_from_values(record_offset, START_END, CHAIN_HANDLE, rank);
}
bool SnarlDistanceIndex::SimpleSnarlRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {
    size_t node_count = get_node_count();
    for (size_t i = 0 ; i < node_count ; i++) {
        net_handle_t child_handle = get_net_handle_from_values(record_offset, START_END, CHAIN_HANDLE, i+2);
        bool result = iteratee(child_handle);
        if (result == false) {
            return false;
        }
    }
    return true;
}
void SnarlDistanceIndex::SimpleSnarlRecordWriter::add_child(size_t rank, nid_t node_id, size_t node_length, bool is_reversed) {
    //Let the root know where to find the node
    (*records)->at(get_node_pointer_offset(node_id,
                                          (*records)->at(MIN_NODE_ID_OFFSET),
                                          (*records)->at(COMPONENT_COUNT_OFFSET))) 
                = record_offset;
    (*records)->at(get_node_pointer_offset(node_id,
                                          (*records)->at(MIN_NODE_ID_OFFSET),
                                          (*records)->at(COMPONENT_COUNT_OFFSET))+1) 
                = rank;
    //The rank gets decremented because the first 2 ranks are always the start and end nodes
    rank = rank-2;
    (*records)->at(record_offset+SIMPLE_SNARL_RECORD_SIZE+(rank*2)) = node_id;
    (*records)->at(record_offset+SIMPLE_SNARL_RECORD_SIZE+(rank*2)+1) = (node_length << 1) | is_reversed;
#ifdef debug_distance_indexing
    cerr << (get_node_pointer_offset(node_id,
                                          (*records)->at(MIN_NODE_ID_OFFSET),
                                          (*records)->at(COMPONENT_COUNT_OFFSET)))
         << " Node pointer for node " << node_id << ": " << record_offset << endl;
    cerr << (get_node_pointer_offset(node_id,
                                          (*records)->at(MIN_NODE_ID_OFFSET),
                                          (*records)->at(COMPONENT_COUNT_OFFSET)) + 1)
         << " Node record offset for node " << node_id << ": " << rank << endl;
#endif

}

SnarlDistanceIndex::NodeRecord::NodeRecord (size_t pointer, size_t node_offset, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = pointer;
    records = tree_records;

#ifdef debug_distance_indexing
    assert(get_record_type() == NODE || get_record_type() == DISTANCED_NODE);
#endif  
}

SnarlDistanceIndex::NodeRecord::NodeRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    records = tree_records;
    record_offset = get_record_offset(net);
    
#ifdef debug_distance_indexing
    assert(get_handle_type(net) == NODE_HANDLE || get_handle_type(net) == CHAIN_HANDLE);
    assert(get_record_type() == NODE || get_record_type() == DISTANCED_NODE);
    assert(get_connectivity(net) == START_END || get_connectivity(net) == END_START
          || get_connectivity(net) == START_START || get_connectivity(net) == END_END);
#endif      
}


nid_t SnarlDistanceIndex::NodeRecord::get_node_id() const {
    return (*records)->at(record_offset + NODE_ID_OFFSET);
}
size_t SnarlDistanceIndex::NodeRecord::get_node_length() const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    return get_min_length();
    //if (get_record_type()== TRIVIAL_SNARL) {
    //    if (node_record_offset == 1) {
    //        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE+1);
    //    } else {
    //        return ((*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + ((node_record_offset-1)*2) + 1) - 
    //                (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + ((node_record_offset-1)*2) - 1));
    //    }
    //} else {
    //    return (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    //}
}
size_t SnarlDistanceIndex::NodeRecord::get_distance_left_start() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + NODE_DISTANCE_LEFT_START_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::NodeRecord::get_distance_right_start() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + NODE_DISTANCE_RIGHT_START_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::NodeRecord::get_distance_left_end() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + NODE_DISTANCE_LEFT_END_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::NodeRecord::get_distance_right_end() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + NODE_DISTANCE_RIGHT_END_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}

SnarlDistanceIndex::TrivialSnarlRecord::TrivialSnarlRecord (size_t offset, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    records = tree_records;
    record_offset = offset;
    
#ifdef debug_distance_indexing
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
    //assert(get_connectivity(net) == START_END || get_connectivity(net) == END_START
    //      || get_connectivity(net) == START_START || get_connectivity(net) == END_END);
#endif
}


size_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_count() const {
    return (*records)->at(record_offset+TRIVIAL_SNARL_NODE_COUNT_OFFSET);
}

tuple<size_t, size_t, size_t, size_t> SnarlDistanceIndex::TrivialSnarlRecord::get_chain_values(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t prefix_sum = (*records)->at(record_offset+TRIVIAL_SNARL_PREFIX_SUM_OFFSET);  
    size_t forward_loop = (*records)->at(record_offset+TRIVIAL_SNARL_FORWARD_LOOP_OFFSET);  
    size_t reverse_loop = (*records)->at(record_offset+TRIVIAL_SNARL_REVERSE_LOOP_OFFSET);  
    size_t component = (node_rank == 0 && record_offset == ChainRecord(get_parent_record_offset(), records).get_first_node_offset()) ? 0: (*records)->at(record_offset+TRIVIAL_SNARL_COMPONENT_OFFSET); 

    //Get the right prefix sum value
    prefix_sum = prefix_sum == 0 ? std::numeric_limits<size_t>::max() : prefix_sum - 1;
    if (node_rank != 0) {
        prefix_sum =  sum(prefix_sum,  
               (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE+ (node_rank*2)-1));
    } 

    /*Get the forward loop value
     */

    forward_loop = forward_loop == 0 ? std::numeric_limits<size_t>::max() : forward_loop - 1;
        
    //How many nodes in the snarl
    size_t last_node = get_node_count()-1; 
    //The right prefix sum of the last node, which is the length of the snarl
    size_t snarl_length = (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (last_node*2) + 1); 

    //The distance from the right side of this node to the end of the trivial snarl
    //the length of the snarl - the right prefix sum of the node
    size_t right_offset = snarl_length - (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) + 1);

    forward_loop = sum(forward_loop, right_offset*2);

    /* Get the reverse loop value
     */
    reverse_loop = reverse_loop == 0 ? std::numeric_limits<size_t>::max() : reverse_loop - 1;
        
    //The offset of the left side of this node in the trivial snarl
    size_t left_offset = node_rank == 0 ? 0 : 
                          (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) - 1);

    reverse_loop = sum(reverse_loop, left_offset*2);



    return make_tuple(prefix_sum, forward_loop, reverse_loop, component);

}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_max_prefix_sum(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t prefix_sum = (*records)->at(record_offset+TRIVIAL_SNARL_MAX_PREFIX_SUM_OFFSET);

    //Get the right prefix sum value
    prefix_sum = prefix_sum == 0 ? std::numeric_limits<size_t>::max() : prefix_sum - 1;
    if (node_rank != 0) {
        prefix_sum =  sum(prefix_sum,  
               (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE+ (node_rank*2)-1));
    } 

    return prefix_sum;

}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_prefix_sum(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t prefix_sum = (*records)->at(record_offset+TRIVIAL_SNARL_PREFIX_SUM_OFFSET);  
    prefix_sum = prefix_sum == 0 ? std::numeric_limits<size_t>::max() : prefix_sum - 1;
    if (node_rank != 0) {
        return sum(prefix_sum,  
               (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE+ (node_rank*2)-1));
    } else {
        return prefix_sum;
    }
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_forward_loop(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distances
    assert(get_record_type()== TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t forward_loop = (*records)->at(record_offset+TRIVIAL_SNARL_FORWARD_LOOP_OFFSET);  
    forward_loop = forward_loop == 0 ? std::numeric_limits<size_t>::max() : forward_loop - 1;
        
    //How many nodes in the snarl
    size_t last_node = get_node_count()-1; 
    //The right prefix sum of the last node, which is the length of the snarl
    size_t snarl_length = (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (last_node*2) + 1); 

    //The distance from the right side of this node to the end of the trivial snarl
    //the length of the snarl - the right prefix sum of the node
    size_t right_offset = snarl_length - (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) + 1);

    return sum(forward_loop, right_offset*2);
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_reverse_loop(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t reverse_loop = (*records)->at(record_offset+TRIVIAL_SNARL_REVERSE_LOOP_OFFSET);  
    reverse_loop = reverse_loop == 0 ? std::numeric_limits<size_t>::max() : reverse_loop - 1;
        
    //The offset of the left side of this node in the trivial snarl
    size_t left_offset = node_rank == 0 ? 0 : 
                          (*records)->at(record_offset+DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) - 1);

    return sum(reverse_loop, left_offset*2);
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_chain_component(size_t node_rank, bool get_end) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    if (!get_end && node_rank == 0 && record_offset == ChainRecord(get_parent_record_offset(), records).get_first_node_offset()){
        //If we want the component of the first node in the chain
        return 0;
    }
    return (*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET);
}

size_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_length(size_t node_rank) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    if (node_rank == 0) {
        return (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)+1);
    } else {
        return (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)+1) - 
               (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)-1);
    }
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_rank_in_parent(size_t node_rank) const {
    return record_offset+node_rank;
}
nid_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_id(size_t node_rank) const {
    return get_record_type() == TRIVIAL_SNARL
          ? (*records)->at(record_offset + DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE + node_rank) >> 1
          : (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)) >> 1;
}
bool SnarlDistanceIndex::TrivialSnarlRecord::get_is_reversed_in_parent(size_t node_rank) const {
    return get_record_type() == TRIVIAL_SNARL
          ? (*records)->at(record_offset + DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE + node_rank) & 1
          : (*records)->at(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)) & 1;
}

SnarlDistanceIndex::NodeRecordWriter::NodeRecordWriter (size_t pointer, size_t node_offset, record_t type,
             bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* snarl_records, nid_t node_id){
    records = snarl_records;
    record_offset =  pointer;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlTreeRecordWriter::records = records;
    NodeRecord::record_offset = pointer;
    NodeRecord::records = records;
    
    (*records)->resize((*records)->size() + NODE_RECORD_SIZE); 
#ifdef count_allocations
    cerr << "new_node\t" <<  NODE_RECORD_SIZE << "\t" << (*records)->size() << endl;
#endif
    set_record_type(type);
    set_start_end_connected();
    
    //Set the pointer for the node to this record
#ifdef debug_distance_indexinging
    assert (type == NODE || type == DISTANCED_NODE);
    
    cerr << get_node_pointer_offset(node_id,
                                           (*records)->at(MIN_NODE_ID_OFFSET),
                                           (*records)->at(COMPONENT_COUNT_OFFSET))
         << " Set pointer to node " << node_id << " record: " << pointer << endl;
#endif
    //Tell the root where to find this node, and a 0 for the offset in a trivial snarl
    (*records)->at(get_node_pointer_offset(node_id,
                                           (*records)->at(MIN_NODE_ID_OFFSET),
                                           (*records)->at(COMPONENT_COUNT_OFFSET))) = pointer;
    (*records)->at(get_node_pointer_offset(node_id,
                                           (*records)->at(MIN_NODE_ID_OFFSET),
                                           (*records)->at(COMPONENT_COUNT_OFFSET))+1) = 0;

}


void SnarlDistanceIndex::NodeRecordWriter::set_node_id(nid_t value) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_ID_OFFSET << " set node id " << value << endl;
    assert((*records)->at(record_offset + NODE_ID_OFFSET) == 0);
#endif

    (*records)->at(record_offset + NODE_ID_OFFSET) = value;
}
void SnarlDistanceIndex::NodeRecordWriter::set_node_length(size_t length) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_LENGTH_OFFSET << " set node length " << length << endl;
    assert((*records)->at(record_offset + NODE_LENGTH_OFFSET) == 0);
#endif
    (*records)->at(record_offset + NODE_LENGTH_OFFSET) = length;
}
void SnarlDistanceIndex::NodeRecordWriter::set_rank_in_parent(size_t value) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_RANK_OFFSET << " set node rank " << value << endl;
    assert((*records)->at(record_offset + NODE_RANK_OFFSET) == 0);
#endif
    (*records)->at(record_offset + NODE_RANK_OFFSET) = value;
}
void SnarlDistanceIndex::NodeRecordWriter::set_distance_left_start(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + NODE_DISTANCE_LEFT_START_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::NodeRecordWriter::set_distance_right_start(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + NODE_DISTANCE_RIGHT_START_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::NodeRecordWriter::set_distance_left_end(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + NODE_DISTANCE_LEFT_END_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::NodeRecordWriter::set_distance_right_end(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + NODE_DISTANCE_RIGHT_END_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}

SnarlDistanceIndex::TrivialSnarlRecordWriter::TrivialSnarlRecordWriter (size_t pointer, record_t type,
             bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* snarl_records, bool new_record){

    records = snarl_records;
    record_offset =  pointer;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlTreeRecordWriter::records = records;
    TrivialSnarlRecord::record_offset = pointer;
    TrivialSnarlRecord::records = records;
    
    assert (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL);
    
#ifdef debug_distance_indexing
    cerr << " Resizing array to add trivial snarl: length " << (*records)->size() << " -> "  << 
               (*records)->size() + (type == TRIVIAL_SNARL ? DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE : DISTANCED_TRIVIAL_SNARL_RECORD_SIZE) << endl;
#endif
 
    if (new_record) {
        if (type == TRIVIAL_SNARL) {
            (*records)->resize(record_offset + DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE);
        } else {
            (*records)->resize(record_offset + DISTANCED_TRIVIAL_SNARL_RECORD_SIZE);
        }
        set_record_type(type);
        set_start_end_connected();
#ifdef count_allocations
        cerr << "new_trivial_snarl\t" << (type == TRIVIAL_SNARL ? DISTANCELESS_TRIVIAL_SNARL_RECORD_SIZE : DISTANCED_TRIVIAL_SNARL_RECORD_SIZE) << "\t" << (*records)->size() << endl;
#endif
    }
}


void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_node_count(size_t value) const {
    (*records)->at(record_offset+TRIVIAL_SNARL_NODE_COUNT_OFFSET)=value;
}
void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_prefix_sum(size_t value) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET << " set node prefix sum " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET) == 0);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    (*records)->at(record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET) = value;
}
void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_max_prefix_sum(size_t value) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_MAX_PREFIX_SUM_OFFSET << " set node prefix sum " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_MAX_PREFIX_SUM_OFFSET) == 0);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    (*records)->at(record_offset + TRIVIAL_SNARL_MAX_PREFIX_SUM_OFFSET) = value;
}
void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_forward_loop(size_t value) const { 
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_FORWARD_LOOP_OFFSET << " set node forward loop " << value << endl;
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    (*records)->at(record_offset + TRIVIAL_SNARL_FORWARD_LOOP_OFFSET) = value;
}
void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_reverse_loop(size_t value) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET << " set node reverse loop " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET) == 0);
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    //Only do this for the first node
    (*records)->at(record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET) = value;
    
}
void SnarlDistanceIndex::TrivialSnarlRecordWriter::set_chain_component(size_t value) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET << " set node component " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET) == 0);
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    (*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET) = value;
}

SnarlDistanceIndex::ChainRecord::ChainRecord (size_t pointer, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){

    record_offset = pointer;
    records = tree_records;
    net_handle_record_t record_type= get_record_handle_type();
    if (record_type == NODE_HANDLE) {
        net_handle_record_t parent_type = SnarlTreeRecord(
            NodeRecord(pointer, 0, records).get_parent_record_offset(), records
        ).get_record_handle_type();
#ifdef debug_distance_indexing
        assert(parent_type == ROOT_HANDLE || parent_type == SNARL_HANDLE);
#endif  
    } else {
#ifdef debug_distance_indexing
    assert(get_record_handle_type() == CHAIN_HANDLE);
#endif
    }   
}       

SnarlDistanceIndex::ChainRecord::ChainRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records){
    record_offset = get_record_offset(net);
    records = tree_records;
    
    net_handle_record_t record_type = get_record_handle_type();
#ifdef debug_distance_indexing
    if (record_type == NODE_HANDLE) {
        net_handle_record_t parent_type = SnarlTreeRecord(
            NodeRecord(record_offset, 0, records).get_parent_record_offset(), records
        ).get_record_handle_type();
        assert(parent_type == ROOT_HANDLE || parent_type == SNARL_HANDLE);
    } else {
        assert(get_record_handle_type() == CHAIN_HANDLE);
    }
#endif
}   

SnarlDistanceIndex::ChainRecord::ChainRecord (net_handle_t net, const bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* tree_records, size_t tag){
    record_offset = get_record_offset(net);
    records = tree_records;
    
#ifdef debug_distance_indexing
    net_handle_record_t record_type= SnarlDistanceIndex::get_record_handle_type(SnarlDistanceIndex::get_record_type(tag     ));
    if (record_type == NODE_HANDLE) {
        net_handle_record_t parent_type = SnarlTreeRecord(
            NodeRecord(record_offset, 0, records).get_parent_record_offset(), records
        ).get_record_handle_type();
        assert(parent_type == ROOT_HANDLE || parent_type == SNARL_HANDLE);
    } else {
        assert(get_record_handle_type() == CHAIN_HANDLE);
    }       
#endif
}   



size_t SnarlDistanceIndex::ChainRecord::get_node_count() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        return 1;
    } else {
        return (*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET);
    }
}

tuple<size_t, bool, size_t> SnarlDistanceIndex::ChainRecord::get_last_child_offset() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get children of a node");
    } else {
        size_t val = (*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET);
        size_t record_offset = val>>2;
        bool is_snarl = (val>>1) & 1;
        size_t node_record_offset = is_snarl ? 0 : TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return make_tuple(record_offset, is_snarl, node_record_offset);
    }
}
bool SnarlDistanceIndex::ChainRecord::get_is_looping_chain_connected_backwards() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get children of a node");
    } else {
        size_t val = (*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET);
        return val & 1;
    }
}


size_t SnarlDistanceIndex::ChainRecord::get_distance(size_t rank1, bool left_side1, size_t node_length1, 
    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1, size_t end_component1,
    size_t rank2, bool left_side2, size_t node_length2,
    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2, size_t end_component2) const { 
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }

    //If 1 comes after 2, swap them
    if (rank1 > rank2) {
        size_t tmp_rank=rank1; bool tmp_side=left_side1; size_t tmp_len=node_length1;
        size_t tmp_pre=prefix_sum1; size_t tmp_fd = forward_loop1; size_t tmp_rev = reverse_loop1; size_t tmp_comp = component1;
        size_t tmp_end_comp = end_component1;
        rank1 = rank2; left_side1 = left_side2; node_length1 = node_length2;
        prefix_sum1 = prefix_sum2; forward_loop1 = forward_loop2; reverse_loop1 = reverse_loop2; component1 = component2; end_component1 = end_component2;
        rank2 = tmp_rank; left_side2 = tmp_side; node_length2 = tmp_len;
        prefix_sum2 = tmp_pre; forward_loop2 = tmp_fd; reverse_loop2 = tmp_rev; component2 = tmp_comp; end_component2 = tmp_end_comp;
    } 
    bool is_looping_chain = get_start_id() == get_end_id();
#ifdef debug_distances
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
#endif

    if (get_record_type() == MULTICOMPONENT_CHAIN) {
        if (component1 != component2) {
            if (is_looping_chain) {
                //If this is a looping chain, then the first/last node could be in two
                //components
                return get_distance_taking_chain_loop(rank1, left_side1, node_length1, 
                            prefix_sum1, forward_loop1, reverse_loop1, end_component1,
                            rank2, left_side2, node_length2,
                            prefix_sum2, forward_loop2, reverse_loop2, end_component2);
            } else {
                return std::numeric_limits<size_t>::max();
            }
        }
    }


    size_t distance;

    if (!left_side1 && left_side2) {
        //Right of 1 and left of 2, so a simple forward traversal of the chain
        if (rank1 == rank2) {
            //If these are the same node, then the path would need to go around the node
            distance = sum(sum(forward_loop1,reverse_loop2),node_length1);
        } else {
            distance = minus(prefix_sum2 - prefix_sum1, node_length1);
        }
    } else if (!left_side1 && !left_side2) {
        //Right side of 1 and right side of 2
        if (rank1 == rank2) {
            distance = forward_loop2;

        } else {
            distance = minus( sum(sum(prefix_sum2 - prefix_sum1, node_length2), forward_loop2),
                         node_length1);
        }
    } else if (left_side1 && left_side2) {
        //Left side of 1 and left side of 2
        if (rank1 == rank2) {
            distance = reverse_loop1;

        } else {
            distance = sum(prefix_sum2 - prefix_sum1, reverse_loop1);
        }
    } else {
        //Left side of 1 and right side of 2
        distance = sum(sum(sum(prefix_sum2 - prefix_sum1, reverse_loop1),
                        forward_loop2), node_length2);

    }
    if (is_looping_chain) {
        distance = std::min(distance, get_distance_taking_chain_loop(rank1, left_side1, node_length1, 
                            prefix_sum1, forward_loop1, reverse_loop1, end_component1,
                            rank2, left_side2, node_length2,
                            prefix_sum2, forward_loop2, reverse_loop2, end_component2));
    }
    return distance;
}


size_t SnarlDistanceIndex::ChainRecord::get_distance_taking_chain_loop(size_t rank1, bool left_side1, size_t node_length1, 
    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1,
    size_t rank2, bool left_side2, size_t node_length2,
    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2) const {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    //This is only called by get_distance, so the nodes should be ordered
#ifdef debug_distances
    assert (rank1 <= rank2);
    assert(get_start_id() == get_end_id());
    assert(get_record_handle_type() != NODE_HANDLE);
#endif

    /*Note: Because we assume that the nodes are ordered and that we want to take the loop in the chain,
     * we leave the start node going left (either by going left or taking a loop to the right), and
     * enter the end node from the right (from the right or from the left and then looping)
     */
     if (get_record_type() == MULTICOMPONENT_CHAIN) {
        size_t last_component = TrivialSnarlRecord(get_first_node_offset() , records).get_chain_component(0, true);
        bool first_in_first_component = component1 == 0 || component1 == last_component;
        bool second_in_first_component = component2 == 0 || component2 == last_component;
        bool can_loop = get_is_looping_chain_connected_backwards();

        if (!can_loop || !first_in_first_component || ! second_in_first_component) {
            //If this is a multicomponent chain then it can only reach around backwards if both nodes
            //are in the first (last) component
            return std::numeric_limits<size_t>::max();
        } 

        //The first component of a multicomponent chain is the same as the last component
        //If we want to find the distance ending at the right side of a node in the first component
        //or starting at the left side of a node in the last component, then the chain has to be connected.
        //This is because this function assumes that we're taking the loop around the back of the chain,
        //and taking one of these paths would require the chain to be connected. 
        //eg. if we want to end at the right side of a node in the first component, then the start node must have
        //also been in the first component, so we'd go left, take the chain loop, then walk through the chain 
        //backwards from the end boundary node of the chain to the end node. If the end node is in the first component,
        //then the chain must be connected for the path to be valid
        bool end_at_right_of_first = component2 == 0 && !left_side2;
        bool start_at_left_of_last = component1 == last_component && left_side1; 
        if ((end_at_right_of_first || start_at_left_of_last)) {
            return std::numeric_limits<size_t>::max();
        }
    }


    size_t distance;

    if (!left_side1 && left_side2) {
        //Right of 1 and left of 2, so a simple forward traversal of the chain
        //loop forward from the first node, from the start of the chain to the first
        //node, from the end of the node to the second node, and the reverse loop of the second
        distance = sum(sum(sum(sum(forward_loop1, node_length1), prefix_sum1),
                        minus(get_min_length(), prefix_sum2)),
                        reverse_loop2);
    } else if (!left_side1 && !left_side2) {
        //Right side of 1 and right side of 2

        //Check distance for taking loop in chain: loop forward from the first node, from the start of the
        //chain to the first node, from the end of the node to the second node
        distance = sum(sum(sum(forward_loop1, node_length1), prefix_sum1),
                        minus(minus(get_min_length(), prefix_sum2), node_length2));
    } else if (left_side1 && left_side2) {
        //Left side of 1 and left side of 2

        //from the first node left to the start, around the
        //chain loop, then the reverse loop of the second node
        //This assumes that the length of the chain only includes the start/end node's length once,
        //which it does but might change
        distance = sum(sum(prefix_sum1,
                        minus(get_min_length(), prefix_sum2)),reverse_loop2);
    } else {
        //Left side of 1 and right side of 2

        //Check the distance going backwards around the chain
        distance = sum(prefix_sum1,
                        minus(minus(get_min_length(), prefix_sum2),node_length2));
    }
    return distance;
}

size_t SnarlDistanceIndex::ChainRecord::get_distance_left_start() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + CHAIN_DISTANCE_LEFT_START_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::ChainRecord::get_distance_right_start() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + CHAIN_DISTANCE_RIGHT_START_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::ChainRecord::get_distance_left_end() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + CHAIN_DISTANCE_LEFT_END_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}
size_t SnarlDistanceIndex::ChainRecord::get_distance_right_end() {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    size_t stored_value = (*records)->at(record_offset + CHAIN_DISTANCE_RIGHT_END_OFFSET);
    return stored_value == 0 ? std::numeric_limits<size_t>::max() : stored_value - 1;
}

size_t SnarlDistanceIndex::ChainRecord::get_first_node_offset() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain traversal from a node");
    }
    return record_offset + CHAIN_RECORD_SIZE + 1;
}

net_handle_t SnarlDistanceIndex::ChainRecord::get_next_child(const net_handle_t& net_handle, bool go_left) const {
    //get the next child in the chain. net_handle must point to a snarl or node in the chain

    if (get_record_handle_type() == NODE_HANDLE) {
        //If this "chain" record is a node pretending to be a chain, then there is no next child
#ifdef debug_snarl_traversal
        assert(get_record_offset(net_handle) == record_offset);
#endif
        return net_handle;
    }


    //If we are inside a trivial snarl and we aren't at the ends, then we just walk through the trivial snarl
    if (get_handle_type(net_handle) == NODE_HANDLE) {
        //If this is a node in a trivial snarl
#ifdef debug_snarl_traversal
        cerr << "GEt next in chain after " << TrivialSnarlRecord(get_record_offset(net_handle), records).get_node_id(get_node_record_offset(net_handle)) << endl;
#endif
        if (go_left && get_node_record_offset(net_handle) != 0) {
            //If we are going left and this is not the first node in the trivial snarl,
            //then keep everything the same but decrement the node record offset by one
            //to move one node left in the trivial snarl
            connectivity_t connectivity = get_connectivity(net_handle);
            if (get_node_record_offset(net_handle) == 1 && get_record_offset(net_handle) == get_first_node_offset() &&
                get_start_id() == get_end_id()) {
                //If this is the second thing in a looping chain and we're walking left to the first thing in the chain
                //then remember that the next handle is walking out of the chain
                connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), 
                                                                get_start_endpoint(connectivity));
            }
            if (TrivialSnarlRecord(get_record_offset(net_handle), records).get_is_reversed_in_parent(get_node_record_offset(net_handle)) !=
                TrivialSnarlRecord(get_record_offset(net_handle), records).get_is_reversed_in_parent(get_node_record_offset(net_handle)-1)) {
                //If this node is facing the opposite direction of the next nod in the chain
                connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), 
                                                                get_start_endpoint(connectivity));

            }
            return get_net_handle_from_values(get_record_offset(net_handle), connectivity, 
                                  get_handle_type(net_handle), get_node_record_offset(net_handle)-1);

        } else if (!go_left && get_node_record_offset(net_handle) != 
                      TrivialSnarlRecord(get_record_offset(net_handle), records).get_node_count()-1){

            //If we are going right and this is not the last node in the trivial snarl
            //then keep everything the same but increment the node record offset by one
            //to move one node right in the trivial snarl
            connectivity_t connectivity = get_connectivity(net_handle);
            if (TrivialSnarlRecord(get_record_offset(net_handle), records).get_is_reversed_in_parent(get_node_record_offset(net_handle)) !=
                TrivialSnarlRecord(get_record_offset(net_handle), records).get_is_reversed_in_parent(get_node_record_offset(net_handle)+1)) {
                //If this node is facing the opposite direction of the next nod in the chain
                connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), 
                                                                get_start_endpoint(connectivity));

            }
            return get_net_handle_from_values(get_record_offset(net_handle), connectivity, 
                                      get_handle_type(net_handle), get_node_record_offset(net_handle)+1);
        }
        //Otherwise, if we are going left from the first node or right from the last node, we pretend this is a 
        //snarl and use get_next_child of the offset, which will check for the length of this record properly
    }


    //Otherwise we need to skip through snarl and trivial snarl records
    //These will be arranged:
    //
    //trivial_snarl_record | trivial snarl length | snarl length | snarl record | snarl length | trivial snarl length | trivial snarl record ... 
    if (go_left && get_record_offset(net_handle) == get_first_node_offset() ) {
        //IF we are going left out the start of the chain
#ifdef debug_distance_indexing
        //If we weren't going left from the first, then we already would have found this
        assert(get_node_record_offset(net_handle) == 0);
#endif
        if (get_start_id() != get_end_id()) {
            //If we are going left from the first node of a non-looping chain
            return net_handle;
        } else {
            //IF we are going left from the first node of a looping chain
            tuple<size_t, bool, size_t> last_child = get_last_child_offset();
            bool is_reversed_in_parent = std::get<1>(last_child) ? false : 
                TrivialSnarlRecord(get_first_node_offset(),records).get_is_reversed_in_parent(TrivialSnarlRecord(get_first_node_offset(),records).get_node_count()-1);
            
            return get_net_handle_from_values(std::get<0>(last_child), 
                                  (is_reversed_in_parent ? START_END : END_START),
                                  std::get<1>(last_child) ? SNARL_HANDLE : NODE_HANDLE,
                                  std::get<2>(last_child));
        }
    } else if (!go_left && get_record_offset(net_handle) == std::get<0>(get_last_child_offset())) {
        //IF we are going right out the end of the chain
        if (get_start_id() != get_end_id()) {
            //If we are going right from the last node of a non-looping chain
            return net_handle;
        } else {
            //IF we are going right from the last node of a looping chain, then
            //return the first node going in
            bool is_reversed_in_parent = TrivialSnarlRecord(get_first_node_offset(),records).get_is_reversed_in_parent(0);
            return get_net_handle_from_values(get_first_node_offset(), 
                                  (is_reversed_in_parent ? START_START : END_END),
                                  NODE_HANDLE,
                                  0);
        }
    }

    //At this point, we know that we need to leave a snarl or a trivial snarl and that we're in the middle of the chain
    //So the we need to skip to the start of the previous thing or to the end of this thing
    size_t next_pointer =  get_record_offset(net_handle) + 
                            (go_left ? -(*records)->at(get_record_offset(net_handle)-2)-2 : (*records)->at(get_record_offset(net_handle)-1)+2);

    if (SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == SNARL || 
        SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == DISTANCED_SNARL|| 
        SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == OVERSIZED_SNARL) {
        //If the next thing is a snarl, then just return the snarl going in the direction we just moved in
        return get_net_handle_from_values(next_pointer, (go_left ? END_START : START_END), SNARL_HANDLE); 
    } else if (SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == SIMPLE_SNARL || 
        SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == DISTANCED_SIMPLE_SNARL) {
        //If the next thing is a snarl, then just return the snarl going in the direction we just moved in
        return get_net_handle_from_values(next_pointer, (go_left ? END_START : START_END), SNARL_HANDLE, 1); 
    } else{

        //Otherwise, this is a node (trivial snarl of nodes) handle
        TrivialSnarlRecord next_record(next_pointer, records);

        //The node in the trivial snarl is the last one if we're going left, the first one if we're going right
        size_t node_offset = go_left ? next_record.get_node_count()-1 : 0;

        //Is the next node (the first thing in the snarl if we're going right, last thing if we're going left) reversed
        bool next_is_reversed_in_parent = next_record.get_is_reversed_in_parent( node_offset);

        //Get the direction of the next handle
        //Handle will point in whichever direction we moved in
        connectivity_t connectivity = go_left == next_is_reversed_in_parent ? START_END : END_START;

        return get_net_handle_from_values(next_pointer, connectivity, NODE_HANDLE, node_offset);
    }
}

bool SnarlDistanceIndex::ChainRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {

    if (get_record_handle_type() == NODE_HANDLE) {
        //If this is a node pretending to be a chain, just do it for the node
        return iteratee(get_net_handle_from_values(record_offset, START_END, NODE_HANDLE));
    }


    size_t first_node_offset = get_first_node_offset();
    bool rev_in_parent = TrivialSnarlRecord(first_node_offset, records).get_is_reversed_in_parent(0);
    net_handle_t first_child = get_net_handle_from_values(first_node_offset, rev_in_parent ? END_START : START_END, NODE_HANDLE, 0);
    net_handle_t current_child = get_net_handle_from_values(first_node_offset, rev_in_parent ? END_START : START_END, NODE_HANDLE, 0);
    bool is_first = true;

    while (true) {
        if (!is_first && current_child == first_child){
            //Don't look at the first node a second time
            return true;
        }

        bool result = iteratee(current_child);
        if (result == false) {
            return false;
        }
        net_handle_t next_child = get_next_child(current_child, false);
        if (current_child == next_child || get_start_endpoint(get_connectivity(next_child)) == get_end_endpoint(get_connectivity(next_child))) {
            return true;
        } else {
            current_child = next_child;
        }
        is_first = false;
    }
    return true;
}


SnarlDistanceIndex::ChainRecordWriter::ChainRecordWriter (size_t pointer, record_t type, size_t node_count, bdsg::yomo::UniqueMappedPointer<bdsg::MappedIntVector>* records){
#ifdef debug_distance_indexing
    assert(type == CHAIN ||
           type == DISTANCED_CHAIN ||
           type == MULTICOMPONENT_CHAIN);
#endif
    record_offset = pointer;
    records = records;
    SnarlTreeRecordWriter::record_offset = pointer;
    SnarlTreeRecordWriter::records = records;
    ChainRecord::record_offset = pointer;
    ChainRecord::records = records;
#ifdef debug_distance_indexing
    cerr << " Resizing array to add chain: length " << (*records)->size() << " -> "  << (*records)->size() + CHAIN_RECORD_SIZE << endl;
#endif
    (*records)->resize((*records)->size() + CHAIN_RECORD_SIZE);
    set_node_count(node_count);
    set_record_type(type);
#ifdef count_allocations
    cerr << "new_chain\t" << CHAIN_RECORD_SIZE << "\t" << (*records)->size() << endl;
#endif
}


void SnarlDistanceIndex::ChainRecordWriter::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_NODE_COUNT_OFFSET << " set chain node count " << node_count << endl;
    assert((*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET) = node_count;
}
size_t SnarlDistanceIndex::ChainRecord::get_depth() const {
    return (*records)->at(record_offset+CHAIN_DEPTH_OFFSET) ;
}
void SnarlDistanceIndex::ChainRecordWriter::set_depth(size_t depth) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_DEPTH_OFFSET << " set chain depth " << depth << endl;
    assert((*records)->at(record_offset + CHAIN_DEPTH_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_DEPTH_OFFSET) = depth;
}

//The offset of the last child, if it is a snarl, and if it can loop
void SnarlDistanceIndex::ChainRecordWriter::set_last_child_offset(size_t offset, bool is_snarl, bool loopable) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_LAST_CHILD_OFFSET << " set chain last child offset " << offset << endl;
    assert((*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET) = ((offset << 2) | (is_snarl<<1)) | loopable;
}

void SnarlDistanceIndex::ChainRecordWriter::set_distance_left_start(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + CHAIN_DISTANCE_LEFT_START_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::ChainRecordWriter::set_distance_right_start(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + CHAIN_DISTANCE_RIGHT_START_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::ChainRecordWriter::set_distance_left_end(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + CHAIN_DISTANCE_LEFT_END_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}
void SnarlDistanceIndex::ChainRecordWriter::set_distance_right_end(size_t distance) {
    if (!has_distances(get_record_type())) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }
    (*records)->at(record_offset + CHAIN_DISTANCE_RIGHT_END_OFFSET) = (distance == std::numeric_limits<size_t>::max() ? 0 : distance + 1);
}

//Add a snarl to the end of the chain and return a SnarlRecordWriter pointing to it
SnarlDistanceIndex::SnarlRecordWriter SnarlDistanceIndex::ChainRecordWriter::add_snarl(size_t snarl_size, record_t type, size_t previous_child_offset) {

    size_t snarl_record_size = SnarlRecord::record_size(type, snarl_size);
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child snarl length to the end of the array " << endl;
    cerr << "Previous child was at " << previous_child_offset << endl;
    assert(SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== DISTANCED_TRIVIAL_SNARL || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== TRIVIAL_SNARL);
#endif

    
    //The last child we saw was a trivial snarl and we need to record its size
    (*records)->at(previous_child_offset-1) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    size_t current_size = (*records)->size();
    (*records)->resize(current_size+1);
    (*records)->at(current_size) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    
    size_t start_i = (*records)->size();
    (*records)->resize(start_i+1);
    (*records)->at(start_i) = snarl_record_size;
    (*records)->reserve(start_i + snarl_record_size);
    SnarlRecordWriter snarl_record(snarl_size, records, type);
    snarl_record.set_parent_record_offset(get_offset());
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child snarl length to the end of the array " << endl;
#endif
    start_i = (*records)->size();
    (*records)->resize(start_i+1);
    (*records)->at(start_i) = snarl_record_size;
#ifdef count_allocations
    cerr << "chain\t2\t" <<  (*records)->size() << endl;
#endif
    return snarl_record;
}
//Add a simple snarl to the end of the chain and return a SnarlRecordWriter pointing to it
SnarlDistanceIndex::SimpleSnarlRecordWriter SnarlDistanceIndex::ChainRecordWriter::add_simple_snarl(size_t snarl_size, record_t type, size_t previous_child_offset) {

    size_t snarl_record_size = SIMPLE_SNARL_RECORD_SIZE + 2*snarl_size;
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding simple snarl to the end of the array " << endl;
    assert(SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== DISTANCED_TRIVIAL_SNARL || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== TRIVIAL_SNARL);
#endif

    
    //The last child we saw was a trivial snarl and we need to record its size
    (*records)->at(previous_child_offset-1) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    size_t current_size = (*records)->size();
    (*records)->resize(current_size+1);
    (*records)->at(current_size) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    
    //Add another place in snarl_tree_records for the size of this snarl
    size_t start_i = (*records)->size();
    (*records)->resize(start_i+1);
    (*records)->at(start_i) = snarl_record_size;
    (*records)->reserve(start_i + snarl_record_size);
    SimpleSnarlRecordWriter snarl_record(snarl_size, records, type);
    snarl_record.set_parent_record_offset(get_offset());
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child snarl length to the end of the array " << endl;
#endif
    start_i = (*records)->size();
    (*records)->resize(start_i+1);
    (*records)->at(start_i) = snarl_record_size;
#ifdef count_allocations
    cerr << "chain\t2\t" <<  (*records)->size() << endl;
#endif
    return snarl_record;
}


size_t SnarlDistanceIndex::ChainRecordWriter::add_node(nid_t node_id, size_t node_length, bool is_reversed_in_parent,
    size_t prefix_sum, size_t forward_loop, size_t reverse_loop, size_t component, size_t max_prefix_sum, size_t previous_child_offset, bool new_record,
    bool include_distances) {
#ifdef debug_distance_indexing
    cerr << "Adding new node to chain, with previous child at offset " << previous_child_offset << endl;
#endif
    if ((SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_TRIVIAL_SNARL ||
         SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == TRIVIAL_SNARL)
            && (TrivialSnarlRecord(previous_child_offset, records).get_node_count() == MAX_TRIVIAL_SNARL_NODE_COUNT 
                || new_record || reverse_loop == 0)) {
        //If the last thing was a trivial snarl and it is full, then finish it off
        (*records)->at(previous_child_offset-1) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
        size_t current_size = (*records)->size();
        (*records)->resize(current_size+1);
        (*records)->at(current_size) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    }

    if (previous_child_offset == 0
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == OVERSIZED_SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == SIMPLE_SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_SIMPLE_SNARL 
            || TrivialSnarlRecord(previous_child_offset, records).get_node_count() == MAX_TRIVIAL_SNARL_NODE_COUNT
            || reverse_loop == 0 || new_record) {
        //If the last thing was a snarl or nothing (previous_child_offset == 0, meaning that this is the 
        //first thing in the chain), then create a new trivial snarl
        size_t start_i = (*records)->size();
        (*records)->resize(start_i+1);
        (*records)->at(start_i) = 0; //Add a place for the size of this trivial snarl, to be filled in later

        //Set the node to point to the correct node record
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))) 
                    = (*records)->size();
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))+1) 
                    = 0;
    

        //Create a new trivial snarl record
        TrivialSnarlRecordWriter trivial_snarl_record((*records)->size(), include_distances ? DISTANCED_TRIVIAL_SNARL : TRIVIAL_SNARL, records, true);
        trivial_snarl_record.set_parent_record_offset(record_offset);
        trivial_snarl_record.set_node_count(1);
        if (include_distances) {
            trivial_snarl_record.set_prefix_sum(prefix_sum);
            trivial_snarl_record.set_max_prefix_sum(max_prefix_sum);;
            trivial_snarl_record.set_forward_loop(forward_loop);
            trivial_snarl_record.set_reverse_loop(reverse_loop);
            trivial_snarl_record.set_chain_component(component);
        }

        start_i = (*records)->size(); 
        if (include_distances) {
            (*records)->resize(start_i+2);
            (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
            (*records)->at(start_i+1) = node_length;
        } else {
            (*records)->resize(start_i+1);
            (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
        }
#ifdef debug_distance_indexing
        cerr << start_i << " Node in trivial snarl " <<  ((node_id<<1) | is_reversed_in_parent) << endl;
        if (get_record_type() == DISTANCED_CHAIN) {
            cerr << start_i +1 << " prefix sum in trivial snarl " <<  ((*records)->at(start_i-1) + node_length) << endl;
        }
#endif
#ifdef count_allocations
        if (get_record_type() == DISTANCED_CHAIN) {
            cerr << "trivial_snarl\t2\t" <<  (*records)->size() << endl;
        } else {
            cerr << "trivial_snarl\t1\t" <<  (*records)->size() << endl;
        }
#endif

        //Return the offset offset of the new trivial snarl record
        return trivial_snarl_record.record_offset;
    } else {
        //The last thing on the chain is a trivial snarl that we are adding to the end of
        //
        //Add the node and its right prefix sum
        size_t start_i = (*records)->size(); 
        if (include_distances) {
            (*records)->resize(start_i+2);
            (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
            (*records)->at(start_i+1) = (*records)->at(start_i-1) + node_length;
        } else {
            (*records)->resize(start_i+1);
            (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
        }
#ifdef debug_distance_indexing
        cerr << start_i << " Node in trivial snarl " <<  ((node_id<<1) | is_reversed_in_parent) << endl;
        cerr << start_i +1 << " prefix sum in trivial snarl " <<  ((*records)->at(start_i-1) + node_length) << endl;
#endif
#ifdef count_allocations
    cerr << "trivial_snarl\t2\t" <<  (*records)->size() << endl;
#endif

        TrivialSnarlRecordWriter trivial_snarl_record(previous_child_offset, include_distances ? DISTANCED_TRIVIAL_SNARL : TRIVIAL_SNARL, records, false);

        //Increment the node count
        size_t old_node_count = trivial_snarl_record.get_node_count();
        trivial_snarl_record.set_node_count(old_node_count+1);
        //The forward loop value changes
        if (include_distances) {
            trivial_snarl_record.set_forward_loop(forward_loop);
        }

        //Set the node to point to the correct node record
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))) 
                    = previous_child_offset;
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))+1) 
                    = old_node_count ;
        //REturn the offset of the old trivial snarl record that we added t:
        return trivial_snarl_record.record_offset;
    }
}
string SnarlDistanceIndex::net_handle_as_string(const net_handle_t& net) const {
    net_handle_record_t type = get_handle_type(net);
    SnarlTreeRecord record (net, &snarl_tree_records);
    net_handle_record_t record_type = record.get_record_handle_type();
    string result;
    if (type == ROOT_HANDLE) {
        if (record.get_record_type() == ROOT_SNARL || record.get_record_type() == DISTANCED_ROOT_SNARL) {
            return "root snarl";
        } else {
            return "root"; 
        }
    } else if (type == NODE_HANDLE) {
        if (ends_at(net) == starts_at(net)) {
            return "node" + std::to_string( node_id(net)) + (ends_at(net) == START ? "rev" : "fd") + " that is the end node of a looping chain";
        }
        return  "node " + std::to_string( node_id(net)) + (ends_at(net) == START ? "rev" : "fd");
    } else if (type == SNARL_HANDLE) {
        if (record.get_record_type() == ROOT) {
            return "root snarl";
        }
        if (get_node_record_offset(net) == 1) {
            result += "simple snarl ";
        } else if (get_node_record_offset(net) > 1) {
            result += " node in simple snarl ";
            result += node_id(net);
        } else {
            result += "snarl ";         
        }
    } else if (type == CHAIN_HANDLE && record_type == NODE_HANDLE) {
        return  "node " + std::to_string( NodeRecord(net, &snarl_tree_records).get_node_id())
               + (ends_at(net) == START ? "rev" : "fd") + " pretending to be a chain";
    }  else if (type == CHAIN_HANDLE && record_type == SNARL_HANDLE) {
        return  "node " + std::to_string( SimpleSnarlRecord(net, &snarl_tree_records).get_node_id())
               + (ends_at(net) == START ? "rev" : "fd") + " pretending to be a chain in a simple snarl";
    }else if (type == CHAIN_HANDLE) {
        result += "chain ";
    } else if (type == SENTINEL_HANDLE) {
        result += "sentinel of snarl ";
    } else {
        throw runtime_error("error: Unknown net_handle_t type");
    }
    result += ( std::to_string(record.get_start_id())
            + (record.get_start_orientation() ? "rev" : "fd")
            + "->"
            + std::to_string(record.get_end_id())
            + (record.get_end_orientation() ? "rev" : "fd"));
    result += "traversing ";
    result += (starts_at(net) == START ? "start" : (starts_at(net) == END ? "end" : "tip"));
    result += "->";
    result += (ends_at(net) == START ? "start" : (ends_at(net) == END ? "end" : "tip"));
    return result;
}

bool SnarlDistanceIndex::traverse_decomposition(const std::function<bool(const net_handle_t&)>& snarl_iteratee,
                                                const std::function<bool(const net_handle_t&)>& chain_iteratee,
                                                const std::function<bool(const net_handle_t&)>& node_iteratee) const {
    net_handle_t root = get_root();
    return for_each_child(root, [&] (const net_handle_t& child) {
        return traverse_decomposition_helper(child, snarl_iteratee, chain_iteratee, node_iteratee);
    });

}
bool SnarlDistanceIndex::traverse_decomposition_helper(const net_handle_t& net,
                                                const std::function<bool(const net_handle_t&)>& snarl_iteratee,
                                                const std::function<bool(const net_handle_t&)>& chain_iteratee,
                                                const std::function<bool(const net_handle_t&)>& node_iteratee) const {
    if (is_node(net)) {
        //If this is a node, then run iteratee and we're done
        return node_iteratee(net);
    } else if (is_snarl(net)) {
        //IF this is a snarl, run iteratee and if it returns false, return false to stop iterating
        bool result = snarl_iteratee(net);
        if (!result){
            return false;
        }
    } else if (is_chain(net)) {
        //IF this is a chain, run iteratee and if it returns false, return false to stop iterating
        bool result = chain_iteratee(net);
        if (!result){
            return false;
        }
    } else {
        throw runtime_error("error: traversing the wrong kind of net_handle_t in traverse_decomposition");
    }
    //If this was a snarl or chain, recurse
    return for_each_child(net, [&] (const net_handle_t& child) {
        return traverse_decomposition_helper(child, snarl_iteratee, chain_iteratee, node_iteratee);
    });

}



//Print entire index to cout as csv:
//self, parent, # children, depth
void SnarlDistanceIndex::print_self() const {
    cout << "#self,parent,#children,depth" << endl; 
    print_descendants_of(get_root());

}
void SnarlDistanceIndex::print_descendants_of(const net_handle_t net) const {
    SnarlTreeRecord record (net, &snarl_tree_records);
    //What the record thinks it is
    net_handle_record_t record_type = record.get_record_handle_type();
    if (record_type == NODE_HANDLE) {
        net_handle_t parent = get_parent(net);
        cout << net_handle_as_string(net) << "," << net_handle_as_string(parent) << ",1,"<< get_depth(net) << endl; 
        return;
    } else {
        size_t child_count;
        string parent;
        if (record_type == ROOT_HANDLE) {
            parent = "none";
            child_count = RootRecord(get_root(), &snarl_tree_records).get_connected_component_count();
        } else {
            parent = net_handle_as_string(get_parent(net));
            if (record_type == CHAIN_HANDLE) {
                child_count =  ChainRecord(net, &snarl_tree_records).get_node_count();
            } else if (record.get_record_type() == SNARL ||
                        record.get_record_type() == DISTANCED_SNARL||
                        record.get_record_type() == OVERSIZED_SNARL  
                        ){
 
                child_count = SnarlRecord(net, &snarl_tree_records).get_node_count();
            } else if (record.get_record_type() == TRIVIAL_SNARL ||
                        record.get_record_type() == DISTANCED_TRIVIAL_SNARL) {
                child_count = TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_count();
            }else if (record.get_record_type() == SIMPLE_SNARL ||
                        record.get_record_type() == DISTANCED_SIMPLE_SNARL) {
                child_count = SimpleSnarlRecord(net, &snarl_tree_records).get_node_count();
            } else {
                throw runtime_error("error: printing the wrong kind of record");
            }
                                
        }
        cout << net_handle_as_string(net) << "," << parent << "," << child_count << ","<< get_depth(net) << endl; 
        if (!is_node(net)) {
        for_each_child(net, [&](const net_handle_t& child) {
            print_descendants_of(child);
        });
        }
    }
}

void SnarlDistanceIndex::print_snarl_stats() const {
    cout << "#start_id\tend_id\tsnarl_size\tsnarl_depth" << endl;
    traverse_decomposition(
        [&](const net_handle_t& snarl_child) {
            //Iteratee for a snarl child
            SnarlTreeRecord record(snarl_child, &snarl_tree_records);

            //Get the number of children depending on the type of record
            size_t child_count;
            if (record.get_record_type() == SNARL ||
                        record.get_record_type() == DISTANCED_SNARL||
                        record.get_record_type() == OVERSIZED_SNARL  
                        ){
 
                child_count = SnarlRecord(snarl_child, &snarl_tree_records).get_node_count();
            } else if (record.get_record_type() == SIMPLE_SNARL ||
                        record.get_record_type() == DISTANCED_SIMPLE_SNARL) {
                child_count = SimpleSnarlRecord(snarl_child, &snarl_tree_records).get_node_count();
            } else {
                throw runtime_error("error: getting the snarl child count of the wrong type of record");
            }
            //Print the stats
            cout << record.get_start_id() << "\t" 
                 << record.get_end_id() << "\t" 
                 << child_count << "\t" 
                 << get_depth(snarl_child) << endl; 
            return true;
        }, 
        [&](const net_handle_t& chain_child) {
            //Iteratee of a chain child -do nothing
            return true;
        }, 
        [&](const net_handle_t& node_child) {
            //Iteratee of a node child - do nothing
            return true;
        });
}

void SnarlDistanceIndex::write_snarls_to_json() const {
    traverse_decomposition(
        [&](const net_handle_t& snarl_child) {
            //Iteratee for a snarl child

            //Make a new json object for this snarl
            json_t* out_json = json_object();
            json_object_set_new(out_json, "type", json_string("snarl")); 
            SnarlTreeRecord record(snarl_child, &snarl_tree_records);

            //Get the start node pointing in
            net_handle_t start_bound = get_node_from_sentinel(get_bound(snarl_child, false, true));
            json_t* start_json = json_object();
            json_object_set_new(start_json, "node_id", json_integer(node_id(start_bound)));
            json_object_set_new(start_json, "is_rev", json_boolean(ends_at(start_bound) == START));
            //Set the start node
            json_object_set_new(out_json, "start_node", start_json);

            //Get the end node pointing in
            net_handle_t end_bound = get_node_from_sentinel(get_bound(snarl_child, true, true));
            json_t* end_json = json_object();
            json_object_set_new(end_json, "node_id", json_integer(node_id(end_bound)));
            json_object_set_new(end_json, "is_rev", json_boolean(ends_at(end_bound) == START));
            //Set the end node
            json_object_set_new(out_json, "end_node", end_json);

            //Set the parent
            net_handle_t parent = get_parent(snarl_child);
                json_t* parent_json = json_object();
            if (is_root(parent)) {
                json_object_set_new(parent_json, "type", json_string("root"));
            } else {
                json_object_set_new(parent_json, "type", json_string("chain"));
                net_handle_t parent_start = get_bound(parent, false, true);
                json_object_set_new(parent_json, "start_id", json_integer(node_id(parent_start)));
                net_handle_t parent_end = get_bound(parent, true, false);
                json_object_set_new(parent_json, "end_id", json_integer(node_id(parent_end)));
            }
            json_object_set_new(out_json, "parent", parent_json);

            //Get the number of children depending on the type of record
            if (record.get_record_type() == SNARL ||
                        record.get_record_type() == DISTANCED_SNARL||
                        record.get_record_type() == OVERSIZED_SNARL  
                        ){
                size_t child_count = SnarlRecord(snarl_child, &snarl_tree_records).get_node_count();
                json_object_set_new(out_json, "child_count", json_integer(child_count)); 
            } else if (record.get_record_type() == SIMPLE_SNARL ||
                        record.get_record_type() == DISTANCED_SIMPLE_SNARL) {
                size_t child_count = SimpleSnarlRecord(snarl_child, &snarl_tree_records).get_node_count();
                json_object_set_new(out_json, "child_count", json_integer(child_count)); 
            } else {
                throw runtime_error("error: getting the snarl child count of the wrong type of record");
            }

            //Set the min and max length, if applicable
            if (record.get_record_type() == DISTANCED_SNARL ||
                        record.get_record_type() == DISTANCED_SIMPLE_SNARL){
                json_object_set_new(out_json, "minimum_length", json_integer(minimum_length(snarl_child))); 
                json_object_set_new(out_json, "maximum_length", json_integer(maximum_length(snarl_child))); 
            }

            //Set the depth
            json_object_set_new(out_json, "depth", json_integer(get_depth(snarl_child)));

            cout << json_dumps(out_json, JSON_ENCODE_ANY) << endl;

            return true;
        }, 
        [&](const net_handle_t& chain_child) {
            //Iteratee of a chain child
            if (is_trivial_chain(chain_child)) {
                //If this is a trivial chain, do nothing, handle it as a node
                return true;
            }
            json_t* out_json = json_object();
            json_object_set_new(out_json, "type", json_string("chain"));

            //Make a new json object for this chain
            ChainRecord record(chain_child, &snarl_tree_records);

            //Get the start node pointing in
            net_handle_t start_bound = get_bound(chain_child, false, true);
            json_t* start_json = json_object();
            json_object_set_new(start_json, "node_id", json_integer(node_id(start_bound)));
            json_object_set_new(start_json, "is_rev", json_boolean(ends_at(start_bound) == START));
            //Set the start node
            json_object_set_new(out_json, "start_node", start_json);

            //Get the end node pointing in
            net_handle_t end_bound = get_bound(chain_child, true, true);
            json_t* end_json = json_object();
            json_object_set_new(end_json, "node_id", json_integer(node_id(end_bound)));
            json_object_set_new(end_json, "is_rev", json_boolean(ends_at(end_bound) == START));
            //Set the end node
            json_object_set_new(out_json, "end_node", end_json);

            //Set the parent
            net_handle_t parent = get_parent(chain_child);
                json_t* parent_json = json_object();
            if (is_root(parent)) {
                json_object_set_new(parent_json, "type", json_string("root"));
            } else {
                json_object_set_new(parent_json, "type", json_string("snarl"));
                net_handle_t parent_start = get_bound(parent, false, true);
                json_object_set_new(parent_json, "start_id", json_integer(node_id(parent_start)));
                net_handle_t parent_end = get_bound(parent, true, false);
                json_object_set_new(parent_json, "end_id", json_integer(node_id(parent_end)));
            }
            json_object_set_new(out_json, "parent", parent_json);

            //Get the number of children depending on the type of record
            json_object_set_new(out_json, "child_count", json_integer(record.get_node_count())); 

            //Set the min and max length, if applicable
            if (record.get_record_type() == DISTANCED_CHAIN) {
                json_object_set_new(out_json, "minimum_length", json_integer(minimum_length(chain_child))); 
                json_object_set_new(out_json, "maximum_length", json_integer(maximum_length(chain_child))); 
            }

            //Set the depth
            json_object_set_new(out_json, "depth", json_integer(get_depth(chain_child)));

            cout << json_dumps(out_json, JSON_ENCODE_ANY) << endl;
            return true;
        }, 
        [&](const net_handle_t& node_child) {
            //Iteratee of a node child
            json_t* out_json = json_object();

            net_handle_t parent = get_parent(node_child);
            if (is_trivial_chain(parent)) {
                json_object_set_new(out_json, "type", json_string("trivial_chain"));
                json_object_set_new(out_json, "node_id", json_integer(node_id(node_child)));
                net_handle_t grandparent_snarl = get_parent(parent);

                json_t* parent_json = json_object();
                if (is_root(parent)) {
                    json_object_set_new(parent_json, "type", json_string("root"));
                } else {
                    json_object_set_new(parent_json, "type", json_string("snarl"));
                    net_handle_t parent_start = get_bound(grandparent_snarl, false, true);
                    json_object_set_new(parent_json, "start_id", json_integer(node_id(parent_start)));
                    net_handle_t parent_end = get_bound(grandparent_snarl, true, false);
                    json_object_set_new(parent_json, "end_id", json_integer(node_id(parent_end)));
                }
                json_object_set_new(out_json, "parent", parent_json);
            } else {
                //Otherwise this is just a node
                json_object_set_new(out_json, "type", json_string("node"));
                json_object_set_new(out_json, "node_id", json_integer(node_id(node_child)));

                json_t* parent_json = json_object();
                if (is_root(parent)) {
                    json_object_set_new(parent_json, "type", json_string("root"));
                } else {
                    json_object_set_new(parent_json, "type", json_string("chain"));
                    net_handle_t parent_start = get_bound(parent, false, true);
                    json_object_set_new(parent_json, "start_id", json_integer(node_id(parent_start)));
                    net_handle_t parent_end = get_bound(parent, true, false);
                    json_object_set_new(parent_json, "end_id", json_integer(node_id(parent_end)));
                }
                json_object_set_new(out_json, "parent", parent_json);
            }
            json_object_set_new(out_json, "length", json_integer(minimum_length(node_child)));
            cout << json_dumps(out_json, JSON_ENCODE_ANY) << endl;
            return true;
        });
}

void SnarlDistanceIndex::validate_index() const {
    //Go down tree and validate
    net_handle_t root = get_root();
    validate_descendants_of(root);
    RootRecord root_record(root, &snarl_tree_records); 

    //Go up tree and validate
    size_t node_count = 0;
    size_t max_node_id = root_record.get_min_node_id() + root_record.get_node_count();
    for (nid_t id = root_record.get_min_node_id() ; id < max_node_id ; id++){
        if (has_node(id)) {
            validate_ancestors_of(get_node_net_handle(id));
            node_count++;
        }
    }

}
//Recursively check descendants of net
void SnarlDistanceIndex::validate_descendants_of(net_handle_t net) const {
    cerr << "Looking at descendants of " << net_handle_as_string(net) << endl;

    SnarlTreeRecord record (net, &snarl_tree_records);
    //What the record thinks it is
    net_handle_record_t record_type = record.get_record_handle_type();
    if (record_type == NODE_HANDLE || (is_node(net) && record_type == SNARL_HANDLE)) {
        cerr << "\tIt's a node so we're done" << endl;
        return;
    } else {
        for_each_child(net, [&](const net_handle_t& child) {
            cerr << "for parent " << net_handle_as_string(net) << " check it's child " << net_handle_as_string(child) << endl;
            assert(is_root(child) || canonical(get_parent(child)) == canonical(net));
            validate_descendants_of(child);
        });
    }
}
//Recursively check ancestors of net
void SnarlDistanceIndex::validate_ancestors_of(net_handle_t net) const {
    cerr << "Looking at ancestors of " << net_handle_as_string(net) << endl;
    SnarlTreeRecord record (net, &snarl_tree_records);
    //What the record thinks it is
    net_handle_record_t record_type = record.get_record_handle_type();
    if (record_type == ROOT_HANDLE) {
        return;
    }
    net_handle_t parent_handle = get_parent(net);
    validate_ancestors_of(parent_handle);
}
std::tuple<size_t, size_t, size_t> SnarlDistanceIndex::get_usage() {
    return snarl_tree_records.get_usage();
}


void SnarlDistanceIndex::get_snarl_tree_records(const vector<const TemporaryDistanceIndex*>& temporary_indexes, const HandleGraph* graph) {

#ifdef debug_distance_indexing
    cerr << "Convert a temporary distance index into a permanent one" << endl;
#endif

    //Convert temporary distance indexes into the final index stored as a single vector
    size_t total_component_count = 0;
    handlegraph::nid_t min_node_id = 0;
    handlegraph::nid_t max_node_id = 0;

    size_t maximum_distance = 0;
    size_t maximum_index_size = 0;
    size_t maximum_tree_depth = 0;
    //The maximum distance value that gets stored in the index, and the maximum possible
    //length of the index

    /*Go through each of the indexes to count how many nodes, components, etc */
    for (const TemporaryDistanceIndex* temp_index : temporary_indexes) {
        total_component_count += temp_index->root_structure_count;
        min_node_id = min_node_id == 0 ? temp_index->min_node_id
                                       : std::min(min_node_id, temp_index->min_node_id);
        max_node_id = std::max(max_node_id, temp_index->max_node_id);
        maximum_distance = std::max(temp_index->max_distance, maximum_distance);
        maximum_index_size += temp_index->get_max_record_length();
        maximum_tree_depth = std::max(maximum_tree_depth, temp_index->max_tree_depth);
    }

#ifdef debug_distance_indexing
    cerr << "Converting " << temporary_indexes.size() << " temporary indexes with "
         << total_component_count << " connected components from node "
         << min_node_id << " to " << max_node_id << endl;
    cerr << " Adding root record" << endl;
#endif


    //Set the width of the values to the minimum needed to fit everything
    size_t max_dist_bit_width = bit_width(std::max(maximum_distance, (size_t) max_node_id));
    size_t max_address_bit_width = bit_width(maximum_index_size);
    size_t new_width = std::max(std::max(max_dist_bit_width, max_address_bit_width)+2, (size_t)26); //26 is the size for the simple snarl node count + node lengths
    if (new_width > 64) {
        cerr << "The bit width for the distance index is greater than 64 bits, which may cause problems" << endl;
    }
    snarl_tree_records->width(new_width);

#ifdef debug_distance_indexing
    cerr << "Max index size " << maximum_index_size << endl;
#endif
    snarl_tree_records->reserve(maximum_index_size);

    /*Allocate memory for the root and the nodes */
    RootRecordWriter root_record(0, total_component_count, max_node_id-min_node_id+1, maximum_tree_depth, min_node_id, &snarl_tree_records);
#ifdef debug_distance_indexing
    cerr << "  Root record had length " << snarl_tree_records->size() << endl;
#endif

    /*Now go through each of the chain/snarl indexes and copy them into snarl_tree_records
     * Walk down the snarl tree and fill in children
     */
    // maps <index into temporary_indexes, <record type, index into chain/snarl/node records>> to new offset
    unordered_map<std::pair<size_t, std::pair<temp_record_t, size_t>>, size_t> record_to_offset;
    //Set the root index
    for (size_t temp_index_i = 0 ; temp_index_i < temporary_indexes.size() ; temp_index_i++) {
        //Any root will point to the same root
        record_to_offset.emplace(make_pair(temp_index_i,make_pair(TEMP_ROOT, 0)), 0);
    }
    //Go through each separate temporary index, corresponding to separate connected components
    for (size_t temp_index_i = 0 ; temp_index_i < temporary_indexes.size() ; temp_index_i++) {
        const TemporaryDistanceIndex* temp_index = temporary_indexes[temp_index_i];
        //Get a stack of temporary snarl tree records to be added to the index
        //Initially, it contains only the root components
        //This reverses the order of the connected components but I don't think that matters
        vector<pair<temp_record_t, size_t>> temp_record_stack = temp_index->components;

        while (!temp_record_stack.empty()) {
            pair<temp_record_t, size_t> current_record_index = temp_record_stack.back();
            temp_record_stack.pop_back();

#ifdef debug_distance_indexing
            cerr << "Translating " << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif

            if (current_record_index.first == TEMP_CHAIN) {
                /*Add a new chain to the index. Each of the chain's child snarls and nodes will also
                 * be added here
                 */
                const TemporaryDistanceIndex::TemporaryChainRecord& temp_chain_record =
                        temp_index->temp_chain_records[current_record_index.second];
                if (!temp_chain_record.is_trivial) {
                    //If this chain contains at least two nodes
#ifdef debug_distance_indexing
                    cerr << "  Adding this chain at offset " << snarl_tree_records->size() << endl;
                    cerr << "            with indices " << current_record_index.first << " " << current_record_index.second << endl;
#endif
                    record_to_offset.emplace(make_pair(temp_index_i,current_record_index), snarl_tree_records->size());


                    //If this chain's parent is a root snarl, then the temporary parent is a snarl but we consider 
                    //the parent to be the root and the chain to have depth 1
                    bool is_child_of_root_snarl = false;
                    if (temp_chain_record.parent.first == TEMP_SNARL) {
                        const TemporaryDistanceIndex::TemporarySnarlRecord& temp_parent_record =
                             temp_index->temp_snarl_records[temp_chain_record.parent.second];
                        if (temp_parent_record.is_root_snarl) {
                            is_child_of_root_snarl = true;
                        }
                    }

                    //We don't keep track of distances for this chain if the snarl_size_limit is 0, or if we only want top-level chain distances
                    // and this is a nested chain
                    bool ignore_distances = (snarl_size_limit == 0) || 
                                            (only_top_level_chain_distances && !(temp_chain_record.parent.first == TEMP_ROOT || is_child_of_root_snarl));

                    ChainRecordWriter chain_record_constructor;

                    if (temp_chain_record.chain_components.back() == 0 || ignore_distances) {
                        record_t record_type = ignore_distances ? CHAIN : DISTANCED_CHAIN;
                        chain_record_constructor = ChainRecordWriter(snarl_tree_records->size(), record_type,
                                                               temp_chain_record.prefix_sum.size(), &snarl_tree_records);
                        chain_record_constructor.set_start_end_connected();
                    } else {
                        chain_record_constructor = ChainRecordWriter(snarl_tree_records->size(), MULTICOMPONENT_CHAIN,
                                                               temp_chain_record.prefix_sum.size(), &snarl_tree_records);
                    }
                    chain_record_constructor.set_parent_record_offset(
                            record_to_offset[make_pair(temp_index_i, temp_chain_record.parent)]);
                    if (!ignore_distances) {
                        chain_record_constructor.set_distance_left_start(temp_chain_record.distance_left_start);
                        chain_record_constructor.set_distance_right_start(temp_chain_record.distance_right_start);
                        chain_record_constructor.set_distance_left_end(temp_chain_record.distance_left_end);
                        chain_record_constructor.set_distance_right_end(temp_chain_record.distance_right_end);
                    }

                    //Set the depth of this chain
                    if (temp_chain_record.parent.first == TEMP_ROOT || is_child_of_root_snarl) {
                        //If the parent is the root, then its depth is 0
                        chain_record_constructor.set_depth(1);
                    } else {
                        //Otherwise, go to the grandparent chain and add 1
                        size_t parent_record_offset = chain_record_constructor.get_parent_record_offset();
                        size_t grandparent_record_offset = SnarlRecord(parent_record_offset, &snarl_tree_records).get_parent_record_offset();  
                        chain_record_constructor.set_depth(
                            ChainRecord(grandparent_record_offset, &snarl_tree_records).get_depth() + 1);
                    }


                    if (!ignore_distances) {
                        chain_record_constructor.set_min_length(temp_chain_record.min_length);
                        chain_record_constructor.set_max_length(temp_chain_record.max_length);
                    }
                    chain_record_constructor.set_rank_in_parent(temp_chain_record.rank_in_parent);
                    chain_record_constructor.set_start_node(temp_chain_record.start_node_id, temp_chain_record.start_node_rev);
                    chain_record_constructor.set_end_node(temp_chain_record.end_node_id, temp_chain_record.end_node_rev);


                    size_t chain_node_i = 0; //How far along the chain are we?
                    pair<size_t, bool> last_child_offset = make_pair(0, false);

                    //A trivial snarl might not actually be a trivial snarl if it has loops in it. Keep track of whether the last 
                    //child node actually had a 0 value forward loop, making the last child a non-trivial snarl
                    bool last_child_was_nontrivial_snarl = false;

                    for (size_t child_record_index_i = 0 ; child_record_index_i < temp_chain_record.children.size() ; child_record_index_i++) {
                        const pair<temp_record_t, size_t>& child_record_index = temp_chain_record.children[child_record_index_i];
                        //Go through each node and snarl in the chain and add them to the index
#ifdef debug_distance_indexing
                        cerr << "  Adding child of the chain: " << temp_index->structure_start_end_as_string(child_record_index) << endl;
#endif

                        if (child_record_index.first == TEMP_NODE) {
                            //Add a node to the chain
                            if (!(chain_node_i != 0 && child_record_index == temp_chain_record.children.front())) {
                                //If this is not a looping chain, then we haven't seen this node yet

                                //Get the temporary node record
                                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record = 
                                    temp_index->temp_node_records[child_record_index.second-min_node_id];


                                //Make a new node record
                                size_t new_offset = chain_record_constructor.add_node(
                                        temp_node_record.node_id, temp_node_record.node_length, temp_node_record.reversed_in_parent,
                                        temp_chain_record.prefix_sum[chain_node_i], temp_chain_record.forward_loops[chain_node_i],
                                        temp_chain_record.backward_loops[chain_node_i], temp_chain_record.chain_components[chain_node_i],
                                        temp_chain_record.max_prefix_sum[chain_node_i],
                                        last_child_offset.first, last_child_was_nontrivial_snarl,
                                        !ignore_distances);

                                //Remember this node as the last thing in the chain
                                last_child_offset = make_pair(new_offset, false);
                                if (temp_chain_record.forward_loops[chain_node_i] == 0) {
                                    last_child_was_nontrivial_snarl = true;
                                } else {
                                    last_child_was_nontrivial_snarl = false;
                                }

                            } 
#ifdef debug_distance_indexing
                            else {

                                //If this is the last node in the chain, and it is the same as the first node -
                                // it is a looping chain and we set this and don't re-record the node
                            cerr << "    This is a looping chain"  << endl;
                            }
#endif

                            chain_node_i++;


                        } else {
                            //Add a snarl to the chain
                            assert(child_record_index.first == TEMP_SNARL);
                            //Get the temporary snarl record
                            const TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record =
                                 temp_index->temp_snarl_records[child_record_index.second];
                            if (!temp_snarl_record.is_trivial && !temp_snarl_record.is_simple) {
                                //If this is an actual snarl that we need to make


                                //Add the snarl to the chain, and get back the record to fill it in

                                bool ignore_distances = (snarl_size_limit == 0) || only_top_level_chain_distances;

                                record_t record_type = ignore_distances ? SNARL :
                                    (temp_snarl_record.node_count < snarl_size_limit ? DISTANCED_SNARL : OVERSIZED_SNARL);
                                SnarlRecordWriter snarl_record_constructor =
                                    chain_record_constructor.add_snarl(temp_snarl_record.node_count, record_type, last_child_offset.first);

                                //Record how to find the new snarl record
                                record_to_offset.emplace(make_pair(temp_index_i, child_record_index), snarl_record_constructor.record_offset);

                                //Fill in snarl info
                                if (!ignore_distances) {
                                    snarl_record_constructor.set_min_length(temp_snarl_record.min_length);
                                    snarl_record_constructor.set_max_length(temp_snarl_record.max_length);
                                }
                                snarl_record_constructor.set_distance_start_start(temp_snarl_record.distance_start_start);
                                snarl_record_constructor.set_distance_end_end(temp_snarl_record.distance_end_end);

                                //Add distances and record connectivity
                                for (const auto& it : temp_snarl_record.distances) {
                                    pair<size_t, size_t> node_rank1 = it.first.first;
                                    pair<size_t, size_t> node_rank2 = it.first.second;
                                    const size_t distance = it.second;

                                    if (!ignore_distances) {
                                        //If we are keeping track of distances
                                        //If the distance exceeded the limit, then it wasn't found in the first place
                                        snarl_record_constructor.set_distance(node_rank1.first, node_rank1.second,
                                            node_rank2.first, node_rank2.second, distance);

                                        if (temp_snarl_record.tippy_child_ranks.count(node_rank1.first)
                                            && temp_snarl_record.tippy_child_ranks.count(node_rank2.first)) {
                                            snarl_record_constructor.set_tip_tip_connected();
                                        }
#ifdef debug_distance_indexing
                                        assert(distance <= temp_snarl_record.max_distance);
                                        assert(snarl_record_constructor.get_distance(node_rank1.first, node_rank1.second,
                                               node_rank2.first, node_rank2.second) ==  distance);
#endif
                                    }
                                }
                                //Now set the connectivity of this snarl
                                if (temp_snarl_record.distance_start_start != std::numeric_limits<size_t>::max()) {
                                    snarl_record_constructor.set_start_start_connected();
                                }
                                if (temp_snarl_record.min_length != std::numeric_limits<size_t>::max()) {
                                    snarl_record_constructor.set_start_end_connected();
                                }
                                if (temp_snarl_record.distance_end_end != std::numeric_limits<size_t>::max()) {
                                    snarl_record_constructor.set_end_end_connected();
                                }

#ifdef debug_distance_indexing
                            cerr << "    The snarl record is at offset " << snarl_record_constructor.record_offset << endl;
                            cerr << "    This child snarl has " << snarl_record_constructor.get_node_count() << " children: " << endl;
#endif
                                for (const pair<temp_record_t, size_t>& child : temp_snarl_record.children) {
                                    temp_record_stack.emplace_back(child);
#ifdef debug_distance_indexing
                                    cerr << "      " << temp_index->structure_start_end_as_string(child) << endl;
#endif
                                }


                                //Add connectivity in chain based on this snarl
                                //TODO: Tip-tip connected?
                                //TODO: What about if it's a multicomponent chain?
                                if (!snarl_record_constructor.get_is_reversed_in_parent()) {
                                    //If this snarl is oriented forward in the chain
                                    if (snarl_record_constructor.is_start_start_connected()) {
                                        chain_record_constructor.set_start_start_connected();
                                    }
                                    if (snarl_record_constructor.is_end_end_connected()) {
                                        chain_record_constructor.set_end_end_connected();
                                    }
                                    if (snarl_record_constructor.is_start_tip_connected()) {
                                        chain_record_constructor.set_start_tip_connected();
                                    }
                                    if (snarl_record_constructor.is_end_tip_connected()) {
                                        chain_record_constructor.set_end_tip_connected();
                                    }
                                } else {
                                    //If this snarl is oriented backwards in the chain
                                    if (snarl_record_constructor.is_start_start_connected()) {
                                        chain_record_constructor.set_end_end_connected();
                                    }
                                    if (snarl_record_constructor.is_end_end_connected()) {
                                        chain_record_constructor.set_start_start_connected();
                                    }
                                    if (snarl_record_constructor.is_start_tip_connected()) {
                                        chain_record_constructor.set_end_tip_connected();
                                    }
                                    if (snarl_record_constructor.is_end_tip_connected()) {
                                        chain_record_constructor.set_start_tip_connected();
                                    }
                                }
                                last_child_offset = make_pair(snarl_record_constructor.record_offset, true);
                            } else if (!temp_snarl_record.is_trivial && temp_snarl_record.is_simple) {
                                //Make a simple snarl

                                //Add the snarl to the chain, and get back the record to fill it in
                                bool ignore_distances = (snarl_size_limit == 0) || only_top_level_chain_distances;

                                record_t record_type = ignore_distances ? SIMPLE_SNARL : DISTANCED_SIMPLE_SNARL;
                                SimpleSnarlRecordWriter snarl_record_constructor =
                                    chain_record_constructor.add_simple_snarl(temp_snarl_record.node_count, record_type, last_child_offset.first);

                                //Record how to find the new snarl record
                                record_to_offset.emplace(make_pair(temp_index_i, child_record_index), snarl_record_constructor.record_offset);

                                //Fill in snarl info
                                if (!ignore_distances) {
                                    snarl_record_constructor.set_min_length(temp_snarl_record.min_length);
                                    snarl_record_constructor.set_max_length(temp_snarl_record.max_length);
                                }

                                //Add the children of the simple snarl
                                for (size_t i = 0 ; i < temp_snarl_record.node_count ; i++ ) {
                                    const pair<temp_record_t, size_t>& child_index = temp_snarl_record.children[i];
                                    if( child_index.first == TEMP_CHAIN) {
                                        assert(temp_index->temp_chain_records[child_index.second].children.size() == 1);
                                        const pair<temp_record_t, size_t>& node_index = temp_index->temp_chain_records[child_index.second].children.front();
                                        const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                                             temp_index->temp_node_records[node_index.second-min_node_id];
                                        //If there is a way to go from the node forward to the start node,
                                        //then it is reversed
                                        size_t rank =temp_index->temp_chain_records[child_index.second].rank_in_parent;

                                        snarl_record_constructor.add_child(i+2, temp_node_record.node_id,  
                                                temp_node_record.node_length, temp_node_record.reversed_in_parent);
                                    } else {
                                        assert(child_index.first == TEMP_NODE);
                                        const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                                             temp_index->temp_node_records[child_index.second-min_node_id];
                                        size_t rank =temp_node_record.rank_in_parent;
                                        snarl_record_constructor.add_child(i+2, temp_node_record.node_id,  
                                                temp_node_record.node_length, temp_node_record.reversed_in_parent);
                                    }
                                }

#ifdef debug_distance_indexing
                            cerr << "    The simple snarl record is at offset " << snarl_record_constructor.record_offset << endl;
                            cerr << "    This child snarl has " << snarl_record_constructor.get_node_count() << " children: " << endl;
#endif
                                last_child_offset = make_pair(snarl_record_constructor.record_offset, true);
                            }
                            last_child_was_nontrivial_snarl = false;
                        }
                    }
                    //Does the chain loop and is the last node connected to the rest of the chain through the last snarl
                    bool last_node_connected = temp_chain_record.loopable && (temp_chain_record.start_node_id==temp_chain_record.end_node_id);
                    chain_record_constructor.set_last_child_offset(last_child_offset.first, last_child_offset.second, last_node_connected);
                    //Finish the chain by adding two 0's
                } else {
                    //If the chain is trivial, then only record the node
#ifdef debug_distance_indexing
                    cerr << "        this chain is actually just a node "
                         << temp_index->structure_start_end_as_string(temp_chain_record.children[0]) << endl;
#endif
                    assert(temp_chain_record.children.size() == 1);
                    assert(temp_chain_record.children[0].first == TEMP_NODE);
                    const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                            temp_index->temp_node_records[temp_chain_record.children[0].second-min_node_id];


                    bool ignore_distances = (snarl_size_limit == 0) || only_top_level_chain_distances;

                    record_t record_type = ignore_distances ? NODE : DISTANCED_NODE;
                    NodeRecordWriter node_record(snarl_tree_records->size(), 0, record_type, &snarl_tree_records, temp_node_record.node_id);
                    node_record.set_node_id(temp_node_record.node_id);
                    node_record.set_rank_in_parent(temp_chain_record.rank_in_parent);
                    node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_chain_record.parent)]);
                    if (!ignore_distances) {
                        node_record.set_node_length(temp_node_record.node_length);
                        node_record.set_distance_left_start(temp_chain_record.distance_left_start);
                        node_record.set_distance_right_start(temp_chain_record.distance_right_start);
                        node_record.set_distance_left_end(temp_chain_record.distance_left_end);
                        node_record.set_distance_right_end(temp_chain_record.distance_right_end);
                    }

                    record_to_offset.emplace(make_pair(temp_index_i, current_record_index), node_record.record_offset);

                }
            } else if (current_record_index.first == TEMP_SNARL) {
#ifdef debug_distance_indexing
                cerr << "        this is a root-level snarl "
                     << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif

                bool ignore_distances = (snarl_size_limit == 0) || only_top_level_chain_distances;
                //This is a root-level snarl
                record_t record_type = ignore_distances ? ROOT_SNARL : DISTANCED_ROOT_SNARL;

                const TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record = temp_index->temp_snarl_records[current_record_index.second];
                record_to_offset.emplace(make_pair(temp_index_i,current_record_index), snarl_tree_records->size());

                SnarlRecordWriter snarl_record_constructor (temp_snarl_record.node_count, &snarl_tree_records, record_type);

                //Fill in snarl info
                snarl_record_constructor.set_parent_record_offset(0);

                //Add distances and record connectivity

                if (!ignore_distances ) {


                    for (const auto& it : temp_snarl_record.distances) {
                        const pair<size_t, bool> node_rank1 = it.first.first;
                        const pair<size_t, bool> node_rank2 = it.first.second;
                        const size_t distance = it.second;
                        //If we are keeping track of distances and either this is a small enough snarl,
                        //or the snarl is too big but we are looking at the boundaries
#ifdef debug_distance_indexing
                        assert(distance <= temp_snarl_record.max_distance);
#endif
                        if ((temp_snarl_record.node_count < snarl_size_limit)) {
                            snarl_record_constructor.set_distance(node_rank1.first, node_rank1.second,
                             node_rank2.first, node_rank2.second, distance);
#ifdef debug_distance_indexing
                            assert(snarl_record_constructor.get_distance(node_rank1.first, node_rank1.second,
                                    node_rank2.first, node_rank2.second) == distance);
#endif
                        }
                    }
                }

#ifdef debug_distance_indexing
                cerr << "    The snarl record is at offset " << snarl_record_constructor.record_offset << endl;
                cerr << "    This child snarl has " << snarl_record_constructor.get_node_count() << " children: " << endl;
#endif
                for (const pair<temp_record_t, size_t>& child : temp_snarl_record.children) {
                        temp_record_stack.emplace_back(child);
                }

            } else {
                assert(current_record_index.first == TEMP_NODE);
                //and then add them all after adding the snarl
#ifdef debug_distance_indexing
                cerr << "        this just a node "
                     << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif
                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                        temp_index->temp_node_records[current_record_index.second-min_node_id];

                bool ignore_distances = (snarl_size_limit == 0) || only_top_level_chain_distances;
                record_t record_type = ignore_distances ? NODE : DISTANCED_NODE;
                NodeRecordWriter node_record(snarl_tree_records->size(), 0, record_type, &snarl_tree_records, temp_node_record.node_id);
                node_record.set_node_id(temp_node_record.node_id);
                node_record.set_rank_in_parent(temp_node_record.rank_in_parent);
                node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_node_record.parent)]);

                if (!ignore_distances) {
                    node_record.set_node_length(temp_node_record.node_length);
                    node_record.set_distance_left_start(temp_node_record.distance_left_start);
                    node_record.set_distance_right_start(temp_node_record.distance_right_start);
                    node_record.set_distance_left_end(temp_node_record.distance_left_end);
                    node_record.set_distance_right_end(temp_node_record.distance_right_end);
                }

                record_to_offset.emplace(make_pair(temp_index_i, current_record_index), node_record.record_offset);
            }
#ifdef debug_distance_indexing
            cerr << "Finished translating " << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif
        }
#ifdef debug_distance_indexing
        cerr << "Adding roots" << endl;
#endif

        for (size_t component_num = 0 ; component_num < temp_index->components.size() ; component_num++){
            const pair<temp_record_t, size_t>& component_index = temp_index->components[component_num];
            //Let the root record know that it has another root
            root_record.add_component(component_num,record_to_offset[make_pair(temp_index_i,component_index)]);

            SnarlTreeRecord record (record_to_offset[make_pair(temp_index_i, component_index)],
                                    &snarl_tree_records);
            SnarlTreeRecordWriter record_constructor(record_to_offset[make_pair(temp_index_i, component_index)],
                                                              &snarl_tree_records);

            if (record.get_record_handle_type() == CHAIN_HANDLE || record.get_record_handle_type() == ROOT_HANDLE) {
                record_constructor.set_rank_in_parent(component_num);
            }
            if (record.get_record_type() != ROOT_SNARL && record.get_record_type() != DISTANCED_ROOT_SNARL) {
                //If this isn't a root snarl
                handle_t start_out = graph->get_handle(record.get_start_id(), !record.get_start_orientation());
                handle_t end_out = graph->get_handle(record.get_end_id(), record.get_end_orientation());
                handle_t start_in = graph->get_handle(record.get_start_id(), record.get_start_orientation());
                handle_t end_in = graph->get_handle(record.get_end_id(), !record.get_end_orientation());


                graph->follow_edges(start_out, false, [&](const handle_t& h) {
                    if (h == start_in) {
                        record_constructor.set_externally_start_start_connected();
                    } else if (h == end_in) {
                        record_constructor.set_externally_start_end_connected();
                    }
                    return true;
                });
                graph->follow_edges(end_out, false, [&](const handle_t& h) {
                    if (h == end_in) {
                        record_constructor.set_externally_end_end_connected();
                    } else if (h == start_in) {
                        record_constructor.set_externally_start_end_connected();
                    }
                    return true;
                });
            }


#ifdef debug_distance_indexing
            cerr << temp_index->structure_start_end_as_string(component_index) << endl;
            //assert(record.get_parent_record_offset() == 0);
#endif
        }
    }



#ifdef debug_distance_indexing
    cerr << "Now filling in children of each snarl" << endl;
    cerr << "The index currently has size " << snarl_tree_records->size() << endl;
#endif

    /* Now go through everything again and give everything children */
    for (size_t temp_index_i = 0 ; temp_index_i < temporary_indexes.size() ; temp_index_i++) {
        const TemporaryDistanceIndex* temp_index = temporary_indexes[temp_index_i];
        for (size_t temp_snarl_i = 0 ; temp_snarl_i < temp_index->temp_snarl_records.size() ; temp_snarl_i ++) {
            //Get the temporary index for this snarl
            const TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record = temp_index->temp_snarl_records[temp_snarl_i];
            if (!temp_snarl_record.is_trivial && !temp_snarl_record.is_simple) {
                //And a constructor for the permanent record, which we've already created
                SnarlRecordWriter snarl_record_constructor (&snarl_tree_records,
                        record_to_offset[make_pair(temp_index_i, make_pair(TEMP_SNARL, temp_snarl_i))]);
                //Now add the children and tell the record where to find them
                snarl_record_constructor.set_child_record_pointer(snarl_tree_records->size());
                for (pair<temp_record_t, size_t> child : temp_snarl_record.children) {
                    snarl_record_constructor.add_child(record_to_offset[make_pair(temp_index_i, child)]);

                    //Check if the child is a tip, and if so set start/end_tip connectivity of parent snarl
                    if (child.first == TEMP_NODE) {
                        auto temp_node_record = temp_index->temp_node_records[child.second-min_node_id];
                        if (temp_node_record.is_tip) {
                            if (temp_node_record.distance_left_start != std::numeric_limits<size_t>::max() ||
                                 temp_node_record.distance_right_start != std::numeric_limits<size_t>::max()){
                                snarl_record_constructor.set_start_tip_connected();
                            }
                            if (temp_node_record.distance_left_end != std::numeric_limits<size_t>::max() ||
                                 temp_node_record.distance_right_end != std::numeric_limits<size_t>::max()){
                                snarl_record_constructor.set_end_tip_connected();
                            }
                        }
                    } else {
                        auto temp_chain_record = temp_index->temp_chain_records[child.second];
                        if (temp_chain_record.is_tip) {
                            if (temp_chain_record.distance_left_start != std::numeric_limits<size_t>::max() ||
                                 temp_chain_record.distance_right_start != std::numeric_limits<size_t>::max()){
                                snarl_record_constructor.set_start_tip_connected();
                            }
                            if (temp_chain_record.distance_left_end != std::numeric_limits<size_t>::max() ||
                                 temp_chain_record.distance_right_end != std::numeric_limits<size_t>::max()){
                                snarl_record_constructor.set_end_tip_connected();
                            }
                        }
                    }
#ifdef debug_distance_indexing
                cerr << "       child " << temp_index->structure_start_end_as_string(child) << endl;
                cerr << "        " << child.first << " " << child.second << endl;
                //cerr << "        Add child " << net_handle_as_string(get_net_handle_from_values(record_to_offset[make_pair(temp_index_i, child)], START_END))
                cerr     << "     at offset " << record_to_offset[make_pair(temp_index_i, child)]
                     << "     to child list at offset " << snarl_tree_records->size() << endl;
#endif
                }
            }
        }
    }
#ifdef debug_distance_indexing
    //Repack the vector to use fewer bits
    //This doesn't actually get used anymore but keep it around in case I change things and can't
    //predict the size anymore
    size_t max_val = 0;
    for (size_t i = 0 ; i < snarl_tree_records->size() ; i++ ) {
        max_val = std::max(max_val, (size_t) snarl_tree_records->at(i));
    }
    size_t ideal_bit_width = std::max((size_t)log2(max_val)+1, (size_t)26);
    if (ideal_bit_width < snarl_tree_records->width()) {
        cerr << "Resetting bit width from " << snarl_tree_records->width() << " to " << ideal_bit_width << endl;
        //snarl_tree_records->repack(ideal_bit_width, snarl_tree_records->size());
    }
#endif
#ifdef debug_distance_indexing
    tuple<size_t, size_t, size_t> usage =get_usage();
    cerr << "total\t" << snarl_tree_records->size() << endl;
    cerr << "Usage: " << std::get<0>(usage) << " total bytes, " << std::get<1>(usage) << " free bytes " << std::get<2>(usage) << " reclaimable free bytes " << endl;
    cerr << "bit width " << snarl_tree_records->width() << endl;
    cerr << "Max value " << max_val << endl;
    cerr << "Predicted size: " << maximum_index_size << " actual size: " <<  snarl_tree_records->size() << endl;
    //assert(maximum_index_size == snarl_tree_records->size());
    cerr << "Predicted size: " << maximum_index_size << " actual size: " <<  snarl_tree_records->size() << endl;
    //assert(snarl_tree_records->size() <= maximum_index_size); 
#endif
#ifdef count_allocations
    //tuple<size_t, size_t, size_t> usage =get_usage();
    cerr << "total\t" << snarl_tree_records->size() << endl;
    cerr << "Usage: " << std::get<0>(usage) << " total bytes, " << std::get<1>(usage) << " free bytes " << std::get<2>(usage) << " reclaimable free bytes " << endl;
    cerr << "bit width " << snarl_tree_records->width() << endl;
    cerr << "Max value " << max_val << endl;
    cerr << "Predicted size: " << maximum_index_size << " actual size: " <<  snarl_tree_records->size() << endl;
#endif


}


//TODO: Also need to go the other way, from final index to temporary one for merging

void SnarlDistanceIndex::time_accesses() {
    cerr << "Distance index:" << endl;
    cerr << "Vector of length " << snarl_tree_records->size() << " with bit width " << snarl_tree_records->width() << endl;
    uniform_int_distribution<size_t> rand (1, snarl_tree_records->size() -1);
    default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    for (size_t step_size : {1, 100, 1000, 10000, 100000, 1000000, 10000000, 0}) {
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

        for (size_t i = 0 ; i < 100000000 ; i++) {
            size_t pos = step_size == 0
                    ? rand(generator)
                    : (i * step_size) % snarl_tree_records->size();
            size_t val = snarl_tree_records->at( pos);
        }
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cerr << "Step size " << step_size << ": " << elapsed_seconds.count() << " seconds " << endl;
    }

    cerr << "std::vector<size_t>" << endl;
    vector<size_t> empty_vector (snarl_tree_records->size(), 0);

    for (size_t step_size : {1, 100, 1000, 10000, 100000, 1000000, 10000000, 0}) {
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

        for (size_t i = 0 ; i < 100000000 ; i++) {
            size_t pos = step_size == 0
                    ? rand(generator)
                    : (i * step_size) % snarl_tree_records->size();
            size_t val = empty_vector.at(pos);
        }
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cerr << "Step size " << step_size << ": " << elapsed_seconds.count() << " seconds " << endl;
    }
}



}
