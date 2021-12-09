#ifndef BDSG_GRAPH_PROXY_HPP_INCLUDED
#define BDSG_GRAPH_PROXY_HPP_INCLUDED

/**
 * \file graph_proxy.hpp
 * Defines a mechanism for implementing a handle graph by using a contained
 * object that actually does the work.
 */


#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/serializable_handle_graph.hpp>

namespace bdsg {

using namespace std;
using namespace handlegraph;

// !!!!!!!!!!!!!!!!!!!!!! MASSIVE HACK ALERT !!!!!!!!!!!!!!!!!!!!!!

// Binder can't properly bind any of these types if they define methods
// inherited from virtual base classes, for some reason. See
// <https://github.com/RosettaCommons/binder/issues/169>.

// So we code gen the types we need by including unguarded, fragmentary
// class-contents headers into several types of proxy.
// This is so we can achieve code sharing between different proxies with different feature sets, without using inheritance.
// Macros would be nicer, but thay can't easily be multi-line.

// The fragments are protected with an #ifdef BDSG_INSIDE_CLASS / #endif,
// because Binder will pick up on our using includes and put the same includes
// at the tops of its files, even if that's syntactically inadvisable. But they
// lack conventional include guards since we will include them multiple times.
// We also BINDER_IGNORE them to keep them out of the all-headers list, because
// it still tries to parse everything there.

/**
 * Defines a proxy you can inherit to implement PathHandleGraph by referencing a
 * different backing implementation, which implements the concept if not the
 * interface.
 *
 * Can be multiple-inherited alongsize other proxies and will use the same
 * backing implementation.
 */
template<typename BackingGraph> 
struct PathHandleGraphProxy : public PathHandleGraph {
    #define BDSG_INSIDE_CLASS
    #include "bdsg/internal/graph_proxy_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_path_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #undef BDSG_INSIDE_CLASS
};

/**
 * Defines a fully-featured GraphProxy that you can inherit from to implement
 * MutablePathDeletableHandleGraph and SerializableHandleGraph from one backing
 * object that need not implement either, as long as it satisfies the concept.
 */
template<typename BackingGraph>
struct GraphProxy : public MutablePathDeletableHandleGraph, public SerializableHandleGraph {
    #define BDSG_INSIDE_CLASS
    #include "bdsg/internal/graph_proxy_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_path_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_mutable_path_deletable_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #include "bdsg/internal/graph_proxy_serializable_handle_graph_fragment.classfragment" // BINDER_IGNORE
    #undef BDSG_INSIDE_CLASS
};

/**
 * A GraphProxy over an object at a specific address. Must not outlive the
 * backing object, and does not own it.
 */
template<typename BackingGraph>
class NonOwningGraphProxy : public GraphProxy<BackingGraph> {
public:
    NonOwningGraphProxy(BackingGraph* implementation) : implementation(implementation) {
        // Nothing to do!
    }
    
    BackingGraph* get() {
        return implementation;
    }
    
    const BackingGraph* get() const {
        return implementation;
    }
    
protected:
    BackingGraph* implementation;
};


}

#endif

