//
//  packed_graph.cpp
//

#include "bdsg/packed_graph.hpp"

#include <handlegraph/util.hpp>
#include <atomic>

namespace bdsg {

    using namespace handlegraph;
    
    BasePackedGraph<>* PackedGraph::get() {
        return &implementation;
    }
    
    const BasePackedGraph<>* PackedGraph::get() const {
        return &implementation;
    }
    
    BasePackedGraph<MappedBackend>* MappedPackedGraph::get() {
        return implementation.get();
    }
    
    const BasePackedGraph<MappedBackend>* MappedPackedGraph::get() const {
        return implementation.get();
    }
    
    MappedPackedGraph::MappedPackedGraph() {
        // Make sure our implementation pointer is never null.
        implementation.construct(get_prefix());
    }
    
    // Delegate copy and move to the copy and move constructors for the things that exist in mapped memory.
    
    MappedPackedGraph::MappedPackedGraph(const MappedPackedGraph& other) {
        implementation.construct(get_prefix(), *other.get());
    }
    
    MappedPackedGraph& MappedPackedGraph::operator=(const MappedPackedGraph& other) {
        if (get() != other.get()) {
            implementation.construct(get_prefix(), *other.get());
        }
        return *this;
    }
    
    MappedPackedGraph::MappedPackedGraph(MappedPackedGraph&& other) {
        implementation.construct(get_prefix(), std::move(*other.get()));
    }
    
    MappedPackedGraph& MappedPackedGraph::operator=(MappedPackedGraph&& other) {
        if (get() != other.get()) {
            implementation.construct(get_prefix(), std::move(*other.get()));
        }
        return *this;
    }
    
    void MappedPackedGraph::dissociate() {
        implementation.dissociate();
    }
    
    void MappedPackedGraph::serialize(const std::function<void(const void*, size_t)>& iteratee) const {
        // Pass the same iteratee back to the implementation pointer.
        implementation.save(iteratee);
    }
    
    void MappedPackedGraph::serialize(int fd) {
        implementation.save(fd);
    }
    
    void MappedPackedGraph::deserialize(int fd) {
        implementation.load(fd, get_prefix());
    }
    
    void MappedPackedGraph::serialize_members(std::ostream& out) const {
        // libhandlegraph already wrote our magic number prefix.
        implementation.save_after_prefix(out, get_prefix());
    }
    
    void MappedPackedGraph::deserialize_members(std::istream& in) {
        // libhandlegraph stole our magic number, and checked it.
        implementation.load_after_prefix(in, get_prefix());
    }
    
    uint32_t MappedPackedGraph::get_magic_number() const {
        // Chosen by fair dice roll, guaranteed to be magic.
        return 672226447;
    }
    
    std::string MappedPackedGraph::get_prefix() const {
        // Put into network byte order
        uint32_t magic_number = htonl(get_magic_number());
        // Then convert to a string, bounding length because it is not null terminated.
        return std::string((char*) &magic_number, sizeof(magic_number) / sizeof(char));
    }
    
   
}
