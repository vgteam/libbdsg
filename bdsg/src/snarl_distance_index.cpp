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
    return get_handle_type(net) == SNARL_HANDLE 
            && (SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE ||
                SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == ROOT_SNARL ||  
                SnarlTreeRecord(net, &snarl_tree_records).get_record_type() == DISTANCED_ROOT_SNARL);
}

bool SnarlDistanceIndex::is_chain(const net_handle_t& net) const {
    return get_handle_type(net) == CHAIN_HANDLE
            && (SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == CHAIN_HANDLE
            || SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE);
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

        return SnarlTreeRecord(child1, &snarl_tree_records).get_rank_in_parent() < SnarlTreeRecord(child2, &snarl_tree_records).get_rank_in_parent();
    }
}

bool SnarlDistanceIndex::is_trivial_chain(const net_handle_t& net) const {
    return get_handle_type(net) == CHAIN_HANDLE
            && SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE;
}

bool SnarlDistanceIndex::is_node(const net_handle_t& net) const {
    return get_handle_type(net) == NODE_HANDLE 
            && SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == NODE_HANDLE;
}
bool SnarlDistanceIndex::is_sentinel(const net_handle_t& net) const {
    return get_handle_type(net) == SENTINEL_HANDLE
            && SnarlTreeRecord(net, &snarl_tree_records).get_record_handle_type() == SNARL_HANDLE;
}

net_handle_t SnarlDistanceIndex::get_net(const handle_t& handle, const handlegraph::HandleGraph* graph) const{
    return get_net_handle(get_offset_from_node_id(graph->get_id(handle)), 
                          graph->get_is_reverse(handle) ? END_START : START_END, 
                          NODE_HANDLE);
}
handle_t SnarlDistanceIndex::get_handle(const net_handle_t& net, const handlegraph::HandleGraph* graph) const{
    //TODO: Maybe also want to be able to get the graph handle of a sentinel
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
        NodeRecord node_record(net, &snarl_tree_records);
        return graph->get_handle(get_node_id_from_offset(get_record_offset(net)), 
                                 ends_at(net) == END ? false : true);
    } else {
        throw runtime_error("error: trying to get a handle from a snarl, chain, or root");
    }
}

net_handle_t SnarlDistanceIndex::get_parent(const net_handle_t& child) const {

    //If the child is the sentinel of a snarl, just return the snarl
    if (get_handle_type(child) == SENTINEL_HANDLE) {
        return get_net_handle(get_record_offset(child), START_END, SNARL_HANDLE); 
    } else if (get_handle_type(child) == ROOT_HANDLE) {
        throw runtime_error("error: trying to find the parent of the root");
    } 

    //Otherwise, we need to move up one level in the snarl tree

    //Get the pointer to the parent, and keep the connectivity of the current handle
    size_t parent_pointer = SnarlTreeRecord(child, &snarl_tree_records).get_parent_record_offset();
    connectivity_t child_connectivity = get_connectivity(child);

    //TODO: I"m going into the parent record here, which could be avoided if things knew what their parents were, but I think if you're doing this you'd later go into the parent anyway so it's probably fine
    net_handle_record_t parent_type = SnarlTreeRecord(parent_pointer, &snarl_tree_records).get_record_handle_type();
    connectivity_t parent_connectivity = START_END;
    if ((child_connectivity == START_END || child_connectivity == END_START) 
        && (parent_type == CHAIN_HANDLE)) {
        //TODO: This also needs to take into account the orientation of the child, which I might be able to get around?
        parent_connectivity = child_connectivity;
    }
    if (get_handle_type(child) == NODE_HANDLE && parent_type != CHAIN_HANDLE) {
        //If this is a node and it's parent is not a chain, we want to pretend that its 
        //parent is a chain
        return get_net_handle(get_record_offset(child), parent_connectivity, CHAIN_HANDLE);
    } 

    return get_net_handle(parent_pointer, parent_connectivity);
}

net_handle_t SnarlDistanceIndex::get_bound(const net_handle_t& snarl, bool get_end, bool face_in) const {
    if (get_handle_type(snarl) == CHAIN_HANDLE) {
        handlegraph::nid_t id = get_end ? SnarlTreeRecord(snarl, &snarl_tree_records).get_end_id() 
                          : SnarlTreeRecord(snarl, &snarl_tree_records).get_start_id();
        bool rev_in_parent = NodeRecord(get_offset_from_node_id(id), &snarl_tree_records).get_is_reversed_in_parent();
        if (get_end) {
            rev_in_parent = !rev_in_parent;
        }
        if (!face_in){
            rev_in_parent = !rev_in_parent;
        }
        connectivity_t connectivity = rev_in_parent ? END_START : START_END;
        if (SnarlTreeRecord(snarl, &snarl_tree_records).get_start_id() 
                == SnarlTreeRecord(snarl, &snarl_tree_records).get_end_id() &&  get_end) {
            //If this is a looping chain and we're getting the end, then the traversal of the node will be the 
            //end endpoint repeated, instead of the actual traversal
            //TODO: This might cause problems when checking traversals but I think it's fine
            connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), get_end_endpoint(connectivity));
        }
        return get_net_handle(get_offset_from_node_id(id), connectivity,  NODE_HANDLE);
    } else {
        assert(get_handle_type(snarl) == SNARL_HANDLE);
        endpoint_t start = get_end ? END : START;
        endpoint_t end = face_in ? (start == END ? START : END) : start;
        return get_net_handle(get_record_offset(snarl), endpoints_to_connectivity(start, end), SENTINEL_HANDLE);
    }
}

