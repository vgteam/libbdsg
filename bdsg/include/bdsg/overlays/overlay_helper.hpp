//
//  overlay_helper.hpp
//  
//  Wrapper class to help apply overlays to a given graph.  Handles checks to see
//  if they're needed and memory.
//

#ifndef OVERLAY_HELPER_HPP_INCLUDED
#define OVERLAY_HELPER_HPP_INCLUDED

#include <unordered_map>
#include <map>

#include "bdsg/overlays/vectorizable_overlays.hpp"
#include "bdsg/overlays/packed_path_position_overlay.hpp"
#include "bdsg/overlays/reference_path_overlay.hpp"
#include "bdsg/overlays/packed_reference_path_overlay.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

// The templates used to generate the overlay helpers
template<typename T, typename U, typename V> class OverlayHelper;
template<typename T1, typename U1, typename V1, typename T2, typename U2, typename V2> class PairOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the PathPositionHandleGraph interface
typedef OverlayHelper<PathPositionHandleGraph, PackedPositionOverlay, PathHandleGraph> PathPositionOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the PathPositionHandleGraph
/// interface, but optimized for using the paths as reference coordinates, with
/// acceleration for some queries that might be slow in, for example, a
/// GBWTGraph.
typedef OverlayHelper<PathPositionHandleGraph, ReferencePathOverlay, PathHandleGraph> ReferencePathOverlayHelper;
typedef OverlayHelper<PathPositionHandleGraph, PackedReferencePathOverlay, PathHandleGraph> PackedReferencePathOverlayHelper;

/// Helper to ensure that a HandleGraph has the RankedHandleGraph interface.
// TODO: If we write a dedicated, less powerful RankedOverlay, use that here instead.
typedef OverlayHelper<RankedHandleGraph, VectorizableOverlay, HandleGraph> RankedOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the RankedeHandleGraph interface.
// TODO: If we write a dedicated, less powerful RankedOverlay, use that here instead.
typedef OverlayHelper<RankedHandleGraph, PathVectorizableOverlay, PathHandleGraph> PathRankedOverlayHelper;

/// Helper to ensure that a HandleGraph has the VectorizableHandleGraph interface
typedef OverlayHelper<VectorizableHandleGraph, VectorizableOverlay, HandleGraph> VectorizableOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the VectorizableHandleGraph interface
typedef OverlayHelper<VectorizableHandleGraph, PathVectorizableOverlay, PathHandleGraph> PathVectorizableOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the VectorizableHandleGraph and PathPositionHandleGraph interfaces.
typedef PairOverlayHelper<PathPositionHandleGraph, PackedPositionOverlay, PathHandleGraph,
VectorizableHandleGraph, PathPositionVectorizableOverlay, PathPositionHandleGraph> PathPositionVectorizableOverlayHelper;

/// Helper to ensure that a PathHandleGraph has the VectorizableHandleGraph and PathPositionHandleGraph interfaces,
/// But using the GBZ-optimized ReferencePath Overlay instead of the vanilla PathPosition Overlay
typedef PairOverlayHelper<PathPositionHandleGraph, PackedReferencePathOverlay, PathHandleGraph,
VectorizableHandleGraph, PathPositionVectorizableOverlay, PathPositionHandleGraph> ReferencePathVectorizableOverlayHelper;

/// Implementation of overlay helper core functionality.
/// T = desired class
/// U = overlay class
/// V = input class
template<typename T, typename U, typename V>
class OverlayHelper {
public:
    // Handle non-const base graph
    template <typename ...Params>
    T* apply(V* input_graph, Params&&... params) {
        auto mutable_overlaid = dynamic_cast<T*>(input_graph);
        if (mutable_overlaid == nullptr) {
            overlay = make_unique<U>(input_graph, std::forward<Params>(params)...);
            mutable_overlaid = dynamic_cast<T*>(overlay.get());
            assert(mutable_overlaid != nullptr);
        }
        overlaid = mutable_overlaid;
        return mutable_overlaid;
    }

    // Handle const base graph
    template <typename ...Params>
    const T* apply(const V* input_graph, Params&&... params) {
        overlaid = dynamic_cast<const T*>(input_graph);
        if (overlaid == nullptr) {
            overlay = make_unique<U>(input_graph, std::forward<Params>(params)...);
            overlaid = dynamic_cast<T*>(overlay.get());
            assert(overlaid != nullptr);
        }
        return overlaid;
    }
    
    // In general we can only get the const overlay later.
    const T* get() const {
        return overlaid;
    }
protected:
    unique_ptr<U> overlay;
    const T* overlaid = nullptr;
};

/// Implementation of overlay helper functionality for when multiple overlays need to be stacked.
// There must be a way to generalize with variadic templates
// (I had trouble chaining the output of the nested overlays together and getting the types right when trying)
// TODO: Add support for passing overlay constructor arguments through to the second overlay.
template<typename T1, typename U1, typename V1, typename T2, typename U2, typename V2>
class PairOverlayHelper {
public:
    template <typename ...Params>
    T2* apply(V1* input_graph, Params&&... params){
        T1* g1 = overlay1.apply(input_graph, std::forward<Params>(params)...);
        T2* g2 = overlay2.apply(dynamic_cast<V2*>(g1));
        return g2;
    }
    
    template <typename ...Params>
    const T2* apply(const V1* input_graph, Params&&... params){
        const T1* g1 = overlay1.apply(input_graph, std::forward<Params>(params)...);
        const T2* g2 = overlay2.apply(dynamic_cast<const V2*>(g1));
        return g2;
    }

    const T2* get() const {
        return overlay2.get();
    }

protected:
    OverlayHelper<T1, U1, V1> overlay1;
    OverlayHelper<T2, U2, V2> overlay2;
};

}

#endif
