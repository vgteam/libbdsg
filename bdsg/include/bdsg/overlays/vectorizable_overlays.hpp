//
//  vectorizable_overlays.hpp
//  
//  Contains generic overlays for HandleGraph's that add the
//  VectorizableHandleGraph interface methods for querying steps by base-pair
//  position.
//

#ifndef BDSG_VECTORIZABLE_OVERLAYS_HPP_INCLUDED
#define BDSG_VECTORIZABLE_OVERLAYS_HPP_INCLUDED

#include <unordered_map>
#include <map>

#include "sdsl/bit_vectors.hpp"

#include "bdsg/internal/hash_map.hpp"

#include "bdsg/graph_proxy.hpp"

#include <handlegraph/handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>
#include <BooPHF.h>

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the VectorizableHandleGraph interface to a HandleGraph
 * by augmenting it with relatively simple data structures.
 *
 */
class VectorizableOverlay : virtual public VectorizableHandleGraph, virtual public ExpandingOverlayGraph, public HandleGraphProxy<HandleGraph> {
        
public:
    
    VectorizableOverlay(const HandleGraph* graph);
    VectorizableOverlay();
    virtual ~VectorizableOverlay();
    
    ////////////////////////////////////////////////////////////////////////////
    // Vectorizable handle interface
    ////////////////////////////////////////////////////////////////////////////

    /// Return the start position of the node in a (possibly implict) sorted array
    /// constructed from the concatenation of the node sequences
    virtual size_t node_vector_offset(const nid_t& node_id) const;

    /// Return the node overlapping the given offset in the implicit node vector
    virtual nid_t node_at_vector_offset(const size_t& offset) const;

    /// Return a unique index among edges in the graph
    virtual size_t edge_index(const edge_t& edge) const;

    /// Return the rank of a node (ranks start at 1)
    virtual size_t id_to_rank(const nid_t& node_id) const;

    /// Return the node with a given rank
    virtual nid_t rank_to_id(const size_t& rank) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Expanding overlay interface
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Returns the handle in the underlying graph that corresponds to a handle in the
     * overlay
     */
    virtual handle_t get_underlying_handle(const handle_t& handle) const;
    
protected:
    
    /// Get the HandleGraph to which most methods should be delegated.
    inline const HandleGraph* get() const {
        return underlying_graph;
    }
    
    /// Construct the index over path positions
    virtual void index_nodes_and_edges();
    
    /// Re-canonicalize the edge into a representation based on node IDs and orientations.
    pair<pair<nid_t, bool>, pair<nid_t, bool>> canonicalize_edge(const edge_t& edge) const;
    
    /// The graph we're overlaying
    const HandleGraph* underlying_graph = nullptr;

    /// Minimal perfect hash defining an (arbitrary) ordering of the node IDs in the graph.
    /// Needs to be based on node IDs so we get the same arbitrary ordering regardless of handle values.
    /// Uses 0-based ranks.
    unique_ptr<boomphf::mphf<nid_t, boomphf::SingleHashFunctor<nid_t>>> node_to_rank;
    
    /// Rank to node ID (inverse of node_to_rank), but using 1-based ranks.
    vector<nid_t> rank_to_node;
    
    // (I can't get it the pair_hash_map to compile with handle_t's, so using integers
    //  directly until I ca figure it out)
    // for our edge hash table (inspired from vg's hash_map.hpp)
    template<typename A, typename B>
    struct boomph_pair_hash {
        size_t operator()(const std::pair<A, B>& x, uint64_t seed=0) const {
            size_t hash_val = boomphf::SingleHashFunctor<A>()(x.first, seed);
            hash_val ^= boomphf::SingleHashFunctor<A>()(x.second, seed) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
            return hash_val;
        }
    };
    
    template<typename A, typename B, typename C, typename D>
    struct boomph_pair_pair_hash {
        size_t operator()(const std::pair<std::pair<A, B>, std::pair<C, D>>& x, uint64_t seed=0) const {
            size_t hash_val = boomph_pair_hash<A, B>()(x.first, seed);
            hash_val ^= boomph_pair_hash<C, D>()(x.second, seed) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
            return hash_val;
        }
    };
    
    /// Edge to rank. Represents edges as pairs of (node ID, is_reverse), in
    /// whichever orientation gives the smallest-value pair. Can't use pairs of
    /// handles because the arbitrary ordering has to be independent of handle
    /// values.
    unique_ptr<boomphf::mphf<pair<pair<nid_t, bool>, pair<nid_t, bool>>, boomph_pair_pair_hash<nid_t, bool, nid_t, bool>>> edge_to_rank;

    /// Map between global sequence position and node rank
    sdsl::bit_vector s_bv;
    sdsl::rank_support_v<1> s_bv_rank;
    sdsl::bit_vector::select_1_type s_bv_select;
    
};

class PathVectorizableOverlay : public VectorizableOverlay, virtual public PathHandleGraph, public PathProxy<PathHandleGraph> {

public:
    
    PathVectorizableOverlay(const PathHandleGraph* path_graph);
    PathVectorizableOverlay();
    virtual ~PathVectorizableOverlay();

protected:
    
    /// Get the HandleGraph to which most methods should be delegated.
    inline const PathHandleGraph* get() const {
        return underlying_path_graph;
    }
    
    /// Keep this around to avoid dynamic_casting this->underlying_graph every time we need path stuff
    const PathHandleGraph* underlying_path_graph = nullptr;
};


class PathPositionVectorizableOverlay : public PathVectorizableOverlay, virtual public PathPositionHandleGraph, public PositionProxy<PathPositionHandleGraph> {

public:
    
    PathPositionVectorizableOverlay(const PathPositionHandleGraph* path_position_graph);
    PathPositionVectorizableOverlay();
    virtual ~PathPositionVectorizableOverlay();

protected:

    /// Get the HandleGraph to which most methods should be delegated.
    inline const PathPositionHandleGraph* get() const {
        return underlying_path_position_graph;
    }
    
    /// Keep this around to avoid dynamic_casting this->underlying_graph every time we need path stuff
    const PathPositionHandleGraph* underlying_path_position_graph = nullptr;
};


}

#endif
