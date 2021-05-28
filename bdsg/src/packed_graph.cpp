//
//  packed_graph.cpp
//

#include "bdsg/packed_graph.hpp"

#include <handlegraph/util.hpp>
#include <atomic>

namespace bdsg {

    using namespace handlegraph;
    
    template class BasePackedGraph<STLBackend>;
    template class BasePackedGraph<CompatBackend>;
    template class BasePackedGraph<MappedBackend>;
    
   
}
