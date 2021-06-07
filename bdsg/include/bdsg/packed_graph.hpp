//
//  packed_graph.hpp
//  
//  Contains a implementation of a sequence graph based on bit-packed integer
//  vectors.
//

#ifndef BDSG_PACKED_GRAPH_HPP_INCLUDED
#define BDSG_PACKED_GRAPH_HPP_INCLUDED

#include <utility>

#include "bdsg/internal/base_packed_graph.hpp"
#include "bdsg/internal/graph_proxy.hpp"


namespace bdsg {
    
using namespace std;
using namespace handlegraph;

class PackedGraph : public GraphProxy<BasePackedGraph<>> {
protected:
    /**
     * Get the object that actually provides the graph methods.
     */
    BasePackedGraph<>* get();
    
    /**
     * Get the object that actually provides the graph methods.
     */
    const BasePackedGraph<>* get() const;
    
    /**
     * We just directly contain the BasePackedGraph using the default
     * in-normal-memory backend data structures.
     */
    BasePackedGraph<> implementation;
};

class MappedPackedGraph : public GraphProxy<BasePackedGraph<MappedBackend>> {
public:

    // We need constructors, destructors, copy, and move because we are keeping
    // the graph we are proxying for in mapped memory.

    MappedPackedGraph();
    ~MappedPackedGraph() = default;
    
    MappedPackedGraph(const MappedPackedGraph& other);
    MappedPackedGraph& operator=(const MappedPackedGraph& other);
    
    MappedPackedGraph(MappedPackedGraph&& other);
    MappedPackedGraph& operator=(MappedPackedGraph&& other);
protected:
    /**
     * Get the object that actually provides the graph methods.
     */
    BasePackedGraph<MappedBackend>* get();
    
    /**
     * Get the object that actually provides the graph methods.
     */
    const BasePackedGraph<MappedBackend>* get() const;
    
    /**
     * We have a pointer to the memory-mapped graph that actually implements
     * everything.
     */
    yomo::UniqueMappedPointer<BasePackedGraph<MappedBackend>> implementation;
};

}

#endif
