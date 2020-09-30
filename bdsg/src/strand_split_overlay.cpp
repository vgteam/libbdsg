/**
 * \file strand_split_overlay.cpp: contains the implementation of StrandSplitOverlay
 */


#include "bdsg/overlays/strand_split_overlay.hpp"

#include <handlegraph/util.hpp>


namespace bdsg {

using namespace std;
using namespace handlegraph;

    StrandSplitOverlay::StrandSplitOverlay(const HandleGraph* graph) : graph(graph){
        // nothing to do
    }
    
    bool StrandSplitOverlay::has_node(nid_t node_id) const {
        return graph->has_node(node_id >> 1);
    }
    
    handle_t StrandSplitOverlay::get_handle(const nid_t& node_id, bool is_reverse) const {
        return handlegraph::number_bool_packing::pack(node_id, is_reverse);
    }
    
    nid_t StrandSplitOverlay::get_id(const handle_t& handle) const {
        return handlegraph::number_bool_packing::unpack_number(handle);
    }
    
    bool StrandSplitOverlay::get_is_reverse(const handle_t& handle) const {
        return handlegraph::number_bool_packing::unpack_bit(handle);
    }
    
    handle_t StrandSplitOverlay::flip(const handle_t& handle) const {
        return handlegraph::number_bool_packing::toggle_bit(handle);
    }
    
    size_t StrandSplitOverlay::get_length(const handle_t& handle) const {
        return graph->get_length(graph->get_handle(get_id(handle) >> 1));
    }
    
    string StrandSplitOverlay::get_sequence(const handle_t& handle) const {
        return graph->get_sequence(get_underlying_handle(handle));
    }
    
    bool StrandSplitOverlay::follow_edges_impl(const handle_t& handle, bool go_left,
                                             const function<bool(const handle_t&)>& iteratee) const {
        
        return graph->follow_edges(get_underlying_handle(handle), go_left,
                                   [&] (const handle_t& next) {
            return iteratee(get_handle((graph->get_id(next) << 1) + (graph->get_is_reverse(next) != get_is_reverse(handle)),
                                       get_is_reverse(handle)));
        });
    }
    
    bool StrandSplitOverlay::for_each_handle_impl(const function<bool(const handle_t&)>& iteratee,
                                                bool parallel) const {
        return graph->for_each_handle([&](const handle_t& underlying_handle) {
            nid_t node_id = graph->get_id(underlying_handle);
            // forward version of the node
            bool keep_going = iteratee(get_handle(node_id << 1));
            // reverse version of the node
            if (keep_going) {
                keep_going = iteratee(get_handle((node_id << 1) | 1));
            }
            return keep_going;
        }, parallel);
    }
    
    size_t StrandSplitOverlay::get_node_count() const {
        return graph->get_node_count() << 1;
    }
    
    nid_t StrandSplitOverlay::min_node_id() const {
        return graph->min_node_id() << 1;
    }
    
    nid_t StrandSplitOverlay::max_node_id() const {
        return (graph->max_node_id() << 1) | 1;
    }

    handle_t StrandSplitOverlay::get_underlying_handle(const handle_t& handle) const {
        return graph->get_handle(get_id(handle) >> 1,
                                 ((get_id(handle) & 1) == 1) != get_is_reverse(handle));
    }
}

