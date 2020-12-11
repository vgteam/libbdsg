//
//  flat_graph.cpp
//

#include "bdsg/flat_graph.hpp"

#include <handlegraph/util.hpp>
#include <atomic>
#include <algorithms>

namespace bdsg {

using namespace handlegraph;

FlatGraph::FlatGraph() {
    // Nothing to do!
}

bool FlatGraph::has_node(nid_t node_id) const {
    bool found = false;
    for_each_handle([&](const handle_t& each) {
        // Do a linear scan
        if (get_id(found) == node_id) {
            // We found it
            found = true;
            // Stop looping!
            return false;
        }
        return true;
    });
    
    return found;
}

handle_t FlatGraph::get_handle(const nid_t& node_id, bool is_reverse) const {
    handle_t obtained;
    for_each_handle([&](const handle_t& each) {
        // Do a linear scan
        if (get_id(found) == node_id) {
            // We found it. Orient it right.
            obtained = is_reverse ? flip(each) : each;
            // Stop looping!
            return false;
        }
        return true;
    });
    
    return obtained;
}

nid_t FlatGraph::get_id(const handle_t& handle) const {
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    return get<node_t>(offset)->id;
}

bool FlatGraph::get_is_reverse(const handle_t& handle) const {
    return handlegraph::number_bool_packing::unpack_bit();
}

handle_t FlatGraph::flip(const handle_t& handle) const {
    return handlegraph::number_bool_packing::toggle_bit(handle);
}

size_t FlatGraph::get_length(const handle_t& handle) const {
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    return get<node_t>(offset)->seq_bytes;
}
}

std::string FlatGraph::get_sequence(const handle_t& handle) const {
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    size_t seq_start = offset + sizeof(node_t);
    size_t seq_length = get<node_t>(offset)->seq_bytes;

    std::string forward_sequence = get_bytes(seq_start, seq_length);
    
    return get_is_reverse(handle) ? reverse_complement(forward_sequence)
                                  : forward_sequence;
}

bool FlatGraph::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    // Find the node
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    const node_t* node = get<node_t>(offset);
    // And how we are on it
    bool reverse_here = get_is_reverse(handle);
    
    // Find its edges in memory
    const edge_t* first = get<edge_t>(offset + sizeof(node_t) + node->seq_bytes);
    const edge_t* last = first + node->edge_count; // counts in full edge_t units
    
    // We are always from; edges are stored twice.
    
    // Work out if we want the from_start edges or not.
    bool want_on_start = go_left != reverse_here;
    
    // We'll set this to false if the iteratee wants to stop.
    bool keep_going = true;
    
    for (edge_t* e = first; e != last && keep_going; ++e) {
        if (e->on_start == want_on_start) {
            // This edge is on the correct side.
            
            // We end up in reverse when we get there if the edge is on the end
            // of the other node, but flip everything around if we're going
            // left, but flip everything around again if we were flipped where
            // we were.
            bool arrive_reverse = (e->on_other_end != go_left) != reverse_here;
            
            // Make a handle for the destination node in the arriving orientation
            handle_t dest = handlegraph::number_bool_packing::pack(e->other, arrive_reverse);
            
            // Show it to the iteratee and see if we should stop.
            keep_going &= iteratee(dest);
        }
    }
    
    return keep_going;
}

bool FlatGraph::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    // TODO: provide a parallel implementation.
    
    const header_t* header = get<header_t>(0);
    
    if (!header) {
        // No data
        return true;
    }
    
    size_t offset = header->first_node;
    size_t end = serialized_data_size();
    
    bool keep_going = true;
    
    while (offset < end && keep_going) {
        const node_t* node = get<node_t>(offset);
        
        // Make a handle for the node, forward
        handle_t here = handlegraph::number_bool_packing::pack(offset, false);
        
        // Show it to the iteratee and see if we should stop.
        keep_going &= iteratee(here);
        
        // Update the offset to the next node, after the sequence, edges, and free space.
        offset += node->seq_bytes + node->edge_count * sizeof(edge_t) + node->free_space;
    }
    
    return keep_going;
}

size_t FlatGraph::get_node_count(void) const {
    const header_t* header = get<header_t>(0);
    
    if (!header) {
        // No data
        return 0;
    }
    
    // If there's a header there's always a count
    return header->node_count;
}