net_handle_t SnarlDistanceIndex::flip(const net_handle_t& net) const {
    connectivity_t old_connectivity = get_connectivity(net);
    connectivity_t new_connectivity =  endpoints_to_connectivity(get_end_endpoint(old_connectivity), 
                                                                get_start_endpoint(old_connectivity));
    return get_net_handle(get_record_offset(net), new_connectivity, get_handle_type(net));
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
    return get_net_handle(get_record_offset(net), connectivity, get_handle_type(net));
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
    if (record_type == SNARL_HANDLE) {
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
        if (!iteratee(get_net_handle(get_record_offset(item), START_END, get_handle_type(item)))) {
            return false;
        }
        if (!iteratee(get_net_handle(get_record_offset(item), END_START, get_handle_type(item)))) {
            return false;
        }
    }
    SnarlTreeRecord record(item, &snarl_tree_records);
    for ( size_t type = 1 ; type <= 9 ; type ++ ){
        connectivity_t connectivity = static_cast<connectivity_t>(type);
        if (record.has_connectivity(connectivity)) {
            if (!iteratee(get_net_handle(get_record_offset(item), connectivity, get_handle_type(item)))) {
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
                if (!iteratee(get_net_handle(get_record_offset(here), START_END, get_handle_type(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from end back to the end
                if (!iteratee(get_net_handle(get_record_offset(here), END_START, get_handle_type(here)))) {
                    return false;
                }
            }
        } else {
            //Follow edges leaving the root structure at the end
            if (this_record.is_externally_start_end_connected()) {
                //Follow edge from start to end
                if (!iteratee(get_net_handle(get_record_offset(here), END_START, get_handle_type(here)))) {
                    return false;
                }
            }
            if (this_record.is_externally_end_end_connected()) {
                //Follow edge from the start back to the start
                if (!iteratee(get_net_handle(get_record_offset(here), START_END, get_handle_type(here)))) {
                    return false;
                }
            }

        }
        return true;

    } else if (get_handle_type(here) == CHAIN_HANDLE || get_handle_type(here) == SENTINEL_HANDLE) {
        assert(parent_record.get_record_handle_type() == SNARL_HANDLE ||
               parent_record.get_record_handle_type() == ROOT_HANDLE);//It could also be the root
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
        } else if (get_handle_type(here) == NODE_HANDLE) {
            graph_handle = graph->get_handle(get_node_id_from_offset(get_record_offset(here)), 
                                ends_at(here) == END ? go_left : !go_left);
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
                assert(graph->get_is_reverse(h) == !parent_record.get_start_orientation());
#ifdef debug_snarl_traversal
                cerr << "    -> start of parent " << endl;
#endif
                return iteratee(get_bound(get_parent(here), false, false));
            } else if (graph->get_id(h) == parent_record.get_end_id()) {
                assert(graph->get_is_reverse(h) == parent_record.get_end_orientation());
#ifdef debug_snarl_traversal
                cerr << "    -> end of parent " << endl;
#endif
                return iteratee(get_bound(get_parent(here), true, false));
            } else {
                //It is either another chain or a node, but the node needs to pretend to be a chain

                //Get the node record of the next node
                NodeRecord next_node_record(get_offset_from_node_id(graph->get_id(h)), &snarl_tree_records);

                if (next_node_record.get_parent_record_offset() == parent_record.record_offset) {
                    //If the next node's parent is also the current node's parent, then it is a node
                    //make a net_handle_t of a node pretending to be a chain
                    net_handle_t next_net = get_net_handle(next_node_record.record_offset, 
                                                           graph->get_is_reverse(h) ? END_START : START_END, 
                                                           CHAIN_HANDLE);
#ifdef debug_snarl_traversal
                cerr << "    -> actual child node " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                } else {
                    //next_node_record is also the start of a chain

                    bool rev = graph->get_id(h) == next_node_record.get_is_reversed_in_parent() ? false : true;
                    net_handle_t next_net = get_net_handle(next_node_record.get_parent_record_offset(), 
                                                           rev ? END_START : START_END, 
                                                           CHAIN_HANDLE);
#ifdef debug_snarl_traversal
                    assert(SnarlTreeRecord(next_node_record.get_parent_record_offset(), &snarl_tree_records).get_record_handle_type() == CHAIN_HANDLE);
                    assert(get_node_id_from_offset(next_node_record.record_offset) 
                        == SnarlTreeRecord(next_node_record.get_parent_record_offset(), &snarl_tree_records).get_start_id() || 
                        get_node_id_from_offset(next_node_record.record_offset) 
                        == SnarlTreeRecord(next_node_record.get_parent_record_offset(), &snarl_tree_records).get_end_id());
                cerr << "    -> child chain " << net_handle_as_string(next_net) << endl;
#endif
                   return iteratee(next_net);
                }
            }
            return false;
        });

        
    } else if (get_handle_type(here) == SNARL_HANDLE || get_handle_type(here) == NODE_HANDLE) {
        assert(parent_record.get_record_handle_type() == CHAIN_HANDLE);
        //If this is a snarl or node, then it is the component of a (possibly pretend) chain
        ChainRecord parent_chain(this_record.get_parent_record_offset(), &snarl_tree_records);
        if (ends_at(here) == START) {
            go_left = !go_left;
        }
        if (SnarlTreeRecord(get_record_offset(here), &snarl_tree_records).get_is_reversed_in_parent()) {
            go_left = !go_left;
        }
        net_handle_t next_net = parent_chain.get_next_child(here, go_left);
        if (next_net == here) {
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
                              SNARL_HANDLE);
    } else {
        //These are the endpoints or tips in a chain
        SnarlTreeRecord start_record = get_snarl_tree_record(traversal_start);
        SnarlTreeRecord end_record = get_snarl_tree_record(traversal_end);
        if (start_record.get_parent_record_offset() != end_record.get_parent_record_offset()) {
            throw runtime_error("error: Looking for parent traversal of two non-siblings");
        }
        SnarlTreeRecord parent_record (start_record.get_parent_record_offset(), &snarl_tree_records);
        assert(parent_record.get_record_handle_type() == CHAIN_HANDLE);

        //Figure out what the start and end of the traversal are
        endpoint_t start_endpoint;
        if (start_handle_type == NODE_HANDLE && 
            get_node_id_from_offset(get_record_offset(traversal_start)) == parent_record.get_start_id() &&
            (get_start_endpoint(traversal_start) == START && !parent_record.get_start_orientation() ||
             get_start_endpoint(traversal_start) == END && parent_record.get_start_orientation()) ){
            //If traversal_start is a node and is also the start node oriented into the parent
            start_endpoint = START;
    
        } else if (start_handle_type == NODE_HANDLE && 
            get_node_id_from_offset(get_record_offset(traversal_start)) == parent_record.get_end_id() &&
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
            get_node_id_from_offset(get_record_offset(traversal_end)) == parent_record.get_start_id() &&
            (get_start_endpoint(traversal_end) == START && parent_record.get_start_orientation() ||
             get_start_endpoint(traversal_end) == END && !parent_record.get_start_orientation())){
            //If traversal_end is a node and also the start node oriented out of the parent
            end_endpoint = START;
        } else if (end_handle_type == NODE_HANDLE && 
            get_node_id_from_offset(get_record_offset(traversal_end)) == parent_record.get_end_id() &&
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

void SnarlDistanceIndex::load(const string& filename) {
    snarl_tree_records.dissociate();
    snarl_tree_records.reset();
    std::ifstream in (filename);
    snarl_tree_records.load(in, tag);
}
void SnarlDistanceIndex::load(int fd) {
    snarl_tree_records.dissociate();
    snarl_tree_records.reset();
    snarl_tree_records.load(fd, tag);
}
void SnarlDistanceIndex::load(std::istream& in) {
    snarl_tree_records.dissociate();
    snarl_tree_records.reset();
    snarl_tree_records.load(in, tag);
}
void SnarlDistanceIndex::save(const string& filename) const {
    std::ofstream out(filename);
    snarl_tree_records.save(out);
}
void SnarlDistanceIndex::save(int fd) {
    snarl_tree_records.save(fd);
}
void SnarlDistanceIndex::save(std::ostream& out) const {
    snarl_tree_records.save(out);
}


size_t SnarlDistanceIndex::distance_in_parent(const net_handle_t& parent, 
        const net_handle_t& child1, const net_handle_t& child2, const HandleGraph* graph) const {

    assert(canonical(parent) == canonical(get_parent(child1)));
    assert(canonical(parent) == canonical(get_parent(child2)));

    if (is_root(parent)) {
        //If the parent is the root, then the children must be in the same root snarl for them to be
        //connected
        size_t parent_record_offset1 = SnarlTreeRecord(child1, &snarl_tree_records).get_parent_record_offset();
        size_t parent_record_offset2 = SnarlTreeRecord(child2, &snarl_tree_records).get_parent_record_offset();
        if (parent_record_offset1 != parent_record_offset2) {
            //If the children are in different connected components
            return std::numeric_limits<size_t>::max();
        } else if (SnarlTreeRecord(parent_record_offset1, &snarl_tree_records).get_record_type() 
                    != DISTANCED_ROOT_SNARL){
            //If they are in the same connected component, but it is not a root snarl
            if (canonical(child1) == canonical(child2)) {
                //If they are the same child of the root but not in a snarl, then check the external connectivity
                if (ends_at(child1) == START && ends_at(child2) == START) {
                    return has_external_connectivity(child1, START, START) ? 0 : std::numeric_limits<size_t>::max();
                } else if (ends_at(child1) == END && ends_at(child2) == END) {
                    return has_external_connectivity(child1, END, END) ? 0 : std::numeric_limits<size_t>::max();
                } else if ((ends_at(child1) == START && ends_at(child2) == END) ||
                            (ends_at(child1) == END && ends_at(child2) == START)) {
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
            //They are in the same root snarl, so find the distance between them
            //
            SnarlRecord snarl_record(parent_record_offset1, &snarl_tree_records);
            SnarlTreeRecord child_record1 (child1, &snarl_tree_records);
            SnarlTreeRecord child_record2 (child2, &snarl_tree_records);
            size_t rank1 = child_record1.get_rank_in_parent();
            size_t rank2 = child_record2.get_rank_in_parent();

            //TODO: Double check orientations
            return snarl_record.get_distance(rank1, ends_at(child1) == END, rank2, ends_at(child2) == END);
        }


    } else if (is_chain(parent)) {
        ChainRecord chain_record(get_parent(child1), &snarl_tree_records);
        assert(is_node(child1) || is_snarl(child1));
        assert(is_node(child2) || is_snarl(child2));

        if (is_trivial_chain(parent)) {
            return std::numeric_limits<size_t>::max();
        }

        if (is_node(child1) && is_node(child2)) {
            //The distance between two nodes
            NodeRecord child_record1 (child1, &snarl_tree_records);
            NodeRecord child_record2 (child2, &snarl_tree_records);

            bool go_left1 = child_record1.get_is_reversed_in_parent();
            if (ends_at(child1) == START){
                go_left1 = !go_left1;
            }
            bool go_left2 = child_record2.get_is_reversed_in_parent();
            if (ends_at(child2) == START) {
                go_left2 = !go_left2;
            }
//#ifdef debug_distances
//            cerr << "Finding distances between two nodes with ranks " << child_record1.get_rank_in_parent() << " " << go_left1 << " " << child_record1.get_node_length() << " and " << endl << child_record2.get_rank_in_parent() << " " << go_left2 << " " << child_record2.get_node_length() << endl;
//            cerr << "            => " << chain_record.get_distance(
//                make_tuple(child_record1.get_rank_in_parent(), go_left1, child_record1.get_node_length()), 
//                make_tuple(child_record2.get_rank_in_parent(), go_left2, child_record2.get_node_length())) << endl;
//#endif

            return chain_record.get_distance(
                make_tuple(child_record1.get_rank_in_parent(), go_left1, child_record1.get_node_length()), 
                make_tuple(child_record2.get_rank_in_parent(), go_left2, child_record2.get_node_length()));
        } else if (is_node(child1) != is_node(child2)) {
            //If one of them is a node and one is a snarl
            //It doesn't matter which is which, since we're looking at the distance pointing towards each other
            NodeRecord node_record (is_node(child1) ? child1 : child2, &snarl_tree_records);
            bool go_left_node = node_record.get_is_reversed_in_parent();
            if (ends_at(is_node(child1) ? child1 : child2) == START){
                go_left_node = !go_left_node;
            }

            SnarlRecord snarl_record (is_snarl(child1) ? child1 : child2, &snarl_tree_records);
            handlegraph::nid_t snarl_node = ends_at(is_snarl(child1) ? child1 : child2) == START 
                             ? snarl_record.get_start_id() : snarl_record.get_end_id();
            bool go_left_snarl = ends_at(is_snarl(child1) ? child1 : child2) == START ;
                                
            if (node_record.get_node_id() == snarl_node && go_left_node != go_left_snarl) {
                //If the node is the boundary of the snarl facing in
//#ifdef debug_distances
//                cerr << "        => 0" << endl;
//#endif
                return 0;
            } else {
                //Otherwise, find the actual distance from the node to the correct boundary of the snarl,
                //and add the length of the boundary of the snarl, since it is not included in the chain distance
                NodeRecord snarl_node_record (get_offset_from_node_id(snarl_node), &snarl_tree_records);
//#ifdef debug_distances
//            cerr << "            => " << sum({chain_record.get_distance(
//                               make_tuple(node_record.get_rank_in_parent(), go_left_node, node_record.get_node_length()), 
//                               make_tuple(snarl_node_record.get_rank_in_parent(), go_left_snarl, snarl_node_record.get_node_length())) 
//                           , snarl_node_record.get_node_length()}) << endl;
//#endif
                return sum({chain_record.get_distance(
                               make_tuple(node_record.get_rank_in_parent(), go_left_node, node_record.get_node_length()), 
                               make_tuple(snarl_node_record.get_rank_in_parent(), go_left_snarl, snarl_node_record.get_node_length())) 
                           , snarl_node_record.get_node_length()});
            }
        } else {
            assert(is_snarl(child1));
            assert(is_snarl(child2));
            SnarlRecord child_record1 (child1, &snarl_tree_records);
            SnarlRecord child_record2 (child2, &snarl_tree_records);

            handlegraph::nid_t node_id1 = ends_at(child1) == START ? child_record1.get_start_id() : child_record1.get_end_id();
            handlegraph::nid_t node_id2 = ends_at(child2) == START ? child_record2.get_start_id() : child_record2.get_end_id();
            NodeRecord node_record1 (get_offset_from_node_id(node_id1), &snarl_tree_records);
            NodeRecord node_record2 (get_offset_from_node_id(node_id2), &snarl_tree_records); 

            bool go_left1 = ends_at(child1) == START;
            bool go_left2 = ends_at(child2) == START;

            if (node_id1 == node_id2 && go_left1 != go_left2) {
                //If the snarls are adjacent (and not the same snarl)
//#ifdef debug_distances
//            cerr << "            => " << node_record1.get_node_length() << endl;
//#endif

                return node_record1.get_node_length();
            } else {
//#ifdef debug_distances
//            cerr << "            => " << sum({chain_record.get_distance(
//                            make_tuple(node_record1.get_rank_in_parent(), go_left1, node_record1.get_node_length()), 
//                            make_tuple(node_record2.get_rank_in_parent(), go_left2, node_record2.get_node_length())) 
//                    , node_record1.get_node_length() , node_record2.get_node_length()}) << endl;
//#endif
                return sum({chain_record.get_distance(
                            make_tuple(node_record1.get_rank_in_parent(), go_left1, node_record1.get_node_length()), 
                            make_tuple(node_record2.get_rank_in_parent(), go_left2, node_record2.get_node_length())) 
                    , node_record1.get_node_length() , node_record2.get_node_length()});
            }
        }

    } else if (is_snarl(parent)) {
        SnarlRecord snarl_record(get_parent(child1), &snarl_tree_records);
        SnarlTreeRecord child_record1 (child1, &snarl_tree_records);
        SnarlTreeRecord child_record2 (child2, &snarl_tree_records);
        size_t rank1, rank2; bool rev1, rev2;
        if (is_sentinel(child1)) {
            rank1 = starts_at(child1) == START ? 0 : 1;
            rev1 = false;
        } else {
            rank1 = child_record1.get_rank_in_parent();
            rev1 = ends_at(child1) == END;
        }
        if (is_sentinel(child2)) {
            rank2 = starts_at(child2) == START ? 0 : 1;
            rev2 = false;
        } else {
            rank2 = child_record2.get_rank_in_parent();
            rev2 = ends_at(child2) == END;
        }
        if ((is_sentinel(child1) && starts_at(child1) == ends_at(child1)) ||
            (is_sentinel(child2) && starts_at(child2) == ends_at(child2)) ) {
            //If this is a sentinel pointing out of the snarl
//#ifdef debug_distances
//            cerr << "            => " << std::numeric_limits<size_t>::max() << endl;
//#endif
            return std::numeric_limits<size_t>::max();
        }

//#ifdef debug_distances
//            cerr << "             between ranks " << rank1 << " " << rev1 << " " << rank2 << " " << rev2 << endl;
//            cerr << "            => " << snarl_record.get_distance(rank1, rev1, rank2, rev2) << endl;
//#endif
//
        if (snarl_record.get_record_type() == OVERSIZED_SNARL 
            && !(rank1 == 0 || rank1 == 1 || rank2 == 0 || rank2 == 1) 
            && graph != nullptr) {
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

            
        } else {
           return snarl_record.get_distance(rank1, rev1, rank2, rev2);
        }
    } else {
        throw runtime_error("error: Trying to find distance in the wrong type of handle");
    }
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
    net_handle_t net1 = get_net_handle(get_offset_from_node_id(id1), START_END);
    net_handle_t net2 = get_net_handle(get_offset_from_node_id(id2), START_END);
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
    assert(is_node(net) || is_sentinel(net));
    if (is_node(net)) {
        return NodeRecord(net, &snarl_tree_records).get_node_length();
    } else if (is_sentinel(net)) {
        //If this is the sentinel of a snarl, return the node length
        //TODO: It might be better to store the node lengths in the chains
        //This would use more memory, but it would mean a faster lookup since you wouldn't have
        //to go back down to the level of nodes. But at some point you would probably have 
        //encountered those nodes anyway, so it might not be that expensive to do it this way
        SnarlRecord snarl_record(net, &snarl_tree_records);
        NodeRecord node_record;
        if (get_start_endpoint(net) == START) {
            node_record = NodeRecord(get_offset_from_node_id(snarl_record.get_start_id()), &snarl_tree_records);
        } else {
            node_record = NodeRecord(get_offset_from_node_id(snarl_record.get_end_id()), &snarl_tree_records);
        }
        return node_record.get_node_length();
    } else {
        throw runtime_error("error: Looking for the node length of a non-node net_handle_t");
    }

}


//TODO: This is kind of redundant with node_length 
size_t SnarlDistanceIndex::minimum_length(const net_handle_t& net) const {
        return SnarlTreeRecord(net, &snarl_tree_records).get_min_length();
}
nid_t SnarlDistanceIndex::node_id(const net_handle_t& net) const {
    assert(is_node(net) || is_sentinel(net));
    if (is_node(net)) {
        return NodeRecord(net, &snarl_tree_records).get_node_id();
    } else if (is_sentinel(net)) {
        SnarlRecord snarl_record(net, &snarl_tree_records);
        NodeRecord node_record;
        if (get_start_endpoint(net) == START) {
            return snarl_record.get_start_id();
        } else {
            return snarl_record.get_end_id();
        }
    } else {
        throw runtime_error("error: Looking for the node length of a non-node net_handle_t");
    }

}
bool SnarlDistanceIndex::has_node(const nid_t id) const {
    return snarl_tree_records->at(get_offset_from_node_id(id)) != 0;
}
net_handle_t SnarlDistanceIndex::get_node_net_handle(const nid_t id) const {
    return get_net_handle(get_offset_from_node_id(id), START_END, NODE_HANDLE); 
}

size_t SnarlDistanceIndex::get_max_tree_depth() const {
    return RootRecord(get_root(), &snarl_tree_records).get_max_tree_depth();
}

size_t SnarlDistanceIndex::get_depth(const net_handle_t& net) const {
    if (is_root(net)) {
        return 0;
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
bool SnarlDistanceIndex::has_external_connectivity(const net_handle_t& net, endpoint_t start, endpoint_t end) const {
    SnarlTreeRecord record(net, &snarl_tree_records);
    return record.has_external_connectivity(start, end);
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
    if (type == DISTANCED_NODE) {
        return (*records)->at(record_offset + NODE_LENGTH_OFFSET);
    } else if (type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        val =  (*records)->at(record_offset + SNARL_MIN_LENGTH_OFFSET);
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
    if (type == DISTANCED_NODE) {
        val = (*records)->at(record_offset + NODE_LENGTH_OFFSET);
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
        return (*records)->at(record_offset + NODE_RANK_OFFSET) >> 1;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
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
        return (*records)->at(record_offset + NODE_RANK_OFFSET) & 1;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
        return (*records)->at(record_offset + SNARL_RANK_OFFSET) & 1;
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
        return get_id_from_offset(record_offset);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        return ((*records)->at(record_offset + SNARL_START_NODE_OFFSET)) >> 1;
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
    } else if (type == NODE || type == DISTANCED_NODE) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the start of a node" << endl;
        return false;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        return ((*records)->at(record_offset + SNARL_START_NODE_OFFSET) & 1);
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
    } else if (type == NODE || type == DISTANCED_NODE) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the end of a node" << endl;
        //TODO: Put this in its own function? Also double check for off by ones
        //Offset of the start of the node vector
        return get_id_from_offset(record_offset);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        return ((*records)->at(record_offset + SNARL_END_NODE_OFFSET)) >> 1;
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
    } else if (type == NODE || type == DISTANCED_NODE) {
        //TODO: I'm not sure if I want to allow this
        //cerr << "warning: Looking for the end of a node" << endl;
        //TODO: Put this in its own function? Also double check for off by ones
        //Offset of the start of the node vector
        return get_id_from_offset(record_offset);
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        return ((*records)->at(record_offset + SNARL_END_NODE_OFFSET)) & 1;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        return ((*records)->at(record_offset + CHAIN_END_NODE_OFFSET)) & 1;
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: trying to find the end node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
}

size_t SnarlDistanceIndex::SnarlTreeRecord::get_offset_from_id (const handlegraph::nid_t id) const {
    size_t node_records_offset = (*records)->at(COMPONENT_COUNT_OFFSET) + ROOT_RECORD_SIZE;
    size_t offset = (id-(*records)->at(MIN_NODE_ID_OFFSET)) * NODE_RECORD_SIZE;
    return node_records_offset + offset;

}
handlegraph::nid_t SnarlDistanceIndex::SnarlTreeRecord::get_id_from_offset(size_t offset) const {
    size_t min_node_id = (*records)->at(MIN_NODE_ID_OFFSET);
    size_t node_records_offset = (*records)->at(COMPONENT_COUNT_OFFSET) + ROOT_RECORD_SIZE;
    return ((offset-node_records_offset) / NODE_RECORD_SIZE) + min_node_id;
}


bool SnarlDistanceIndex::SnarlTreeRecord::has_connectivity(endpoint_t start, endpoint_t end){
    return has_connectivity(endpoints_to_connectivity(start, end));
}

bool SnarlDistanceIndex::SnarlTreeRecord::has_external_connectivity(endpoint_t start, endpoint_t end) {
    if (start == START && end == START) {
        return is_externally_start_start_connected();
    } else if (start == END && end == END) {
        return is_externally_end_end_connected();
    } else if ( (start == START && end == END ) || (start == END && end == START)) {
        return is_externally_start_end_connected();
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
#ifdef debug_indexing
    cerr << record_offset << " set start_start_connected" << endl;
#endif    

    (*records)->at(record_offset) = (*records)->at(record_offset) | 32;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_end_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set start_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 16;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_tip_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set start_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 8;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_end_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set end_end_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 4;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_tip_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set end_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 2;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_tip_tip_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set tpi_tip_connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 1;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_start_end_connected() {
#ifdef debug_indexing
    cerr << record_offset << " set externally start_end connected" << endl;
#endif

    (*records)->at(record_offset) = (*records)->at(record_offset) | 64;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_start_start_connected() const {
#ifdef debug_indexing
    cerr << record_offset << " set externally start_start connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 128;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_externally_end_end_connected() const {
#ifdef debug_indexing
    cerr << record_offset << " set externally end_end connected" << endl;
#endif
    (*records)->at(record_offset) = (*records)->at(record_offset) | 256;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_record_type(record_t type) {
#ifdef debug_indexing
    cerr << record_offset << " set record type to be " << type << endl;
    assert((*records)->at(record_offset) == 0);
#endif
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
    } else if (type == NODE || type == SNARL || type == CHAIN ) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    } else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the length of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
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
    } else if (type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_MAX_LENGTH_OFFSET;
    } else if (type == DISTANCED_NODE || type == SNARL || type == CHAIN) {
        throw runtime_error("error: trying to access get distance in a distanceless index");
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the length of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
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
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
        offset = record_offset + SNARL_RANK_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_RANK_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
    cerr << offset << " set rank in parent to be " << rank << endl;
    assert((*records)->at(offset) >> 1 == 0);
#endif

    bool rev = (*records)->at(offset) & 1;
    (*records)->at(offset) = (rank << 1) | rev;
};
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_is_reversed_in_parent(bool rev) {
    record_t type = get_record_type();
    size_t offset;
    if (type == NODE || type == DISTANCED_NODE) {
        offset = record_offset + NODE_RANK_OFFSET;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
        offset = record_offset + SNARL_RANK_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_RANK_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
    cerr << offset << " set rev in parent to be " << rev << endl;
#endif

    (*records)->at(offset) =  (((*records)->at(offset)>>1)<<1) | rev;
};
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_parent_record_offset(size_t pointer){
    record_t type = get_record_type();
    size_t offset;
    if (type == NODE || type == DISTANCED_NODE) {
        offset = record_offset + NODE_PARENT_OFFSET;
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL
            || type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL)  {
#ifdef debug_indexing
        if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
            assert(pointer == 0);
        }
#endif

        offset = record_offset + SNARL_PARENT_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_PARENT_OFFSET;
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
    cerr << offset << " set parent offset to be " << pointer << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = pointer;
};
//Rev is true if the node is traversed backwards to enter the snarl
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_start_node(handlegraph::nid_t id, bool rev) {
    record_t type = get_record_type();
    size_t offset;
    if (type == ROOT || type == NODE || type == DISTANCED_NODE) {
        throw runtime_error("error: trying to set the node id of a node or root");
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        offset = record_offset + SNARL_START_NODE_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_START_NODE_OFFSET;
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the start node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
    cerr << offset << " set start node to be " << id << " facing " << (rev ? "rev" : "fd") << endl;
    assert((*records)->at(offset) == 0);
#endif

    (*records)->at(offset) = (id << 1) | rev;
}
void SnarlDistanceIndex::SnarlTreeRecordConstructor::set_end_node(handlegraph::nid_t id, bool rev) const {
    record_t type = get_record_type();
    size_t offset;
    if (type == ROOT || type == NODE || type == DISTANCED_NODE) {
        throw runtime_error("error: trying to set the node id of a node or root");
    } else if (type == SNARL || type == DISTANCED_SNARL || type == OVERSIZED_SNARL)  {
        offset = record_offset + SNARL_END_NODE_OFFSET;
    } else if (type == CHAIN || type == DISTANCED_CHAIN || type == MULTICOMPONENT_CHAIN)  {
        offset = record_offset + CHAIN_END_NODE_OFFSET;
    }  else if (type == ROOT_SNARL || type == DISTANCED_ROOT_SNARL) {
        throw runtime_error("error: set the end node of a root snarl");
    } else {
        throw runtime_error("error: trying to access a snarl tree node of the wrong type");
    }
#ifdef debug_indexing
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
#ifdef debug_indexing
    cerr << RootRecord::record_offset+COMPONENT_COUNT_OFFSET << " set connected component to be " << connected_component_count << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+COMPONENT_COUNT_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+COMPONENT_COUNT_OFFSET)=connected_component_count;
}
void SnarlDistanceIndex::RootRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_indexing
    cerr << RootRecord::record_offset+NODE_COUNT_OFFSET << " set node count to be " << node_count << endl;
    assert((*RootRecord::records)->at(RootRecord::record_offset+NODE_COUNT_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+NODE_COUNT_OFFSET)=node_count;
}
void SnarlDistanceIndex::RootRecordConstructor::set_max_tree_depth(size_t tree_depth) {
#ifdef debug_indexing
    cerr << RootRecord::record_offset+MAX_TREE_DEPTH_OFFSET << " set max tree depth to be " << tree_depth << endl;
    assert((*RootRecord::records)->at(RootRecord::record_offset+MAX_TREE_DEPTH_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+MAX_TREE_DEPTH_OFFSET)=tree_depth;
}
void SnarlDistanceIndex::RootRecordConstructor::set_min_node_id(handlegraph::nid_t node_id) {
#ifdef debug_indexing
    cerr << RootRecord::record_offset+MIN_NODE_ID_OFFSET << " set min node id to be " << node_id << endl;
    assert((*RootRecord::records)->at(RootRecord::record_offset+MIN_NODE_ID_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+MIN_NODE_ID_OFFSET)=node_id;
}
void SnarlDistanceIndex::RootRecordConstructor::add_component(size_t index, size_t offset) {
#ifdef debug_indexing
    cerr << RootRecord::record_offset+ROOT_RECORD_SIZE+index << " set new component " << offset << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+ROOT_RECORD_SIZE+index) == 0);
    assert(index < get_connected_component_count());
#endif

    (*SnarlTreeRecordConstructor::records)->at(RootRecord::record_offset+ROOT_RECORD_SIZE+index)
        = offset;

}


size_t SnarlDistanceIndex::SnarlRecord::distance_vector_size(record_t type, size_t node_count, size_t distance_values_per_vector_element, size_t snarl_tree_record_bit_width) {
    if (type == SNARL || type == ROOT_SNARL){
        //For a normal snarl, its just the record size and the pointers to children
        return 0;
    } else if (type == DISTANCED_SNARL) {
        //For a normal min distance snarl, record size and the pointers to children, and
        //matrix of distances
        size_t node_side_count = node_count * 2 + 2;
        size_t vector_size =  (((node_side_count+1)*node_side_count) / 2);
        return vector_size / distance_values_per_vector_element + 
                (vector_size % distance_values_per_vector_element == 0 ? 0 : 1);
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

size_t SnarlDistanceIndex::SnarlRecord::record_size (record_t type, size_t node_count, size_t distance_values_per_vector_element, size_t snarl_tree_record_bit_width) {
    //TODO: I don't think this should include the extra node_count?
    return SNARL_RECORD_SIZE + distance_vector_size(type, node_count, distance_values_per_vector_element, snarl_tree_record_bit_width) + node_count;
}
size_t SnarlDistanceIndex::SnarlRecord::record_size() {
    record_t type = get_record_type();
   return record_size(type, get_node_count(), get_distance_values_per_vector_element(), (*records)->width());
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
            return node_count + 2 + rank2;
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

void SnarlDistanceIndex::SnarlRecordConstructor::set_distance_bit_width(size_t max_distance_value) {

    size_t val = bit_width(max_distance_value);
#ifdef debug_indexing
    cerr <<  SnarlRecord::record_offset + SNARL_BIT_WIDTH_OFFSET << " set distance bit width " << val << endl;
#endif
    (*SnarlTreeRecordConstructor::records)->at(SnarlRecord::record_offset + + SNARL_BIT_WIDTH_OFFSET) = val;
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance_bit_width() const {

    return (*SnarlTreeRecord::records)->at(SnarlRecord::record_offset + SNARL_BIT_WIDTH_OFFSET);
}
size_t SnarlDistanceIndex::SnarlRecord::get_distance_values_per_vector_element() const {
    return (*SnarlTreeRecord::records)->width() / get_distance_bit_width();
}

pair<size_t, size_t> SnarlDistanceIndex::SnarlRecord::get_offset_of_distance_entry_in_vector_and_element(size_t distance_vector_offset) const {
    //How many bits are we using to store this value?
    size_t distance_bit_width = get_distance_bit_width();

    //And how many distances are we storing in each slot in snarl_tree_records
    size_t distances_per_element = get_distance_values_per_vector_element();
    assert ((distances_per_element * distance_bit_width ) <= (size_t)(*SnarlTreeRecord::records)->width());

    //offset of the start of the distance vectors in snarl_tree_records
    size_t distance_vector_start = SnarlRecord::record_offset + SNARL_RECORD_SIZE;

    //Offset of the element in snarl_tree_records that this value gets stored in
    size_t snarl_tree_record_vector_offset = distance_vector_offset / distances_per_element;

    //How many distances are to the left of the distance we're inserting
    size_t distances_to_the_left = distance_vector_offset % distances_per_element;

    //How many bits are to the right of the distance we're inserting
    size_t bits_to_the_right =  (*SnarlTreeRecord::records)->width() - 
        ((distances_to_the_left+1)* distance_bit_width);

    assert(bits_to_the_right <= (size_t)(*SnarlTreeRecord::records)->width()-distance_bit_width);
    assert((distances_to_the_left*distance_bit_width) + distance_bit_width + bits_to_the_right == (size_t)(*SnarlTreeRecord::records)->width());

    return make_pair(snarl_tree_record_vector_offset+distance_vector_start, bits_to_the_right);

}

void SnarlDistanceIndex::SnarlRecordConstructor::set_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2, size_t distance) {

#ifdef debug_indexing 
    assert((distance >> get_distance_bit_width()) == 0);
#endif
    //This distance hasn't been set yet (or it's equal to what we're setting)
    assert((get_distance(rank1, right_side1, rank2, right_side2) == std::numeric_limits<size_t>::max()
           || get_distance(rank1, right_side1, rank2, right_side2) == distance));
    //This distance can fit in the bit width we've chose
    assert(distance < (1 << get_distance_bit_width())-1);
#ifdef debug_indexing
    //This distance hasn't been set yet (or it's equal to what we're setting)
    assert((get_distance(rank1, right_side1, rank2, right_side2) == std::numeric_limits<size_t>::max() ||
           get_distance(rank1, right_side1, rank2, right_side2) == distance));
    //This distance can fit in the bit width we've chose
    assert(distance < (1 << get_distance_bit_width())-1);
#endif

    //Offset of this particular distance in the distance vector
    size_t distance_vector_offset = get_distance_vector_offset(rank1, right_side1, rank2, right_side2);

    //Get the offsets for packing the value into fewer bits
    pair<size_t, size_t> offsets = get_offset_of_distance_entry_in_vector_and_element(distance_vector_offset);

    //Offset of the element in snarl_tree_records that this value gets stored in
    size_t snarl_tree_record_vector_offset = offsets.first;
    //How many bits are to the right of the distance we're inserting
    size_t bits_to_the_right =  offsets.second;


    //Value we actually want to save
    size_t val = distance == std::numeric_limits<size_t>::max() ? 0 : distance+1;


    size_t old_value = (*SnarlTreeRecordConstructor::records)->at(snarl_tree_record_vector_offset);
    size_t new_value = old_value | (val << bits_to_the_right);

    (*SnarlTreeRecordConstructor::records)->at(snarl_tree_record_vector_offset) = new_value;
}

size_t SnarlDistanceIndex::SnarlRecord::get_distance(size_t rank1, bool right_side1, size_t rank2, bool right_side2) const {

    //Offset of this particular distance in the distance vector
    size_t distance_vector_offset = get_distance_vector_offset(rank1, right_side1, rank2, right_side2);


    //Get the offsets for packing the distances into fewer bits
    pair<size_t, size_t> offsets = get_offset_of_distance_entry_in_vector_and_element(distance_vector_offset);

    //Offset of the element in snarl_tree_records that this value gets stored in
    size_t snarl_tree_record_vector_offset = offsets.first;
    //How many bits are to the right of the distance we're inserting
    size_t bits_to_the_right =  offsets.second;

    size_t entry = (*records)->at(snarl_tree_record_vector_offset);

    size_t bit_mask = (1 << get_distance_bit_width())-1;
    size_t val = (entry >> bits_to_the_right) & bit_mask;

    return  val == 0 ? std::numeric_limits<size_t>::max() : val-1;

}
size_t SnarlDistanceIndex::SnarlRecord::get_node_count() const {
    return (*records)->at(record_offset + SNARL_NODE_COUNT_OFFSET);
}

//Node count is the number of nodes in the snarl, not including boundary nodes
void SnarlDistanceIndex::SnarlRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_indexing
    cerr << SnarlRecord::record_offset + SNARL_NODE_COUNT_OFFSET << " set snarl node count " << node_count << endl;
    assert(node_count > 0);//TODO: Don't bother making a record for trivial snarls
    assert((*SnarlTreeRecordConstructor::records)->at(SnarlRecord::record_offset + SNARL_NODE_COUNT_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(SnarlRecord::record_offset + SNARL_NODE_COUNT_OFFSET) = node_count;
}

size_t SnarlDistanceIndex::SnarlRecord::get_child_record_pointer() const {
    return (*records)->at(record_offset+SNARL_CHILD_RECORD_OFFSET) ;
}

void SnarlDistanceIndex::SnarlRecordConstructor::set_child_record_pointer(size_t pointer) {
#ifdef debug_indexing
    cerr << SnarlRecord::record_offset+SNARL_CHILD_RECORD_OFFSET << " set snarl child record pointer " << pointer << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(SnarlRecord::record_offset+SNARL_CHILD_RECORD_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(SnarlRecord::record_offset+SNARL_CHILD_RECORD_OFFSET) = pointer;
}

//Add a reference to a child of this snarl. Assumes that the index is completed up
//to here
void SnarlDistanceIndex::SnarlRecordConstructor::add_child(size_t pointer){
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << " Adding child pointer to the end of the array " << endl;
#endif
    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+1);
    (*SnarlTreeRecordConstructor::records)->at(start_i) = pointer;
}

size_t SnarlDistanceIndex::ChainRecord::get_node_count() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        return 1;
    } else {
        return (*records)->at(record_offset + CHAIN_NODE_COUNT_OFFSET);
    }
}

pair<size_t, bool> SnarlDistanceIndex::ChainRecord::get_last_child_offset() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get children of a node");
    } else {
        size_t val = (*records)->at(record_offset + CHAIN_LAST_CHILD_OFFSET);
        return make_pair(val>>2, (val>>1) & 1);
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

size_t SnarlDistanceIndex::ChainRecord::get_chain_node_id(size_t pointer) const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
    return (*records)->at(pointer+CHAIN_NODE_ID_OFFSET);
}
size_t SnarlDistanceIndex::ChainRecord::get_prefix_sum_value(size_t pointer) const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
    size_t val = (*records)->at(pointer+CHAIN_NODE_PREFIX_SUM_OFFSET);
    return val == 0 ? std::numeric_limits<size_t>::max() : val-1;
}
size_t SnarlDistanceIndex::ChainRecord::get_forward_loop_value(size_t pointer) const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
    size_t val = (*records)->at(pointer+CHAIN_NODE_FORWARD_LOOP_OFFSET);
    return val == 0 ? std::numeric_limits<size_t>::max() : val-1;
}
size_t SnarlDistanceIndex::ChainRecord::get_reverse_loop_value(size_t pointer) const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
    size_t val =  (*records)->at(pointer+CHAIN_NODE_REVERSE_LOOP_OFFSET);
    return val == 0 ? std::numeric_limits<size_t>::max() : val-1;
}
size_t SnarlDistanceIndex::ChainRecord::get_chain_component(size_t pointer, bool get_end) const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    }
    if (get_record_type() != MULTICOMPONENT_CHAIN){
        throw runtime_error("error: Trying to get the component of a single component chain");
    }
    if (get_chain_node_id(pointer) == get_start_id() && !get_end) {
        //The first component is always 0
        return 0;
    } else {
        return (*records)->at(pointer+CHAIN_NODE_COMPONENT_OFFSET);
    }
}

size_t SnarlDistanceIndex::ChainRecord::get_distance(tuple<size_t, bool, size_t> node1,
                     tuple<size_t, bool, size_t> node2) const {

    if (std::get<0>(node1) > std::get<0>(node2)) {
        //If the first node comes after the second in the chain, reverse them
        tuple<size_t, bool, size_t> tmp = node1;
        node1 = node2;
        node2 = tmp;

    }

    bool is_looping_chain = get_start_id() == get_end_id();
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    } else if (get_record_type() == MULTICOMPONENT_CHAIN) {
        if (get_chain_component(std::get<0>(node1)) != get_chain_component(std::get<0>(node2))) {
            if (is_looping_chain) {
                //If this is a looping chain, then the first/last node could be in two
                //components
                return get_distance_taking_chain_loop(node1, node2);
            } else {
                return std::numeric_limits<size_t>::max();
            }
        }
    }


    size_t distance;

    if (!std::get<1>(node1) && std::get<1>(node2)) {
        //Right of 1 and left of 2, so a simple forward traversal of the chain
        if (std::get<0>(node1) == std::get<0>(node2)) {
            //If these are the same node, then the path would need to go around the node
            distance = sum({get_forward_loop_value(std::get<0>(node1)),
                        get_reverse_loop_value(std::get<0>(node1)),
                        std::get<2>(node1)});
        } else {
            distance = minus(get_prefix_sum_value(std::get<0>(node2)) - get_prefix_sum_value(std::get<0>(node1)),
                 std::get<2>(node1));
        }
    } else if (!std::get<1>(node1) && !std::get<1>(node2)) {
        //Right side of 1 and right side of 2
        if (std::get<0>(node1) == std::get<0>(node2)) {
            distance = get_forward_loop_value(std::get<0>(node2));

        } else {
            distance = minus( sum({get_prefix_sum_value(std::get<0>(node2)) - get_prefix_sum_value(std::get<0>(node1)) ,
                               std::get<2>(node2),
                               get_forward_loop_value(std::get<0>(node2))}),
                         std::get<2>(node1));
        }
    } else if (std::get<1>(node1) && std::get<1>(node2)) {
        //Left side of 1 and left side of 2
        if (std::get<0>(node1) == std::get<0>(node2)) {
            distance = get_reverse_loop_value(std::get<0>(node1));

        } else {
            distance = sum({get_prefix_sum_value(std::get<0>(node2)) - get_prefix_sum_value(std::get<0>(node1)),
                            get_reverse_loop_value(std::get<0>(node1))});
        }
    } else {
        assert(std::get<1>(node1) && !std::get<1>(node2));
        //Left side of 1 and right side of 2
        distance = sum({get_prefix_sum_value(std::get<0>(node2)) - get_prefix_sum_value(std::get<0>(node1)),
                        get_reverse_loop_value(std::get<0>(node1)),
                        get_forward_loop_value(std::get<0>(node2)),
                        std::get<2>(node2)});

    }
    if (is_looping_chain) {
        distance = std::min(distance, get_distance_taking_chain_loop(node1, node2));
    }
    return distance;
}


size_t SnarlDistanceIndex::ChainRecord::get_distance_taking_chain_loop(tuple<size_t, bool, size_t> node1,
                     tuple<size_t, bool, size_t> node2) const {
    //Each node is a tuple of <pointer, left_side, and length of the node>
    //This is only called by get_distance, so the nodes should be ordered
    assert (std::get<0>(node1) <= std::get<0>(node2));

    /*Note: Because we assume that the nodes are ordered and that we want to take the loop in the chain,
     * we leave the start node going left (either by going left or taking a loop to the right), and
     * enter the end node from the right (from the right or from the left and then looping)
     */

    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain distances from a node");
    } else if (get_record_type() == MULTICOMPONENT_CHAIN) {
        size_t last_component = get_chain_component(get_first_node_offset(), true);
        bool first_in_first_component = get_chain_component(std::get<0>(node1)) == 0 || get_chain_component(std::get<0>(node1)) == last_component;
        bool second_in_first_component = get_chain_component(std::get<0>(node2)) == 0 || get_chain_component(std::get<0>(node2)) == last_component;
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
        bool end_at_right_of_first = get_chain_component(std::get<0>(node2)) == 0 && !std::get<1>(node2);
        bool start_at_left_of_last = get_chain_component(std::get<0>(node1)) == last_component && std::get<1>(node1); 
        if (end_at_right_of_first || start_at_left_of_last) {
            return std::numeric_limits<size_t>::max();
        }
    }


    size_t distance;
    assert(get_start_id() == get_end_id());

    if (!std::get<1>(node1) && std::get<1>(node2)) {
        //Right of 1 and left of 2, so a simple forward traversal of the chain
        //loop forward from the first node, from the start of the chain to the first
        //node, from the end of the node to the second node, and the reverse loop of the second
        distance = sum({get_forward_loop_value(std::get<0>(node1)),
                             std::get<2>(node1),
                             get_prefix_sum_value(std::get<0>(node1)),
                             minus(get_min_length(), get_prefix_sum_value(std::get<0>(node2))),
                             get_reverse_loop_value(std::get<0>(node2))});
    } else if (!std::get<1>(node1) && !std::get<1>(node2)) {
        //Right side of 1 and right side of 2

        //Check distance for taking loop in chain: loop forward from the first node, from the start of the
        //chain to the first node, from the end of the node to the second node
        distance = sum({get_forward_loop_value(std::get<0>(node1)),
                             std::get<2>(node1),
                             get_prefix_sum_value(std::get<0>(node1)),
                             minus(minus(get_min_length(), get_prefix_sum_value(std::get<0>(node2))),
                                std::get<2>(node2))});
    } else if (std::get<1>(node1) && std::get<1>(node2)) {
        //Left side of 1 and left side of 2

        //from the first node left to the start, around the
        //chain loop, then the reverse loop of the second node
        //This assumes that the length of the chain only includes the start/end node's length once,
        //which it does but might change
        distance = sum({get_prefix_sum_value(std::get<0>(node1)),
                             minus(get_min_length(), get_prefix_sum_value(std::get<0>(node2))),
                             get_reverse_loop_value(std::get<0>(node2))});
    } else {
        assert(std::get<1>(node1) && !std::get<1>(node2));
        //Left side of 1 and right side of 2

        //Check the distance going backwards around the chain
        distance = sum({get_prefix_sum_value(std::get<0>(node1)),
                             minus(minus(get_min_length(), get_prefix_sum_value(std::get<0>(node2))),
                              std::get<2>(node2))});
    }
    return distance;
}

size_t SnarlDistanceIndex::ChainRecord::get_first_node_offset() const {
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain traversal from a node");
    }
    return record_offset + CHAIN_RECORD_SIZE;
}

pair<size_t, bool> SnarlDistanceIndex::ChainRecord::get_next_child(const pair<size_t, bool> pointer, bool go_left) const {
    //If this is a multicomponent chain, then the size of each node record in the chain is bigger
    if (get_record_handle_type() == NODE_HANDLE) {
        throw runtime_error("error: Trying to get chain traversal from a node");
    }
    size_t node_record_size = get_record_type() == MULTICOMPONENT_CHAIN ? CHAIN_NODE_MULTICOMPONENT_RECORD_SIZE : CHAIN_NODE_RECORD_SIZE;
    if (pointer.second) {
        //This is a snarl
        if (go_left) {
            return make_pair(pointer.first - node_record_size, false);
        } else {
            if (get_start_id() == get_end_id() && pointer.first == get_last_child_offset().first) {
                //If this is the last child in a looping chain
                return make_pair(get_first_node_offset(), false);
            } else {
                size_t snarl_record_length = SnarlRecord(pointer.first, records).record_size();
                return make_pair(pointer.first + snarl_record_length + 1, false);
            }
        }
    } else {
        //This is a node
        if (go_left) {
            //Looking left in the chain
            if (pointer.first == get_first_node_offset()) {
                //If this is the first node in the chain
                if (get_start_id() == get_end_id()) {
                    pair<size_t, bool> last_child = get_last_child_offset();
                    return last_child; //TODO: I'm not sure if this is always true (snarl)
                } else {
                    return make_pair(0, false);
                }
            }
            size_t snarl_record_size = (*records)->at(pointer.first-1);
            if (snarl_record_size == 0) {
                //Just another node to the left
                return make_pair(pointer.first-node_record_size, false);
            } else {
                //There is a snarl to the left of this node
                return make_pair(pointer.first - snarl_record_size - 1, true);
            }
        } else {
            //Looking right in the chain
            if ((*records)->at(pointer.first+node_record_size-1) == 0 &&
                (*records)->at(pointer.first+node_record_size) == 0) {
                //If this is the last node in the chain
                if (get_start_id() == get_end_id()) {
                    //If this loops, go back to the beginning
                    return make_pair(get_first_node_offset(), false);
                } else {
                    return make_pair(0, false);
                }
            }
            size_t snarl_record_size = (*records)->at(pointer.first+node_record_size-1);
            return make_pair(pointer.first+node_record_size, snarl_record_size != 0);
        }
    }
}
net_handle_t SnarlDistanceIndex::ChainRecord::get_next_child(const net_handle_t& net_handle, bool go_left) const {
    //get the next child in the chain. net_handle must point to a snarl or node in the chain
    net_handle_record_t handle_type = get_handle_type(net_handle);
    net_handle_record_t record_type = get_record_handle_type();
    if (record_type == NODE_HANDLE) {
        //If this record is a node pretending to be a chain, then there is no next child
        assert(handle_type == NODE_HANDLE);
        assert(get_record_offset(net_handle) == record_offset);
        return net_handle;
    }

    //Get the current pointer, pointing at the net_handle in the chain
    pair<size_t, bool> next_pointer;
    if (handle_type == NODE_HANDLE) {
        //If this net handle is a node, then it's rank in parent points to it in the chain
        NodeRecord node_record(get_record_offset(net_handle), records);
        assert(node_record.get_parent_record_offset() == record_offset);
        next_pointer = get_next_child(
            make_pair(node_record.get_rank_in_parent(), false), go_left);
        if (next_pointer.first == 0 ){
            return net_handle;
        }
    } else {
        //Otherwise, it is a snarl and we can use the snarl's offset, since it exists in
        //the chain
        assert(handle_type == SNARL_HANDLE) ;
        next_pointer = get_next_child(
            make_pair(get_record_offset(net_handle), true), go_left);
        if (next_pointer.first == 0 ){
            return net_handle;
        }
    }
    bool next_is_reversed_in_parent = next_pointer.second ? false : NodeRecord(
            get_offset_from_id((*records)->at(next_pointer.first)), records
        ).get_is_reversed_in_parent();

    connectivity_t connectivity = go_left == next_is_reversed_in_parent ? START_END : END_START;
    if (!next_pointer.second && next_pointer.first == get_first_node_offset()) {
        connectivity = endpoints_to_connectivity(get_end_endpoint(connectivity), get_end_endpoint(connectivity));
    }
    return get_net_handle(next_pointer.second ? next_pointer.first : get_offset_from_id((*records)->at(next_pointer.first)),
                          connectivity,
                          next_pointer.second ? SNARL_HANDLE : NODE_HANDLE);
}

bool SnarlDistanceIndex::ChainRecord::for_each_child(const std::function<bool(const net_handle_t&)>& iteratee) const {

    if (get_record_handle_type() == NODE_HANDLE) {
        //If this is a node pretending to be a chain, just do it for the node
        return iteratee(get_net_handle(record_offset, START_END, NODE_HANDLE));
    }


    //If this is a node, then the offset of the node in the chain, false
    //If it is a snarl, then the offset of the snarl record, true
    pair<size_t, bool> current_child (get_first_node_offset(), false);
    bool is_first = true;

    while (current_child.first != 0) {
        net_handle_t child_handle = current_child.second 
            ? get_net_handle (current_child.first, START_END, SNARL_HANDLE)
            : get_net_handle (get_offset_from_id((*records)->at(current_child.first)), START_END, NODE_HANDLE);
        if (!is_first && current_child == make_pair(get_first_node_offset(), false)){
            //Don't look at the first node a second time
            return true;
        }

        bool result = iteratee(child_handle);
        if (result == false) {
            return false;
        }
        current_child = get_next_child(current_child, false);
        is_first = false;
    }
    return true;
}


void SnarlDistanceIndex::ChainRecordConstructor::add_node(handlegraph::nid_t id, size_t prefix_sum, size_t forward_loop, size_t reverse_loop) {
    assert(ChainRecord::get_record_type() != MULTICOMPONENT_CHAIN);
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << " - " << (*SnarlTreeRecordConstructor::records)->size() + 3 << " Adding chain's child node " << id << " to the end of the a     rray (values: " << prefix_sum << "," << forward_loop << "," << reverse_loop << ")" << endl;
#endif

    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+4);

    (*SnarlTreeRecordConstructor::records)->at(start_i) = id;
    (*SnarlTreeRecordConstructor::records)->at(start_i+1) = 
            prefix_sum==std::numeric_limits<size_t>::max() ? 0 : prefix_sum+1;
    (*SnarlTreeRecordConstructor::records)->at(start_i+2) = 
            forward_loop==std::numeric_limits<size_t>::max() ? 0 : forward_loop+1;
    (*SnarlTreeRecordConstructor::records)->at(start_i+3) = 
            reverse_loop==std::numeric_limits<size_t>::max() ? 0 : reverse_loop+1;
}
void SnarlDistanceIndex::ChainRecordConstructor::add_node(handlegraph::nid_t id, size_t prefix_sum, size_t forward_loop, size_t reverse_loop, size_t component) {
    assert(ChainRecord::get_record_type() == MULTICOMPONENT_CHAIN);
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << " - " << (*SnarlTreeRecordConstructor::records)->size() + 4 << " Adding chain's child node the end of the array " << endl;
#endif

    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+5);

    (*SnarlTreeRecordConstructor::records)->at(start_i) = id;
    (*SnarlTreeRecordConstructor::records)->at(start_i+1) = prefix_sum==std::numeric_limits<size_t>::max() ? 0 : prefix_sum+1;
    (*SnarlTreeRecordConstructor::records)->at(start_i+2) = forward_loop==std::numeric_limits<size_t>::max() ? 0 : forward_loop+1;
    (*SnarlTreeRecordConstructor::records)->at(start_i+3) = reverse_loop==std::numeric_limits<size_t>::max() ? 0 : reverse_loop+1;
    (*SnarlTreeRecordConstructor::records)->at(start_i+4) = component;
}
void SnarlDistanceIndex::ChainRecordConstructor::set_node_count(size_t node_count) {
#ifdef debug_indexing
    cerr << ChainRecord::record_offset + CHAIN_NODE_COUNT_OFFSET << " set chain node count " << node_count << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_NODE_COUNT_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_NODE_COUNT_OFFSET) = node_count;
}
size_t SnarlDistanceIndex::ChainRecord::get_depth() const {
    return (*records)->at(record_offset+CHAIN_DEPTH_OFFSET) ;
}
void SnarlDistanceIndex::ChainRecordConstructor::set_depth(size_t depth) {
#ifdef debug_indexing
    cerr << ChainRecord::record_offset + CHAIN_DEPTH_OFFSET << " set chain depth " << depth << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_DEPTH_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_DEPTH_OFFSET) = depth;
}

