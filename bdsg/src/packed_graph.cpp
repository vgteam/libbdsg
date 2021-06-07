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
    
    void MappedPackedGraph::serialize(int fd) const {
        implementation.save([&](const void* start, size_t length) {
            // Copy each block to the fd
            size_t written = 0;
            while (written != length) {
                // Bang on the write call until it is all written
                auto result = write(fd, (const void*)((const char*) start + written), length - written);
                if (result == -1) {
                    // Can't write at all, something broke.
                    throw std::runtime_error("Could not write!");
                }
                written += result;
            }
        });
    }
    
    void MappedPackedGraph::serialize(int fd) {
        implementation.save(fd);
    }
    
    void MappedPackedGraph::deserialize(int fd) {
        implementation.load(fd, get_prefix());
    }
    
    void MappedPackedGraph::serialize_members(std::ostream& out) const {
        // TODO: libhandlegraph already wrote our magic number, but the UniqueMappedPointer emits the prefix in the data.
        // Right now we just cut it out.
        size_t dropped = 0;
        
        std::string prefix = get_prefix();
        
        implementation.save([&](const void* start, size_t length) {
            const char* start_char = (const char*) start;
            while (dropped < prefix.size() && length > 0) {
                // Drop all the characters we get from the prefix
                start_char++;
                dropped++;
                length--;
            }
            
            // Write the rest, if any
            out.write(start_char, length);
        });
    }
    
    void MappedPackedGraph::deserialize_members(std::istream& in) {
        // TODO: libhandlegraph stole our magic number, but the UniqueMappedPointer needs the prefix in the data.
        // Right now we just fake it again.
        
        bool is_first_run = true;
        
        // How many bytes should we buffer at a time?
        const size_t MAX_CHUNK_SIZE = 4096;
    
        // Fill up this buffer with chunks of a certian size
        std::string buffer;
    
        implementation.load([&]() {
            if (is_first_run) {
                // Fake the prefix
                is_first_run = false;
                return get_prefix();
            }
            
            // Otherwise use the buffer
            buffer.resize(MAX_CHUNK_SIZE);
            // Grab a chunk
            in.read(&buffer.at(0), MAX_CHUNK_SIZE);
            if (!in) {
                // Didn't read all the characters, so shrink down (maybe to 0)
                buffer.resize(in.gcount());
            }
            // Copy the buffer over to the caller.
            // TODO: can we save a copy here?
            return buffer;
        }, get_prefix());
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
