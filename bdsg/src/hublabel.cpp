/*
hub labeling implementations

based on the pruned landmark labeling approach by Akiba et al. (2013) 
(https://dl.acm.org/doi/abs/10.1145/2463676.2465315)  

Code heavily references https://github.com/yb47438/An-Experimental-Study-on-Hub-Labeling-based-Shortest-Path-Algorithms/blame/7f43a91bcc47a3e74a52cf8fd034bd33a367920b/src/construction.h
*/
#include "bdsg/hublabel.hpp"
 
namespace bdsg {
bool notInf(int d) {
  return d != INF_INT;
}

void sorted_vec_insert(vector<NodeId>& v, NodeId item) {
  auto ins_itr = upper_bound(v.begin(), v.end(), item);
  v.insert(ins_itr, item);
}
void sorted_vec_insert(vector<HubRecord>& v, HubRecord& item) {
  auto ins_itr = lower_bound(v.begin(), v.end(), item);  
  if (ins_itr == v.end() || ins_itr->hub != item.hub) {
    v.insert(ins_itr, item);
  }
} 

bool pqcomp2(const QueueObj2& o1, const QueueObj2& o2) {
  return get<0>(o1) > get<0>(o2);
}; 

bool pqcomp_hw(const QueueObj2& o1, const QueueObj2& o2) {
  return get<0>(o1) > get<0>(o2);
};

void hub_insert(Label& label, NodesideId ns) {
  NodeId hub = nodeside_to_node(ns);
  if (nodeside_left(ns)) {
    sorted_vec_insert(label.first.first, hub);
  } else {
    sorted_vec_insert(label.first.second, hub);
  } 
}
void hub_insert(HubRecsPair& label, NodesideId ns, DIST_UINT dist) {
  NodeId hub = nodeside_to_node(ns);
  HubRecord rec(hub, dist);
  if (nodeside_left(ns)) {
    sorted_vec_insert(label.first, rec);
  } else {
    sorted_vec_insert(label.second, rec);
  } 
}
 
void hub_emplace(HubRecsPair& label, NodesideId ns, DIST_UINT dist) {
  NodeId hub = nodeside_to_node(ns);
  //HubRecord rec(hub, dist);
  if (nodeside_left(ns)) {
    label.first.emplace_back(hub, dist);
  } else {
    label.second.emplace_back(hub, dist);  
  } 
} 

/*
assumes both input vectors are sorted least to greatest
*/
void merge_intersection(vector<NodeId>& v1, vector<NodeId>& v2, vector<NodeId>& out) {
  auto p1 = v1.begin(); auto p2 = v2.begin();
  while (p1 != v1.end() && p2 != v2.end()) {
    if (*p1 < *p2) {
      p1=next(p1);
    } else {
      if (*p1 > *p2) {
        p2=next(p2);
      } else {
        out.push_back(*p1);
        p1=next(p1); p2=next(p2);
      }
    }
  }
}

DIST_UINT merge_intersection_dist(vector<HubRecord>& v1, vector<HubRecord>& v2, bdsg::HashGraph& g) {
  DIST_UINT min_dist = INF_INT;
  auto p1 = v1.begin(); auto p2 = v2.begin();
  while (p1 != v1.end() && p2 != v2.end()) {
    if (p1->hub < p2->hub) {
      p1=next(p1);
    } else {
      if (p1->hub > p2->hub) {
        p2=next(p2);
      } else {   
        DIST_UINT d = p1->dist + g.get_length(g.get_handle(p1->hub,false)) + p2->dist;
        min_dist = min(min_dist, d);
        p1=next(p1); p2=next(p2);
      }
    }
  }

  return min_dist; 
}

/*
assumes both input vectors are sorted least to greatest
*/ 
void binary_intersection(vector<NodeId>& v1, vector<NodeId>& v2, vector<NodeId>& out) {
  vector<NodeId>& key_vec = v1.size() < v2.size() ? v1 : v2; 
  vector<NodeId>& search_vec = v1.size() < v2.size() ? v2 : v1;
 
  auto start_itr = search_vec.begin();
  auto end_itr = search_vec.end();
  for (auto k: key_vec) {
    start_itr = lower_bound(start_itr, end_itr, k); 
    if (*start_itr == k) {
      out.push_back(k);
    } else {
      if (start_itr == end_itr) {
        break;
      }
    }
  }  
}
DIST_UINT binary_intersection_dist(vector<HubRecord>& v1, vector<HubRecord>& v2, bdsg::HashGraph& g) {
  vector<HubRecord>& key_vec = v1.size() < v2.size() ? v1 : v2; 
  vector<HubRecord>& search_vec = v1.size() < v2.size() ? v2 : v1;
 
  auto start_itr = search_vec.begin();
  auto end_itr = search_vec.end(); 
  DIST_UINT min_dist = INF_INT;
  for (auto k: key_vec) {
    start_itr = lower_bound(start_itr, end_itr, k); 
    if (start_itr == end_itr) {
      return min_dist;
    } 
    if (start_itr->hub == k.hub) {
      DIST_UINT d = start_itr->dist + g.get_length(g.get_handle(k.hub,false)) + k.dist;
      min_dist = min(min_dist, d);
    }
  }  
  return min_dist;
}
DIST_UINT binary_intersection_dist(vector<HubRecord>& v1, vector<HubRecord>& v2, vector<DIST_UINT>& rank_node_lens) {
  vector<HubRecord>& key_vec = v1.size() < v2.size() ? v1 : v2; 
  vector<HubRecord>& search_vec = v1.size() < v2.size() ? v2 : v1;
 
  auto start_itr = search_vec.begin();
  auto end_itr = search_vec.end(); 
  DIST_UINT min_dist = INF_INT;
  for (auto k: key_vec) {
    start_itr = lower_bound(start_itr, end_itr, k); 
    if (start_itr == end_itr) {
      return min_dist;
    } 
    if (start_itr->hub == k.hub) {
      DIST_UINT d = start_itr->dist + rank_node_lens[k.hub] + k.dist;
      min_dist = min(min_dist, d);
    }
  }  
  return min_dist;
}
DIST_UINT binary_intersection_dist(vector<HubRecord>& v1, vector<HubRecord>& v2) {
  vector<HubRecord>& key_vec = v1.size() < v2.size() ? v1 : v2; 
  vector<HubRecord>& search_vec = v1.size() < v2.size() ? v2 : v1;
 
  auto start_itr = search_vec.begin();
  auto end_itr = search_vec.end(); 
  DIST_UINT min_dist = INF_INT;
  for (auto k: key_vec) {
    start_itr = lower_bound(start_itr, end_itr, k); 
    if (start_itr == end_itr) {
      return min_dist;
    } 
    if (start_itr->hub == k.hub) {
      DIST_UINT d = start_itr->dist + k.dist;
      min_dist = min(min_dist, d);
    }
  }  
  return min_dist;
} 

vector<NodeId> hub_vec_intersect(vector<NodeId>& v1, vector<NodeId>& v2) {
  vector<NodeId> shared_nodes;
  binary_intersection(v1, v2, shared_nodes);
  return shared_nodes;
} 

DIST_UINT hl_build_intersect(HubRecsPair& l1, HubRecsPair& l2, bdsg::HashGraph& g) {
  DIST_UINT min1 = binary_intersection_dist(l1.first, l2.second, g);
  DIST_UINT min2 = binary_intersection_dist(l1.second, l2.first, g);
  
  return min(min1, min2);
} 

DIST_UINT hl_build_intersect(HubRecsPair& l1, HubRecsPair& l2, vector<DIST_UINT>& rank_node_lens) {
  DIST_UINT min1 = binary_intersection_dist(l1.first, l2.second, rank_node_lens);
  DIST_UINT min2 = binary_intersection_dist(l1.second, l2.first, rank_node_lens);
  
  return min(min1, min2);
} 

DIST_UINT hl_build_intersect(HubRecsPair& l1, HubRecsPair& l2) {
  DIST_UINT min1 = binary_intersection_dist(l1.first, l2.second);
  DIST_UINT min2 = binary_intersection_dist(l1.second, l2.first);
  
  return min(min1, min2);
}  

bool label_contains(HubRecsPair& label, NodesideId query) {
  NodeId query_node = nodeside_to_node(query);
  if (nodeside_left(query)) {
    return binary_search(label.first.begin(), label.first.end(), query_node); 
  } else {
    return binary_search(label.second.begin(), label.second.end(), query_node); 
  }
}

DIST_UINT get_label_dist(HubRecsPair& label, NodesideId query) {
  NodeId query_node = nodeside_to_node(query);
  if (nodeside_left(query)) {
    return (*lower_bound(label.first.begin(), label.first.end(), query_node)).dist; 
  } else {
    return (*lower_bound(label.second.begin(), label.second.end(), query_node)).dist; 
  } 
} 

void set_label_dist(HubRecsPair& label, NodesideId query, DIST_UINT new_dist) {
  NodeId query_node = nodeside_to_node(query);
  if (nodeside_left(query)) {
    (*lower_bound(label.first.begin(), label.first.end(), query_node)).dist = new_dist; 
  } else {
    (*lower_bound(label.second.begin(), label.second.end(), query_node)).dist = new_dist; 
  } 
}

bool prune_check(HubRecsPair& label, vector<DIST_UINT>& rank_node_lens, vector<DIST_UINT>& rank_dists, vector<DIST_UINT>& rank_dists2, const DIST_UINT& cur_dist) { 
  bool prune = false;
  for (auto& rec: label.first) {
    if (rank_dists2[rec.hub] == INF_INT) { continue; }
    const DIST_UINT hl_dist = rec.dist + rank_node_lens[rec.hub] + rank_dists2[rec.hub];
    if (hl_dist <= cur_dist) { return true; } 
  }
  for (auto& rec: label.second) {
    if (rank_dists[rec.hub] == INF_INT) { continue; }
    const DIST_UINT hl_dist = rec.dist + rank_node_lens[rec.hub] + rank_dists[rec.hub];
    if (hl_dist <= cur_dist) { return true; } 
  }  
  return prune;
}

void pruned_dijkstra(bdsg::HashGraph& g, NodesideId start, vector<HubRecsPair>& labels, vector<DIST_UINT>& path_lengths, vector<NodesideId>& rank_to_ns, vector<NODE_UINT>& ns_to_rank, vector<DIST_UINT>& rank_node_lens, int stop_dist) {  
  auto node_count = g.get_node_count();
  DIST_UINT nodeside_count = node_count*2;
  vector<NodesideId> parent(nodeside_count, INF_INT);
  
  const auto rank_start = ns_to_rank[start];  
  bool self_loop = false;  

  vector<DIST_UINT> rank_dists(node_count+1, INF_INT);
  vector<DIST_UINT> rank_dists2(node_count+1, INF_INT); 
  for (auto& rec: labels[ns_to_rank[start]].first) {
    rank_dists[rec.hub] = rec.dist;
  }
  for (auto& rec: labels[ns_to_rank[start]].second) {
    rank_dists2[rec.hub] = rec.dist;
  } 

  //place initial OTHER_NODE nodesides
  PriorityQueue2 nodeside_queue{pqcomp2}; 
  handle_t start_handle = g.get_handle(nodeside_to_node(start), nodeside_left(start));
  g.follow_edges(start_handle, false, [&](const handle_t& new_h){ 
    NodesideId new_nodeside = handle_entry_nodeside(new_h, g); 
    path_lengths[new_nodeside] = 0;
    nodeside_queue.emplace(0, new_nodeside); 
  });

  auto not_stop_cond = [stop_dist] (const QueueObj2& top_item) {
    bool not_stop_dist = (get<0>(top_item) <= stop_dist) && (get<0>(top_item) != INF_INT);
    return not_stop_dist;
  }; 
  NodesideId last_ns = start;
  while ((!nodeside_queue.empty()) && not_stop_cond(nodeside_queue.top())) {
    const QueueObj2 item = nodeside_queue.top(); nodeside_queue.pop();
    const DIST_UINT cur_dist = get<0>(item);
    const NodesideId cur_nodeside = get<1>(item);
    
    //gotta have this since priorities of C++ priority queue elements can't be updated 
    //all nodesides on queue are reached through OTHER_NODE direction
    if (cur_dist != path_lengths[cur_nodeside]){
      continue;
    }
    last_ns = cur_nodeside;
    if (cur_nodeside != start) [[likely]] { 
      //check if we can prune here
      const auto rank_cur_ns = ns_to_rank[cur_nodeside];
      DIST_UINT label_dist = label_contains(labels[rank_cur_ns], rank_start) ? get_label_dist(labels[rank_cur_ns], rank_start) : INF_INT;
      if (label_dist <= cur_dist) {
        continue;
      } 

      if (prune_check(labels[rank_cur_ns], rank_node_lens, rank_dists, rank_dists2, cur_dist)) { continue; }
    
      hub_emplace(labels[rank_cur_ns], rank_start, cur_dist); 
    }
    
    const NodeId cur_node = nodeside_to_node(cur_nodeside);
    //cross "edge" to other nodeside 
    const handle_t cur_handle = g.get_handle(cur_node, !nodeside_left(cur_nodeside));
    const DIST_UINT cur_handle_len = g.get_length(cur_handle);
    const DIST_UINT new_dist = cur_dist + cur_handle_len;

    //now find the new nodesides reached through the OTHER_NODE direction
    g.follow_edges(cur_handle, false, [&](const handle_t& new_h){
      const NodesideId new_ns = handle_entry_nodeside(new_h, g);
      if (new_dist < path_lengths[new_ns]) {
        path_lengths[new_ns] = new_dist;
        nodeside_queue.emplace(new_dist, new_ns);
        parent[new_ns] = cur_nodeside;
      }
    });
  } 

}        



DIST_UINT hl_query(NodesideId i, NodesideId j, vector<HubRecsPair>& labels, bdsg::HashGraph& g) {
  if (i == j) { return 0; }
  return label_contains(labels[i], j) ? get_label_dist(labels[i], j) : hl_build_intersect(labels[i], labels[j], g); 
}
DIST_UINT hl_query(NodesideId i, NodesideId j, vector<HubRecsPair>& labels, vector<DIST_UINT>& rank_node_lens) {
  if (i == j) { return 0; }
  NodesideId important = min(i, j);
  NodesideId less_important = max(i, j);
  
  return label_contains(labels[less_important], important) ? get_label_dist(labels[less_important], important) : hl_build_intersect(labels[less_important], labels[important], rank_node_lens); 
}
DIST_UINT hl_query2(NodesideId i, NodesideId j, vector<HubRecsPair>& labels) {
  if (i == j) { return 0; }
  NodesideId important = min(i, j);
  NodesideId less_important = max(i, j);
  
  return label_contains(labels[less_important], important) ? get_label_dist(labels[less_important], important) : hl_build_intersect(labels[less_important], labels[important]); 
} 

DIST_UINT get_degree(NodeId node, bdsg::HashGraph& g) {
  const handle_t h = g.get_handle(node, false);
  return g.get_degree(h, true) * g.get_degree(h, false);
} 

vector<HubRecsPair> make_labels(bdsg::HashGraph& g) {
  int node_count = g.get_node_count();
  int nodeside_count = g.get_node_count()*2;

  vector<HubRecsPair> labels; labels.resize(nodeside_count);
 
  auto node_ordering_view = ranges::iota_view(1,node_count+1);
  vector<NodeId> node_ordering(node_ordering_view.begin(), node_ordering_view.end());
  sort(node_ordering.begin(), node_ordering.end(), [&](NodeId n1, NodeId n2) {
                                                     return get_degree(n1, g) > get_degree(n2, g);
                                                   });

  vector<DIST_UINT> rank_node_lens(node_count+1, 0);
  
  vector<NodesideId> ns_ordering(nodeside_count, 0);
   
  vector<NODE_UINT> rank(nodeside_count, 0);
  for (size_t i = 0; i < node_ordering.size(); i++) {
    const NodeId node = node_ordering[i]; 
    const auto nodesides = get_node_nodesides(node);
    rank[nodesides[0]] = i*2;
    rank[nodesides[1]] = i*2+1; 
    ns_ordering[i*2] = nodesides[0];
    ns_ordering[i*2+1] = nodesides[1]; 
    rank_node_lens[i+1] = g.get_length(g.get_handle(node, false)); 
  }

  vector<DIST_UINT> path_lengths;
  //for (int i = 0; i < 2; i++) {    
  path_lengths = vector<DIST_UINT>(nodeside_count, INF_INT); 
     

  for (size_t i = 0; i < node_ordering.size(); i++) { 
    NodeId node = node_ordering[i];
    const auto nodesides = get_node_nodesides(node); 
    for (NodesideId ns: nodesides) {
      pruned_dijkstra(g, ns, labels, path_lengths, ns_ordering, rank, rank_node_lens);
      //for (int i = 0; i < 2; i++) {    
      std::fill(path_lengths.begin(), path_lengths.end(), INF_INT); 
      
    }
  } 
  return labels;
} 

}
