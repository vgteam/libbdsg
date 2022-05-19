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

bool PathSubgraphOverlay::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
    bool keep_going = true;
    for (auto path_it = path_subset.begin(); keep_going && path_it != path_subset.end(); ++path_it) {
        keep_going = iteratee(*path_it);
    }

    return keep_going;
}

bool PathSubgraphOverlay::for_each_step_on_handle_impl(const handle_t& handle,
                                                       const std::function<bool(const step_handle_t&)>& iteratee) const {
    return backing_path_graph->for_each_step_on_handle(handle, [&](const step_handle_t& step) -> bool {
        path_handle_t path = backing_path_graph->get_path_handle_of_step(step);
        if (path_subset.count(path)) {
            return iteratee(step);
        } else {
            // Skip steps on paths that are not in the subset.
            return true;
        }
    });
}

bool PathSubgraphOverlay::for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                                      const std::unordered_set<std::string>* samples,
                                                      const std::unordered_set<std::string>* loci,
                                                      const std::function<bool(const path_handle_t&)>& iteratee) const {
    
    return backing_path_graph->for_each_path_matching(senses, samples, loci, [&](const path_handle_t& path) -> bool {
        if (path_subset.count(path)) {
            return iteratee(path);
        } else {
            // Skip paths that are not in the subset.
            // TODO: would filtering the subset ourselves be faster?
            return true;
        }
    });

}
                    
bool PathSubgraphOverlay::for_each_step_of_sense_impl(const handle_t& visited,
                                                      const PathSense& sense,
                                                      const std::function<bool(const step_handle_t&)>& iteratee) const {
                                                      
                                                      
    return backing_path_graph->for_each_step_of_sense(visited, sense, [&](const step_handle_t& step) -> bool {
        path_handle_t path = backing_path_graph->get_path_handle_of_step(step);
        if (path_subset.count(path)) {
            return iteratee(step);
        } else {
            // Skip steps on paths that are not in the subset.
            return true;
        }
    });
}

}
