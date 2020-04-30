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

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

// The templates used to generate the overlay helpers
template<typename T, typename U, typename V> class OverlayHelper;
template<typename T1, typename U1, typename V1, typename T2, typename U2, typename V2> class PairOverlayHelper;

// Helper to ensure that a PathHandleGraph has the PathPositionHandleGraph interface
typedef OverlayHelper<PathPositionHandleGraph, PackedPositionOverlay, PathHandleGraph> PathPositionOverlayHelper;

// Helper to ensure that a HandleGraph has the VectorizableHandleGraph interface
typedef OverlayHelper<VectorizableHandleGraph, VectorizableOverlay, HandleGraph> VectorizableOverlayHelper;

// Helper to ensure that a PathHandleGraph has the PathVectorizableHandleGraph interface
typedef OverlayHelper<VectorizableHandleGraph, PathVectorizableOverlay, PathHandleGraph> PathVectorizableOverlayHelper;

// Helper to ensure that a PathHandleGraph has the PathPositionVectorizableHandleGraph interface
typedef PairOverlayHelper<PathPositionHandleGraph, PackedPositionOverlay, PathHandleGraph,
VectorizableHandleGraph, PathPositionVectorizableOverlay, PathPositionHandleGraph> PathPositionVectorizableOverlayHelper;


/// T = desired class
/// U = overlay class
/// V = input class
template<typename T, typename U, typename V>
class OverlayHelper {
public:
    T* apply(V* input_graph) {
        overlaid = dynamic_cast<T*>(input_graph);
        if (overlaid == nullptr) {
            overlay = make_unique<U>(input_graph);
            overlaid = dynamic_cast<T*>(overlay.get());
            assert(overlaid != nullptr);
        }
        return overlaid;
    }

    T* get() {
        return overlaid;
    }
protected:
    unique_ptr<U> overlay;
    T* overlaid = nullptr;
};

/// There must be a way to generalize with variadic templates
/// (I had trouble chaining the output of the nested overlays together and getting the types right when trying)
template<typename T1, typename U1, typename V1, typename T2, typename U2, typename V2>
class PairOverlayHelper {
public:
    T2* apply(V1* input_graph){
        T1* g1 = overlay1.apply(input_graph);
        T2* g2 = overlay2.apply(dynamic_cast<V2*>(g1));
        return g2;
    }

    T2* get() {
        return overlay2.get();
    }

protected:
    OverlayHelper<T1, U1, V1> overlay1;
    OverlayHelper<T2, U2, V2> overlay2;
};

}

#endif
