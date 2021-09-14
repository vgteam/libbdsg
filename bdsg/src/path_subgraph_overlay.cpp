#include "bdsg/overlays/path_subgraph_overlay.hpp"

#include <atomic>

namespace bdsg {

using namespace std;
using namespace handlegraph;

PathSubgraphOverlay::PathSubgraphOverlay(const PathHandleGraph* backing, const unordered_set<nid_t>* node_subset) :
    SubgraphOverlay(backing, node_subset),
    backing_path_graph(backing) {

    backing->for_each_path_handle([&](const path_handle_t& path_handle) {
            bool fully_contained = true;
            backing->for_each_step_in_path(path_handle, [&](const step_handle_t& step_handle) -> bool {
                    if (!has_node(backing->get_id(backing->get_handle_of_step(step_handle)))) {
                        fully_contained = false;
                        return false;
                    }
                    return true;
                });
            if (fully_contained) {
                path_subset.insert(path_handle);
            }
        });
}

PathSubgraphOverlay::~PathSubgraphOverlay() {
}

size_t PathSubgraphOverlay::get_path_count() const {
    return path_subset.size();
}
    
bool PathSubgraphOverlay::has_path(const std::string& path_name) const {
    return backing_path_graph->has_path(path_name) &&
        path_subset.count(backing_path_graph->get_path_handle(path_name));
}
    
path_handle_t PathSubgraphOverlay::get_path_handle(const std::string& path_name) const {
    if (!has_path(path_name)) {
        throw runtime_error("Path " + path_name + " not in subgraph overlay");
    } else {
        return backing_path_graph->get_path_handle(path_name);
    }
}

std::string PathSubgraphOverlay::get_path_name(const path_handle_t& path_handle) const {
    return backing_path_graph->get_path_name(path_handle);
}
    
bool PathSubgraphOverlay::get_is_circular(const path_handle_t& path_handle) const {
    return backing_path_graph->get_is_circular(path_handle);
}
    
size_t PathSubgraphOverlay::get_step_count(const path_handle_t& path_handle) const {
    return backing_path_graph->get_step_count(path_handle);
}
    
handle_t PathSubgraphOverlay::get_handle_of_step(const step_handle_t& step_handle) const {
    return backing_path_graph->get_handle_of_step(step_handle);
}
    
path_handle_t PathSubgraphOverlay::get_path_handle_of_step(const step_handle_t& step_handle) const {
    return backing_path_graph->get_path_handle_of_step(step_handle);
}

step_handle_t PathSubgraphOverlay::path_begin(const path_handle_t& path_handle) const {
    return backing_path_graph->path_begin(path_handle);
}
    
step_handle_t PathSubgraphOverlay::path_end(const path_handle_t& path_handle) const {
    return backing_path_graph->path_end(path_handle);
}
    
step_handle_t PathSubgraphOverlay::path_back(const path_handle_t& path_handle) const {
    return backing_path_graph->path_back(path_handle);
}
    
step_handle_t PathSubgraphOverlay::path_front_end(const path_handle_t& path_handle) const {
    return backing_path_graph->path_front_end(path_handle);
}

bool PathSubgraphOverlay::has_next_step(const step_handle_t& step_handle) const {
    return backing_path_graph->has_next_step(step_handle);
}

bool PathSubgraphOverlay::has_previous_step(const step_handle_t& step_handle) const {
    return backing_path_graph->has_previous_step(step_handle);
}
    
step_handle_t PathSubgraphOverlay::get_next_step(const step_handle_t& step_handle) const {
    return backing_path_graph->get_next_step(step_handle);
}
    
step_handle_t PathSubgraphOverlay::get_previous_step(const step_handle_t& step_handle) const {
    return backing_path_graph->get_previous_step(step_handle);
}

bool PathSubgraphOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
    bool keep_going = true;
    for (auto path_it = path_subset.begin(); keep_going && path_it != path_subset.end(); ++path_it) {
        keep_going = iteratee(*path_it);
    }

    return keep_going;
}

bool PathSubgraphOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                       const std::function<bool(const step_handle_t&)>& iteratee) const {
    return backing_path_graph->for_each_step_on_handle(handle, iteratee);
}

}
