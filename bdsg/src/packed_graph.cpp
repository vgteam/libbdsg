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
        implementation.construct("MPG1");
    }
    
    // Delegate copy and move to the copy and move constructors for the things that exist in mapped memory.
    
    MappedPackedGraph::MappedPackedGraph(const MappedPackedGraph& other) {
        implementation.construct("MPG1", *other.get());
    }
    
    MappedPackedGraph& MappedPackedGraph::operator=(const MappedPackedGraph& other) {
        if (get() != other.get()) {
            implementation.construct("MPG1", *other.get());
        }
        return *this;
    }
    
    MappedPackedGraph::MappedPackedGraph(MappedPackedGraph&& other) {
        implementation.construct("MPG1", std::move(*other.get()));
    }
    
    MappedPackedGraph& MappedPackedGraph::operator=(MappedPackedGraph&& other) {
        if (get() != other.get()) {
            implementation.construct("MPG1", std::move(*other.get()));
        }
        return *this;
    }
}
