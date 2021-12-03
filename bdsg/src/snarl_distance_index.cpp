//#define debug_distance_indexing
//#define debug_snarl_traversal
//#define debug_distances

#include "bdsg/snarl_distance_index.hpp"

using namespace std;
using namespace handlegraph;
namespace bdsg {


///////////////////////////////////////////////////////////////////////////////////////////////////
//Constructor
SnarlDistanceIndex::SnarlDistanceIndex() {
    snarl_tree_records.construct(tag);
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
size_t SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length() const {
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
        return CHAIN_RECORD_SIZE + (TRIVIAL_SNARL_RECORD_SIZE*trivial_snarl_count) + (total_node_count * 2) + ((trivial_snarl_count + nontrivial_snarl_count) * 2) - 1;
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
        return SimpleSnarlRecord::record_size(node_count); 
    } else {
         if (parent.first == TEMP_ROOT) {
             return SnarlRecord::record_size(DISTANCED_ROOT_SNARL, node_count) + node_count;
         } else {
            return SnarlRecord::record_size(DISTANCED_SNARL, node_count) + node_count;
         }
    }
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//Implement the SnarlDecomposition's functions for moving around the snarl tree
//


net_handle_t SnarlDistanceIndex::get_root() const {
    // The root is the first thing in the index, the traversal is tip to tip
    return get_net_handle(0, START_END, ROOT_HANDLE);
}

bool SnarlDistanceIndex::is_root(const net_handle_t& net) const {
    return get_handle_type(net) == ROOT_HANDLE;
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
bool SnarlDistanceIndex::is_looping_chain(const net_handle_t& net) const {
    if (!is_chain(net)) {
        return false;
    }
    ChainRecord chain_record(net, &snarl_tree_records);
    return chain_record.get_start_id() == chain_record.get_end_id();
}
bool SnarlDistanceIndex::is_ordered_in_chain(const net_handle_t& child1, const net_handle_t& child2) const {
    if (!(is_chain(get_parent(child1)) && get_parent(child1) == get_parent(child2))) {
        throw runtime_error("error: trying to get a handle from a snarl, chain, or root");
    } else {
        size_t rank1 = is_node(child1) ? TrivialSnarlRecord(get_record_offset(child1), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(child1))
                                       : SnarlTreeRecord(child1, &snarl_tree_records).get_rank_in_parent();
        size_t rank2 = is_node(child2) ? TrivialSnarlRecord(get_record_offset(child2), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(child2))
                                       : SnarlTreeRecord(child2, &snarl_tree_records).get_rank_in_parent();

        return  rank1 < rank2;
    }
}
size_t SnarlDistanceIndex::get_record_offset_in_chain(const net_handle_t& child) const {
    if (!is_chain(get_parent(child))) {
        throw runtime_error("error: net handle isn't in a chain");
    } else {

        return is_node(child) ? TrivialSnarlRecord(get_record_offset(child), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(child))
                                       : SnarlTreeRecord(child, &snarl_tree_records).get_rank_in_parent();
;
    }
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
bool SnarlDistanceIndex::is_trivial_chain(const CachedNetHandle& cached_net) const {
    bool handle_is_chain =get_handle_type(cached_net.net) == CHAIN_HANDLE; 
    bool record_is_node = get_record_handle_type(get_record_type(cached_net.record_tag)) == NODE_HANDLE;
    bool record_is_simple_snarl = get_record_type(cached_net.record_tag) == SIMPLE_SNARL ||
                                  get_record_type(cached_net.record_tag) == DISTANCED_SIMPLE_SNARL ;
    bool handle_has_node_offset = get_node_record_offset(cached_net.net) >= 2;
    
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

net_handle_t SnarlDistanceIndex::get_parent(CachedNetHandle& cached_child) const {


    //If the child is the sentinel of a snarl, just return the snarl
    if (get_handle_type(cached_child.net) == SENTINEL_HANDLE) {
        return get_net_handle(get_record_offset(cached_child.net), START_END, SNARL_HANDLE, get_node_record_offset(cached_child.net)); 
    } else if (get_handle_type(cached_child.net) == ROOT_HANDLE) {
        throw runtime_error("error: trying to find the parent of the root");
    } else if (get_record_type(cached_child.record_tag) == SIMPLE_SNARL ||
               get_record_type(cached_child.record_tag) == DISTANCED_SIMPLE_SNARL) {
        //If this is a simple snarl and a node or chain, then the parent offset doesn't change
        if (get_handle_type(cached_child.net) == NODE_HANDLE) {
            //If this is a node, then return it as a chain
            return get_net_handle(get_record_offset(cached_child.net), START_END, CHAIN_HANDLE, get_node_record_offset(cached_child.net));
        } else if (get_handle_type(cached_child.net) == CHAIN_HANDLE) {
            //If this is a chain, then return the same thing as a snarl
            return get_net_handle(get_record_offset(cached_child.net), START_END, SNARL_HANDLE, 1);
        }
    }

    //Otherwise, we need to move up one level in the snarl tree

    //Get the pointer to the parent, and keep the connectivity of the current handle
    size_t parent_pointer = get_cached_parent_offset(cached_child);
    connectivity_t child_connectivity = get_connectivity(cached_child.net);

    //TODO: I"m going into the parent record here, which could be avoided if things knew what their parents were, but I think if you're doing this you'd later go into the parent anyway so it's probably fine
    net_handle_record_t parent_type = SnarlTreeRecord(parent_pointer, &snarl_tree_records).get_record_handle_type();
    connectivity_t parent_connectivity = START_END;
    if ((child_connectivity == START_END || child_connectivity == END_START) 
        && (parent_type == CHAIN_HANDLE)) {
        //TODO: This also needs to take into account the orientation of the child, which I might be able to get around?
        parent_connectivity = child_connectivity;
    }
    if (get_handle_type(cached_child.net) == NODE_HANDLE && parent_type != CHAIN_HANDLE) {
        //If this is a node and it's parent is not a chain, we want to pretend that its 
        //parent is a chain
        return get_net_handle(get_record_offset(cached_child.net), parent_connectivity, CHAIN_HANDLE, get_node_record_offset(cached_child.net));
    } 

    return get_net_handle(parent_pointer, parent_connectivity);
}
net_handle_t SnarlDistanceIndex::get_parent(const net_handle_t& child) const {
    CachedNetHandle cached_child = get_cached_net_handle(child);
    return get_parent(cached_child);
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
            //TODO: This might cause problems when checking traversals but I think it's fine
            connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), get_end_endpoint(connectivity));
        }
        return get_net_handle(offset, connectivity,  NODE_HANDLE, node_offset);

    } else if (is_root(snarl)) {
        throw runtime_error("Trying to get the bounds of a root");
    } else {
        assert(get_handle_type(snarl) == SNARL_HANDLE);
        endpoint_t start = get_end ? END : START;
        endpoint_t end = face_in ? (start == END ? START : END) : start;
        return get_net_handle(get_record_offset(snarl), endpoints_to_connectivity(start, end), SENTINEL_HANDLE, get_node_record_offset(snarl));
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

    bool go_left = starts_at(sentinel) == START;
    if (SnarlRecord(snarl, &snarl_tree_records).get_is_reversed_in_parent()) {
        //TODO: I think this is always false
        assert(false);
        go_left = !go_left;
    }

    net_handle_t next_handle = chain_record.get_next_child(snarl, go_left);
    //If the sentinel is facing into the snarl, then flip the node after getting it from the chain
    return ends_at(sentinel) != starts_at(sentinel) ? flip(next_handle) : next_handle ;


}

net_handle_t SnarlDistanceIndex::flip(const net_handle_t& net) const {
    connectivity_t old_connectivity = get_connectivity(net);
    connectivity_t new_connectivity =  endpoints_to_connectivity(get_end_endpoint(old_connectivity), 
                                                                get_start_endpoint(old_connectivity));
    return get_net_handle(get_record_offset(net), new_connectivity, get_handle_type(net), get_node_record_offset(net));
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
        connectivity = START_END; //TODO: put this back throw runtime_error("error: This node has no connectivity");
    }
    return get_net_handle(get_record_offset(net), connectivity, get_handle_type(net), get_node_record_offset(net));
}

SnarlDecomposition::endpoint_t SnarlDistanceIndex::starts_at(const net_handle_t& traversal) const {
    return get_start_endpoint(get_connectivity(traversal));

}
SnarlDecomposition::endpoint_t SnarlDistanceIndex::ends_at(const net_handle_t& traversal) const {
    return get_end_endpoint( get_connectivity(traversal));
}

//TODO: I'm also allowing this for the root
bool SnarlDistanceIndex::for_each_child_impl(const net_handle_t& traversal, const std::function<bool(const net_handle_t&)>& iteratee) const {
#ifdef debug_snarl_traversal
    cerr << "Go through children of " << net_handle_as_string(traversal) << endl;
#endif
    //What is this according to the snarl tree
    net_handle_record_t record_type = SnarlTreeRecord(traversal, &snarl_tree_records).get_record_handle_type();
    //What is this according to the handle 
    //(could be a trivial chain but actually a node according to the snarl tree)
    net_handle_record_t handle_type = get_handle_type(traversal);
    if (SnarlTreeRecord(traversal, &snarl_tree_records).get_record_type() == SIMPLE_SNARL ||
        SnarlTreeRecord(traversal, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL ) {
        //If this is a simple snarl then it is a bit different
        if (handle_type == CHAIN_HANDLE) {
            //If the handle thinks it's a chain, then it is a trivial chain in the snarl so we do
            //iteratee for the node
            return iteratee(get_net_handle(get_record_offset(traversal), get_connectivity(traversal), 
                                           NODE_HANDLE, get_node_record_offset(traversal)));
        } else if (handle_type == SNARL_HANDLE) {
            return SimpleSnarlRecord(traversal, &snarl_tree_records).for_each_child(iteratee);
        } else if (handle_type == CHAIN_HANDLE) {
            return iteratee(get_net_handle(get_record_offset(traversal), get_connectivity(traversal), NODE_HANDLE, get_node_record_offset(traversal)));
        } else {
            throw runtime_error("error: Looking for children of a node or sentinel in a simple snarl");
        }
    } else if (record_type == SNARL_HANDLE) {
        SnarlRecord snarl_record(traversal, &snarl_tree_records);
        return snarl_record.for_each_child(iteratee);
    } else if (record_type == CHAIN_HANDLE) {
        ChainRecord chain_record(traversal, &snarl_tree_records);
        return chain_record.for_each_child(iteratee);
    } else if (record_type == ROOT_HANDLE) {
        RootRecord root_record(traversal, &snarl_tree_records);
        return root_record.for_each_child(iteratee);
    } else if (record_type == NODE_HANDLE && handle_type == CHAIN_HANDLE) {
        //This is actually a node but we're pretending it's a chain
#ifdef debug_snarl_traversal
        cerr << "     which is actually a node pretending to be a chain" << endl;
#endif
        return iteratee(get_net_handle(get_record_offset(traversal), get_connectivity(traversal), NODE_HANDLE));
    } else {
        throw runtime_error("error: Looking for children of a node or sentinel");
    }
   
}

bool SnarlDistanceIndex::for_each_traversal_impl(const net_handle_t& item, const std::function<bool(const net_handle_t&)>& iteratee) const {
    if (get_handle_type(item) == SENTINEL_HANDLE) {
        //TODO: I'm not sure what to do here?
        if (!iteratee(get_net_handle(get_record_offset(item), START_END, get_handle_type(item), get_node_record_offset(item)))) {
            return false;
        }
        if (!iteratee(get_net_handle(get_record_offset(item), END_START, get_handle_type(item), get_node_record_offset(item)))) {
            return false;
        }
    }
    SnarlTreeRecord record(item, &snarl_tree_records);
    for ( size_t type = 1 ; type <= 9 ; type ++ ){
        connectivity_t connectivity = static_cast<connectivity_t>(type);
        if (record.has_connectivity(connectivity)) {
            if (!iteratee(get_net_handle(get_record_offset(item), connectivity, get_handle_type(item), get_node_record_offset(item)))) {
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

    if (parent_record.get_record_handle_type() == ROOT_HANDLE) {
        //TODO: Double check that this is the right way to handle this
        //If this is a root-level chain or node
        if ((ends_at(here) == END && !go_left) || (ends_at(here) == START && go_left)) {
            //Follow edges leaving the root structure at the end
            if (this_record.is_externally_start_end_connected()) {
                //Follow edge from end to start
                if (!iteratee(get_net_handle(get_record_offset(here), START_END, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from end back to the end
                if (!iteratee(get_net_handle(get_record_offset(here), END_START, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
        } else {
            //Follow edges leaving the root structure at the end
            if (this_record.is_externally_start_end_connected()) {
                //Follow edge from start to end
                if (!iteratee(get_net_handle(get_record_offset(here), END_START, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from the start back to the start
                if (!iteratee(get_net_handle(get_record_offset(here), START_END, get_handle_type(here), get_node_record_offset(here)))) {
                    return false;
                }
            }

        }
        return true;

    } else if (get_handle_type(here) == CHAIN_HANDLE || get_handle_type(here) == SENTINEL_HANDLE) {
#ifdef debug_distances
        assert(parent_record.get_record_handle_type() == SNARL_HANDLE ||
               parent_record.get_record_handle_type() == ROOT_HANDLE);
        //It could also be the root
#endif
        //If this is a chain (or a node pretending to be a chain) and it is the child of a snarl
        //Or if it is the sentinel of a snarl, then we walk through edges in the snarl
        //It can either run into another chain (or node) or the boundary node
        //TODO: What about if it is the root?


        //Get the graph handle for the end node of whatever this is, pointing in the right direction
        handle_t graph_handle;
        if (get_handle_type(here) == SENTINEL_HANDLE) {
            if ((get_connectivity(here) == START_END && !go_left) ||
                (get_connectivity(here) == START_START && go_left)) {
                graph_handle = graph->get_handle(parent_record.get_start_id(), parent_record.get_start_orientation());
            } else if ((get_connectivity(here) == END_START && !go_left) ||
                       (get_connectivity(here) == END_END && go_left)) {
                graph_handle = graph->get_handle(parent_record.get_end_id(), !parent_record.get_end_orientation());
            } else {
                //This is facing out, so don't do anything 
                return true;
            }
        } else if (get_handle_type(here) == NODE_HANDLE ||is_trivial_chain(here)) {
            nid_t id = is_trivial_chain(here) ? NodeRecord(here, &snarl_tree_records).get_node_id()
                                                  : node_id(here);
            graph_handle = graph->get_handle(id, ends_at(here) == END ? go_left : !go_left);
        } else {
            //TODO: This might not be the best way to handle orientation because it's a bit inconsistent with tips
            //Might be better to just use go_left and pretend the traversal is forward, but that might be 
            //unintuitive if you have a sentinel of a snarl that you think should be going in or out of the snarl
            //
            //If the traversal explicitly goes out the start, then we assume that it is oriented backwards
            //and go the opposite direction of go_left. Otherwise, assume that it is oriented forwards
            if (ends_at(here) == START) {
                go_left = !go_left;
            } 
            graph_handle = get_handle(get_bound(here, !go_left, false), graph);
        }
#ifdef debug_snarl_traversal
        cerr << "        traversing graph from actual node " << graph->get_id(graph_handle) << (graph->get_is_reverse(graph_handle) ? "rev" : "fd") << endl;
#endif
        graph->follow_edges(graph_handle, false, [&](const handle_t& h) {
#ifdef debug_snarl_traversal
            cerr << "  reached actual node " << graph->get_id(h) << (graph->get_is_reverse(h) ? "rev" : "fd") << endl;
#endif

            if (graph->get_id(h) == parent_record.get_start_id()) {
                //If this is the start boundary node of the parent snarl, then do this on the sentinel
#ifdef debug_snarl_traversal
                cerr << "    -> start of parent " << endl;
                assert(graph->get_is_reverse(h) == !parent_record.get_start_orientation());
#endif
                return iteratee(get_bound(get_parent(here), false, false));
            } else if (graph->get_id(h) == parent_record.get_end_id()) {
#ifdef debug_snarl_traversal
                assert(graph->get_is_reverse(h) == parent_record.get_end_orientation());
                cerr << "    -> end of parent " << endl;
#endif
                return iteratee(get_bound(get_parent(here), true, false));
            } else {
                //It is either another chain or a node, but the node needs to pretend to be a chain
                net_handle_t node_net_handle = get_node_net_handle(graph->get_id(h));

                if (get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == NODE || 
                    get_record_type(snarl_tree_records->at(get_record_offset(node_net_handle))) == DISTANCED_NODE   ) {
                    //If this is a node make a net_handle_t of a node pretending to be a chain
                    net_handle_t next_net = get_net_handle(get_record_offset(node_net_handle), 
                                                           graph->get_is_reverse(h) ? END_START : START_END, 
                                                           CHAIN_HANDLE);
#ifdef debug_snarl_traversal
                cerr << "    -> actual child node " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                } else {
                    //next_node_record is also the start of a chain
                    bool rev = get_record_offset(node_net_handle) != 
                               ChainRecord(get_parent(node_net_handle), &snarl_tree_records).get_first_node_offset();
                    net_handle_t next_net = get_net_handle(get_record_offset(get_parent(node_net_handle)), 
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
        //If this is a snarl or node, then it is the component of a (possibly pretend) chain
        ChainRecord parent_chain(this_record.get_parent_record_offset(), &snarl_tree_records);
        if (ends_at(here) == START) {
            go_left = !go_left;
        }
        bool is_rev = is_node(here) ? TrivialSnarlRecord(get_record_offset(here), &snarl_tree_records).get_is_reversed_in_parent(get_node_record_offset(here))
                                    : false;
        if (is_rev) {
            go_left = !go_left;
        }
        net_handle_t next_net = parent_chain.get_next_child(here, go_left);
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
        //TODO: Make sure this is handling possible orientations properly
        assert(end_handle_type == SENTINEL_HANDLE);
        endpoint_t start_endpoint = get_start_endpoint(get_connectivity(traversal_start));
        endpoint_t end_endpoint = get_start_endpoint(get_connectivity(traversal_end));
        return get_net_handle(get_record_offset(get_parent(traversal_start)), 
                              endpoints_to_connectivity(start_endpoint, end_endpoint),
                              SNARL_HANDLE, get_node_record_offset(traversal_start));
    } else {
        //These are the endpoints or tips in a chain
        SnarlTreeRecord start_record = get_snarl_tree_record(traversal_start);
        SnarlTreeRecord end_record = get_snarl_tree_record(traversal_end);
        if (start_record.get_parent_record_offset() != end_record.get_parent_record_offset()) {
            throw runtime_error("error: Looking for parent traversal of two non-siblings");
        }
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

        if (!parent_record.has_connectivity(start_endpoint, end_endpoint)) {
            throw runtime_error("error: Trying to get parent traversal that is not connected");
        }

        return get_net_handle(parent_record.record_offset, 
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
    snarl_tree_records.save(fd);
}
void SnarlDistanceIndex::deserialize(int fd) {
    snarl_tree_records.load(fd, tag);
}

void SnarlDistanceIndex::serialize_members(std::ostream& out) const {
    snarl_tree_records.save_after_prefix(out, get_prefix());
}
void SnarlDistanceIndex::deserialize_members(std::istream& in){
    snarl_tree_records.load_after_prefix(in, get_prefix());
}

uint32_t SnarlDistanceIndex::get_magic_number()const {
    //random number?
    return 1738636486;
}
//Copied from MappedPackedGraph
std::string SnarlDistanceIndex::get_prefix() const {
    //TODO: I don't know if this is ok
    return tag;
}


size_t SnarlDistanceIndex::distance_in_parent(const net_handle_t& parent, 
        const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph) const {

    CachedNetHandle cached_parent = get_cached_net_handle(parent);
    CachedNetHandle cached_child1 = get_cached_net_handle(child1);
    CachedNetHandle cached_child2 = get_cached_net_handle(child2);
    return distance_in_parent(cached_parent, cached_child1, false, cached_child2, false, graph);

}

size_t SnarlDistanceIndex::distance_in_parent(CachedNetHandle& cached_parent, 
        CachedNetHandle& cached_child1, bool go_left1, 
        CachedNetHandle& cached_child2, bool go_left2, const HandleGraph* graph) const {

    net_handle_t child1 = go_left1 ? flip(cached_child1.net) : cached_child1.net;
    net_handle_t child2 = go_left2 ? flip(cached_child2.net) : cached_child2.net;
    net_handle_t parent = cached_parent.net;
#ifdef debug_distances
    assert(canonical(cached_parent.net) == canonical(get_parent(child1)));
    assert(canonical(cached_parent.net) == canonical(get_parent(child2)));
    cerr << "\t\tFind distance between " << net_handle_as_string(child1) 
         << " and " << net_handle_as_string(child2) 
         << "\tin parent " << net_handle_as_string(cached_parent.net) << endl;
#endif

    if (is_root(parent)) {
        //If the parent is the root, then the children must be in the same root snarl for them to be
        //connected
        size_t parent_record_offset1 = get_cached_parent_offset(cached_child1);
        size_t parent_record_offset2 = get_cached_parent_offset(cached_child2);

        if (parent_record_offset1 != parent_record_offset2) {
            //If the children are in different connected components
            return std::numeric_limits<size_t>::max();
        } else if (get_record_type(snarl_tree_records->at(parent_record_offset1)) != DISTANCED_ROOT_SNARL){
            //If they are in the same connected component, but it is not a root snarl
            if (get_record_offset(child1) == get_record_offset(child2)) {
                //If they are the same child of the root but not in a snarl, then check the external connectivity
                if (ends_at(child1) == START && ends_at(child2) == START) {
                    return has_external_connectivity(cached_child1.record_tag, START, START) ? 0 : std::numeric_limits<size_t>::max();
                } else if (ends_at(child1) == END && ends_at(child2) == END) {
                    return has_external_connectivity(cached_child1.record_tag, END, END) ? 0 : std::numeric_limits<size_t>::max();
                } else if ((ends_at(child1) == START && ends_at(child2) == END) ||
                            (ends_at(child1) == END && ends_at(child2) == START)) {
                    if (has_external_connectivity(cached_child1.record_tag, START, END)) {
                        //If we can take an edge around the snarl
                        return 0;
                    } else if (has_external_connectivity(cached_child1.record_tag, START, START) && has_external_connectivity(cached_child1.record_tag, END, END)) {
                        //If we can take the loops on the two ends of the snarl, walk through the snarl
                        return get_cached_min_length(cached_child1);
                    }
                }             
            }
            return std::numeric_limits<size_t>::max();
        } else {
            //They are in the same root snarl, so find the distance between them
            SnarlRecord snarl_record(parent_record_offset1, &snarl_tree_records);

            return snarl_record.get_distance(get_cached_rank(cached_child1), ends_at(child1) == END, 
                                             get_cached_rank(cached_child2), ends_at(child2) == END);
        }


    } else if (is_chain(parent)) {
        if (get_record_handle_type(get_record_type(cached_parent.record_tag)) == NODE_HANDLE ||
            get_record_handle_type(get_record_type(cached_parent.record_tag)) == SNARL_HANDLE) {
            return std::numeric_limits<size_t>::max();
        }
        ChainRecord chain_record(parent, &snarl_tree_records, cached_parent.record_tag);
#ifdef debug_distances
        assert(is_node(child1) || is_snarl(child1));
        assert(is_node(child2) || is_snarl(child2));
#endif

        //TODO: This is awful
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

        //The node lengths of snarl boundaries, to be added to the total distance
        size_t node_lengths_to_add = 0;

        //Get all the relevant values from the cached net handle
        //Looks up the values from the distance index only if necessary
        for (size_t i = 0 ; i <=1 ; i ++ ) {
            CachedNetHandle& cached_child = i == 0 ? cached_child1 : cached_child2;
            net_handle_t& net = i == 0 ? child1 : child2;
            size_t& rank_in_chain= i == 0 ? rank_in_chain1: rank_in_chain2;
            bool& go_left_in_chain = i == 0 ? go_left1  : go_left2;
            size_t& node_length  = i == 0 ? node_length1  : node_length2;
            size_t& prefix_sum   = i == 0 ? prefix_sum1   : prefix_sum2;
            size_t& forward_loop = i == 0 ? forward_loop1 : forward_loop2;
            size_t& reverse_loop = i == 0 ? reverse_loop1 : reverse_loop2;
            size_t& component    = i == 0 ? component1    : component2;

            if (is_node(net)) {
                if (!cached_child.contains_node_values) {
                    set_cached_node_values(cached_child);
                }

                bool go_left = get_cached_is_reverse(cached_child);
                if (ends_at(net) == START){
                    go_left = !go_left;
                }
                rank_in_chain=get_cached_rank(cached_child);
                go_left_in_chain = go_left;
                node_length = get_cached_min_length(cached_child);
                std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                    get_cached_chain_values(cached_child);
                
            } else {
                //If this is a snarl, then find the appropriate boundary node and fill it in
                assert(is_snarl(net));
                if (ends_at(net) == START) {
                    //If we're going backwards in the chain, then we want the start node

                    //Make sure that the cached net handle has the start bound set
                    set_cached_start_bound(cached_child, true, true); 
                    //And turn it into a node, since it will be a sentinel
                    net_handle_t start_bound = flip(cached_child.start_bound_in);

                    size_t start_length = get_cached_start_bound_length(cached_child);

                        //set values for first
                    rank_in_chain=get_record_offset(start_bound) + get_node_record_offset(start_bound);
                    go_left_in_chain = true;
                    node_length = start_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) = 
                        get_cached_start_chain_values(cached_child);

                    node_lengths_to_add += start_length;
                } else {
                    //Do the same thing for the snarl end node if we're going forwards
                    set_cached_end_bound(cached_child,  true, true); 
                    net_handle_t end_bound = flip(cached_child.end_bound_in);
                    size_t end_length = get_cached_end_bound_length(cached_child);
                    size_t end_rank = get_record_offset(end_bound) + get_node_record_offset(end_bound);

                    //set values for first
                    rank_in_chain=end_rank;
                    go_left_in_chain = false;
                    node_length = end_length;
                    std::tie(prefix_sum, forward_loop, reverse_loop, component) =
                            get_cached_end_chain_values(cached_child);

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
        //True if we checked tha there is a loop
        bool checked_looping_chain = cached_parent.contains_start_bound && cached_parent.contains_end_bound;
        bool is_looping_chain = checked_looping_chain 
                              ?  get_record_offset(get_cached_start_bound(cached_parent)) == get_record_offset(get_cached_end_bound(cached_parent))
                              : false;
            

        return sum({chain_record.get_distance(rank_in_chain1, go_left1,
                                              node_length1, prefix_sum1,
                                              forward_loop1, reverse_loop1,
                                              component1, 
                                              rank_in_chain2, go_left2,
                                              node_length2, prefix_sum2,
                                              forward_loop2, reverse_loop2,
                                              component2, checked_looping_chain, is_looping_chain),
                    node_lengths_to_add});

    } else if (is_snarl(parent)) {
        size_t rank1, rank2; bool rev1, rev2;
        if (is_sentinel(child1)) {
            rank1 = starts_at(child1) == START ? 0 : 1;
            rev1 = false;
        } else {
            rank1 = get_cached_rank(cached_child1);
            rev1 = ends_at(child1) == END;
        }
        if (is_sentinel(child2)) {
            rank2 = starts_at(child2) == START ? 0 : 1;
            rev2 = false;
        } else {
            rank2 = get_cached_rank(cached_child2);
            rev2 = ends_at(child2) == END;
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

        if (SnarlTreeRecord(parent, &snarl_tree_records).get_record_type() == OVERSIZED_SNARL 
            && !(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) ) {
            //If this is an oversized snarl and we're looking for internal distances, then we didn't store the
            //distance and we have to find it using dijkstra's algorithm
            if (graph == nullptr) {
                cerr << "warning: trying to find the distance in an oversized snarl without a graph. Returning inf" << endl;
                return std::numeric_limits<size_t>::max();
            }
            handle_t handle1 = is_node(child1) ? get_handle(child1, graph) : get_handle(get_bound(child1, ends_at(child1) == END, false), graph); 
            handle_t handle2 = is_node(child2) ? get_handle(child2, graph) : get_handle(get_bound(child2, ends_at(child2) == END, false), graph);
            handle2 = graph->flip(handle2);

            size_t distance = std::numeric_limits<size_t>::max();
            handlegraph::algorithms::dijkstra(graph, handle1, [&](const handle_t& reached, size_t dist) {
                if (reached == handle2) {
                    //TODO: Also give up if the distance is too great
                    distance = dist;
                    return false;
                }
                return true;
            }, false);
            return distance;

            
        } else if (SnarlTreeRecord(parent, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL) {
            return SimpleSnarlRecord(parent, &snarl_tree_records).get_distance(rank1, rev1, rank2, rev2);
        } else {
           return SnarlRecord(parent, &snarl_tree_records).get_distance(rank1, rev1, rank2, rev2);
        }
    } else {
        throw runtime_error("error: Trying to find distance in the wrong type of handle");
    }
}
size_t SnarlDistanceIndex::distance_to_parent_bound(CachedNetHandle& cached_parent, bool to_start, CachedNetHandle& child, bool go_left, const HandleGraph* graph) const {

    //If the parent is a snarl, then the bound is actually the node and we want the sentinel
    CachedNetHandle parent_bound = is_snarl(cached_parent.net) 
                                 ? get_cached_net_handle(get_bound(cached_parent.net, !to_start, true), cached_parent.record_tag)
                                 : get_cached_bound(cached_parent, to_start);
    //The node length of the boundary node, only set for chains 
    size_t bound_length = !is_chain(cached_parent.net) ? 0 : 
        (to_start ? get_cached_start_bound_length(cached_parent) : 
                    get_cached_end_bound_length(cached_parent));

    if (is_chain(cached_parent.net) && !to_start){
        if(get_record_offset(get_cached_start_bound(cached_parent)) ==
                            get_record_offset(get_cached_end_bound(cached_parent))){
            //If this is a looping chain and we want the end 
            bound_length = 0;
        }
    }
    
    return sum({bound_length, 
                distance_in_parent(cached_parent, parent_bound, false, child, go_left, graph)});
}



pair<net_handle_t, bool> SnarlDistanceIndex::lowest_common_ancestor(const net_handle_t& net1, const net_handle_t& net2) const {
    net_handle_t parent1 = net1;
    net_handle_t parent2 = net2;

    std::unordered_set<net_handle_t> net1_ancestors;
    while (!is_root(parent1)){
        net1_ancestors.insert(canonical(parent1));
        parent1 = canonical(get_parent(parent1));
    }

    while (net1_ancestors.count(canonical(parent2)) == 0 && !is_root(parent2)){
        //Go up until the parent2 matches something in the ancestors of net1
        //This loop will end because everything is in the same root eventually
        parent2 = canonical(get_parent(parent2));
    }

    bool is_connected = true;
    if (is_root(parent2) && is_root(parent1)){
        size_t parent_record_offset1 = SnarlTreeRecord(parent1, &snarl_tree_records).get_parent_record_offset();
        size_t parent_record_offset2 = SnarlTreeRecord(parent2, &snarl_tree_records).get_parent_record_offset();
        if (parent_record_offset1 != parent_record_offset2) {
            is_connected = false;
        }
    }
    return make_pair(parent2, is_connected);
}

size_t SnarlDistanceIndex::minimum_distance(const handlegraph::nid_t id1, const bool rev1, const size_t offset1, const handlegraph::nid_t id2, const bool rev2, const size_t offset2, bool unoriented_distance, const HandleGraph* graph) const {


#ifdef debug_distances
        cerr << endl;
        cerr << "Find the minimum distance between " << id1 << " " <<rev1 <<" " << offset1 << " and " << id2 << " " << rev2 << " " << offset2 << endl;
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
    auto update_distances = [&](net_handle_t& net, net_handle_t& parent, size_t& dist_start, size_t& dist_end) {
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
                : sum({start_length, distance_in_parent(parent, start_bound, flip(net), graph)});
        size_t distance_start_end = start_bound == flip(net) ? 0 
                : sum({start_length, distance_in_parent(parent, start_bound, net, graph)});
        size_t distance_end_start = end_bound == net ? 0 
                : sum({end_length, distance_in_parent(parent, end_bound, flip(net), graph)});
        size_t distance_end_end = end_bound == flip(net) ? 0 
                : sum({end_length, distance_in_parent(parent, end_bound, net, graph)});

        size_t distance_start = dist_start;
        size_t distance_end = dist_end; 


        dist_start = std::min( sum({distance_start_start, distance_start}), 
                                    sum({distance_start_end , distance_end}));
        dist_end = std::min(sum({distance_end_start , distance_start}), 
                            sum({distance_end_end , distance_end}));
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
    net_handle_t common_ancestor = std::move(lowest_ancestor.first);

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


    /*
     * Walk up the snarl tree until net1 and net2 are children of the lowest common ancestor
     * Keep track of the distances to the ends of the net handles as we go
     */
 
    if (canonical(net1) == canonical(net2)){
        if (sum({distance_to_end1 , distance_to_start2}) > node_length(net1) && 
            sum({distance_to_end1 , distance_to_start2}) != std::numeric_limits<size_t>::max()) {
            //If the positions are on the same node and are pointing towards each other, then
            //check the distance between them in the node
            minimum_distance = minus(sum({distance_to_end1 , distance_to_start2}), node_length(net1));
        }
        if (sum({distance_to_start1 , distance_to_end2}) > node_length(net1) && 
            sum({distance_to_start1 , distance_to_end2}) != std::numeric_limits<size_t>::max()) {
            minimum_distance = std::min(minus(sum({distance_to_start1 , distance_to_end2}), node_length(net1)), minimum_distance);
        }
        common_ancestor = get_parent(net1);
    } else {

        //Get the distance from position 1 up to the ends of a child of the common ancestor
#ifdef debug_distances
        cerr << "Reaching the children of the lowest common ancestor for first position..." << endl;
#endif   
        while (canonical(get_parent(net1)) != canonical(common_ancestor)) {
            net_handle_t parent = get_parent(net1);
            update_distances(net1, parent, distance_to_start1, distance_to_end1);
            net1 = parent;
        }
#ifdef debug_distances
        cerr << "Reached node " << net_handle_as_string(net1) << " for position 1" << endl;
        cerr << "   with distances to ends " << distance_to_start1 << " and " << distance_to_end1 << endl;
        cerr << "Reaching the children of the lowest common ancestor for position 2..." << endl;
#endif   
        //And the same for position 2
        while (canonical(get_parent(net2)) != canonical(common_ancestor)) {
            net_handle_t parent = get_parent(net2);
            update_distances(net2, parent, distance_to_start2, distance_to_end2);
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

        //Find the minimum distance between the two children (net1 and net2)
        size_t distance_start_start = distance_in_parent(common_ancestor, flip(net1), flip(net2), graph);
        size_t distance_start_end = distance_in_parent(common_ancestor, flip(net1), net2, graph);
        size_t distance_end_start = distance_in_parent(common_ancestor, net1, flip(net2), graph);
        size_t distance_end_end = distance_in_parent(common_ancestor, net1, net2, graph);

        //And add those to the distances we've found to get the minimum distance between the positions
        minimum_distance = std::min(minimum_distance, 
                           std::min(sum({distance_start_start , distance_to_start1 , distance_to_start2}),
                           std::min(sum({distance_start_end , distance_to_start1 , distance_to_end2}),
                           std::min(sum({distance_end_start , distance_to_end1 , distance_to_start2}),
                                    sum({distance_end_end , distance_to_end1 , distance_to_end2})))));

#ifdef debug_distances
            cerr << "    Found distances between nodes: " << distance_start_start << " " << distance_start_end << " " << distance_end_start << " " << distance_end_end << endl;
            cerr << "  best distance is " << minimum_distance << endl;
#endif
        if (!is_root(common_ancestor)) {
            //Update the distances to reach the ends of the common ancestor
            update_distances(net1, common_ancestor, distance_to_start1, distance_to_end1);
            update_distances(net2, common_ancestor, distance_to_start2, distance_to_end2);

            //Update which net handles we're looking at
            net1 = common_ancestor;
            net2 = common_ancestor;
            common_ancestor = get_parent(common_ancestor);
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

    //minimum distance currently includes both positions
    return minimum_distance == std::numeric_limits<size_t>::max() ? std::numeric_limits<size_t>::max() : minimum_distance-1;



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


//TODO: This is kind of redundant with node_length 
size_t SnarlDistanceIndex::minimum_length(const net_handle_t& net) const {
    if (SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == TRIVIAL_SNARL || 
        SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_TRIVIAL_SNARL) {
        return TrivialSnarlRecord(get_record_offset(net), &snarl_tree_records).get_node_length(get_node_record_offset(net));
    } else if (SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == SIMPLE_SNARL || 
               SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_SIMPLE_SNARL) {
        if (is_snarl(net)) {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_min_length();
        } else {
            return SimpleSnarlRecord(net, &snarl_tree_records).get_node_length();
        }
    } else {
        return SnarlTreeRecord(net, &snarl_tree_records).get_min_length();
    }
}
nid_t SnarlDistanceIndex::node_id(const net_handle_t& net) const {
    if (is_node(net)) {
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
    } else if (record.get_record_type() == SIMPLE_SNARL || record.get_record_type() == DISTANCED_SIMPLE_SNARL) {
        return SimpleSnarlRecord(net, &snarl_tree_records).get_node_is_reversed();
    } else {
        return record.get_is_reversed_in_parent();
    }
}
net_handle_t SnarlDistanceIndex::get_node_net_handle(const nid_t id) const {
    RootRecord root_record (get_root(), &snarl_tree_records);
    size_t node_pointer_offset = get_node_pointer_offset(id, root_record.get_min_node_id(), root_record.get_connected_component_count());
    size_t record_offset = snarl_tree_records->at(node_pointer_offset);
    size_t node_record_offset = snarl_tree_records->at(node_pointer_offset+1);
    return get_net_handle(record_offset, START_END, NODE_HANDLE, node_record_offset); 
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
        size_t snarl_depth = get_depth(get_parent(get_net_handle(get_record_offset(net), START_END, SNARL_HANDLE, 1)));
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


bool SnarlDistanceIndex::has_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const {
    SnarlTreeRecord record(net, &snarl_tree_records);
    return record.has_connectivity(start, end);
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
size_t SnarlDistanceIndex::SnarlTreeRecord::get_min_length() const {
    record_t type = get_record_type();
    size_t val;
    if (type == DISTANCED_NODE ) {
        return (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        size_t last_node_offset = TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (last_node_offset*2) + 1);
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
        val = (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        size_t last_node_offset = TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (last_node_offset*2) + 1);
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        val = (*records)->at(record_offset + SNARL_MAX_LENGTH_OFFSET);
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
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == SIMPLE_SNARL
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
        return false;//TODO: I'm pretty sure a snarl is always pointing forwards in the chain
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return (*records)->at(record_offset + CHAIN_RANK_OFFSET) & 1;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
};

handlegraph::nid_t SnarlDistanceIndex::SnarlTreeRecord::get_start_id() const {
    record_t type = get_record_type();
    if (type == ROOT) {
        //TODO: Also not totally sure what this should do
        throw runtime_error("error: trying to get the start node of the root");
    } else if (type == NODE || type == DISTANCED_NODE) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the start of a node" << endl;
        return (*records)->at(record_offset + NODE_ID_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
             || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        //To get the start node of a snarl, get the thing to the left of it in the chain
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle(record_offset, START_END, SNARL_HANDLE, node_offset), true);
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
        //TODO: Also not totally sure what this should do
        throw runtime_error("error: trying to get the start node of the root");
    } else if (type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the start of a node" << endl;
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
             || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        //Get the next node in the chain (going left)
        //The handle will be pointing in the direction we just moved, so if it is going START_END, then it is reversed
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle(record_offset, START_END, SNARL_HANDLE, node_offset), true);
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
        //TODO: Also not totally sure what this should do
        throw runtime_error("error: trying to get the end node of the root");
    } else if (type == NODE || type == DISTANCED_NODE ) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the end of a node" << endl;
        //TODO: Put this in its own function? Also double check for off by ones
        //Offset of the start of the node vector
        return (*records)->at(record_offset + NODE_ID_OFFSET);
    } else if (type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        size_t last_node_offset = TrivialSnarlRecord(record_offset, records).get_node_count()-1;
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (last_node_offset*2));
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        //For a snarl, walk right in the chain
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle(record_offset, START_END, SNARL_HANDLE, node_offset), false);
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
        //TODO: Also not totally sure what this should do
        throw runtime_error("error: trying to get the end node of the root");
    } else if (type == NODE || type == DISTANCED_NODE || type == TRIVIAL_SNARL || type == DISTANCED_TRIVIAL_SNARL) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the end of a node" << endl;
        //TODO: Put this in its own function? Also double check for off by ones
        //Offset of the start of the node vector
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL)  {
        ChainRecord parent_record (get_parent_record_offset(), records);
        size_t node_offset = (type == SIMPLE_SNARL || type == DISTANCED_SIMPLE_SNARL) ? 1 : 0;
        //Get the next node in the chain (going right)
        net_handle_t next_node_in_chain = parent_record.get_next_child(get_net_handle(record_offset, START_END, SNARL_HANDLE, node_offset), false);
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

size_t SnarlDistanceIndex::SnarlTreeRecord::get_offset_from_id (const handlegraph::nid_t id) const {

    //Get the pointer to the pointer to the node record
    size_t offset = SnarlDistanceIndex::get_node_pointer_offset(id, 
            (*records)->at(MIN_NODE_ID_OFFSET),
            (*records)->at(COMPONENT_COUNT_OFFSET));
    
    return (*records)->at(offset);

}


bool SnarlDistanceIndex::SnarlTreeRecord::has_connectivity(endpoint_t start, endpoint_t end){
    return has_connectivity(endpoints_to_connectivity(start, end));
}

bool SnarlDistanceIndex::has_external_connectivity(const size_t tag, endpoint_t start, endpoint_t end) const {
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

SnarlDistanceIndex::record_t SnarlDistanceIndex::SnarlTreeRecordConstructor::get_record_type() const {
    return static_cast<record_t>((*records)->at(record_offset) >> 9);
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_start_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_start_connected" << endl;
#endif    

    (*records)->at(record_offset) = (*records)->at(record_offset) | 32;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 16;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set start_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 8;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set end_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 4;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set end_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 2;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_tip_tip_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set tpi_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 1;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_start_end_connected() {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally start_end connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 64;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_start_start_connected() const {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally start_start connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 128;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_end_end_connected() const {
#ifdef debug_distance_indexing
    cerr << record_offset << " set externally end_end connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 256;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_record_type(record_t type) {
    assert((*records)->at(record_offset) == 0);
    (*records)->at(record_offset) = ((static_cast<size_t>(type) << 9) | ((*records)->at(record_offset) & 511));
}


void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_min_length(size_t length) {
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
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_max_length(size_t length) {
    record_t type = get_record_type();
    size_t offset;
    if (type == DISTANCED_NODE) {
        offset = record_offset + NODE_LENGTH_OFFSET;
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

void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_rank_in_parent(size_t rank) {
    record_t type = get_record_type();
    size_t offset;
    if (type == NODE || type == DISTANCED_NODE) {
        offset = record_offset + NODE_RANK_OFFSET;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL || type == SIMPLE_SNARL
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
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_is_reversed_in_parent(bool rev) {
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
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_parent_record_offset(size_t pointer){
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
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_node(handlegraph::nid_t id, bool rev) {
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
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_node(handlegraph::nid_t id, bool rev) const {
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



bool SnarlDistanceIndex::RootRecord::for_each_child(const std::function<bool(const handlegraph::net_handle_t&)>& iteratee) const {
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
                child_handle = get_net_handle(child_offset, START_END, CHAIN_HANDLE);
            } else {
                child_handle =  get_net_handle(child_offset, START_END, type);
            }
            if (!iteratee(child_handle)) {
                return false;
            }
        }
    }
    return true;
}

void SnarlDistanceIndex::RootRecordConstructor::set_connected_component_count(size_t connected_component_count) {
#ifdef debug_distance_indexing
    cerr << record_offset+COMPONENT_COUNT_OFFSET << " set connected component to be " << connected_component_count << endl;
    assert((*records)->at(record_offset+COMPONENT_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset+COMPONENT_COUNT_OFFSET)=connected_component_count;
}
void SnarlDistanceIndex::RootRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset+NODE_COUNT_OFFSET << " set node count to be " << node_count << endl;
    assert((*records)->at(record_offset+NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset+NODE_COUNT_OFFSET)=node_count;
}
void SnarlDistanceIndex::RootRecordConstructor::set_max_tree_depth(size_t tree_depth) {
#ifdef debug_distance_indexing
    cerr << record_offset+MAX_TREE_DEPTH_OFFSET << " set max tree depth to be " << tree_depth << endl;
    assert((*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET) == 0);
#endif

    (*records)->at(record_offset+MAX_TREE_DEPTH_OFFSET)=tree_depth;
}
void SnarlDistanceIndex::RootRecordConstructor::set_min_node_id(handlegraph::nid_t node_id) {
#ifdef debug_distance_indexing
    cerr << record_offset+MIN_NODE_ID_OFFSET << " set min node id to be " << node_id << endl;
    assert((*records)->at(record_offset+MIN_NODE_ID_OFFSET) == 0);
#endif

    (*records)->at(record_offset+MIN_NODE_ID_OFFSET)=node_id;
}
void SnarlDistanceIndex::RootRecordConstructor::add_component(size_t index, size_t offset) {
#ifdef debug_distance_indexing
    cerr << record_offset+ROOT_RECORD_SIZE+index << " set new component " << offset << endl;
    cerr << "Old value " << (*records)->at(record_offset+ROOT_RECORD_SIZE+index) << endl;
    assert((*records)->at(record_offset+ROOT_RECORD_SIZE+index) == 0);
    assert(index < get_connected_component_count());
#endif

    (*records)->at(record_offset+ROOT_RECORD_SIZE+index) = offset;

}


size_t SnarlDistanceIndex::SnarlRecord::distance_vector_size(record_t type, size_t node_count) {
    if (type == SNARL || type == ROOT_SNARL){
        //For a normal snarl, its just the record size and the pointers to children
        return 0;
    } else if (type == DISTANCED_SNARL) {
        //For a normal min distance snarl, record size and the pointers to children, and
        //matrix of distances
        size_t node_side_count = node_count * 2 + 2;
        size_t vector_size =  (((node_side_count+1)*node_side_count) / 2);
        return vector_size;
    } else if (type ==  OVERSIZED_SNARL){
        //For a large min_distance snarl, record the side, pointers to children, and just
        //the min distances from each node side to the two boundary nodes
        size_t node_side_count = node_count * 2 + 2;
        return  (node_side_count * 2);
    } else if (type == DISTANCED_ROOT_SNARL) {
        size_t node_side_count = node_count * 2;
        return  (((node_side_count+1)*node_side_count) / 2);
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


size_t SnarlDistanceIndex::SnarlRecord::get_distance_vector_offset(size_t rank1, bool right_side1, size_t rank2,
        bool right_side2, size_t node_count, record_t type) {

    //how many node sides in this snarl
    size_t node_side_count = type == DISTANCED_ROOT_SNARL ? node_count * 2 : node_count * 2 + 2;

    //make sure we're looking at the correct node side
    //If this is the start or end node, then we don't adjust based on orientation
    //because we only care about the inner sides. If this is a root snarl, then
    //there is no start or end node and the ranks 0 and 1 are not special
     if (type == DISTANCED_ROOT_SNARL) {
        rank1 = rank1 * 2;
        if (right_side1) {
            rank1 += 1;
        }
     } else if (rank1 != 0 && rank1 != 1) {
        rank1 = (rank1-1) * 2;
        if (right_side1) {
            rank1 += 1;
        }
    }
    if (type == DISTANCED_ROOT_SNARL) {
        rank2 = rank2 * 2;
        if (right_side2) {
            rank2 += 1;
        }
    } else if (rank2 != 0 && rank2 != 1) {
        rank2 = (rank2-1) * 2;
        if (right_side2) {
            rank2 += 1;
        }
    }

    //reverse order of ranks if necessary
    if (rank1 > rank2) {
        size_t tmp = rank1;
        rank1 = rank2;
        rank2 = tmp;
    }

    if (type == SNARL || type == ROOT_SNARL) {
        throw runtime_error("error: trying to access distance in a distanceless snarl tree");
    } else if (type == DISTANCED_SNARL || type == DISTANCED_ROOT_SNARL) {
        //normal distance index
        size_t k = node_side_count-rank1;
        return (((node_side_count+1) * node_side_count)/2) - (((k+1)*k) / 2) + rank2 - rank1;
    } else if (type ==  OVERSIZED_SNARL) {
        //abbreviated distance index storing only the distance from each node side to the
        //start and end
        if (rank1 == 0) {
            return rank2;
        } else if (rank1 == 1) {
            return node_side_count + rank2;
        } else {
            throw runtime_error("error: trying to access distance in an oversized snarl");
        }
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
        net_handle_record_t type = SnarlTreeRecord(child_offset, records).get_record_handle_type();
        assert(type == NODE_HANDLE || type == CHAIN_HANDLE);
        net_handle_t child_handle =  get_net_handle (child_offset, START_END, CHAIN_HANDLE);
        bool result = iteratee(child_handle);
        if (result == false) {
            return false;
        }
    }
    return true;
}

void SnarlDistanceIndex::SnarlRecordConstructor::set_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2, size_t distance) {

#ifdef debug_distance_indexing 
    //This distance hasn't been set yet (or it's equal to what we're setting)
    assert((get_distance(rank1, right_side1, rank2, right_side2) == std::numeric_limits<size_t>::max() ||
           get_distance(rank1, right_side1, rank2, right_side2) == distance));
#endif

    //Offset of this particular distance in the distance vector
    size_t distance_vector_offset = get_distance_vector_offset(rank1, right_side1, rank2, right_side2);
    assert(distance_vector_offset <= distance_vector_size(get_record_type(), get_node_count())); 

    //Value we actually want to save
    size_t val = distance == std::numeric_limits<size_t>::max() ? 0 : distance+1;

    (*records)->at(distance_vector_offset+record_offset+SNARL_RECORD_SIZE) = val;
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const {
    if (get_record_type() == OVERSIZED_SNARL) {
        assert(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1);
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
void SnarlDistanceIndex::SnarlRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset + SNARL_NODE_COUNT_OFFSET << " set snarl node count " << node_count << endl;
    assert(node_count > 0);//TODO: Don't bother making a record for trivial snarls
    assert((*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET) = node_count;
}

size_t SnarlDistanceIndex::SnarlRecord::get_child_record_pointer() const {
    return (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) ;
}

void SnarlDistanceIndex::SnarlRecordConstructor::set_child_record_pointer(size_t pointer) {
#ifdef debug_distance_indexing
    cerr << (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) << endl;
    assert((*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) == 0);
#endif

    (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) = pointer;
}

//Add a reference to a child of this snarl. Assumes that the index is completed up
//to here
void SnarlDistanceIndex::SnarlRecordConstructor::add_child(size_t pointer){
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

size_t SnarlDistanceIndex::SimpleSnarlRecord::record_size() {
    
    return SIMPLE_SNARL_RECORD_SIZE + get_node_count()*2;
}
size_t SnarlDistanceIndex::SimpleSnarlRecord::get_node_count() const {

    return (*records)->at(record_offset + SIMPLE_SNARL_NODE_COUNT_AND_LENGTHS_OFFSET) >> 22;
}
void SnarlDistanceIndex::SimpleSnarlRecordConstructor::set_node_count(size_t node_count) {
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
    if (rank1 > rank2) {
        //Order the nodes
        size_t tmp = rank1; bool tmp_rev = right_side1;
        rank1 = rank2; right_side1 = right_side2;
        rank2 = tmp; right_side2 = tmp_rev;
    }
    if (rank1 == 0 && rank2 == 1) {
        return get_min_length();
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
bool SnarlDistanceIndex::SimpleSnarlRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {
    size_t node_count = get_node_count();
    for (size_t i = 0 ; i < node_count ; i++) {
        net_handle_t child_handle = get_net_handle(record_offset, START_END, CHAIN_HANDLE, i+2);
        bool result = iteratee(child_handle);
        if (result == false) {
            return false;
        }
    }
    return true;
}
void SnarlDistanceIndex::SimpleSnarlRecordConstructor::add_child(size_t rank, nid_t node_id, size_t node_length, bool is_reversed) {
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
nid_t SnarlDistanceIndex::NodeRecord::get_node_id() const {
    return (*records)->at(record_offset + NODE_ID_OFFSET);
}
size_t SnarlDistanceIndex::NodeRecord::get_node_length() const {
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


size_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_count() const {
    return (*records)->at(record_offset+TRIVIAL_SNARL_NODE_COUNT_OFFSET);
}
tuple<size_t, size_t, size_t, size_t> SnarlDistanceIndex::TrivialSnarlRecord::get_chain_values(size_t node_rank) const {
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
        prefix_sum =  sum({prefix_sum,  
               (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE+ (node_rank*2)-1)});
    } 

    /*Get the forward loop value
     */

    forward_loop = forward_loop == 0 ? std::numeric_limits<size_t>::max() : forward_loop - 1;
        
    //How many nodes in the snarl
    size_t last_node = get_node_count()-1; 
    //The right prefix sum of the last node, which is the length of the snarl
    size_t snarl_length = (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (last_node*2) + 1); 

    //The distance from the right side of this node to the end of the trivial snarl
    //the length of the snarl - the right prefix sum of the node
    size_t right_offset = snarl_length - (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) + 1);

    forward_loop = sum({forward_loop, right_offset*2});

    /* Get the reverse loop value
     */
    reverse_loop = reverse_loop == 0 ? std::numeric_limits<size_t>::max() : reverse_loop - 1;
        
    //The offset of the left side of this node in the trivial snarl
    size_t left_offset = node_rank == 0 ? 0 : 
                          (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) - 1);

    reverse_loop = sum({reverse_loop, left_offset*2});



    return make_tuple(prefix_sum, forward_loop, reverse_loop, component);

}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_prefix_sum(size_t node_rank) const {
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t prefix_sum = (*records)->at(record_offset+TRIVIAL_SNARL_PREFIX_SUM_OFFSET);  
    prefix_sum = prefix_sum == 0 ? std::numeric_limits<size_t>::max() : prefix_sum - 1;
    if (node_rank != 0) {
        return sum({prefix_sum,  
               (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE+ (node_rank*2)-1)});
    } else {
        return prefix_sum;
    }
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_forward_loop(size_t node_rank) const {
#ifdef debug_distances
    assert(get_record_type()== TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t forward_loop = (*records)->at(record_offset+TRIVIAL_SNARL_FORWARD_LOOP_OFFSET);  
    forward_loop = forward_loop == 0 ? std::numeric_limits<size_t>::max() : forward_loop - 1;
        
    //How many nodes in the snarl
    size_t last_node = get_node_count()-1; 
    //The right prefix sum of the last node, which is the length of the snarl
    size_t snarl_length = (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (last_node*2) + 1); 

    //The distance from the right side of this node to the end of the trivial snarl
    //the length of the snarl - the right prefix sum of the node
    size_t right_offset = snarl_length - (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) + 1);

    return sum({forward_loop, right_offset*2});
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_reverse_loop(size_t node_rank) const {
#ifdef debug_distances
    assert(get_record_type() == TRIVIAL_SNARL || get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    size_t reverse_loop = (*records)->at(record_offset+TRIVIAL_SNARL_REVERSE_LOOP_OFFSET);  
    reverse_loop = reverse_loop == 0 ? std::numeric_limits<size_t>::max() : reverse_loop - 1;
        
    //The offset of the left side of this node in the trivial snarl
    size_t left_offset = node_rank == 0 ? 0 : 
                          (*records)->at(record_offset+TRIVIAL_SNARL_RECORD_SIZE + (node_rank*2) - 1);

    return sum({reverse_loop, left_offset*2});
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_chain_component(size_t node_rank, bool get_end) const {
    if (!get_end && node_rank == 0 && record_offset == ChainRecord(get_parent_record_offset(), records).get_first_node_offset()){
        //If we want the component of the first node in the chain
        return 0;
    }
    return (*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET);
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_length(size_t node_rank) const {
    if (node_rank == 0) {
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)+1);
    } else {
        return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)+1) - 
               (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)-1);
    }
}
size_t SnarlDistanceIndex::TrivialSnarlRecord::get_rank_in_parent(size_t node_rank) const {
    return record_offset+node_rank;
}
nid_t SnarlDistanceIndex::TrivialSnarlRecord::get_node_id(size_t node_rank) const {
    return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)) >> 1;
}
bool SnarlDistanceIndex::TrivialSnarlRecord::get_is_reversed_in_parent(size_t node_rank) const {
    return (*records)->at(record_offset + TRIVIAL_SNARL_RECORD_SIZE + (2*node_rank)) & 1;
}

void SnarlDistanceIndex::NodeRecordConstructor::set_node_id(nid_t value) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_ID_OFFSET << " set node id " << value << endl;
    assert((*records)->at(record_offset + NODE_ID_OFFSET) == 0);
#endif

    (*records)->at(record_offset + NODE_ID_OFFSET) = value;
}
void SnarlDistanceIndex::NodeRecordConstructor::set_node_length(size_t length) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_LENGTH_OFFSET << " set node length " << length << endl;
    assert((*records)->at(record_offset + NODE_LENGTH_OFFSET) == 0);
#endif
    (*records)->at(record_offset + NODE_LENGTH_OFFSET) = length;
}
void SnarlDistanceIndex::NodeRecordConstructor::set_rank_in_parent(size_t value) {
#ifdef debug_distance_indexing
    cerr <<record_offset + NODE_RANK_OFFSET << " set node rank " << value << endl;
    assert((*records)->at(record_offset + NODE_RANK_OFFSET) == 0);
#endif
    (*records)->at(record_offset + NODE_RANK_OFFSET) = value;
}

void SnarlDistanceIndex::TrivialSnarlRecordConstructor::set_node_count(size_t value) const {
    (*records)->at(record_offset+TRIVIAL_SNARL_NODE_COUNT_OFFSET)=value;
}
void SnarlDistanceIndex::TrivialSnarlRecordConstructor::set_prefix_sum(size_t value) const {
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET << " set node prefix sum " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET) == 0);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    (*records)->at(record_offset + TRIVIAL_SNARL_PREFIX_SUM_OFFSET) = value;
}
void SnarlDistanceIndex::TrivialSnarlRecordConstructor::set_forward_loop(size_t value) const { 
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_FORWARD_LOOP_OFFSET << " set node forward loop " << value << endl;
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    (*records)->at(record_offset + TRIVIAL_SNARL_FORWARD_LOOP_OFFSET) = value;
}
void SnarlDistanceIndex::TrivialSnarlRecordConstructor::set_reverse_loop(size_t value) const {
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET << " set node reverse loop " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET) == 0);
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    value = value == std::numeric_limits<size_t>::max() ? 0 : value +1;
    //Only do this for the first node
    (*records)->at(record_offset + TRIVIAL_SNARL_REVERSE_LOOP_OFFSET) = value;
    
}
void SnarlDistanceIndex::TrivialSnarlRecordConstructor::set_chain_component(size_t value) const {
#ifdef debug_distance_indexing
    cerr <<record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET << " set node component " << value << endl;
    assert((*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET) == 0);
    assert(get_record_type() == DISTANCED_TRIVIAL_SNARL);
#endif
    (*records)->at(record_offset + TRIVIAL_SNARL_COMPONENT_OFFSET) = value;
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
    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1,
    size_t rank2, bool left_side2, size_t node_length2,
    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2, bool checked_loop, bool is_looping_chain) const { 

    //If 1 comes after 2, swap them
    if (rank1 > rank2) {
        size_t tmp_rank=rank1; bool tmp_side=left_side1; size_t tmp_len=node_length1;
        size_t tmp_pre=prefix_sum1; size_t tmp_fd = forward_loop1; size_t tmp_rev = reverse_loop1; size_t tmp_comp = component1;
        rank1 = rank2; left_side1 = left_side2; node_length1 = node_length2;
        prefix_sum1 = prefix_sum2; forward_loop1 = forward_loop2; reverse_loop1 = reverse_loop2; component1 = component2;
        rank2 = tmp_rank; left_side2 = tmp_side; node_length2 = tmp_len;
        prefix_sum2 = tmp_pre; forward_loop2 = tmp_fd; reverse_loop2 = tmp_rev; component2 = tmp_comp;
    } 
    is_looping_chain = checked_loop ? is_looping_chain : get_start_id() == get_end_id();
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
                            prefix_sum1, forward_loop1, reverse_loop1, component1,
                            rank2, left_side2, node_length2,
                            prefix_sum2, forward_loop2, reverse_loop2, component2);
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
            distance = sum({forward_loop1,reverse_loop2,node_length1});
        } else {
            distance = minus(prefix_sum2 - prefix_sum1, node_length1);
        }
    } else if (!left_side1 && !left_side2) {
        //Right side of 1 and right side of 2
        if (rank1 == rank2) {
            distance = forward_loop2;

        } else {
            distance = minus( sum({prefix_sum2 - prefix_sum1, node_length2, forward_loop2}),
                         node_length1);
        }
    } else if (left_side1 && left_side2) {
        //Left side of 1 and left side of 2
        if (rank1 == rank2) {
            distance = reverse_loop1;

        } else {
            distance = sum({prefix_sum2 - prefix_sum1, reverse_loop1});
        }
    } else {
        //Left side of 1 and right side of 2
        distance = sum({prefix_sum2 - prefix_sum1, reverse_loop1,
                        forward_loop2, node_length2});

    }
    if (is_looping_chain) {
        distance = std::min(distance, get_distance_taking_chain_loop(rank1, left_side1, node_length1, 
                            prefix_sum1, forward_loop1, reverse_loop1, component1,
                            rank2, left_side2, node_length2,
                            prefix_sum2, forward_loop2, reverse_loop2, component2));
    }
    return distance;
}


size_t SnarlDistanceIndex::ChainRecord::get_distance_taking_chain_loop(size_t rank1, bool left_side1, size_t node_length1, 
    size_t prefix_sum1, size_t forward_loop1, size_t reverse_loop1, size_t component1,
    size_t rank2, bool left_side2, size_t node_length2,
    size_t prefix_sum2, size_t forward_loop2, size_t reverse_loop2, size_t component2) const {
    //This is only called by get_distance, so the nodes should be ordered
#ifdef debug_distances
    assert (rank1 <= rank2);
    assert(get_start_id() == get_end_id());
#endif

    /*Note: Because we assume that the nodes are ordered and that we want to take the loop in the chain,
     * we leave the start node going left (either by going left or taking a loop to the right), and
     * enter the end node from the right (from the right or from the left and then looping)
     */

    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    } else if (get_record_type() == MULTICOMPONENT_CHAIN) {
        size_t last_component = TrivialSnarlRecord(get_first_node_offset(), records).get_chain_component(0,true);
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
        if (end_at_right_of_first || start_at_left_of_last) {
            return std::numeric_limits<size_t>::max();
        }
    }


    size_t distance;

    if (!left_side1 && left_side2) {
        //Right of 1 and left of 2, so a simple forward traversal of the chain
        //loop forward from the first node, from the start of the chain to the first
        //node, from the end of the node to the second node, and the reverse loop of the second
        distance = sum({forward_loop1, node_length1, prefix_sum1,
                        minus(get_min_length(), prefix_sum2),
                        reverse_loop2});
    } else if (!left_side1 && !left_side2) {
        //Right side of 1 and right side of 2

        //Check distance for taking loop in chain: loop forward from the first node, from the start of the
        //chain to the first node, from the end of the node to the second node
        distance = sum({forward_loop1, node_length1, prefix_sum1,
                        minus(minus(get_min_length(), prefix_sum2), node_length2)});
    } else if (left_side1 && left_side2) {
        //Left side of 1 and left side of 2

        //from the first node left to the start, around the
        //chain loop, then the reverse loop of the second node
        //This assumes that the length of the chain only includes the start/end node's length once,
        //which it does but might change
        distance = sum({prefix_sum1,
                        minus(get_min_length(), prefix_sum2),reverse_loop2});
    } else {
        //Left side of 1 and right side of 2

        //Check the distance going backwards around the chain
        distance = sum({prefix_sum1,
                        minus(minus(get_min_length(), prefix_sum2),node_length2)});
    }
    return distance;
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
            return get_net_handle(get_record_offset(net_handle), connectivity, 
                                  get_handle_type(net_handle), get_node_record_offset(net_handle)-1);

        } else if (!go_left && get_node_record_offset(net_handle) != 
                      TrivialSnarlRecord(get_record_offset(net_handle), records).get_node_count()-1){

            //If we are going right and this is not the last node in the trivial snarl
            //then keep everything the same but increment the node record offset by one
            //to move one node right in the trivial snarl
            return get_net_handle(get_record_offset(net_handle), get_connectivity(net_handle), 
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
            
            return get_net_handle(std::get<0>(last_child), 
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
            return get_net_handle(get_first_node_offset(), 
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
        return get_net_handle(next_pointer, (go_left ? END_START : START_END), SNARL_HANDLE); 
    } else if (SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == SIMPLE_SNARL || 
        SnarlDistanceIndex::get_record_type((*records)->at(next_pointer)) == DISTANCED_SIMPLE_SNARL) {
        //If the next thing is a snarl, then just return the snarl going in the direction we just moved in
        return get_net_handle(next_pointer, (go_left ? END_START : START_END), SNARL_HANDLE, 1); 
    } else{

        //Otherwise, this is a node (trivial snarl of nodes) handle
        TrivialSnarlRecord next_record(next_pointer, records);

        //The node in the trivial snarl is the last one if we're going left, the first one if we're going right
        size_t node_offset = go_left ? next_record.get_node_count()-1 : 0;

        //Is the next node (the first thing in the snarl if we're going right, last thing if we're going left) reversed
        bool next_is_reversed_in_parent = next_record.get_is_reversed_in_parent( node_offset);

        //Get the direction of the next handle
        //Handle will point in whichever direction we moved in
        //TODO: Should it?
        connectivity_t connectivity = go_left == next_is_reversed_in_parent ? START_END : END_START;

        return get_net_handle(next_pointer, connectivity, NODE_HANDLE, node_offset);
    }
}

bool SnarlDistanceIndex::ChainRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {

    if (get_record_handle_type() == NODE_HANDLE) {
        //If this is a node pretending to be a chain, just do it for the node
        return iteratee(get_net_handle(record_offset, START_END, NODE_HANDLE));
    }


    net_handle_t first_child = get_net_handle(get_first_node_offset(), START_END, NODE_HANDLE, 0);
    net_handle_t current_child = get_net_handle(get_first_node_offset(), START_END, NODE_HANDLE, 0);
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


void SnarlDistanceIndex::ChainRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_NODE_COUNT_OFFSET << " set chain node count " << node_count << endl;
    assert((*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET) = node_count;
}
size_t SnarlDistanceIndex::ChainRecord::get_depth() const {
    return (*records)->at(record_offset+CHAIN_DEPTH_OFFSET) ;
}
void SnarlDistanceIndex::ChainRecordConstructor::set_depth(size_t depth) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_DEPTH_OFFSET << " set chain depth " << depth << endl;
    assert((*records)->at(record_offset + CHAIN_DEPTH_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_DEPTH_OFFSET) = depth;
}

//The offset of the last child, if it is a snarl, and if it can loop
void SnarlDistanceIndex::ChainRecordConstructor::set_last_child_offset(size_t offset, bool is_snarl, bool loopable) {
#ifdef debug_distance_indexing
    cerr << record_offset + CHAIN_LAST_CHILD_OFFSET << " set chain last child offset " << offset << endl;
    assert((*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET) == 0);
#endif

    (*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET) = ((offset << 2) | (is_snarl<<1)) | loopable;
}

//Add a snarl to the end of the chain and return a SnarlRecordConstructor pointing to it
SnarlDistanceIndex::SnarlRecordConstructor SnarlDistanceIndex::ChainRecordConstructor::add_snarl(size_t snarl_size, record_t type, size_t previous_child_offset) {

    size_t snarl_record_size = SnarlRecord::record_size(type, snarl_size);
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child snarl length to the end of the array " << endl;
    assert(SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== DISTANCED_TRIVIAL_SNARL);
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
    SnarlRecordConstructor snarl_record(snarl_size, records, type);
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
//Add a simple snarl to the end of the chain and return a SnarlRecordConstructor pointing to it
SnarlDistanceIndex::SimpleSnarlRecordConstructor SnarlDistanceIndex::ChainRecordConstructor::add_simple_snarl(size_t snarl_size, record_t type, size_t previous_child_offset) {

    size_t snarl_record_size = SIMPLE_SNARL_RECORD_SIZE + 2*snarl_size;
#ifdef debug_distance_indexing
    cerr << (*records)->size() << " Adding child snarl length to the end of the array " << endl;
    assert(SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset))== DISTANCED_TRIVIAL_SNARL);
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
    SimpleSnarlRecordConstructor snarl_record(snarl_size, records, type);
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


size_t SnarlDistanceIndex::ChainRecordConstructor::add_node(nid_t node_id, size_t node_length, bool is_reversed_in_parent,
    size_t prefix_sum, size_t forward_loop, size_t reverse_loop, size_t component, size_t previous_child_offset) {
#ifdef debug_distance_indexing
    cerr << "Adding new node to chain, with previous child at offset " << previous_child_offset << endl;
#endif
    if (SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_TRIVIAL_SNARL
            && TrivialSnarlRecord(previous_child_offset, records).get_node_count() == MAX_TRIVIAL_SNARL_NODE_COUNT) {
        //If the last thing was a trivial snarl and it is full, then finish it off
        (*records)->at(previous_child_offset-1) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
        size_t current_size = (*records)->size();
        (*records)->resize(current_size+1);
        (*records)->at(current_size) = TrivialSnarlRecord(previous_child_offset, records).get_record_size();
    }

    if (previous_child_offset == 0
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == OVERSIZED_SNARL 
            || SnarlDistanceIndex::get_record_type((*records)->at(previous_child_offset)) == DISTANCED_SIMPLE_SNARL 
            || TrivialSnarlRecord(previous_child_offset, records).get_node_count() == MAX_TRIVIAL_SNARL_NODE_COUNT) {
        //If the last thing was a snarl or nothing (previous_child_offset == 0, meaning that this is the 
        //first thing in the chain), then create a new trivial snarl
        size_t start_i = (*records)->size();
        (*records)->resize(start_i+1);
        (*records)->at(start_i) = 0; //Add a place for the size of this trivial snarl, to be filled in later

        //Set the node to point to the correct node record
        //TODO: Shouldn't this have been done in the node constructor?
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))) 
                    = (*records)->size();
        (*records)->at(get_node_pointer_offset(node_id,
                                              (*records)->at(MIN_NODE_ID_OFFSET),
                                              (*records)->at(COMPONENT_COUNT_OFFSET))+1) 
                    = 0;
    

        //Create a new trivial snarl record
        TrivialSnarlRecordConstructor trivial_snarl_record((*records)->size(), DISTANCED_TRIVIAL_SNARL, records, true);
        trivial_snarl_record.set_parent_record_offset(record_offset);
        trivial_snarl_record.set_node_count(1);
        trivial_snarl_record.set_prefix_sum(prefix_sum);
        trivial_snarl_record.set_forward_loop(forward_loop);
        trivial_snarl_record.set_reverse_loop(reverse_loop);
        trivial_snarl_record.set_chain_component(component);

        start_i = (*records)->size(); 
        (*records)->resize(start_i+2);
        (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
        (*records)->at(start_i+1) = node_length;
#ifdef debug_distance_indexing
        cerr << start_i << " Node in trivial snarl " <<  ((node_id<<1) | is_reversed_in_parent) << endl;
        cerr << start_i +1 << " prefix sum in trivial snarl " <<  ((*records)->at(start_i-1) + node_length) << endl;
#endif
#ifdef count_allocations
        cerr << "trivial_snarl\t2\t" <<  (*records)->size() << endl;
#endif

        //Return the offset offset of the new trivial snarl record
        return trivial_snarl_record.record_offset;
    } else {
        //The last thing on the chain is a trivial snarl that we are adding to the end of
        //
        //Add the node and its right prefix sum
        size_t start_i = (*records)->size(); 
        (*records)->resize(start_i+2);
        (*records)->at(start_i) = (node_id<<1) | is_reversed_in_parent;
        (*records)->at(start_i+1) = (*records)->at(start_i-1) + node_length;
#ifdef debug_distance_indexing
        cerr << start_i << " Node in trivial snarl " <<  ((node_id<<1) | is_reversed_in_parent) << endl;
        cerr << start_i +1 << " prefix sum in trivial snarl " <<  ((*records)->at(start_i-1) + node_length) << endl;
#endif
#ifdef count_allocations
    cerr << "trivial_snarl\t2\t" <<  (*records)->size() << endl;
#endif

        TrivialSnarlRecordConstructor trivial_snarl_record(previous_child_offset, DISTANCED_TRIVIAL_SNARL, records, false);

        //Increment the node count
        size_t old_node_count = trivial_snarl_record.get_node_count();
        trivial_snarl_record.set_node_count(old_node_count+1);
        //The forward loop value changes
        trivial_snarl_record.set_forward_loop(forward_loop);

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
        return "root"; 
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
            child_count = RootRecord(net, &snarl_tree_records).get_connected_component_count();
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

    //TODO: Make sure not to include trivial chains
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
    size_t new_width = std::max(std::max(max_dist_bit_width, max_address_bit_width), (size_t)26); //26 is the size for the simple snarl node count + node lengths
    snarl_tree_records->width(new_width);//TODO: Fix this

    cerr << "Max index size " << maximum_index_size << endl;
    snarl_tree_records->reserve(maximum_index_size);

    /*Allocate memory for the root and the nodes */
    RootRecordConstructor root_record(0, total_component_count, max_node_id-min_node_id+1, maximum_tree_depth, min_node_id, &snarl_tree_records);
#ifdef debug_distance_indexing
    cerr << "  Root record had length " << snarl_tree_records->size() << endl;
#endif

    /*Now go through each of the chain/snarl indexes and copy them into snarl_tree_records
     * Walk down the snarl tree and fill in children
     */
    //TODO: For now I'm assuming that I'm including distances
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
        //TODO: this is copying the components but it shouldn't be too big so I think it's fine
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

                    ChainRecordConstructor chain_record_constructor;

                    if (temp_chain_record.chain_components.back() == 0 || snarl_size_limit == 0) {
                        record_t record_type = snarl_size_limit == 0 ? CHAIN : DISTANCED_CHAIN;
                        chain_record_constructor = ChainRecordConstructor(snarl_tree_records->size(), record_type,
                                                               temp_chain_record.prefix_sum.size(), &snarl_tree_records);
                        chain_record_constructor.set_start_end_connected();
                    } else {
                        chain_record_constructor = ChainRecordConstructor(snarl_tree_records->size(), MULTICOMPONENT_CHAIN,
                                                               temp_chain_record.prefix_sum.size(), &snarl_tree_records);
                    }
                    chain_record_constructor.set_parent_record_offset(
                            record_to_offset[make_pair(temp_index_i, temp_chain_record.parent)]);//TODO: Get the actual parent

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


                    chain_record_constructor.set_min_length(temp_chain_record.min_length);
                    chain_record_constructor.set_max_length(temp_chain_record.max_length);
                    chain_record_constructor.set_rank_in_parent(temp_chain_record.rank_in_parent);
                    chain_record_constructor.set_start_node(temp_chain_record.start_node_id, temp_chain_record.start_node_rev);
                    chain_record_constructor.set_end_node(temp_chain_record.end_node_id, temp_chain_record.end_node_rev);


                    size_t chain_node_i = 0; //How far along the chain are we?
                    pair<size_t, bool> last_child_offset = make_pair(0, false);

                    for (size_t child_record_index_i = 0 ; child_record_index_i < temp_chain_record.children.size() ; child_record_index_i++) {
                        const pair<temp_record_t, size_t>& child_record_index = temp_chain_record.children[child_record_index_i];
                        //Go through each node and snarl in the chain and add them to the index
#ifdef debug_distance_indexing
                        cerr << "  Adding child of the chain: " << temp_index->structure_start_end_as_string(child_record_index) << endl;
#endif

                        if (child_record_index.first == TEMP_NODE) {
                            //Add a node to the chain
                            if (chain_node_i != 0 && child_record_index == temp_chain_record.children.front()) {
                                //If this is the last node in the chain, and it is the same as the first node -
                                // it is a looping chain and we set this and don't re-record the node
                                // TODO: I'm using externally_start_start_connected here to indicate that 
                                // it's sharing a start and end node, but chains might actually be allowed to
                                // be start-end connected in which case I need a new flag
                                //chain_record_constructor.set_externally_start_end_connected();
#ifdef debug_distance_indexing
                            cerr << "    This is a looping chain"  << endl;
#endif

                            } else {

                                //Get the temporary node record
                                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record = 
                                    temp_index->temp_node_records[child_record_index.second-min_node_id];


                                //Make a new node record
                                size_t new_offset = chain_record_constructor.add_node(
                                        temp_node_record.node_id, temp_node_record.node_length, temp_node_record.reversed_in_parent,
                                        temp_chain_record.prefix_sum[chain_node_i], temp_chain_record.forward_loops[chain_node_i],
                                        temp_chain_record.backward_loops[chain_node_i], temp_chain_record.chain_components[chain_node_i],
                                        last_child_offset.first);

                                //Remember this node as the last thing in the chain
                                last_child_offset = make_pair(new_offset, false);

                            }

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

                                record_t record_type = snarl_size_limit == 0 ? SNARL :
                                    (temp_snarl_record.node_count < snarl_size_limit ? DISTANCED_SNARL : OVERSIZED_SNARL);
                                SnarlRecordConstructor snarl_record_constructor =
                                    chain_record_constructor.add_snarl(temp_snarl_record.node_count, record_type, last_child_offset.first);

                                //Record how to find the new snarl record
                                record_to_offset.emplace(make_pair(temp_index_i, child_record_index), snarl_record_constructor.record_offset);

                                //Fill in snarl info
                                snarl_record_constructor.set_min_length(temp_snarl_record.min_length);
                                snarl_record_constructor.set_max_length(temp_snarl_record.max_length);

                                //Add distances and record connectivity
                                for (const auto& it : temp_snarl_record.distances) {
                                    const pair<pair<size_t, bool>, pair<size_t, bool>>& node_ranks = it.first;
                                    const size_t distance = it.second;

                                    if (snarl_size_limit != 0 &&
                                        (temp_snarl_record.node_count < snarl_size_limit ||
                                         (node_ranks.first.first == 0 || node_ranks.first.first == 1 ||
                                          node_ranks.second.first == 0 || node_ranks.second.first == 1))) {
                                        //If we are keeping track of distances and either this is a small enough snarl,
                                        //or the snarl is too big but we are looking at the boundaries
                                        assert(distance <= temp_snarl_record.max_distance);
                                        snarl_record_constructor.set_distance(node_ranks.first.first, node_ranks.first.second,
                                            node_ranks.second.first, node_ranks.second.second, distance);
                                        assert(snarl_record_constructor.get_distance(node_ranks.first.first, node_ranks.first.second,
                                               node_ranks.second.first, node_ranks.second.second) ==  distance);
                                    }

                                    //Now set the connectivity of this snarl
                                    if (node_ranks.first.first == 0 && node_ranks.second.first == 0) {
                                        snarl_record_constructor.set_start_start_connected();
                                    } else if ((node_ranks.first.first == 0 && node_ranks.second.first == 1) ||
                                               (node_ranks.first.first == 1 && node_ranks.second.first == 0)) {
                                        snarl_record_constructor.set_start_end_connected();
                                    } else if (node_ranks.first.first == 1 && node_ranks.second.first == 1) {
                                        snarl_record_constructor.set_end_end_connected();
                                    } else if ((node_ranks.first.first == 0 || node_ranks.second.first == 0) &&
                                               (temp_snarl_record.tippy_child_ranks.count(node_ranks.first.first)
                                                || temp_snarl_record.tippy_child_ranks.count(node_ranks.second.first))) {
                                        snarl_record_constructor.set_start_tip_connected();
                                    } else if ((node_ranks.first.first == 1 || node_ranks.second.first == 1) &&
                                               (temp_snarl_record.tippy_child_ranks.count(node_ranks.first.first)
                                                || temp_snarl_record.tippy_child_ranks.count(node_ranks.second.first))) {
                                        snarl_record_constructor.set_end_tip_connected();
                                    } else if (temp_snarl_record.tippy_child_ranks.count(node_ranks.first.first)
                                                && temp_snarl_record.tippy_child_ranks.count(node_ranks.second.first)) {
                                        snarl_record_constructor.set_tip_tip_connected();
                                    }
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

                                record_t record_type = snarl_size_limit == 0 ? SIMPLE_SNARL : DISTANCED_SIMPLE_SNARL;
                                SimpleSnarlRecordConstructor snarl_record_constructor =
                                    chain_record_constructor.add_simple_snarl(temp_snarl_record.node_count, record_type, last_child_offset.first);

                                //Record how to find the new snarl record
                                record_to_offset.emplace(make_pair(temp_index_i, child_record_index), snarl_record_constructor.record_offset);

                                //Fill in snarl info
                                snarl_record_constructor.set_min_length(temp_snarl_record.min_length);
                                snarl_record_constructor.set_max_length(temp_snarl_record.max_length);

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
                                        bool reaches_node_end_to_start = temp_snarl_record.distances.count(
                                            std::make_pair(std::make_pair(rank, true),
                                                           std::make_pair(0, false))) != 0;
                                        bool reaches_start_to_node_end = temp_snarl_record.distances.count(
                                            std::make_pair(std::make_pair(0, false),
                                                           std::make_pair(rank, true))) != 0;

                                        bool is_reversed_in_parent = reaches_node_end_to_start || reaches_start_to_node_end; 
                                        snarl_record_constructor.add_child(i+2, temp_node_record.node_id,  
                                                temp_node_record.node_length, is_reversed_in_parent);
                                    } else {
                                        assert(child_index.first == TEMP_NODE);
                                        const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                                             temp_index->temp_node_records[child_index.second-min_node_id];
                                        size_t rank =temp_node_record.rank_in_parent;
                                        bool reaches_node_end_to_start = temp_snarl_record.distances.count(
                                            std::make_pair(std::make_pair(rank, true),
                                                           std::make_pair(0, false))) != 0;
                                        bool reaches_start_to_node_end = temp_snarl_record.distances.count(
                                            std::make_pair(std::make_pair(0, false),
                                                           std::make_pair(rank, true))) != 0;

                                        bool is_reversed_in_parent = reaches_node_end_to_start || reaches_start_to_node_end; 
                                        snarl_record_constructor.add_child(i+2, temp_node_record.node_id,  
                                                temp_node_record.node_length, is_reversed_in_parent);
                                    }
                                }

#ifdef debug_distance_indexing
                            cerr << "    The simple snarl record is at offset " << snarl_record_constructor.record_offset << endl;
                            cerr << "    This child snarl has " << snarl_record_constructor.get_node_count() << " children: " << endl;
#endif
                                last_child_offset = make_pair(snarl_record_constructor.record_offset, true);
                            }
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

                    record_t record_type = snarl_size_limit == 0 ? NODE : DISTANCED_NODE;
                    NodeRecordConstructor node_record(snarl_tree_records->size(), 0, record_type, &snarl_tree_records, temp_node_record.node_id);
                    node_record.set_node_id(temp_node_record.node_id);
                    node_record.set_node_length(temp_node_record.node_length);
                    node_record.set_rank_in_parent(temp_chain_record.rank_in_parent);
                    node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_chain_record.parent)]);

                    record_to_offset.emplace(make_pair(temp_index_i, current_record_index), node_record.record_offset);

                }
            } else if (current_record_index.first == TEMP_SNARL) {
#ifdef debug_distance_indexing
                cerr << "        this is a root-level snarl "
                     << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif
                //This is a root-level snarl
                record_t record_type = snarl_size_limit == 0 ? ROOT_SNARL : DISTANCED_ROOT_SNARL;

                const TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record = temp_index->temp_snarl_records[current_record_index.second];
                record_to_offset.emplace(make_pair(temp_index_i,current_record_index), snarl_tree_records->size());

                SnarlRecordConstructor snarl_record_constructor (temp_snarl_record.node_count, &snarl_tree_records, record_type);

                //Fill in snarl info
                snarl_record_constructor.set_parent_record_offset(0);

                //Add distances and record connectivity

                if (snarl_size_limit != 0 ) {


                    for (const auto& it : temp_snarl_record.distances) {
                        const pair<pair<size_t, bool>, pair<size_t, bool>>& node_ranks = it.first;
                        const size_t distance = it.second;
                        //TODO: I"m checking this but also automatically making a distanced snarl
                        //If we are keeping track of distances and either this is a small enough snarl,
                        //or the snarl is too big but we are looking at the boundaries
#ifdef debug_distance_indexing
                        assert(distance <= temp_snarl_record.max_distance);
#endif
                        if ((temp_snarl_record.node_count < snarl_size_limit ||
                                     (node_ranks.first.first == 0 || node_ranks.first.first == 1 ||
                                      node_ranks.second.first == 0 || node_ranks.second.first == 1))) {
                            snarl_record_constructor.set_distance(node_ranks.first.first, node_ranks.first.second,
                             node_ranks.second.first, node_ranks.second.second, distance);
#ifdef debug_distance_indexing
                            assert(snarl_record_constructor.get_distance(node_ranks.first.first, node_ranks.first.second,
                                    node_ranks.second.first, node_ranks.second.second) == distance);
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
                //TODO: This was a node that was a tip, so it wasn't put in a chain by the temporary index
                assert(current_record_index.first == TEMP_NODE);
                //and then add them all after adding the snarl
#ifdef debug_distance_indexing
                cerr << "        this just a node "
                     << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif
                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                        temp_index->temp_node_records[current_record_index.second-min_node_id];
                record_t record_type = snarl_size_limit == 0 ? NODE : DISTANCED_NODE;
                NodeRecordConstructor node_record(snarl_tree_records->size(), 0, record_type, &snarl_tree_records, temp_node_record.node_id);
                node_record.set_node_id(temp_node_record.node_id);
                node_record.set_node_length(temp_node_record.node_length);
                node_record.set_rank_in_parent(temp_node_record.rank_in_parent);
                node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_node_record.parent)]);

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
            SnarlTreeRecordConstructor record_constructor(record_to_offset[make_pair(temp_index_i, component_index)],
                                                              &snarl_tree_records);
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
                SnarlRecordConstructor snarl_record_constructor (&snarl_tree_records,
                        record_to_offset[make_pair(temp_index_i, make_pair(TEMP_SNARL, temp_snarl_i))]);
                //Now add the children and tell the record where to find them
                snarl_record_constructor.set_child_record_pointer(snarl_tree_records->size());
                for (pair<temp_record_t, size_t> child : temp_snarl_record.children) {
                    snarl_record_constructor.add_child(record_to_offset[make_pair(temp_index_i, child)]);
#ifdef debug_distance_indexing
                cerr << "       child " << temp_index->structure_start_end_as_string(child) << endl;
                cerr << "        " << child.first << " " << child.second << endl;
                cerr << "        Add child " << net_handle_as_string(get_net_handle(record_to_offset[make_pair(temp_index_i, child)], START_END))
                     << "     at offset " << record_to_offset[make_pair(temp_index_i, child)]
                     << "     to child list at offset " << snarl_tree_records->size() << endl;
#endif
                }
            }
        }
    }
    //Repack the vector to use fewer bits
    //TODO: Wouldn't have to do this if I could predict the vector better
    size_t max_val = 0;
    for (size_t i = 0 ; i < snarl_tree_records->size() ; i++ ) {
        max_val = std::max(max_val, (size_t) snarl_tree_records->at(i));
    }
    size_t ideal_bit_width = std::max((size_t)log2(max_val)+1, (size_t)26);
    if (ideal_bit_width < snarl_tree_records->width()) {
        cerr << "Resetting bit width from " << snarl_tree_records->width() << " to " << ideal_bit_width << endl;
        //snarl_tree_records->repack(ideal_bit_width, snarl_tree_records->size());
    }
    tuple<size_t, size_t, size_t> usage =get_usage();
    cerr << "total\t" << snarl_tree_records->size() << endl;
    cerr << "Usage: " << std::get<0>(usage) << " total bytes, " << std::get<1>(usage) << " free bytes " << std::get<2>(usage) << " reclaimable free bytes " << endl;
    cerr << "bit width " << snarl_tree_records->width() << endl;
    cerr << "Max value " << max_val << endl;
    cerr << "Predicted size: " << maximum_index_size << " actual size: " <<  snarl_tree_records->size() << endl;
    //assert(maximum_index_size == snarl_tree_records->size());
#ifdef debug_distance_indexing
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

SnarlDistanceIndex::CachedNetHandle SnarlDistanceIndex::get_cached_bound(
    const CachedNetHandle& parent, bool get_start) const {
    if (is_snarl(parent.net)) {
        //If we want the boundary of a snarl, then all we need is the net handle
        if (get_start && parent.contains_start_bound) {
            return get_cached_net_handle(parent.start_bound_in, parent.start_bound_tag);
        } else if (!get_start && parent.contains_end_bound) {
            return get_cached_net_handle(parent.end_bound_in, parent.end_bound_tag);
        } else {
            return get_cached_net_handle(get_bound(parent.net, !get_start, true));
        }
    } else {
#ifdef debug_distances
        assert(is_chain(parent.net));
#endif
        if (get_start && parent.contains_start_bound) {
            //If we already know what the bound is, copy it over
            CachedNetHandle bound = get_cached_net_handle(parent.start_bound_in, parent.start_bound_tag);
            bound.min_length = parent.start_bound_length;
            if (parent.contains_start_node_values) {
                bound.prefix_sum_val = parent.prefix_sum_val;
                bound.forward_loop_val = parent.forward_loop_val;
                bound.reverse_loop_val = parent.reverse_loop_val;
                bound.chain_component_val = parent.chain_component_val;
                bound.is_reversed = parent.is_reversed;
                bound.rank = parent.start_rank;
                bound.contains_node_values = true;
            }
            return bound;
        } else if (! get_start && parent.contains_end_bound) {
            //And the same for the end bound
            CachedNetHandle bound = get_cached_net_handle(parent.end_bound_in, parent.end_bound_tag);
            bound.min_length = parent.end_bound_length;
            if (parent.contains_end_node_values) {
                bound.prefix_sum_val = parent.end_prefix_sum_val;
                bound.forward_loop_val = parent.end_forward_loop_val;
                bound.reverse_loop_val = parent.end_reverse_loop_val;
                bound.chain_component_val = parent.end_chain_component_val;
                bound.is_reversed = parent.end_is_reversed;
                bound.rank = parent.end_rank;
                bound.contains_node_values = true;
            }
            return bound;
        } else {
            return get_cached_net_handle( get_bound(parent.net, !get_start, true));
        }
    }
}

void SnarlDistanceIndex::set_cached_node_values(CachedNetHandle& cached_handle) const {

    if (!cached_handle.contains_node_values){
        if (get_record_type(cached_handle.record_tag) == DISTANCED_TRIVIAL_SNARL) {
            cached_handle.contains_node_values = true;

            TrivialSnarlRecord record(get_record_offset(cached_handle.net), &snarl_tree_records);
            size_t node_rank = get_node_record_offset(cached_handle.net);

            cached_handle.is_reversed = record.get_is_reversed_in_parent(node_rank);
            std::tie(cached_handle.prefix_sum_val,cached_handle.forward_loop_val,
                     cached_handle.reverse_loop_val,cached_handle.chain_component_val)
                 = record.get_chain_values(node_rank);
        }         
    }
}
void SnarlDistanceIndex::set_cached_rank(CachedNetHandle& cached_handle) const {
    if (cached_handle.rank == std::numeric_limits<size_t>::max()) {
        if (get_record_type(cached_handle.record_tag) == TRIVIAL_SNARL || 
            get_record_type(cached_handle.record_tag) == DISTANCED_TRIVIAL_SNARL) {
            cached_handle.rank = TrivialSnarlRecord(get_record_offset(cached_handle.net), &snarl_tree_records).get_rank_in_parent(get_node_record_offset(cached_handle.net));
        } else if (get_record_type(cached_handle.record_tag) == SIMPLE_SNARL || 
            get_record_type(cached_handle.record_tag) == DISTANCED_SIMPLE_SNARL) {
            if (is_snarl(cached_handle.net)) {
                cached_handle.rank = get_record_offset(cached_handle.net);
            } else {
                cached_handle.rank = get_node_record_offset(cached_handle.net);
            }
        } else {
            cached_handle.rank = SnarlTreeRecord(cached_handle.net, &snarl_tree_records).get_rank_in_parent();
        }
    }
}
void SnarlDistanceIndex::set_cached_min_length(CachedNetHandle& cached_handle) const {
    if (cached_handle.min_length == std::numeric_limits<size_t>::max()) {
        cached_handle.min_length = minimum_length(cached_handle.net);
    }
}
void SnarlDistanceIndex::set_cached_parent_offset(CachedNetHandle& cached_handle) const {
    if (cached_handle.parent_record_offset == std::numeric_limits<size_t>::max()) {
        cached_handle.parent_record_offset = SnarlTreeRecord(cached_handle.net, &snarl_tree_records).get_parent_record_offset();
    }
}
void SnarlDistanceIndex::set_cached_start_bound(CachedNetHandle& cached_handle, bool set_values_in_chain, bool set_length) const {
    if (! cached_handle.contains_start_bound) {
        cached_handle.contains_start_bound=true;
        cached_handle.start_bound_in = get_bound(cached_handle.net, false, true);
        if (is_sentinel(cached_handle.start_bound_in)) {
            cached_handle.start_bound_in = get_node_from_sentinel(cached_handle.start_bound_in);
        }
        cached_handle.start_bound_tag = snarl_tree_records->at(get_record_offset(cached_handle.start_bound_in));
    }
    if ((set_values_in_chain && ! cached_handle.contains_start_node_values) ||
        (set_length && cached_handle.start_bound_length == std::numeric_limits<size_t>::max())) {
        if (set_length && cached_handle.start_bound_length == std::numeric_limits<size_t>::max()) {
            cached_handle.start_bound_length = node_length(cached_handle.start_bound_in);
        }
        if (set_values_in_chain && !cached_handle.contains_start_node_values) {
            assert(SnarlTreeRecord(cached_handle.start_bound_in, &snarl_tree_records).get_record_type() == DISTANCED_TRIVIAL_SNARL);

            cached_handle.contains_start_node_values = true;

            TrivialSnarlRecord record(get_record_offset(cached_handle.start_bound_in), &snarl_tree_records);
            size_t node_offset = get_node_record_offset(cached_handle.start_bound_in);
            cached_handle.prefix_sum_val = record.get_prefix_sum(node_offset);
            cached_handle.forward_loop_val = record.get_forward_loop(node_offset);
            cached_handle.reverse_loop_val = record.get_reverse_loop(node_offset);
            cached_handle.is_reversed = record.get_is_reversed_in_parent(node_offset);
            cached_handle.start_rank = record.get_rank_in_parent(node_offset); //TODO: What is rank needed for?
            cached_handle.chain_component_val = record.get_chain_component(node_offset);
            
        }           
    }
};          
void SnarlDistanceIndex::set_cached_end_bound(CachedNetHandle& cached_handle, bool set_values_in_chain, bool set_length) const {
    if (!cached_handle.contains_end_bound){
        cached_handle.contains_end_bound=true;
        cached_handle.end_bound_in = get_bound(cached_handle.net, true, true);
        if (is_sentinel(cached_handle.end_bound_in)) {
            cached_handle.end_bound_in = get_node_from_sentinel(cached_handle.end_bound_in);
        }
        cached_handle.end_bound_tag = snarl_tree_records->at(get_record_offset(cached_handle.end_bound_in));
    }
    if ((set_values_in_chain && !cached_handle.contains_end_node_values) ||
        (set_length && cached_handle.end_bound_length == std::numeric_limits<size_t>::max())){
        if (set_length && cached_handle.end_bound_length == std::numeric_limits<size_t>::max()) {
            cached_handle.end_bound_length = node_length(cached_handle.end_bound_in);
        }
        if (set_values_in_chain && !cached_handle.contains_end_node_values) {
            assert (SnarlTreeRecord(cached_handle.end_bound_in, &snarl_tree_records).get_record_type() == DISTANCED_TRIVIAL_SNARL);
            cached_handle.contains_end_node_values = true;

            TrivialSnarlRecord record(get_record_offset(cached_handle.end_bound_in), &snarl_tree_records);
            size_t node_offset = get_node_record_offset(cached_handle.end_bound_in);
            cached_handle.end_prefix_sum_val = record.get_prefix_sum(node_offset);
            cached_handle.end_forward_loop_val = record.get_forward_loop(node_offset);
            cached_handle.end_reverse_loop_val = record.get_reverse_loop(node_offset);
            cached_handle.end_is_reversed = record.get_is_reversed_in_parent(node_offset);
            cached_handle.end_rank = record.get_rank_in_parent(node_offset);
            cached_handle.end_chain_component_val = record.get_chain_component(node_offset);
        }
    }
};     

net_handle_t SnarlDistanceIndex::get_cached_start_bound(CachedNetHandle& cached_handle) const {
    if (!cached_handle.contains_start_bound) {
        set_cached_start_bound(cached_handle, false, false);
    }
    return cached_handle.start_bound_in;
}
net_handle_t SnarlDistanceIndex::get_cached_end_bound(CachedNetHandle& cached_handle) const {
    if (!cached_handle.contains_end_bound) {
        set_cached_end_bound(cached_handle, false, false);
    }
    return cached_handle.end_bound_in;
}
size_t SnarlDistanceIndex::get_cached_start_bound_length(CachedNetHandle& cached_handle) const {
    if (!cached_handle.contains_start_bound ||
        cached_handle.start_bound_length == std::numeric_limits<size_t>::max()) {
        set_cached_start_bound(cached_handle, false, true);
    }
    return cached_handle.start_bound_length;
}
size_t SnarlDistanceIndex::get_cached_end_bound_length(CachedNetHandle& cached_handle) const {
    if (!cached_handle.contains_end_bound ||
        cached_handle.end_bound_length == std::numeric_limits<size_t>::max() ) {
        set_cached_end_bound(cached_handle, false, true);
    }
    return cached_handle.end_bound_length;
}
size_t SnarlDistanceIndex::get_cached_parent_offset(CachedNetHandle& cached_handle) const {
    if (cached_handle.parent_record_offset == std::numeric_limits<size_t>::max()) {
        set_cached_parent_offset(cached_handle);
    }
    return cached_handle.parent_record_offset;
}
size_t SnarlDistanceIndex::get_cached_rank(CachedNetHandle& cached_handle) const {
    if (cached_handle.rank == std::numeric_limits<size_t>::max()) {
        set_cached_rank(cached_handle);
    }
    return cached_handle.rank;
}
size_t SnarlDistanceIndex::get_cached_min_length(CachedNetHandle& cached_handle) const {
    if (cached_handle.min_length == std::numeric_limits<size_t>::max()) {
        set_cached_min_length(cached_handle);
    }
    return cached_handle.min_length;
}
bool SnarlDistanceIndex::get_cached_is_reverse(CachedNetHandle& cached_handle) const {
    if (is_node(cached_handle.net)) {
        if (!cached_handle.contains_node_values) {
            set_cached_node_values(cached_handle);
        }
        return cached_handle.is_reversed;
    } else {
        return false;
    }
}
tuple<size_t, size_t, size_t, size_t> SnarlDistanceIndex::get_cached_chain_values(CachedNetHandle& cached_handle) const {
    assert (is_node(cached_handle.net));
    if (!cached_handle.contains_node_values) {
        set_cached_node_values(cached_handle);
    }
    return make_tuple(cached_handle.prefix_sum_val, cached_handle.forward_loop_val,
                      cached_handle.reverse_loop_val, cached_handle.chain_component_val);
    
}
tuple<size_t, size_t, size_t, size_t> SnarlDistanceIndex::get_cached_start_chain_values(CachedNetHandle& cached_handle) const {
    assert (is_snarl(cached_handle.net));
    if (!cached_handle.contains_start_node_values) {
        set_cached_start_bound(cached_handle, true, false);
    }
    return make_tuple(cached_handle.prefix_sum_val, cached_handle.forward_loop_val,
                      cached_handle.reverse_loop_val, cached_handle.chain_component_val);
}
tuple<size_t, size_t, size_t, size_t> SnarlDistanceIndex::get_cached_end_chain_values(CachedNetHandle& cached_handle) const {
    assert (is_snarl(cached_handle.net));
    if (!cached_handle.contains_end_node_values) {
        set_cached_end_bound(cached_handle, true, false);
    }
    return make_tuple(cached_handle.end_prefix_sum_val, cached_handle.end_forward_loop_val,
                      cached_handle.end_reverse_loop_val, cached_handle.end_chain_component_val);
}

//TODO: Also need to go the other way, from final index to temporary one for merging

//TODO: Probably don't want to keep this

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