//The offset of the last child, if it is a snarl, and if it can loop
void SnarlDistanceIndex::ChainRecordConstructor::set_last_child_offset(size_t offset, bool is_snarl, bool loopable) {
#ifdef debug_indexing
    cerr << ChainRecord::record_offset + CHAIN_LAST_CHILD_OFFSET << " set chain last child offset " << offset << endl;
    assert((*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_LAST_CHILD_OFFSET) == 0);
#endif

    (*SnarlTreeRecordConstructor::records)->at(ChainRecord::record_offset + CHAIN_LAST_CHILD_OFFSET) = ((offset << 2) | (is_snarl<<1)) | loopable;
}

void SnarlDistanceIndex::ChainRecordConstructor::add_trivial_snarl() {
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << "  Adding chain's trivial snarl to the end of the array " << endl;
#endif
    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+1);

    (*SnarlTreeRecordConstructor::records)->at(start_i) = 0;
}
//Add a snarl to the end of the chain and return a SnarlRecordConstructor pointing to it
SnarlDistanceIndex::SnarlRecordConstructor SnarlDistanceIndex::ChainRecordConstructor::add_snarl(size_t snarl_size, record_t type, size_t max_snarl_distance) {

        size_t distance_values_per_vector_element = (*ChainRecord::records)->width() / bit_width(max_snarl_distance); 
    size_t snarl_record_size = SnarlRecord::record_size(type, snarl_size, distance_values_per_vector_element, (*ChainRecord::records)->width());
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << " Adding child snarl length to the end of the array " << endl;
#endif

    
    
    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+1);
    (*SnarlTreeRecordConstructor::records)->at(start_i) = snarl_record_size;
    (*SnarlTreeRecordConstructor::records)->reserve(start_i + snarl_record_size);
    SnarlRecordConstructor snarl_record(snarl_size, SnarlTreeRecordConstructor::records, type, max_snarl_distance);
    snarl_record.set_distance_bit_width(max_snarl_distance);
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size() << " Adding child snarl length to the end of the array " << endl;
#endif
    start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+1);
    (*SnarlTreeRecordConstructor::records)->at(start_i) = snarl_record_size;
    return snarl_record;
}
void SnarlDistanceIndex::ChainRecordConstructor::finish_chain(){
#ifdef debug_indexing
    cerr << (*SnarlTreeRecordConstructor::records)->size()  << " - " <<  (*SnarlTreeRecordConstructor::records)->size()+1 << " Adding the last two chain 0's to the end of the array " <<      endl;  
#endif

    size_t start_i = (*SnarlTreeRecordConstructor::records)->size();
    (*SnarlTreeRecordConstructor::records)->resize(start_i+2);
    (*SnarlTreeRecordConstructor::records)->at(start_i) = 0;
    (*SnarlTreeRecordConstructor::records)->at(start_i+1) = 0;
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
            return "node" + std::to_string( get_node_id_from_offset(get_record_offset(net))) + (ends_at(net) == START ? "rev" : "fd") + " that is the end node of a looping chain";
        }
        return  "node " + std::to_string( get_node_id_from_offset(get_record_offset(net))) + (ends_at(net) == START ? "rev" : "fd");
    } else if (type == SNARL_HANDLE) {
        if (record.get_record_type() == ROOT) {
            return "root snarl";
        }
        result += "snarl ";         
    } else if (type == CHAIN_HANDLE && record_type == NODE_HANDLE) {
        return  "node " + std::to_string( get_node_id_from_offset(get_record_offset(net)))
               + (ends_at(net) == START ? "rev" : "fd") + " pretending to be a chain";
    } else if (type == CHAIN_HANDLE) {
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
            child_count = record_type == CHAIN_HANDLE ? ChainRecord(net, &snarl_tree_records).get_node_count() 
                                                   : SnarlRecord(net, &snarl_tree_records).get_node_count();
                                
        }
        cout << net_handle_as_string(net) << "," << parent << "," << child_count << ","<< get_depth(net) << endl; 
        for_each_child(net, [&](const net_handle_t& child) {
            print_descendants_of(child);
        });
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
    if (record_type == NODE_HANDLE) {
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
    //Used for setting the width of ints in the int vector (snarl_tree_records)
    //This only counts distance values, since the max offset value will be the length
    //of the array, I'll check that right before setting the width

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
    size_t maximum_value = std::max(maximum_index_size, std::max(maximum_distance, (size_t) max_node_id));
    size_t new_width = std::max(bit_width(maximum_value), (size_t)13); //13 is the width of the tags
    snarl_tree_records->width(new_width);

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
                    bool prev_node = false;//Was the previous thing in the chain a node?
                    pair<size_t, bool> last_child_offset;

                    for (size_t child_record_index_i = 0 ; child_record_index_i < temp_chain_record.children.size() ; child_record_index_i++) {
                        const pair<temp_record_t, size_t>& child_record_index = temp_chain_record.children[child_record_index_i];
                        //Go through each node and snarl in the chain and add them to the index
#ifdef debug_distance_indexing
                        cerr << "  Adding child of the chain: " << temp_index->structure_start_end_as_string(child_record_index) << endl;
#endif

                        if (child_record_index.first == TEMP_NODE) {
                            //Add a node to the chain
                            if (prev_node) {
                                //If the last thing we saw was a node, then this is the end of a trivial snarl
                                chain_record_constructor.add_trivial_snarl();
#ifdef debug_distance_indexing
                                cerr << "    Also adding a trivial snarl before this node" << endl;
#endif
                            }
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
                                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                                    temp_index->temp_node_records[child_record_index.second-min_node_id];
                                //Fill in this node in the index 
                                record_t record_type = snarl_size_limit == 0 ? NODE : DISTANCED_NODE;
                                NodeRecordConstructor node_record_constructor(
                                    get_offset_from_node_id(temp_node_record.node_id), record_type, &snarl_tree_records);
                                node_record_constructor.set_node_length(temp_node_record.node_length);
                                node_record_constructor.set_parent_record_offset(chain_record_constructor.get_offset());
                                node_record_constructor.set_is_reversed_in_parent(temp_node_record.reversed_in_parent);

                                //TODO: This is not really used
                                //The "rank" of the node actually points to the node in the chain, so it is the
                                //current size of the records (before adding the node to the chain)
                                node_record_constructor.set_rank_in_parent(snarl_tree_records->size());
                                size_t node_offset_in_parent = snarl_tree_records->size();

                                //Add the node to the chain
                                if (chain_record_constructor.ChainRecord::get_record_type() == MULTICOMPONENT_CHAIN) {
                                    chain_record_constructor.add_node(temp_node_record.node_id, temp_chain_record.prefix_sum[chain_node_i],
                                                                  temp_chain_record.forward_loops[chain_node_i],
                                                                  temp_chain_record.backward_loops[chain_node_i],
                                                                  temp_chain_record.chain_components[chain_node_i]);
                                } else {
                                    chain_record_constructor.add_node(temp_node_record.node_id, temp_chain_record.prefix_sum[chain_node_i],
                                                                  temp_chain_record.forward_loops[chain_node_i],
                                                                  temp_chain_record.backward_loops[chain_node_i]);
                                }
                                last_child_offset = make_pair(node_offset_in_parent, false);


#ifdef debug_distance_indexing
                            cerr << "    The node record is at offset " << node_record_constructor.NodeRecord::record_offset << endl;
#endif
                            }

                            chain_node_i++;
                            prev_node = true;


                        } else {
                            //Add a snarl to the chain
                            assert(child_record_index.first == TEMP_SNARL);
                            //Get the temporary snarl record
                            const TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record =
                                 temp_index->temp_snarl_records[child_record_index.second];
                            if (!temp_snarl_record.is_trivial) {
                                //If this is an actual snarl that we need to make

                                //Record how to find the new snarl record
                                record_to_offset.emplace(make_pair(temp_index_i, child_record_index), snarl_tree_records->size()+1);

                                //Add the snarl to the chain, and get back the record to fill it in

                                record_t record_type = snarl_size_limit == 0 ? SNARL :
                                    (temp_snarl_record.node_count < snarl_size_limit ? DISTANCED_SNARL : OVERSIZED_SNARL);
                                SnarlRecordConstructor snarl_record_constructor =
                                    chain_record_constructor.add_snarl(temp_snarl_record.node_count, record_type, temp_snarl_record.max_distance);

                                //Fill in snarl info
                                snarl_record_constructor.set_parent_record_offset(chain_record_constructor.get_offset());
                                snarl_record_constructor.set_start_node(temp_snarl_record.start_node_id,
                                                                         temp_snarl_record.start_node_rev);
                                snarl_record_constructor.set_end_node(temp_snarl_record.end_node_id,
                                                                         temp_snarl_record.end_node_rev);
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
                            cerr << "    The snarl record is at offset " << snarl_record_constructor.SnarlRecord::record_offset << endl;
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
                                last_child_offset = make_pair(snarl_record_constructor.SnarlRecord::record_offset, true);
                            } else {
                                //Add a trivial snarl
#ifdef debug_distance_indexing
                                cerr << "    which is a trivial snarl" << endl;
#endif
                                chain_record_constructor.add_trivial_snarl();
                            }

                            prev_node = false;
                        }
                    }
                    //Does the chain loop and is the last node connected to the rest of the chain through the last snarl
                    bool last_node_connected = temp_chain_record.loopable && (temp_chain_record.start_node_id==temp_chain_record.end_node_id);
                    chain_record_constructor.set_last_child_offset(last_child_offset.first, last_child_offset.second, last_node_connected);
                    //Finish the chain by adding two 0's
                    chain_record_constructor.finish_chain();
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
                    NodeRecordConstructor node_record(
                        get_offset_from_node_id(temp_node_record.node_id), record_type, &snarl_tree_records);
                    node_record.set_node_length(temp_node_record.node_length);
                    node_record.set_rank_in_parent(temp_chain_record.rank_in_parent);
                    node_record.set_is_reversed_in_parent(false);//TODO: Make sure that this is true
                    node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_chain_record.parent)]);

                    record_to_offset.emplace(make_pair(temp_index_i, current_record_index), node_record.NodeRecord::record_offset);

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

                SnarlRecordConstructor snarl_record_constructor (temp_snarl_record.node_count, &snarl_tree_records, record_type, temp_snarl_record.max_distance);

                //Fill in snarl info
                snarl_record_constructor.set_parent_record_offset(0);

                //Add distances and record connectivity

                if (snarl_size_limit != 0 ) {

                    size_t max_snarl_distance = 0;
                    for (const auto& it : temp_snarl_record.distances) {
                        max_snarl_distance = std::max(max_snarl_distance, it.second);
                    }
                    snarl_record_constructor.set_distance_bit_width(temp_snarl_record.max_distance);

                    for (const auto& it : temp_snarl_record.distances) {
                        const pair<pair<size_t, bool>, pair<size_t, bool>>& node_ranks = it.first;
                        const size_t distance = it.second;
                        //TODO: I"m checking this but also automatically making a distanced snarl
                        //If we are keeping track of distances and either this is a small enough snarl,
                        //or the snarl is too big but we are looking at the boundaries
                        assert(distance <= temp_snarl_record.max_distance);
                        snarl_record_constructor.set_distance(node_ranks.first.first, node_ranks.first.second,
                            node_ranks.second.first, node_ranks.second.second, distance);
                        assert(snarl_record_constructor.get_distance(node_ranks.first.first, node_ranks.first.second,
                                node_ranks.second.first, node_ranks.second.second) == distance);
                    }
                }

