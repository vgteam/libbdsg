//
//  hash_graph.cpp
//

#include "bdsg/hash_graph.hpp"

#include <handlegraph/util.hpp>
#include <unordered_set>

namespace bdsg {
    
    using namespace handlegraph;
    
    HashGraph::HashGraph() {
        
    }
    
    HashGraph::~HashGraph() {
        
    }

    HashGraph::HashGraph(const HashGraph& other) {
        *this = other;
    }

    HashGraph::HashGraph(HashGraph&& other) {
        *this = std::move(other);
    }

    HashGraph& HashGraph::operator=(const HashGraph& other) {
        
        max_id = other.max_id;
        min_id = other.min_id;
        path_id = other.path_id;
        paths = other.paths;
        next_path_id = other.next_path_id;
        
        // can't directly copy the nodes, because their pointers to path mappings
        // will be different in the copy
        graph.reserve(other.graph.size());
        for (const auto& node_rec : other.graph) {
            
            graph[node_rec.first] = node_t(node_rec.second.sequence);
            
            node_t& node = graph[node_rec.first];
            node.left_edges = node_rec.second.left_edges;
            node.right_edges = node_rec.second.right_edges;
            
            node.occurrences.reserve(node_rec.second.occurrences.size());
        }
        
        // and rebuild the occurrence index for the new pointers
        for_each_path_handle_impl([&](const path_handle_t& path) {
            for_each_step_in_path(path, [&](const step_handle_t& step) {
                path_mapping_t* mapping = (path_mapping_t*) intptr_t(as_integers(step)[1]);
                graph[get_id(mapping->handle)].occurrences.push_back(mapping);
            });
            return true;
        });
        return *this;
    }

    HashGraph& HashGraph::operator=(HashGraph&& other) {
        max_id = other.max_id;
        min_id = other.min_id;
        graph = std::move(other.graph);
        path_id = std::move(other.path_id);
        paths = std::move(other.paths);
        next_path_id = other.next_path_id;
        return *this;
    }
    
    HashGraph::HashGraph(istream& in) {
        deserialize(in);
    }
    
    
    handle_t HashGraph::create_handle(const string& sequence) {
        return create_handle(sequence, max_id + 1);
    }
    
    handle_t HashGraph::create_handle(const string& sequence, const nid_t& id) {
       
        // TODO: We can't actually ban empty nodes yet. vg::algorithms::extract_extending_graph needs them.
        // Maybe define a tag interface for graphs that can have them?
        graph[id] = node_t(sequence);
        max_id = max(max_id, id);
        min_id = min(min_id, id);
        return get_handle(id, false);
    }
    
    void HashGraph::create_edge(const handle_t& left, const handle_t& right) {
       
        // look for the edge
        bool add_edge = follow_edges(left, false, [&](const handle_t& next) {
            return next != right;
        });
        
        // don't duplicate it
        if (!add_edge) {
            return;
        }
       
        if (get_is_reverse(left)) {
            graph[get_id(left)].left_edges.push_back(right);
        }
        else {
            graph[get_id(left)].right_edges.push_back(right);
        }
        
        // a reversing self-edge only touches one side of one node, so we only want
        // to add it to a single edge list rather than two
        if (left != flip(right)){
            if (get_is_reverse(right)) {
                graph[get_id(right)].right_edges.push_back(flip(left));
            }
            else {
                graph[get_id(right)].left_edges.push_back(flip(left));
            }
        }
    }
    
    bool HashGraph::has_node(nid_t node_id) const {
        return graph.count(node_id);
    }
    
    handle_t HashGraph::get_handle(const nid_t& node_id, bool is_reverse) const {
        return handlegraph::number_bool_packing::pack(node_id, is_reverse);
    }
    
    nid_t HashGraph::get_id(const handle_t& handle) const {
        return handlegraph::number_bool_packing::unpack_number(handle) ;
    }
    
    bool HashGraph::get_is_reverse(const handle_t& handle) const {
        return handlegraph::number_bool_packing::unpack_bit(handle);
    }
    
    handle_t HashGraph::flip(const handle_t& handle) const {
        return handlegraph::number_bool_packing::toggle_bit(handle);
    }
    
    size_t HashGraph::get_length(const handle_t& handle) const {
        return graph.at(get_id(handle)).sequence.size();
    }
    
    string HashGraph::get_sequence(const handle_t& handle) const {
        return get_is_reverse(handle) ? reverse_complement(graph.at(get_id(handle)).sequence)
                                      : graph.at(get_id(handle)).sequence;
    }
    
    bool HashGraph::follow_edges_impl(const handle_t& handle, bool go_left,
                                      const std::function<bool(const handle_t&)>& iteratee) const {
        
        auto& edge_list = get_is_reverse(handle) != go_left ? graph.at(get_id(handle)).left_edges
                                                            : graph.at(get_id(handle)).right_edges;
        
        bool keep_going = true;
        for (auto it = edge_list.begin(); it != edge_list.end() && keep_going; it++) {
            keep_going = iteratee(go_left ? flip(*it) : *it);
        }
        return keep_going;
    }
    
    size_t HashGraph::get_node_count(void) const {
        return graph.size();
    }
    
    nid_t HashGraph::min_node_id(void) const {
        return min_id;
    }
    
    nid_t HashGraph::max_node_id(void) const {
        return max_id;
    }
    