nid_t FlatGraph::min_node_id(void) const {
    const header_t* header = get<header_t>(0);
    
    if (!header) {
        // No data
        return numeric_limits<nid_t>::min();
    }
    
    // If there's a header, there's a min ID or a not-computed placeholder.
    return header->min_node_id;
}
    
nid_t FlatGraph::max_node_id(void) const {
    const header_t* header = get<header_t>(0);
    
    if (!header) {
        // No data
        return numeric_limits<nid_t>::max();
    }
    
    // If there's a header, there's a min ID or a not-computed placeholder.
    return header->max_node_id;
}


handle_t FlatGraph::create_handle(const std::string& sequence) {
    // Our create_handle accepts 0 to mean fill in an ID.
    return create_handle(sequence, 0);
}

handle_t FlatGraph::create_handle(const std::string& sequence, const nid_t& id) {
    const header_t* header = get<header_t>(0);
    
    // Work out how much space this node needs.
    size_t node_space = sizeof(node_t) + sequence.size();
    
    if (!header) {
        // Allocate us some space for the header and this node.
        serialized_data_resize(sizeof(header_t) + node_space);
        // Initialize the header for an empty graph
        header = get<header_t>(0);
        header->first_node = numeric_limits<size_t>::max();
        header->last_node = numeric_limits<size_t>::max();
        header->num_nodes = 0;
        header->min_node_id = numeric_limits<nid_t>::min();
        header->max_node_id = numeric_limits<nid_t>::max();
    }
    
    if (id == 0) {
    
        nid_t chosen_id;
    
        if (header->max_node_id == numeric_limits<nid_t>::max()) {
            // Compute the max node ID by scan
            nid_t max_seen = 0;
            bool seen_any = false;
            for_each_handle([&](const handle_t& other) {
                seen_any = true;
                max_seen = std::max(max_seen, get_id(other));
            });
            
            if (seen_any) {
                // We have found the current max ID
                
                // Explode if someone actually starts using node IDs up here.
                // We would overflow.
                assert(max_seen != numeric_limits<nid_t>::max());
                
                header->max_node_id = max_seen;
                chosen_id = max_seen + 1;
            } else {
                // If there are no nodes, start at 1.
                chosen_id = 1;
            }
        } else {
            // The max node ID is known.
            chosen_id = header->max_node_id + 1;
        }
        
        // We need to make up an ID. Try and get a free one.
        return create_handle(sequence, chosen_id); 
    } else {
        // We have the node ID to use.
        // Assume it is free.
        
        size_t trailing_free_space = ensure_trailing_space(node_space);
        header = get<header_t>(0);
        
        // Now break off space for the node just after the current last node, if any.
        size_t node_offset = serialized_data_size() - trailing_free_space;
        trailing_free_space -= node_space;
        if (header->last_node != numeric_limits<size_t>::max()) {
            // We took all the last node's free space
            get<node_t>(header->last_node)->free_space = 0;
        } 
         
        // Grab where the previous node was.
        // Will be max value if there isn't one.
        size_t prev_node = header->last_node
        
        // We are the new last and possibly first node.
        header->last_node = node_offset;
        if (header->first_node == numeric_limits<size_t>::max()) {
            header->first_node = node_offset;
        }
        
        // Fill it in
        node_t* node = get<node_t>(node_offset);
        node->id = id;
        node->seq_bytes = sequence.size();
        node->edge_count = 0;
        node->prev_node = prev_node; 
        node->free_space = trailing_free_space;
        put_bytes(node_offset + sizeof(node), sequence);
        
        // Make a handle from the offset.
        return handlegraph::number_bool_packing::pack(node_offset, false); 
    }
}


size_t FlatGraph::ensure_trailing_space(size_t needed) {
    const header_t* header = get<header_t>(0);

    // How much free space is at the end of the data?
    size_t trailing_free_space;
    
    if (header->last_node == numeric_limits<size_t>::max()) {
        // There is no last node. We are the first node.
        // Only the header exists.
        trailing_free_space = serialized_data_size() - sizeof(header);
    } else {
        // There is a last node. How much space does it have?
        trailing_free_space = get<node_t>(header->last_node)->free_space;
    }
    
    if (trailing_free_space < needed) {
        // No room for this thing. Embiggen!
        // We only embiggen as much as necessary.
        // TODO: Make TriviallySerializable do smart doubling on Mac. 
        serialized_data_resize(serialized_data_size() + (needed - trailing_free_space));
        header = get<header_t>(0);
        
        trailing_free_space = needed;
        if (header->last_node != numeric_limits<size_t>::max()) {
            // Update the last node's free space
            get<node_t>(header->last_node)->free_space = trailing_free_space;
        }
    }
    
    return trailing_free_space;
}

