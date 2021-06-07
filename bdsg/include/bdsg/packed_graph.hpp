//
//  packed_graph.hpp
//  
//  Contains a implementation of a sequence graph based on bit-packed integer
//  vectors.
//

#ifndef BDSG_PACKED_GRAPH_HPP_INCLUDED
#define BDSG_PACKED_GRAPH_HPP_INCLUDED

#include <utility>

#include <handlegraph/trivially_serializable.hpp>

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

class MappedPackedGraph : public GraphProxy<BasePackedGraph<MappedBackend>>, public TriviallySerializable {
public:

    // We need constructors, destructors, copy, and move because we are keeping
    // the graph we are proxying for in mapped memory.

    MappedPackedGraph();
    ~MappedPackedGraph() = default;
    
    MappedPackedGraph(const MappedPackedGraph& other);
    MappedPackedGraph& operator=(const MappedPackedGraph& other);
    
    MappedPackedGraph(MappedPackedGraph&& other);
    MappedPackedGraph& operator=(MappedPackedGraph&& other);
    
    // We need to say that TriviallySerializable's serialize and deserialize
    // should still be available.
    using TriviallySerializable::serialize;
    using TriviallySerializable::deserialize;
   
    /**
     * Cut the memory mapping connection to any backing file.
     */
    void dissociate();
    
    /**
     * Serialize us to the given file descriptor. We must not change at all.
     */
    void serialize(int fd) const;
    
    /**
     * Serialize us to the given file descriptor.
     */
    void serialize(int fd);
    
    /**
     * Deserialize us from the given file descriptor.
     */
    void deserialize(int fd);
    
    // We aren't allowed to override serialize() and deserialize() for streams;
    // we must use serialize_members and deserialize_members and let
    // libhandlegraph manage the magic number.
    // TODO: That's not how YOMO wants to work; it wants to have the prefix already. 
    
    /**
     * Serialize everything except the magic number to the given stream.
     */
    void serialize_members(std::ostream& out) const;
    
    /**
     * Deserialize everything except the magic number from the given stream.
     */
    void deserialize_members(std::istream& in);
    
   
protected:

    /**
     * Return the magic number to use at the start of files.
     *
     * Different than the magic number that BasePackedGraph<> uses, because we
     * are really storing a memory-mapped actual BasePackedGraph<> and not just
     * a packed-up description.
     */
    uint32_t get_magic_number() const;
    
    /**
     * Return the magic number as a string representing the bytes it will be
     * represented by on disk.
     */
    std::string get_prefix() const;
    
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