#ifdef debug_distance_indexing
                cerr << "    The snarl record is at offset " << snarl_record_constructor.SnarlRecord::record_offset << endl;
                cerr << "    This child snarl has " << snarl_record_constructor.get_node_count() << " children: " << endl;
#endif
                for (const pair<temp_record_t, size_t>& child : temp_snarl_record.children) {
                        temp_record_stack.emplace_back(child);
                }

            } else {
                //TODO: This was a node that was a tip, so it wasn't put in a chain by the temporary index
                assert(current_record_index.first == TEMP_NODE);
#ifdef debug_distance_indexing
                cerr << "        this just a node that is a tip "
                     << temp_index->structure_start_end_as_string(current_record_index) << endl;
#endif
                const TemporaryDistanceIndex::TemporaryNodeRecord& temp_node_record =
                        temp_index->temp_node_records[current_record_index.second-min_node_id];
                record_t record_type = snarl_size_limit == 0 ? NODE : DISTANCED_NODE;
                NodeRecordConstructor node_record(
                    get_offset_from_node_id(temp_node_record.node_id), record_type, &snarl_tree_records);
                node_record.set_node_length(temp_node_record.node_length);
                node_record.set_rank_in_parent(temp_node_record.rank_in_parent);
                node_record.set_is_reversed_in_parent(false);//TODO: Make sure that this is true
                node_record.set_parent_record_offset(record_to_offset[make_pair(temp_index_i, temp_node_record.parent)]);

                record_to_offset.emplace(make_pair(temp_index_i, current_record_index), node_record.NodeRecord::record_offset);
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
            if (!temp_snarl_record.is_trivial) {
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
#ifdef debug_distance_indexing
    cerr << "Predicted size: " << maximum_index_size << " actual size: " <<  snarl_tree_records->size() << endl;
    assert(snarl_tree_records->size() <= maximum_index_size); 
#endif


}
//TODO: Also need to go the other way, from final index to temporary one for merging



}
