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
        
        // Now break off space for the node just after the current last node, if any.
        size_t node_offset = serialized_data_size() - trailing_free_space;
        trailing_free_space -= node_space;
        if (header->last_node != numeric_limits<size_t>::max()) {
            // We took all the last node's free space
            get<node_t>(header->last_node)->free_space = 0;
        } 
        
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
        node->free_space = trailing_free_space;
        put_bytes(node_offset + sizeof(node), sequence);
        
        // Make a handle from the offset.
        return handlegraph::number_bool_packing::pack(node_offset, false); 
    }
}


size_t FlatGraph::ensure_training_space(size_t needed) {
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
    // Follow each edge offset here that isn't to this node
    
    // Scan, swap back edge to end, and remove it
    
    // TODO: we can't actually credit ourselves to the previous node as free
    // space unless we know where, exactly, it is.
    
    // Remove this node by tweaking free space on the previous node, if any.
    
    // And the next node, if any.
}

}
