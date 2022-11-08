#include "bdsg/overlays/packed_path_position_overlay.hpp"
#include "bdsg/overlays/packed_reference_path_overlay.hpp"

#include <omp.h> // BINDER_IGNORE because Binder can't find this

//#define debug

namespace bdsg {

PackedReferencePathOverlay::PackedReferencePathOverlay(const PathHandleGraph* graph, size_t steps_per_index) : PackedPositionOverlay() {
    // We can't just chain to the base class constructor with these arguments
    // because we need virtual methods in this class to be available before the
    // index build starts.
    this->graph = graph;
    this->steps_per_index = steps_per_index;

    // Now do the index build
    index_path_positions();
}

size_t PackedReferencePathOverlay::scan_path(const path_handle_t& path_handle, void*& user_data) {
    return PackedPositionOverlay::scan_path(path_handle, user_data);
}

void PackedReferencePathOverlay::set_index_count(size_t count) {
    PackedPositionOverlay::set_index_count(count);
}

void PackedReferencePathOverlay::index_paths(size_t i, const std::vector<path_handle_t>::const_iterator& begin_path, const std::vector<path_handle_t>::const_iterator& end_path, size_t cumul_path_size, void** user_data_base) {
    PackedPositionOverlay::index_paths(i, begin_path, end_path, cumul_path_size, user_data_base);
}

}