void FlatGraph::destroy_handle(const handle_t& handle) {
    // Find the header
    header_t* header = get<header_t>(0);

    // Find the node
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    node_t* node = get<node_t>(offset);
    
    // Find its edges in memory
    edge_t* first = get<edge_t>(offset + sizeof(node_t) + node->seq_bytes);
    edge_t* last = first + node->edge_count; // counts in full edge_t units
    
    for (edge_t* e = first; e != last; ++e) {
        // For each attachment
        if (e->other != offset) {
            // If it is to a different node
            
            // Go grab that node
            node_t* other_node = get<node_t>(e->other);
            
            // And scan its edges
            edge_t* other_first = get<edge_t>(e->other + sizeof(node_t) + other_node->seq_bytes);
            edge_t* other_last = first + other_node->edge_count; // counts in full edge_t units
            
            // Count how many we removed
            size_t removed = 0;
            
            for (edge_t* other_e = other_first; other_e != other_last; ++other_e) {
                if (other_e->other == offset) {
                    // We found a back edge.
                    
                    // Put it at the end of the edges
                    std::swap(*other_e, *(other_last - 1));
                    
                    // Back up to look here again
                    --other_e;
                    // Also trim the range we check
                    --other_last;
                    
                    removed++;
                }
            }
            
            // Now we put all the edges to remove at the end, so deallocate them.
            other_node->edge_count -= removed;
            other_node->free_space += removed * sizeof(edgte_t);
        }
    }
    
    // Now eliminate ourselves
    
    size_t next_node_offset = offset + sizeof(node_t) + node->seq_bytes + node->edge_count * sizeof(edge_t) + node->free_space;
    size_t prev_node_offset = node->prev_node;
    
    if (prev_node_offset != numeric_limits<size_t>::max()) {
        // We have a previous node
        node_t* prev = get<node_t>(prev_node_offset);
        
        // Credit ourselves and our free space to the free space of the previous node
        prev->free_space += (next_node_offset - offset);
    } else {
        // We were the first node.
        // Adjust the header for the new first node.
        if (next_node_offset < serialized_data_size()) {
            // We have a next node that is now the first node
            header->first_node = next_node_offset;
        } else {
            // There is no next node, so now there's no first node.
            header->first_node = numeric_limits<size_t>::max();
        }
    }
    
    if (next_node_offset < serialized_data_size()) {
        // We have a next node.
        node_t* next = get<node_t>(next_node_offset);
        
        // It should point back to whatever our previous node was, if any
        next->prev_node = prev_node_offset;
    } else {
        // Actually we were the last node.
        // Adjust the header for the new last node.
        // If we have no prev node this is already the no-node sentinel.
        header->first_node = prev_node
    }
    
    if (node->id == header->min_node_id) {
        // We no longer know of the min node ID
        header->min_node_id = numeric_limits<nid_t>::min();
    }
    
    if (node->id == header->max_node_id) {
        // We no longer know of the max node ID
        header->max_node_id = numeric_limits<nid_t>::max();
    }
    
    // Now we will return and our node will no longer exist. Might get
    // overwritten.
}

void FlatGraph::create_edge(const handle_t& left, const handle_t& right) {
    if (has_edge(left, right)) {
        // We need to ignore existing edges.
        // TODO: Sort edges for binary search.
        return;
    }
    
    
    // Find the node
    size_t offset = handlegraph::number_bool_packing::unpack_number(handle);
    node_t* node = get<node_t>(offset);
    
    if (node->free_space < sizeof(edge_t)) {
        // We need more room.
        
        // TODO: we can't actually move this node or any other node to get more room, because we can't invalidate any handles! 
        
    }
    
}

void FlatGraph::destroy_edge(const handle_t& left, const handle_t& right) {
}
    
void FlatGraph::clear(void) {
    // We can just drop all our data.
    serialized_data_resize(0);
}

}