    size_t HashGraph::get_degree(const handle_t& handle, bool go_left) const {
        auto& edge_list = get_is_reverse(handle) != go_left ? graph.at(get_id(handle)).left_edges
                                                            : graph.at(get_id(handle)).right_edges;
        return edge_list.size();
    }
    
    char HashGraph::get_base(const handle_t& handle, size_t index) const {
        const string& seq = graph.at(get_id(handle)).sequence;
        return get_is_reverse(handle) ? reverse_complement(seq.at(seq.size() - index - 1)) : seq.at(index);
    }
    
    string HashGraph::get_subsequence(const handle_t& handle, size_t index, size_t size) const {
        const string& seq = graph.at(get_id(handle)).sequence;
        size = min(size, seq.size() - index);
        return get_is_reverse(handle) ? reverse_complement(seq.substr(seq.size() - index - size, size)) : seq.substr(index, size);
    }
    
    bool HashGraph::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee,
                                         bool parallel) const {
        
        bool keep_going = true;
        if (parallel) {
#pragma omp parallel shared(keep_going)
            {
#pragma omp single
                {
                    for (auto it = graph.begin(); it != graph.end() && keep_going; it++) {
#pragma omp task
                        {
                            bool thread_keep_going = iteratee(get_handle(it->first));
#pragma omp atomic
                            keep_going = keep_going & thread_keep_going; // OMP only likes bitwise AND
                        }
                    }
                }
            }
            
        }
        else {
            for (auto it = graph.begin(); it != graph.end() && keep_going; it++) {
                keep_going = iteratee(get_handle(it->first));
            }
        }
        
        return keep_going;
    }
    
    handle_t HashGraph::apply_orientation(const handle_t& handle) {
        
        // don't do anything if it's already forward
        if (!get_is_reverse(handle)) {
            return handle;
        }
        
        // reverse the sequence
        node_t& node = graph[get_id(handle)];
        node.sequence = reverse_complement(node.sequence);
        
        // reverse the orientation of the handle in the edge lists
        for (vector<handle_t>* edge_list : {&node.left_edges, &node.right_edges}) {
            for (const handle_t& target : *edge_list) {
                node_t& other_node = graph[get_id(target)];
                auto& bwd_edge_list = get_is_reverse(target) ? other_node.right_edges : other_node.left_edges;
                for (handle_t& bwd_handle : bwd_edge_list) {
                    if (get_id(bwd_handle) == get_id(handle)) {
                        bwd_handle = flip(bwd_handle);
                        break;
                    }
                    // note: if a node has an edge to both sides of another node, we will end up flipping the
                    // same edge target twice, but this actually ends with the edge list in the desired state,
                    // so it's okay
                }
            }
        }
        
        // the edge lists switch sides
        swap(node.left_edges, node.right_edges);
        
        // update the occurrences on paths
        for (path_mapping_t* occurrence : node.occurrences) {
            occurrence->handle = flip(occurrence->handle);
        }
        
        // make it forward and return it
        return flip(handle);
    }
    
    vector<handle_t> HashGraph::divide_handle(const handle_t& handle, const vector<size_t>& offsets) {
        
        // put the offsets in forward orientation to simplify subsequent steps
        vector<size_t> forward_offsets = offsets;
        size_t node_length = get_length(handle);
        if (get_is_reverse(handle)) {
            reverse(forward_offsets.begin(), forward_offsets.end());
            for (size_t& off : forward_offsets) {
                off = node_length - off;
            }
        }
        
        // we will also build the return value in forward orientation
        handle_t forward_handle = forward(handle);
        vector<handle_t> return_val;
        return_val.push_back(forward_handle);
        
        // make it easy to handle edge cases by returning here if we're not actually dividing
        if (offsets.empty()) {
            return return_val;
        }
        
        // divvy up the sequence onto separate nodes
        for (size_t i = 0; i < forward_offsets.size(); i++) {
            size_t length = (i + 1 < forward_offsets.size() ? forward_offsets[i + 1] : node_length) - forward_offsets[i];
            return_val.push_back(create_handle(graph[get_id(handle)].sequence.substr(forward_offsets[i], length)));
        }
        graph[get_id(handle)].sequence = graph[get_id(handle)].sequence.substr(0, forward_offsets.front());
        
        // move the edges out the end of the node to the final one
        node_t& final_node = graph[get_id(return_val.back())];
        node_t& orig_node = graph[get_id(handle)];
        final_node.right_edges = std::move(orig_node.right_edges);
        orig_node.right_edges.clear();

        // update the backwards references back onto this node
        for (handle_t& next : final_node.right_edges) {
            if (next == flip(forward_handle)) {
                next = flip(return_val.back());
                continue;
            }
            for (handle_t& bwd_target : get_is_reverse(next) ?
                                        graph[get_id(next)].right_edges :
                                        graph[get_id(next)].left_edges) {
                if (bwd_target == flip(forward_handle)) {
                    bwd_target = flip(return_val.back());
                    break;
                }
            }
        }
        
        // create edges between the segments of the original node
        for (size_t i = 1; i < return_val.size(); i++) {
            graph[get_id(return_val[i - 1])].right_edges.push_back(return_val[i]);
            graph[get_id(return_val[i])].left_edges.push_back(flip(return_val[i - 1]));
        }
        
        // update the paths and the occurrence records
        for (path_mapping_t* mapping : graph[get_id(handle)].occurrences) {
            path_t& path = paths[mapping->path_id];
            if (get_is_reverse(mapping->handle)) {
                for (size_t i = return_val.size() - 1; i > 0; i--) {
                    path_mapping_t* new_mapping = path.insert_before(flip(return_val[i]), mapping);
                    graph[get_id(return_val[i])].occurrences.push_back(new_mapping);
                }
            }
            else {
                mapping = mapping->next;
                for (size_t i = 1; i < return_val.size(); i++) {
                    path_mapping_t* new_mapping = path.insert_before(return_val[i], mapping);
                    graph[get_id(return_val[i])].occurrences.push_back(new_mapping);
                }
            }
        }
        
        if (get_is_reverse(handle)) {
            // reverse the orientation of the return value to match the input
            reverse(return_val.begin(), return_val.end());
            for (handle_t& ret_handle : return_val) {
                ret_handle = flip(ret_handle);
            }
        }
        
        return return_val;
    }
    
    void HashGraph::optimize(bool allow_id_reassignment) {
        /// tighten up the memory allocated to the vectors in the data structure
        for (pair<const nid_t, node_t>& node_record : graph) {
            node_record.second.sequence.shrink_to_fit();
            node_record.second.left_edges.shrink_to_fit();
            node_record.second.right_edges.shrink_to_fit();
            node_record.second.occurrences.shrink_to_fit();
        }
        // reassign hash tables to the midpoint of their max and min load factors
        // TODO: is this a good way to choose load factor?
        graph.rehash(graph.size() * 0.5 * (graph.min_load_factor() + graph.max_load_factor()));
        path_id.rehash(path_id.size() * 0.5 * (path_id.min_load_factor() + path_id.max_load_factor()));
        paths.rehash(paths.size() * 0.5 * (paths.min_load_factor() + paths.max_load_factor()));
    }
    
    bool HashGraph::apply_ordering(const vector<handle_t>& order, bool compact_ids) {
        // TODO: implement ID compaction I guess?
        return false;
    }
    
    void HashGraph::destroy_handle(const handle_t& handle) {
    
        // Clear out any paths on this handle. 
        // We need to first compose a list of distinct visiting paths.
        std::unordered_set<path_handle_t> visiting_paths;
        for_each_step_on_handle(handle, [&](const step_handle_t& step) {
            visiting_paths.insert(get_path_handle_of_step(step)); 
        });
        for (auto& p : visiting_paths) {
            // Then we destroy all of them.
            destroy_path(p);
        }
        
        // remove backwards references from edges on other nodes
        node_t& node = graph[get_id(handle)];
        for (vector<handle_t>* edge_list : {&node.left_edges, &node.right_edges}) {
            for (const handle_t& next : *edge_list) {
                auto& bwd_edge_list = get_is_reverse(next) ? graph[get_id(next)].right_edges : graph[get_id(next)].left_edges;
                for (handle_t& bwd_target : bwd_edge_list) {
                    if (get_id(bwd_target) == get_id(handle)) {
                        bwd_target = bwd_edge_list.back();
                        bwd_edge_list.pop_back();
                        break;
                    }
                }
            }
        }
        
        // remove this node from the relevant indexes
        graph.erase(get_id(handle));
    }

    handle_t HashGraph::change_sequence(const handle_t& handle, const std::string& sequence) {
        
        graph[get_id(handle)].sequence = sequence;
        
        return handle;
    }
    
    void HashGraph::destroy_edge(const handle_t& left, const handle_t& right) {
        
        // remove this edge from left
        node_t& left_node = graph[get_id(left)];
        auto& left_edge_list = get_is_reverse(left) ? left_node.left_edges : left_node.right_edges;
        
        for (handle_t& next : left_edge_list) {
            if (next == right) {
                next = left_edge_list.back();
                left_edge_list.pop_back();
                break;
            }
        }
        
        // remove this edge from right
        node_t& right_node = graph[get_id(right)];
        auto& right_edge_list = get_is_reverse(right) ? right_node.right_edges : right_node.left_edges;
        
        for (handle_t& prev : right_edge_list) {
            if (prev == flip(left)) {
                prev = right_edge_list.back();
                right_edge_list.pop_back();
                break;
            }
        }
    }

    handle_t HashGraph::truncate_handle(const handle_t& handle, bool trunc_left, size_t offset) {
        
        handle_t fwd_handle = forward(handle);
        offset = get_is_reverse(handle) ? get_length(handle) - offset : offset;
        trunc_left = get_is_reverse(handle) != trunc_left;
        
        node_t& node = graph[get_id(fwd_handle)];
        // TODO: repetitive with destroy_edge
        if (trunc_left) {
            // remove references on the other nodes
            for (handle_t left : node.left_edges) {
                if (left == fwd_handle) {
                    continue;
                }
                node_t& left_node = graph[get_id(left)];
                auto& left_edge_list = get_is_reverse(left) ? left_node.right_edges : left_node.left_edges;
                
                for (handle_t& next : left_edge_list) {
                    if (next == fwd_handle) {
                        next = left_edge_list.back();
                        left_edge_list.pop_back();
                        break;
                    }
                }
            }
            // remove references on this node
            node.left_edges.clear();
            node.sequence = node.sequence.substr(offset, string::npos);
        }
        else {
            // remove references on the other nodes
            for (handle_t right : node.right_edges) {
                if (right == flip(fwd_handle)) {
                    continue;
                }
                node_t& right_node = graph[get_id(right)];
                auto& right_edge_list = get_is_reverse(right) ? right_node.right_edges : right_node.left_edges;
                
                for (handle_t& prev : right_edge_list) {
                    if (prev == flip(fwd_handle)) {
                        prev = right_edge_list.back();
                        right_edge_list.pop_back();
                        break;
                    }
                }
            }
            // remove references on this node
            node.right_edges.clear();
            node.sequence.resize(offset);
        }
        
        return handle;
    }
    
    void HashGraph::clear(void) {
        max_id = 0;
        min_id = numeric_limits<nid_t>::max();
        next_path_id = 1;
        graph.clear();
        path_id.clear();
        paths.clear();
    }
    
    size_t HashGraph::get_path_count() const {
        return paths.size();
    }
    
    bool HashGraph::has_path(const std::string& path_name) const {
        return path_id.count(path_name);
    }
    
    path_handle_t HashGraph::get_path_handle(const std::string& path_name) const {
        return as_path_handle(path_id.at(path_name));
    }
    
    string HashGraph::get_path_name(const path_handle_t& path_handle) const {
        return paths.at(as_integer(path_handle)).name;
    }
    
    bool HashGraph::get_is_circular(const path_handle_t& path_handle) const {
        return paths.at(as_integer(path_handle)).is_circular;
    }
    
    size_t HashGraph::get_step_count(const path_handle_t& path_handle) const {
        return paths.at(as_integer(path_handle)).count;
    }
    
    bool HashGraph::for_each_path_handle_impl(const std::function<bool(const path_handle_t&)>& iteratee) const {
        for (auto it = paths.begin(); it != paths.end(); it++) {
            if (!iteratee(as_path_handle(it->first))) {
                return false;
            }
        }
        return true;
    }
    
    handle_t HashGraph::get_handle_of_step(const step_handle_t& step_handle) const {
        return ((path_mapping_t*) intptr_t(as_integers(step_handle)[1]))->handle;
    }
    
    step_handle_t HashGraph::path_begin(const path_handle_t& path_handle) const {
        step_handle_t step;
        as_integers(step)[0] = as_integer(path_handle);
        as_integers(step)[1] = intptr_t(paths.at(as_integer(path_handle)).head);
        return step;
    }
    
    step_handle_t HashGraph::path_end(const path_handle_t& path_handle) const {
        step_handle_t step;
        as_integers(step)[0] = as_integer(path_handle);
        as_integers(step)[1] = intptr_t(nullptr);
        return step;
    }
    
    step_handle_t HashGraph::path_back(const path_handle_t& path_handle) const {
        step_handle_t step;
        as_integers(step)[0] = as_integer(path_handle);
        as_integers(step)[1] = intptr_t(paths.at(as_integer(path_handle)).tail);
        return step;
    }
    
    step_handle_t HashGraph::path_front_end(const path_handle_t& path_handle) const {
        // we'll actually use the same sentinel
        return path_end(path_handle);
    }
    
    bool HashGraph::has_next_step(const step_handle_t& step_handle) const {
        return ((path_mapping_t*) intptr_t(as_integers(step_handle)[1]))->next != nullptr;
    }
    
    bool HashGraph::has_previous_step(const step_handle_t& step_handle) const {
        return ((path_mapping_t*) intptr_t(as_integers(step_handle)[1]))->prev != nullptr;
    }
    
    step_handle_t HashGraph::get_next_step(const step_handle_t& step_handle) const {
        step_handle_t next;
        as_integers(next)[0] = as_integers(step_handle)[0];
        as_integers(next)[1] = intptr_t(((path_mapping_t*) intptr_t(as_integers(step_handle)[1]))->next);
        return next;
    }
    
    step_handle_t HashGraph::get_previous_step(const step_handle_t& step_handle) const {
        step_handle_t prev;
        as_integers(prev)[0] = as_integers(step_handle)[0];
        as_integers(prev)[1] = as_integers(step_handle)[1] != intptr_t(nullptr) ? intptr_t(((path_mapping_t*) intptr_t(as_integers(step_handle)[1]))->prev)
                                                                                : intptr_t(paths.at(as_integers(step_handle)[0]).tail);
        return prev;
    }
    
    path_handle_t HashGraph::get_path_handle_of_step(const step_handle_t& step_handle) const {
        return as_path_handle(as_integers(step_handle)[0]);
    }
    
    bool HashGraph::for_each_step_on_handle_impl(const handle_t& handle,
                                                 const function<bool(const step_handle_t&)>& iteratee) const {
        for (path_mapping_t* mapping : graph.at(get_id(handle)).occurrences) {
            step_handle_t step;
            as_integers(step)[0] = mapping->path_id;
            as_integers(step)[1] = intptr_t(mapping);
            
            if (!iteratee(step)) {
                return false;
            }
        }
        return true;
    }

    void HashGraph::destroy_path(const path_handle_t& path) {
        destroy_paths({path});
    }
    
    void HashGraph::destroy_paths(const vector<path_handle_t>& paths) {
        
        unordered_set<int64_t> path_ids;
        for (auto path : paths) {
            path_ids.emplace(as_integer(path));
        }
        int64_t first_path = -1;
        if (path_ids.size() == 1) {
            first_path = *path_ids.begin();
        }
        unordered_set<nid_t> nodes_visited;
        
        for (auto path : paths) {
            // remove the records of nodes occurring on this path
            for_each_step_in_path(path, [&](const step_handle_t& step) {
                path_mapping_t* mapping = (path_mapping_t*) intptr_t(as_integers(step)[1]);
                if (paths.size() > 1) {
                    bool did_insert = nodes_visited.insert(get_id(mapping->handle)).second;
                    if (!did_insert) {
                        // we've already deleted on this node
                        return;
                    }
                }
                vector<path_mapping_t*>& node_occs = graph[get_id(mapping->handle)].occurrences;
                for (size_t i = 0; i < node_occs.size(); ) {
                    if (first_path != -1 ? node_occs[i]->path_id == first_path : path_ids.count(node_occs[i]->path_id)) {
                        node_occs[i] = node_occs.back();
                        node_occs.pop_back();
                    }
                    else {
                        ++i;
                    }
                }
            });
            
            // erase the path itself
            path_id.erase(this->paths[as_integer(path)].name);
            this->paths.erase(as_integer(path));
        }
    }
    
    path_handle_t HashGraph::create_path_handle(const string& name, bool is_circular) {
        path_id[name] = next_path_id;
        paths[next_path_id] = path_t(name, next_path_id, is_circular);
        next_path_id++;
        return as_path_handle(next_path_id - 1);
    }
    
    step_handle_t HashGraph::append_step(const path_handle_t& path, const handle_t& to_append) {
        
        path_t& path_list = paths[as_integer(path)];
        path_mapping_t* mapping = path_list.push_back(to_append);
        graph[get_id(to_append)].occurrences.push_back(mapping);
        
        step_handle_t step;
        as_integers(step)[0] = as_integer(path);
        as_integers(step)[1] = intptr_t(mapping);
        return step;
    }
    
    step_handle_t HashGraph::prepend_step(const path_handle_t& path, const handle_t& to_prepend) {
        
        path_t& path_list = paths[as_integer(path)];
        path_mapping_t* mapping = path_list.push_front(to_prepend);
        graph[get_id(to_prepend)].occurrences.push_back(mapping);
        
        step_handle_t step;
        as_integers(step)[0] = as_integer(path);
        as_integers(step)[1] = intptr_t(mapping);
        return step;
    }
    
    pair<step_handle_t, step_handle_t> HashGraph::rewrite_segment(const step_handle_t& segment_begin,
                                                                  const step_handle_t& segment_end,
                                                                  const std::vector<handle_t>& new_segment) {
        
        if (get_path_handle_of_step(segment_begin) != get_path_handle_of_step(segment_end)) {
            cerr << "error:[HashGraph] attempted to rewrite a path segment delimited by steps on two different paths" << endl;
            exit(1);
        }
        
        path_mapping_t* begin = (path_mapping_t*) intptr_t(as_integers(segment_begin)[1]);
        path_mapping_t* end = (path_mapping_t*) intptr_t(as_integers(segment_end)[1]);
        
        path_t& path_list = paths[as_integers(segment_begin)[0]];
        
        for (path_mapping_t* mapping = begin; mapping != end;) {
            
            // remove this occurrence of the mapping from the occurrences index
            auto& node_occurrences = graph[get_id(mapping->handle)].occurrences;
            for (size_t i = 0; i < node_occurrences.size(); ++i){
                if (node_occurrences[i] == mapping) {
                    node_occurrences[i] = node_occurrences.back();
                    node_occurrences.pop_back();
                    break;
                }
            }
            
            path_mapping_t* next = mapping->next;
            
            // remove the step from the path
            path_list.remove(mapping);
            
            mapping = next;
        }
        
        // init the new range for the return value
        pair<step_handle_t, step_handle_t> new_range(segment_end, segment_end);
        
        // add the new segment into the slot
        bool first_iter = true;
        for (const handle_t& handle : new_segment) {
            
            path_mapping_t* mapping = path_list.insert_before(handle, end);
            graph[get_id(handle)].occurrences.push_back(mapping);
            
            if (first_iter) {
                as_integers(new_range.first)[1] = intptr_t(mapping);
                first_iter = false;
            }
        }
        
        return new_range;
    }
    
    void HashGraph::set_circularity(const path_handle_t& path, bool circular) {
        // set the annotation
        path_t& path_list = paths[as_integer(path)];
        path_list.is_circular = circular;
        
        // set the circular connection between the head and tail
        if (path_list.head) {
            if (circular) {
                path_list.head->prev = path_list.tail;
                path_list.tail->next = path_list.head;
            }
            else {
                path_list.head->prev = nullptr;
                path_list.tail->next = nullptr;
            }
        }
    }

    void HashGraph::set_id_increment(const nid_t& min_id) {
        // no-op as this implementation does not require this hint for decent construction performance
    }

    void HashGraph::increment_node_ids(nid_t increment) {
        reassign_node_ids([&increment](const nid_t& node_id) { return node_id + increment; });
    }
    
    void HashGraph::reassign_node_ids(const std::function<nid_t(const nid_t&)>& get_new_id) {
        // We're going to have to move all the nodes to this new map.
        hash_map<nid_t, node_t> new_graph;
        
        nid_t new_max_id = 0;
        nid_t new_min_id = std::numeric_limits<nid_t>::max();
        
        auto it = graph.begin();
        while (it != graph.end()) {
            // For each node we have
            auto& record = it->second;
            
            // Convert its ID
            nid_t new_id = get_new_id(it->first);
            new_max_id = std::max(new_max_id, new_id);
            new_min_id = std::min(new_min_id, new_id);
            
            // Set up a new record for it
            auto& new_record = new_graph[new_id];
            
            // Move and copy to fill it
            new_record.sequence = std::move(record.sequence);
            new_record.left_edges.reserve(record.left_edges.size());
            for (const handle_t& old_handle : record.left_edges) {
                // Transform the IDs in all the handles for left edges
                new_record.left_edges.emplace_back(set_id(old_handle, get_new_id(get_id(old_handle))));
            }
            for (const handle_t& old_handle : record.right_edges) {
                // Transform the IDs in all the handles for right edges
                new_record.right_edges.emplace_back(set_id(old_handle, get_new_id(get_id(old_handle))));
            }
            new_record.occurrences = std::move(record.occurrences);
        
            // Remove from old map
            it = graph.erase(it);
        }
        
        // Apply the new graph
        graph = std::move(new_graph);
        
        // Now we just have to change the paths, in place, so we don't break occurrence pointers
        for (auto& id_and_path : paths) {
            // For each path
            bool first_iter = true;
            for (path_mapping_t* here = id_and_path.second.head;
                here != nullptr && (first_iter || here != id_and_path.second.head);
                here = here->next) {
                
                // For each mapping in the possibly circular linked list, transform the ID in the handle.
                here->handle = set_id(here->handle, get_new_id(get_id(here->handle)));
                first_iter = false;
            }
        }
        
        // Now apply the graph metadata (and zero the ID offset).
        max_id = new_max_id;
        min_id = new_min_id;
    }
    
    HashGraph::path_t::path_t() {
        
    }
    
    HashGraph::path_t::path_t(const string& name, const int64_t& path_id, bool is_circular) : name(name), path_id(path_id), is_circular(is_circular) {
        
    }
    
    HashGraph::path_t::path_t(path_t&& other) : head(other.head), tail(other.tail), path_id(other.path_id), count(other.count), name(std::move(other.name)), is_circular(other.is_circular) {
        // we grabbed the data in the initializer, now make sure the other one is in a valid state
        other.head = nullptr;
        other.tail = nullptr;
        other.path_id = 0;
        other.count = 0;
    }
    
    HashGraph::path_t& HashGraph::path_t::operator=(path_t&& other) {
        if (this != &other) {
            // free existing list
            this->~path_t();
            
            // steal other list
            head = other.head;
            tail = other.tail;
            other.head = nullptr;
            other.tail = nullptr;
            
            name = std::move(other.name);
            
            is_circular = other.is_circular;
            
            path_id = other.path_id;
            other.path_id = 0;
            
            count = other.count;
            other.count = 0;
        }
        return *this;
    }
    
    HashGraph::path_t::path_t(const path_t& other) : path_id(other.path_id), count(other.count), name(other.name), is_circular(other.is_circular) {
        
        path_mapping_t* prev = nullptr;
        bool first_iter = true;
        for (path_mapping_t* mapping = other.head;
             mapping != nullptr && (first_iter || mapping != other.head); // in case we loop around a circular path
             mapping = mapping->next) {
            
            path_mapping_t* copied = new path_mapping_t(mapping->handle, mapping->path_id);
            
            if (!head) {
                head = copied;
            }
            if (prev) {
                prev->next = copied;
                copied->prev = prev;
            }
            prev = copied;
            first_iter = false;
        }
        tail = prev;
        
        // we only loop over each mapping one time, so we may need to add in the last connection
        // in a circular path
        if (is_circular) {
            tail->next = head;
            head->prev = tail;
        }
    }
    
    HashGraph::path_t& HashGraph::path_t::operator=(const path_t& other) {
        if (this != &other) {
            // free existing list
            this->~path_t();
            
            // copy the other list
            path_mapping_t* prev = nullptr;
            bool first_iter = true;
            for (path_mapping_t* mapping = other.head;
                 mapping != nullptr && (first_iter || mapping != other.head); // in case we loop around a circular path
                 mapping = mapping->next) {
                
                path_mapping_t* copied = new path_mapping_t(mapping->handle, mapping->path_id);
                
                if (!head) {
                    head = copied;
                }
                if (prev) {
                    prev->next = copied;
                    copied->prev = prev;
                }
                prev = copied;
                first_iter = false;
            }
            tail = prev;
            
            // copy the rest of the info
            path_id = other.path_id;
            name = other.name;
            count = other.count;
            is_circular = other.is_circular;
            
            // we only loop over each mapping one time, so we may need to add in the last connection
            // in a circular path
            if (is_circular) {
                tail->next = head;
                head->prev = tail;
            }
        }
        return *this;
    }
    
    HashGraph::path_t::~path_t() {
        bool first_iter = true;
        for (path_mapping_t* mapping = head;
             mapping != nullptr && (first_iter || mapping != head);) { // for circular paths
            
            path_mapping_t* next = mapping->next;
            delete mapping;
            mapping = next;
            
            first_iter = false;
        }
    }
    
    HashGraph::path_mapping_t* HashGraph::path_t::push_back(const handle_t& handle) {
        return insert_before(handle, nullptr);
    }
    
    HashGraph::path_mapping_t* HashGraph::path_t::push_front(const handle_t& handle) {
        return insert_before(handle, head);
    }
    
    void HashGraph::path_t::remove(path_mapping_t* mapping) {
        if (mapping == head) {
            head = mapping->next != mapping ? mapping->next : nullptr;
        }
        if (mapping == tail) {
            tail = mapping->prev != mapping ? mapping->prev : nullptr;
        }
        if (mapping->next) {
            mapping->next->prev = mapping->prev;
        }
        if (mapping->prev) {
            mapping->prev->next = mapping->next;
        }
        count--;
        delete mapping;
    }
    
    HashGraph::path_mapping_t* HashGraph::path_t::insert_before(const handle_t& handle, path_mapping_t* mapping) {
        
        path_mapping_t* inserting = new path_mapping_t(handle, path_id);
        
        if (mapping) {
            inserting->prev = mapping->prev;
            if (mapping->prev) {
                mapping->prev->next = inserting;
            }
            mapping->prev = inserting;
            inserting->next = mapping;
            
            if (mapping == head) {
                head = inserting;
            }
        }
        else if (tail) {
            
            // handle the potential circular connection
            inserting->next = tail->next;
            if (inserting->next) {
                inserting->next->prev = inserting;
            }
            
            inserting->prev = tail;
            tail->next = inserting;
            
            tail = inserting;
        }
        else {
            // the list is empty so far, so initialize it
            head = tail = inserting;
            
            // make an initial circular connection
            if (is_circular) {
                inserting->next = inserting->prev = inserting;
            }
        }
        
        count++;
        return inserting;
    }
    
    void HashGraph::path_t::serialize(ostream& out) const {
        
        out.write((const char*) &is_circular, sizeof(is_circular) / sizeof(char));
        
        int64_t path_id_out = endianness<int64_t>::to_big_endian(path_id);
        out.write((const char*) &path_id_out, sizeof(path_id_out) / sizeof(char));
        
        uint64_t name_size_out = endianness<uint64_t>::to_big_endian(name.size());
        out.write((const char*) &name_size_out, sizeof(name_size_out) / sizeof(char));

        out.write(name.c_str(), name.size());
        
        uint64_t count_out = endianness<uint64_t>::to_big_endian(count);
        out.write((const char*) &count_out, sizeof(count_out) / sizeof(char));
        
        path_mapping_t* mapping = head;
        bool first_iter = true;
        while (mapping && (first_iter || mapping != head)) { // extra condition for circular paths
            int64_t step = endianness<int64_t>::to_big_endian(as_integer(mapping->handle));

            out.write((const char*) &step, sizeof(step) / sizeof(char));
            mapping = mapping->next;
            
            first_iter = false;
        }
    }
    
    void HashGraph::path_t::deserialize(istream& in) {
        // free the current path if it exists
        this->~path_t();
        in.read((char*) &is_circular, sizeof(is_circular) / sizeof(char));
        
        int64_t path_id_in;
        in.read((char*) &path_id_in, sizeof(path_id_in) / sizeof(char));
        path_id = endianness<int64_t>::from_big_endian(path_id_in);
        
        uint64_t name_size_in;
        in.read((char*) &name_size_in, sizeof(name_size_in) / sizeof(char));
        uint64_t name_size = endianness<uint64_t>::from_big_endian(name_size_in);
        
        name.resize(name_size);
        for (size_t i = 0; i < name.size(); i++) {
            in.read((char*) &name[i], sizeof(char));
        }
        
        uint64_t num_mappings_in;
        in.read((char*) &num_mappings_in, sizeof(num_mappings_in) / sizeof(char));
        uint64_t num_mappings = endianness<uint64_t>::from_big_endian(num_mappings_in);
        
        // note: count will be incremented in the push_back method
        count = 0;
        for (size_t i = 0; i < num_mappings; i++) {
            int64_t step_in;
            in.read((char*) &step_in, sizeof(step_in) / sizeof(char));
            int64_t step = endianness<int64_t>::from_big_endian(step_in);
            push_back(as_handle(step));
        }
    }
    
    void HashGraph::node_t::serialize(ostream& out) const {
        
        uint64_t seq_size_out = endianness<uint64_t>::to_big_endian( sequence.size());
        out.write((const char*) &seq_size_out, sizeof(seq_size_out) / sizeof(char));
        out.write(sequence.c_str(), sequence.size());
        
        uint64_t left_edges_size_out = endianness<uint64_t>::to_big_endian(left_edges.size());
        out.write((const char*) &left_edges_size_out, sizeof(left_edges_size_out) / sizeof(char));
        for (size_t i = 0; i < left_edges.size(); i++) {
            int64_t next_out = endianness<int64_t>::to_big_endian(as_integer(left_edges[i]));
            out.write((const char*) &next_out, sizeof(next_out) / sizeof(char));
        }
        
        uint64_t right_edges_size_out = endianness<uint64_t>::to_big_endian(right_edges.size());
        out.write((const char*) &right_edges_size_out, sizeof(right_edges_size_out) / sizeof(char));
        for (size_t i = 0; i < right_edges.size(); i++) {
            int64_t next_out = endianness<int64_t>::to_big_endian(as_integer(right_edges[i]));
            out.write((const char*) &next_out, sizeof(next_out) / sizeof(char));
        }
        
        // don't serialize the occurrences, since they are redundant information with the actual
        // path and the value requires an in memory representation of the paths
    }
    
    void HashGraph::node_t::deserialize(istream& in) {
        
        uint64_t seq_size_in;
        in.read((char*) &seq_size_in, sizeof(seq_size_in) / sizeof(char));
        uint64_t seq_size = endianness<uint64_t>::from_big_endian(seq_size_in);
        sequence.resize(seq_size);
        for (size_t i = 0; i < sequence.size(); i++) {
            in.read((char*) &sequence[i], sizeof(char));
        }
        
        uint64_t num_left_edges_in;
        in.read((char*) &num_left_edges_in, sizeof(num_left_edges_in) / sizeof(char));
        uint64_t num_left_edges = endianness<uint64_t>::from_big_endian(num_left_edges_in);
        left_edges.resize(num_left_edges);
        for (size_t i = 0; i < left_edges.size(); i++) {
            int64_t next_in;
            in.read((char*) &next_in, sizeof(next_in) / sizeof(char));
            int64_t next = endianness<int64_t>::from_big_endian(next_in);
            left_edges[i] = as_handle(next);
        }
        
        uint64_t num_right_edges_in;
        in.read((char*) &num_right_edges_in, sizeof(num_right_edges_in) / sizeof(char));
        uint64_t num_right_edges = endianness<uint64_t>::from_big_endian(num_right_edges_in);
        right_edges.resize(num_right_edges);
        for (size_t i = 0; i < right_edges.size(); i++) {
            int64_t next_in;
            in.read((char*) &next_in, sizeof(next_in) / sizeof(char));
            int64_t next = endianness<int64_t>::from_big_endian(next_in);
            right_edges[i] = as_handle(next);
        }
    }
    
    void HashGraph::serialize_members(ostream& out) const {
        nid_t max_id_out = endianness<nid_t>::to_big_endian(max_id);
        out.write((const char*) &max_id_out, sizeof(max_id_out) / sizeof(char));
        
        nid_t min_id_out = endianness<nid_t>::to_big_endian(min_id);
        out.write((const char*) &min_id_out, sizeof(min_id_out) / sizeof(char));
        
        int64_t next_path_id_out = endianness<int64_t>::to_big_endian(next_path_id);
        out.write((const char*) &next_path_id_out, sizeof(next_path_id_out) / sizeof(char));
        
        uint64_t graph_size_out = endianness<uint64_t>::to_big_endian(graph.size());
        out.write((const char*) &graph_size_out, sizeof(graph_size_out) / sizeof(char));
        
        for (const pair<nid_t, node_t>& node_record : graph) {
            nid_t node_id_out = endianness<nid_t>::to_big_endian(node_record.first);
            out.write((const char*) &node_id_out, sizeof(node_id_out) / sizeof(char));
            node_record.second.serialize(out);
        }
        
        uint64_t paths_size_out = endianness<uint64_t>::to_big_endian(paths.size());
        out.write((const char*) &paths_size_out, sizeof(paths_size_out) / sizeof(char));
        for (const pair<int64_t, path_t>& path_record : paths) {
            path_record.second.serialize(out);
        }
    }
    
    void HashGraph::deserialize_members(istream& in) {
        clear();
        
        nid_t max_id_in;
        in.read((char*) &max_id_in, sizeof(max_id_in) / sizeof(char));
        max_id = endianness<nid_t>::from_big_endian(max_id_in);
        
        nid_t min_id_in;
        in.read((char*) &min_id_in, sizeof(min_id_in) / sizeof(char));
        min_id = endianness<nid_t>::from_big_endian(min_id_in);
        
        int64_t next_path_id_in;
        in.read((char*) &next_path_id_in, sizeof(next_path_id_in) / sizeof(char));
        next_path_id = endianness<int64_t>::from_big_endian(next_path_id_in);
        
        uint64_t num_nodes_in;
        in.read((char*) &num_nodes_in, sizeof(num_nodes_in) / sizeof(char));
        uint64_t num_nodes = endianness<uint64_t>::from_big_endian(num_nodes_in);
        
        graph.reserve(num_nodes);
        for (size_t i = 0; i < num_nodes; i++) {
            nid_t node_id_in;
            in.read((char*) &node_id_in, sizeof(node_id_in) / sizeof(char));
            nid_t node_id = endianness<nid_t>::from_big_endian(node_id_in);
            graph[node_id].deserialize(in);
        }
        
        uint64_t num_paths_in;
        in.read((char*) &num_paths_in, sizeof(num_paths_in) / sizeof(char));
        uint64_t num_paths = endianness<uint64_t>::from_big_endian(num_paths_in);
        
        paths.reserve(num_paths);
        path_id.reserve(num_paths);
        for (size_t i = 0; i < num_paths; i++) {
            path_t path;
            path.deserialize(in);
            path_id[path.name] = path.path_id;
            paths[path.path_id] = std::move(path);
        }
        
        // we need to rebuild the occurrences of node mapping, which is not
        // part of the serialized format
        for (pair<const int64_t, path_t>& path_record : paths) {
            path_t& path = path_record.second;
            bool first_iter = true;
            for (path_mapping_t* mapping = path.head;
                 mapping != nullptr && (first_iter || mapping != path.head); // for circular paths
                 mapping = mapping->next) {
                
                graph[get_id(mapping->handle)].occurrences.push_back(mapping);
                first_iter = false;
            }
        }
    }

    uint32_t HashGraph::get_magic_number() const {
        return 676155192ul;
    }
    
    handle_t HashGraph::set_id(const handle_t& handle, nid_t new_id) {
        bool is_reverse = handlegraph::number_bool_packing::unpack_bit(handle);
        return handlegraph::number_bool_packing::pack(new_id, is_reverse);
    }
    
}
