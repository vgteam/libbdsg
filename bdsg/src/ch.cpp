/*
file for quickly playing around with stuff
*/
#include "bdsg/ch.hpp"

//#define debug_boost_graph
//#define debug_create

namespace bdsg {
bdsg::HashGraph make_test() {
  bdsg::HashGraph g;
  vector<handle_t> h; h.resize(13);
  for (int i = 1; i <= 12; i++) {
    h[i] = g.create_handle("A");
  }
  g.create_edge(h[1], h[2]); g.create_edge(h[2], h[3]);
  g.create_edge(h[2], h[4]); g.create_edge(h[2], h[11]);
  g.create_edge(h[2], g.flip(h[7])); g.create_edge(h[3], h[5]);
  g.create_edge(h[4], h[6]); g.create_edge(h[4], h[7]);
  g.create_edge(h[5], h[4]); g.create_edge(h[5], h[6]);
  g.create_edge(h[6], h[8]); g.create_edge(h[7], h[8]);
  g.create_edge(h[7], h[9]); g.create_edge(h[7], h[10]);
  g.create_edge(g.flip(h[7]), g.flip(h[10])); 
  g.create_edge(h[9], g.flip(h[10])); g.create_edge(h[11], h[12]);
  g.create_edge(h[12], g.flip(h[1]));
  return g;
}

NODE_UINT bgid(const handle_t& h, const bdsg::HashGraph& hg) {
  auto nid = hg.get_id(h);  
  return hg.get_is_reverse(h) ? (nid-1)*2+1 : (nid-1)*2;
}

NODE_UINT bgid(size_t net_rank, bool is_reverse, bool is_source) {
  // The diagram is:
  //   1<-3   (reverse traversal: start_rev (source) <- end_rev (not source))
  //   0->2   (forward traversal: start_fwd (not source) -> end_fwd (source))
  return net_rank * 4 + ((is_source ^ is_reverse) ? 2 : 0) + (is_reverse ? 1 : 0);
}

NODE_UINT rev_bgid(NODE_UINT n) {
  return n ^ 1;
}

CHOverlay make_boost_graph(const bdsg::HashGraph& hg) {
  NODE_UINT node_count = hg.get_node_count();
  CHOverlay g(node_count*2);
  hg.for_each_edge([&](const edge_t& edge_h) {
    auto& [h1, h2] = edge_h;
    auto i1 = bgid(h1, hg), i2 = bgid(h2, hg);
    add_edge(i1, i2, g);  
    if (i1 != rev_bgid(i2)) {
      add_edge(rev_bgid(i2), rev_bgid(i1), g); 
    }
  });
  
  hg.for_each_handle([&](const handle_t& h) {
    auto nid = bgid(h, hg);
    // Initialize all the seqlen fields
    g[nid].seqlen = hg.get_length(h);
    g[rev_bgid(nid)].seqlen = g[nid].seqlen;
  });

 
  return g;
}

CHOverlay make_boost_graph(const SnarlDistanceIndex::TemporaryDistanceIndex& temp_index, const SnarlDistanceIndex::temp_record_ref_t& snarl_index, const SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record, const vector<SnarlDistanceIndex::temp_record_ref_t>& all_children, const HandleGraph* hgraph) {
  
  // Every net graph element (start node at rank 0, end node at rank 1, each
  // child nose/chain) needs to have 4 Boost graph nodes. We need separate
  // representations for forward and reverse orientations, and within each
  // orientation we need an in "port" and an out "port" so that we can draw the
  // subgraphs describing internal reversals inside chains. We use the in ports
  // to represent each element as a destination, and the out ports to represent
  // each element as a source.
  //
  // We wouldn't really need all 4 vertices to represent node children, or the
  // start and end, but we need O(1) mapping from net graph rank.
  //
  // TODO: Can we reassign the net graph ranks so all the snarls are first and
  // store a number of snarl children to let us throw out a bunch of
  // never-queried labels?
  
  size_t total_vertices = all_children.size() * 4;

#ifdef debug_boost_graph
  cerr << "=== make_boost_graph: Building net graph ===" << endl;
  cerr << "Number of children: " << all_children.size() << endl;
  cerr << "Allocating " << total_vertices << " Boost vertices" << endl;
#endif

  CHOverlay ov(total_vertices);
  
  // Maps inward-facing handle to Boost graph vertex ID.
  // Doesn't include outward-facing handles.
  unordered_map<handle_t, CHOverlay::vertex_descriptor> handle_bgnid_map;

#ifdef debug_boost_graph
  cerr << "--- Phase 1: Creating Boost vertices for each net graph child ---" << endl;
#endif

  for (auto& child : all_children) {
    // Ignore index in all_children and use whether the ID matches the
    // start/end or else use the stored rank to determine the child number.

    // Depending on the child type we need to load these different ways and some might be INF_INT.
    // The start and end handles point forward, not inward.
    size_t child_net_rank;
    handle_t start_handle;
    handle_t end_handle;
    DIST_UINT start_end_distance;
    DIST_UINT start_start_distance;
    DIST_UINT end_end_distance;
    
    if (child.first == bdsg::SnarlDistanceIndex::TEMP_CHAIN) {
      // This is a child chain
      auto& record = temp_index.get_chain(child);

      // A child chain can never be the start or end boundary
      child_net_rank = record.rank_in_parent;

      start_handle = hgraph->get_handle(record.start_node_id, record.start_node_rev);
      end_handle = hgraph->get_handle(record.end_node_id, record.end_node_rev);

      // Fetch straight-through distance.
      // Will be std::numeric_limits<size_t>::max() if unconnected.
      start_end_distance = demote_distance(record.min_length);

      // Fetch looping distances (thanks Xian!)
      // If no loop is actually possible, the loop value will be std::numeric_limits<size_t>::max()
      if (!record.forward_loops.empty() && record.forward_loops[0] != std::numeric_limits<size_t>::max()) {
        // We know a chain always has a first child that's a node, so we can
        // get the start node length.
        auto& first_child = record.children.front();
        assert(first_child.first == bdsg::SnarlDistanceIndex::TEMP_NODE);
        DIST_UINT start_node_length = demote_distance(temp_index.get_node(first_child).node_length);
        // We know nothing can be infinite-distance here.
        // TODO: Check for overflow?
        start_start_distance = demote_distance(record.forward_loops[0]) + 2 * start_node_length;
      } else {
        start_start_distance = INF_INT;
      }
      if (!record.backward_loops.empty() && record.backward_loops.back() != std::numeric_limits<size_t>::max()) {
        // The end node length is already helpfully stored for us.
        // // We know nothing can be infinite-distance here.
        // TODO: Check for overflow?
        end_end_distance = record.backward_loops.back() + 2 * demote_distance(record.end_node_length);
      } else {
        end_end_distance = INF_INT;
      }

      if (record.reversed_in_parent) {
        // Fix up everything so we're thinking of the orientation of the chain
        // in its parent, rather than its local forward orientation.
        auto temp = start_handle;
        start_handle = hgraph->flip(end_handle);
        end_handle = hgraph->flip(temp);
        std::swap(start_start_distance, end_end_distance);
      }

#ifdef debug_boost_graph
      cerr << "Child " << child_net_rank << " is CHAIN: start_node=" << record.start_node_id
           << " (rev=" << record.start_node_rev << "), end_node=" << record.end_node_id
           << " (rev=" << record.end_node_rev << "), min_length=" << record.min_length << endl;
      cerr << "  start_handle: id=" << hgraph->get_id(start_handle) << " rev=" << hgraph->get_is_reverse(start_handle) << endl;
      cerr << "  end_handle: id=" << hgraph->get_id(end_handle) << " rev=" << hgraph->get_is_reverse(end_handle) << endl;
      cerr << " (reversed_in_parent=" << record.reversed_in_parent << ")" << endl;
      cerr << "  Boost vertices: " << bgid(child_net_rank, false, false) << " (start_fwd), "
           << bgid(child_net_rank, true, true) << " (start_rev), "
           << bgid(child_net_rank, false, true) << " (end_fwd), "
           << bgid(child_net_rank, true, false) << " (end_rev)" << endl;
#endif

    } else if (child.first == bdsg::SnarlDistanceIndex::TEMP_NODE) {
      // This is a child node
      auto& record = temp_index.get_node(child);

      // The rank may need to be 0 or 1 if we are a start or end bound.
      if (record.node_id == temp_snarl_record.start_node_id) {
        // TODO: Don't we need to handle having the same node as a start and an end bound???
        child_net_rank = 0;
        // Handles need to point along snarl
        start_handle = hgraph->get_handle(temp_snarl_record.start_node_id, temp_snarl_record.start_node_rev);
      } else if (record.node_id == temp_snarl_record.end_node_id) {
        child_net_rank = 1;
        // Handles need to point along snarl
        start_handle = hgraph->get_handle(temp_snarl_record.end_node_id, temp_snarl_record.end_node_rev);
      } else {
        child_net_rank = record.rank_in_parent;
        // Handle needs to represent the thing in the orientation we have it in in the snarl.
        start_handle = hgraph->get_handle(record.node_id, record.reversed_in_parent);
      }
      
      // Node is potentially reachable in both directions (though we only want to index one of these for bounds)
      end_handle = start_handle;

      start_end_distance = record.node_length;
      start_start_distance = INF_INT;
      end_end_distance = INF_INT;

#ifdef debug_boost_graph
      cerr << "Child " << child_net_rank << " is NODE: node_id=" << record.node_id
           << " (reversed_in_parent=" << record.reversed_in_parent << "), length=" << record.node_length << endl;
      cerr << "  id=" << hgraph->get_id(start_handle) << " rev=" << hgraph->get_is_reverse(start_handle) << endl;
#endif
    } else {
      throw std::runtime_error("unexpected rec_type: " + std::to_string(child.first));
    }

    // Initialize all the seqlen fields of the vertices to 0; we only use edge
    // weights in this mode, but we're still responsible for them.
    // TODO: Is it worth doing this just as a separate scan in order instead?
    for (bool is_reverse : {false, true}) {
      for (bool is_source : {false, true}) {
        ov[bgid(child_net_rank, is_reverse, is_source)].seqlen = 0;  
      }
    }
    
    // Map inward orientations of start and end handles
    if (child_net_rank != 0) {
      // We can arrive at the start of everything but our own start.
      handle_bgnid_map[start_handle] = bgid(child_net_rank, false, false);
    }
    if (child_net_rank != 1) {
      // We can arrive at the end of everything but our own end.
      handle_bgnid_map[hgraph->flip(end_handle)] = bgid(child_net_rank, true, false);
    }

#ifdef debug_boost_graph
    cerr << "  Mapping start_handle (inward) -> Boost " << handle_bgnid_map[start_handle] << endl;
    cerr << "  Mapping flip(end_handle) (inward) -> Boost " << handle_bgnid_map[hgraph->flip(end_handle)] << endl;
#endif

    if (start_end_distance != INF_INT) {
      // Add edges representing distance across chain
      auto new_edge = add_edge(bgid(child_net_rank, false, false), bgid(child_net_rank, false, true), ov);
      ov[new_edge.first].weight = start_end_distance;
#ifdef debug_boost_graph
      cerr << "  Edge " << bgid(child_net_rank, false, false) << " -> " << bgid(child_net_rank, false, true) << " (fwd traversal, weight=" << start_end_distance << ")" << endl;
#endif

      new_edge = add_edge(bgid(child_net_rank, true, false), bgid(child_net_rank, true, true), ov);
      ov[new_edge.first].weight = start_end_distance;
#ifdef debug_boost_graph
      cerr << "  Edge " << bgid(child_net_rank, true, false) << " -> " << bgid(child_net_rank, true, true) << " (rev traversal, weight=" << start_end_distance << ")" << endl;
#endif
    }

#ifdef debug_boost_graph
    cerr << "  Loop distances: start_start=" << start_start_distance << ", end_end=" << end_end_distance << endl;
#endif

    if (end_end_distance != INF_INT) {
      // Loops are edges between different orientations of the same endpoint
      auto new_loop_edge = add_edge(bgid(child_net_rank, true, false), bgid(child_net_rank, false, true), ov);
      ov[new_loop_edge.first].weight = end_end_distance;
#ifdef debug_boost_graph
      cerr << "  Edge " << bgid(child_net_rank, true, false) << " -> " << bgid(child_net_rank, false, true) << " (end loop, weight=" << end_end_distance << ")" << endl;
#endif
    }

    if (start_start_distance != INF_INT) {
      auto new_loop_edge = add_edge(bgid(child_net_rank, false, false), bgid(child_net_rank, true, true), ov);
      ov[new_loop_edge.first].weight = start_start_distance;
#ifdef debug_boost_graph
      cerr << "  Edge " << bgid(child_net_rank, false, false) << " -> " << bgid(child_net_rank, true, true) << " (start loop, weight=" << start_start_distance << ")" << endl;
#endif
    }
  }

#ifdef debug_boost_graph
  cerr << "--- Phase 2: Adding edges between children based on handle graph edges ---" << endl;
#endif

  for (auto [handle_in, vertex_id_in] : handle_bgnid_map) {
    // The map contains inward-facing orientations of every handle.
    // So get the outward-facing versioin.
    handle_t handle = hgraph->flip(handle_in);
    NODE_UINT vertex_id = rev_bgid(vertex_id_in);

#ifdef debug_boost_graph
    cerr << "  handle(id=" << hgraph->get_id(handle) << ", rev=" << hgraph->get_is_reverse(handle) << ") -> Boost " << vertex_id << endl;
#endif

    // We need to get all the edges off the right side of this outward-facing
    // handle and create them if they don't already exist.
    hgraph->follow_edges(handle, false, [&] (const handle_t& next) {
        auto found = handle_bgnid_map.find(next);
        if (found == handle_bgnid_map.end()) {
            // We're looking outside our net graph, or have reached something
            // not inward-facing (like across the inside of a chain).
            // Don't add the edge.
            return;
        }
        NODE_UINT next_id = found->second;

#ifdef debug_boost_graph
        cerr << "    Connects to handle(id=" << hgraph->get_id(next) << ", rev=" << hgraph->get_is_reverse(next) << ") -> Boost " << next_id << endl;
#endif

        auto edge_info = edge(vertex_id, next_id, ov);
        if (!edge_info.second) {
#ifdef debug_boost_graph
          cerr << "      Adding edge " << vertex_id << " -> " << next_id << endl;
          cerr << "      Adding reverse edge " << rev_bgid(next_id) << " -> " << rev_bgid(vertex_id) << endl;
#endif
          add_edge(vertex_id, next_id, ov);
          add_edge(rev_bgid(next_id), rev_bgid(vertex_id), ov);
        } else {
#ifdef debug_boost_graph
          cerr << "      Edge already exists" << endl;
#endif
        }
    });
  }


#ifdef debug_boost_graph
  cerr << "=== make_boost_graph complete ===" << endl;
#endif
  return ov;
}


int edge_diff(ContractedGraph::vertex_descriptor nid, ContractedGraph& ch, CHOverlay& ov, vector<DIST_UINT>& node_dists, int hop_limit = 2) {
  //using namespace boost;
  auto [out_start, out_end] = out_edges(nid, ch);
  auto [in_start, in_end] = in_edges(nid, ch);

  ov[nid].arc_cover = 0; 
  int eadd = 0; 
  //thanks https://theboostcpplibraries.com/boost.graph-vertices-and-edges for iteration code
  std::for_each(in_start, in_end, [&](ContractedGraph::edge_descriptor eid) {
    auto in_node = source(eid, ch);
    DIST_UINT in_w = ch[eid].weight;
    DIST_UINT stop_dist = in_w + ov[nid].seqlen + ov[nid].max_out; 
    
    std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q;
    auto [_, __] = out_edges(in_node, ch);
    std::for_each(_, __, [&](auto edge) { q.emplace(ch[edge].weight, target(edge, ch)); });
    int num_iter = 0; 
    vector<int> to_reset;
    //five hops limit idea from https://turing.iem.thm.de/routeplanning/hwy/contract.pdf
    while ((!q.empty()) && (num_iter < hop_limit)) {
      auto [cur_dist, cur_node] = q.top();
      to_reset.push_back(cur_node);
      if (cur_dist > stop_dist) { break; }
      q.pop();
      
      std::tie(_, __) = out_edges(cur_node, ch);
      std::for_each(_, __, [&](auto edge) { 
        DIST_UINT new_dist = ch[edge].weight + cur_dist + ov[cur_node].seqlen;
        auto t = target(edge,ch);
        if (new_dist < node_dists[t]) {
          q.emplace(new_dist, t); 
          node_dists[t] = new_dist;
        }
      }); 
      num_iter += 1;
    }
    
    std::for_each(out_start, out_end, [&](ContractedGraph::edge_descriptor eid2) { 
      if (in_w+ch[nid].seqlen+ch[eid2].weight <= node_dists[target(eid2, ch)]) {
        eadd += 1;
        ov[nid].arc_cover += (ov[eid].arc_cover + ov[eid2].arc_cover);
      }
      
    });

    for (auto n: to_reset) { node_dists[n] = INF_INT; }

    while (!q.empty()) { node_dists[get<1>(q.top())] = INF_INT; q.pop(); }
  });  

  int edel = out_degree(nid, ch) + in_degree(nid, ch);
  int ediff = eadd - edel;
  
  return ediff;  
}

void contract(CHOverlay::vertex_descriptor nid, ContractedGraph& ch, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<bool>& shouldnt_contract, int hop_limit = 2) {
  auto [out_start, out_end] = out_edges(nid, ch);
  auto [in_start, in_end] = in_edges(nid, ch);
  
  //thanks https://theboostcpplibraries.com/boost.graph-vertices-and-edges for iteration code
  std::for_each(in_start, in_end, [&](ContractedGraph::edge_descriptor eid) {
    auto in_node = source(eid, ch);
    if (in_node == nid) { return; }
    DIST_UINT in_w = ch[eid].weight;
    DIST_UINT stop_dist = in_w + ov[nid].seqlen + ov[nid].max_out;
    
    
    std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q;
    auto [_, __] = out_edges(in_node, ch);
    std::for_each(_, __, [&](auto edge) { 
      q.emplace(ch[edge].weight, target(edge, ch));  
    });

    int num_iter = 0; 
    vector<int> to_reset;
    while ((!q.empty()) && (num_iter < hop_limit)) {
      auto [cur_dist, cur_node] = q.top();
      to_reset.push_back(cur_node);
      if (cur_dist > stop_dist) { break; }
      q.pop();
      
      std::tie(_, __) = out_edges(cur_node, ch);
      std::for_each(_, __, [&](auto edge) { 
        DIST_UINT new_dist = ch[edge].weight + cur_dist + ov[cur_node].seqlen;
        auto t = target(edge,ch);
        if (new_dist < node_dists[t]) {
          q.emplace(new_dist, t); 
          node_dists[t] = new_dist;
        }
      }); 
      
      num_iter += 1;
    }
    
    std::for_each(out_start, out_end, [&](ContractedGraph::edge_descriptor out_e) {
      DIST_UINT new_len = in_w + ch[nid].seqlen + ch[out_e].weight;
      auto out_node = target(out_e, ch);
      if (new_len <= node_dists[out_node] && (out_node != nid)) { 
        auto edge_info = edge(in_node, out_node, ov);
        if (!edge_info.second){
          auto new_edge = add_edge(in_node, out_node, ov).first;
          ov[new_edge].weight = new_len; 
          ov[new_edge].arc_cover = ov[out_e].arc_cover + ov[eid].arc_cover;
          ov[new_edge].ori = false; 
          ov[in_node].max_out = max(ov[in_node].max_out, new_len);
        } else {
          if (new_len < ov[edge_info.first].weight) { 
            ov[edge_info.first].weight = new_len;
            ov[edge_info.first].arc_cover = ov[out_e].arc_cover + ov[eid].arc_cover;
            ov[in_node].max_out = max(ov[in_node].max_out, new_len);  
       
          }
          ov[edge_info.first].contracted = false; 
        } 
      }
    });
    
    for (auto n: to_reset) { node_dists[n] = INF_INT; }

    while (!q.empty()) { node_dists[get<1>(q.top())] = INF_INT; q.pop(); }   
  }); 
                            
  //update contracted neighbor counts
  std::for_each(in_start, in_end, [&](ContractedGraph::edge_descriptor eid) {
    auto in_node = source(eid, ch);
    ov[in_node].contracted_neighbors += 1;
    ov[in_node].level = max(ov[in_node].level, ov[nid].level); 
    ch[eid].contracted = true;
    shouldnt_contract[in_node] = true;  
  });

  //std::tie(out_start, out_end) = out_edges(nid, ch);
  std::for_each(out_start, out_end, [&](ContractedGraph::edge_descriptor eid) {
    auto out_node = target(eid, ch);
    ov[out_node].contracted_neighbors += 1;  
    ov[out_node].level = max(ov[out_node].level, ov[nid].level); 
    ch[eid].contracted = true;  
    shouldnt_contract[out_node] = true;  
  }); 

  ov[nid].contracted = true;
} 

int get_hop_limit(CHOverlay& ov) {
  //staggered hop limit idea from https://www.microsoft.com/en-us/research/wp-content/uploads/2011/05/hl-sea.pdf
  int l = 1;
  double deg = (double)num_edges(ov)/num_vertices(ov); 
  if (deg >= 1) {
    l = 1;
  } 
  
  if (deg >= 2) {
    l = 1;
  } 

  if (deg >= 3) {
    l = 2;
  }

  if (deg >= 5) {
    l = 3;
  }

  if (deg >= 7) {
    l = 5;
  } 
  return l; 
}

void make_contraction_hierarchy(CHOverlay& ov) {
#ifdef debug_create
  cerr << "starting degree: " << (double)num_edges(ov)/num_vertices(ov) << endl;
#endif
  
  //thanks https://stackoverflow.com/questions/53490593/boostget-with-boostfiltered-graph-on-adjacency-list-with-netsed-properties for filtered_graph code
  auto contracted_filter = [&](CHOverlay::edge_descriptor eid) { return !(ov[eid].contracted); };  
  
  ContractedGraph contracted_g(ov, contracted_filter);
  vector<bool> skip(num_vertices(ov), false); 
  int num_con = 0; 
  
  vector<DIST_UINT> node_dists(num_vertices(ov), INF_INT);  

  vector<int> nodes; nodes.resize(num_vertices(ov));
  iota(nodes.begin(), nodes.end(), 0); 

  for (int rnd = 0; rnd < 1; rnd++) {  
    std::fill(skip.begin(), skip.end(), false);  
    
    //for (NODE_UINT i = 0; i < num_vertices(ov); i+=1) {
    for (auto i: nodes) {
      if (ov[i].contracted || skip[i]) { continue; }  
      int edif = edge_diff(i, contracted_g, ov, node_dists, 250); 
      
      int min_pri = INF_INT;

      auto [out_start, out_end] = out_edges(i, contracted_g);
      auto [in_start, in_end] = in_edges(i, contracted_g);
      std::for_each(out_start, out_end, [&] (auto out_edge) {   
        auto neigh = target(out_edge, ov);
        if (skip[neigh]) {return;} 
        int neigh_edif = edge_diff(neigh, contracted_g, ov, node_dists, 250); 
       
        int neigh_pri = (2*neigh_edif) + (1*ov[neigh].contracted_neighbors) + (5*(ov[neigh].level+1)) + ov[neigh].arc_cover;
        
        if (neigh_pri < min_pri) { min_pri = neigh_pri; } 
      
      });

      std::for_each(in_start, in_end, [&] (auto in_edge) {  
        auto neigh = source(in_edge, ov);
        if (skip[neigh]) {return;} 
        int neigh_edif = edge_diff(neigh, contracted_g, ov, node_dists, 250);  
       
        int neigh_pri = (2*neigh_edif) + (1*ov[neigh].contracted_neighbors) + (5*(ov[neigh].level+1)) + ov[neigh].arc_cover;
 
        if (neigh_pri < min_pri) { min_pri = neigh_pri; }
      
      }); 
 
      int priority = (2*edif) + (1*ov[i].contracted_neighbors) + (5*(ov[i].level+1)) + ov[i].arc_cover;
      
      if ((priority <= min_pri)) {
        
        std::for_each(out_start, out_end, [&] (auto out_edge) {   
          auto neigh = target(out_edge, ov);
          skip[neigh] = true;
        });

        std::for_each(in_start, in_end, [&] (auto in_edge) {   
          auto neigh = source(in_edge, ov);
          skip[neigh] = true;
        });
        //}
       
      } else {
        skip[i] = true;
      }
    } 

    for (auto i = 0u; i < num_vertices(ov); i+=1) {
     
      if (ov[i].contracted) { continue; }
      if ((!skip[i])) {
        ov[i].new_id = num_vertices(ov)-1-num_con;
        contract(i, contracted_g, ov, node_dists, skip, 250);
        skip[i] = true;
        num_con += 1;
      }
    }  

#ifdef debug_create
    cerr << "num contr: " << num_con << endl;
    cerr << "after round " << rnd+1 << " degree: " << (double)num_edges(ov)/num_vertices(ov) << endl;
#endif
  }
  

#ifdef debug_create
  cerr << "left over: " << num_vertices(ov) - num_con << endl; 
#endif
  //std::fill(skip.begin(), skip.end(), false);  
  //for (auto n: arti_pts) { skip[n] = true; }

  // We maintain a priority queue that lest us find the smallest-priority item.
  //
  // We keep all but the last item heap-ified, and the smallest-priority item
  // last, as our invariant.
  vector<tuple<int, CHOverlay::vertex_descriptor>> queue_objs; queue_objs.reserve(num_vertices(ov)/2); 
  for (int i = 0; i < num_vertices(ov); i+=1) {
    if (ov[i].contracted) { continue; }

    int edif = edge_diff(i, contracted_g, ov, node_dists, 1000);
    
    //priority formula based off that given in https://www.microsoft.com/en-us/research/wp-content/uploads/2011/05/hl-sea.pdf
    int priority = (2*edif) + (1*ov[i].contracted_neighbors) + (5*(ov[i].level+1)) + ov[i].arc_cover;
    queue_objs.emplace_back(priority, i);
  } 
  make_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>());
  pop_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>()); 
 
 
  while (queue_objs.size() > 2) { 
    auto [pri, node] = queue_objs.back(); queue_objs.pop_back();
    //preparing for next pop
    pop_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>());
    
    int hop_limit = 1000;//get_hop_limit(ov);
    int edif = edge_diff(node, contracted_g, ov, node_dists, hop_limit);
   
    int new_pri = ((2*edif)+ (1*ov[node].contracted_neighbors)) + (5*(ov[node].level+1)) + ov[node].arc_cover;
    
    if (new_pri > get<0>(queue_objs.back())) {
      // After recomputing priority, the priority is actually greater than the nex-tlowest-priority entry.
      // Put this back so we can get that one instead.
      // First we need to put what's the current last item back in its proper place.
      push_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>());
      // Then we put this item back and heapify everything
      queue_objs.emplace_back(new_pri, node); 
      push_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>());
      // Then we find the new smallest-priority item.
      pop_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>()); 
      continue;
    } 
    ov[node].level += 1;
 
#ifdef debug_create
    //if (queue_objs.size() % 100 == 1) {
    cerr << "remaining: " << queue_objs.size() << ", deg: " << (double)num_edges(ov)/num_vertices(ov) << endl;
    cerr << "lv: " << ov[node].level << endl;
    //}
#endif
    
   
    ov[node].new_id = num_vertices(ov)-1-num_con;
    contract(node, contracted_g, ov, node_dists, skip, hop_limit); num_con += 1;
  }
  
  while (!queue_objs.empty()) {  
    auto [pri, node] = queue_objs.back(); queue_objs.pop_back();
    //preparing for next pop
    pop_heap(queue_objs.begin(), queue_objs.end(), greater<tuple<int, CHOverlay::vertex_descriptor>>());
    
    
    //cerr << "in out: " << in_degree(node, ov) << " " << out_degree(node, ov) << endl;
    ov[node].new_id = num_vertices(ov)-1-num_con;
    //contract(node, contracted_g, ov, node_dists, skip, 50); 
    num_con += 1;
  }

  auto ori_filter = [&](CHOverlay::edge_descriptor eid) { return !(ov[eid].ori); }; 
  remove_edge_if(ori_filter, ov);
  
  vector<tuple<int,int>> v2; v2.reserve(num_vertices(ov));
  for (auto i = 0u; i < num_vertices(ov); i+=1) {
    v2.emplace_back(in_degree(i,ov)*out_degree(i,ov), i);
  }
  sort(v2.rbegin(), v2.rend());
  /* 
  for (int i: {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
    auto [p, n] = v2[i];
    cerr << n << " (" << ov[n].new_id << "): " << in_degree(n,ov) << " " << out_degree(n,ov) << endl;
  } */
}

DIST_UINT binary_intersection_ch(vector<HubRecord>& v1, vector<HubRecord>& v2) {
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
/*                           
template <typename ItrType>
ItrType get_dist_itr(ItrType start_itr, ItrType hub_itr) {
  auto node_count = *start_itr;
  auto last_fwd_end_bound_itr = next(start_itr, 1+node_count);
  if (hub_itr >= next(start_itr, *last_fwd_end_bound_itr)) {
    //backwards label
    auto first_back_bound_itr = next(start_itr, 1+node_count+1); 
    auto last_back_bound_itr = next(start_itr, 1+node_count+1+node_count);
    auto jump_to_dist = (*last_back_bound_itr) - *first_back_bound_itr;
    return next(hub_itr, jump_to_dist);
  } else {
    //forwards label
    auto first_fwd_bound_itr = next(start_itr, 1); 
    auto last_fwd_bound_itr = next(start_itr, 1+node_count);
    auto jump_to_dist = (*last_fwd_bound_itr) - *first_fwd_bound_itr;
    return next(hub_itr, jump_to_dist); 
  }
}
*/





void down_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_back) {
  auto in_node = node;

  // TODO: We used to add -ov[node].seqlen to labels_back[node] for the hub
  // ov[node].new_id. But this involved doing unsigned overflow shenanigans,
  // and gave us values in the labels that are maximally wide and can't later
  // be packed into the reduced bit width in a SnarlDistanceIndex.
  //
  // The tests didn't seem to cover a case where these entries were needed, so
  // we just don't do that anymore.
  std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q; 
 
  auto [_, __] = out_edges(in_node, ov);
  std::for_each(_, __, [&](auto edge) { 
    auto t = target(edge, ov);  
    
    if (!ov[edge].ori) { return; } 
    q.emplace(ov[edge].weight, t); 
    node_dists[t] = ov[edge].weight;
  });

  vector<int> to_reset;
 
  while ((!q.empty())) {
    auto [cur_dist, cur_node] = q.top(); q.pop();
    to_reset.push_back(cur_node); 

    if (node_dists[cur_node] < cur_dist) { continue; } 
    DIST_UINT check_dist = binary_intersection_ch(labels_back[cur_node], labels[node]);
    
    if (check_dist <= cur_dist) {continue;}
    labels_back[cur_node].emplace_back(ov[node].new_id, cur_dist);

    
    std::tie(_, __) = out_edges(cur_node, ov);
    std::for_each(_, __, [&](auto edge) { 
      auto t = target(edge, ov);  
      
      if (!ov[edge].ori) { return; } 
      DIST_UINT new_dist = ov[edge].weight + cur_dist + ov[cur_node].seqlen;   
      if (new_dist < node_dists[t]) {
        q.emplace(new_dist, t); 
        node_dists[t] = new_dist;
      }
    }); 
  } 

  node_dists[node] = INF_INT;
  for (auto n: to_reset) { node_dists[n] = INF_INT; }

  while (!q.empty()) { node_dists[get<1>(q.top())] = INF_INT; q.pop(); } 
}

void down_dijk_back(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_back) {
  auto in_node = node;
  labels[node].emplace_back(ov[node].new_id, 0); 
  node_dists[node] = 0;

  //std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q;
  vector<tuple<DIST_UINT, CHOverlay::vertex_descriptor>> q; if (ov[node].new_id < 100) { q.reserve(num_vertices(ov)/2); } 
  
  auto [_, __] = in_edges(in_node, ov);
  std::for_each(_, __, [&](auto edge) {
    auto s = source(edge, ov);
    
    if (!ov[edge].ori) { return; } 
    q.emplace_back(ov[edge].weight, s); 
    node_dists[s] = ov[edge].weight;
  });
  make_heap(q.begin(), q.end(), greater<tuple<DIST_UINT, CHOverlay::vertex_descriptor>>());
  
  vector<int> to_reset; if (ov[node].new_id < 100) { to_reset.reserve(num_vertices(ov)/2); } 
  while ((!q.empty())) {
    pop_heap(q.begin(), q.end(), greater<tuple<DIST_UINT, CHOverlay::vertex_descriptor>>());       
    auto [cur_dist, cur_node] = q.back(); q.pop_back();  
                                                                     
    to_reset.push_back(cur_node);  
    if (node_dists[cur_node] < cur_dist) {
      continue; 
    } 

    DIST_UINT check_dist = binary_intersection_ch(labels[cur_node], labels_back[node]);
    if (check_dist <= cur_dist) {continue;} 
    labels[cur_node].emplace_back(ov[node].new_id, cur_dist+ov[node].seqlen);
    
    std::tie(_, __) = in_edges(cur_node, ov);
    std::for_each(_, __, [&](auto edge) { 
      auto t = source(edge, ov);  
      
      if (!ov[edge].ori) { return; } 
      DIST_UINT new_dist = ov[edge].weight + cur_dist + ov[cur_node].seqlen;   
      if (new_dist < node_dists[t]) {
        q.emplace_back(new_dist, t); 
        push_heap(q.begin(), q.end(), greater<tuple<DIST_UINT, CHOverlay::vertex_descriptor>>()); 
        node_dists[t] = new_dist;
      }
    }); 
                                                                 
  }  
  
  node_dists[node] = INF_INT;
  for (auto n: to_reset) { node_dists[n] = INF_INT; }

  for (auto& t: q) { node_dists[get<1>(t)] = INF_INT; } 
  q.clear(); 
} 

void test_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_back) {
  auto in_node = node; //node_dists[node] = 0;
 
  std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q;
  auto [_, __] = out_edges(in_node, ov);
  std::for_each(_, __, [&](auto edge) { 
    auto t = target(edge, ov);  
    
    if (!ov[edge].ori) { return; }
    q.emplace(ov[edge].weight, t); 
    node_dists[t] = ov[edge].weight;
  });

  vector<int> to_reset;
 
  while ((!q.empty())) {
    auto [cur_dist, cur_node] = q.top(); q.pop();
    to_reset.push_back(cur_node);

    if (cur_dist > node_dists[cur_node]) { continue; }
  
    std::tie(_, __) = out_edges(cur_node, ov);
    std::for_each(_, __, [&](auto edge) { 
      auto t = target(edge, ov);  
     
      if (!ov[edge].ori) { return; }
      DIST_UINT new_dist = ov[edge].weight + cur_dist + ov[cur_node].seqlen;   
      if (new_dist < node_dists[t]) {
        q.emplace(new_dist, t); 
        node_dists[t] = new_dist;
      }
    }); 
  }

  for (int cur_node = 0; cur_node < static_cast<int>(num_vertices(ov)); cur_node++) {
 
    DIST_UINT check_dist = binary_intersection_ch(labels_back[cur_node], labels[node]);
    if (cur_node == node) {
      check_dist = min(check_dist, labels_back[cur_node].back().dist + labels[node].back().dist);
    }
    
    if (check_dist != node_dists[cur_node]) {
      cerr << "node " << cur_node << " mismatch: " << check_dist << ", actual: " << node_dists[cur_node] << endl; 
    }  
  }
  
  node_dists[node] = INF_INT;
  for (auto n: to_reset) { node_dists[n] = INF_INT; }

  while (!q.empty()) { node_dists[get<1>(q.top())] = INF_INT; q.pop(); } 
} 

void test_dijk_back(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_back) {
  auto in_node = node;
 
  std::priority_queue<tuple<DIST_UINT, int>, vector<tuple<DIST_UINT, int>>, greater<tuple<DIST_UINT, int>>> q;
  auto [_, __] = in_edges(in_node, ov);
  std::for_each(_, __, [&](auto edge) { 
    auto s = source(edge, ov);  
    
    if (!ov[edge].ori) { return; }
    q.emplace(ov[edge].weight, s); 
    node_dists[s] = ov[edge].weight;
  });

  vector<int> to_reset;
 
  while ((!q.empty())) {
    auto [cur_dist, cur_node] = q.top(); q.pop();
    to_reset.push_back(cur_node);

    if (cur_dist > node_dists[cur_node]) { continue; }
  
    std::tie(_, __) = in_edges(cur_node, ov);
    std::for_each(_, __, [&](auto edge) { 
      auto s = source(edge, ov);  
      //if (ov[t].new_id <= ov[node].new_id) { return; }
      if (!ov[edge].ori) { return; }
      DIST_UINT new_dist = ov[edge].weight + cur_dist + ov[cur_node].seqlen;   
      if (new_dist < node_dists[s]) {
        q.emplace(new_dist, s); 
        node_dists[s] = new_dist;
      }
    }); 
  }
  for (auto cur_node = 0u; cur_node < num_vertices(ov); cur_node++) {
    DIST_UINT check_dist = binary_intersection_ch(labels[cur_node], labels_back[node]);
    if (cur_node == node) {
      check_dist = min(check_dist, labels[cur_node].back().dist + labels_back[node].back().dist);
    } 
    
    if (check_dist != node_dists[cur_node]) {
      cerr << "node " << cur_node << " mismatch: " << check_dist << ", actual: " << node_dists[cur_node] << endl;
    } 
    
  } 
  
  node_dists[node] = INF_INT;
  for (auto n: to_reset) { node_dists[n] = INF_INT; }

  while (!q.empty()) { node_dists[get<1>(q.top())] = INF_INT; q.pop(); } 
} 

void create_labels(vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_back, CHOverlay& ov) { 
#ifdef debug_create
  cerr << "start create labels" << endl;
#endif
  vector<DIST_UINT> node_dists(num_vertices(ov), INF_INT);
  vector<int> v; v.resize(num_vertices(ov));
  for (auto i = 0u; i < num_vertices(ov); i++) {
    v[ov[i].new_id] = i;
  } 

  for (auto j = 0u; j < num_vertices(ov); j++) { 
    
#ifdef debug_create
    //if (j % 100 == 1) { 
    cerr << j << "th node, " << v[j] << endl;
#endif

    //cerr << "starting dijkstra: " << endl;
    down_dijk_back(v[j], ov, node_dists, labels, labels_back);
  
    //cerr << "starting rev dijkstra: " << endl;
    down_dijk(v[j], ov, node_dists, labels, labels_back);
  } 
}

vector<size_t> pack_labels(const vector<vector<HubRecord>>& labels, const vector<vector<HubRecord>>& labels_back) {
  auto label_count = labels.size();
  //label_count+1 is so we can look at the next offset to determine end of range
  size_t flat_vec_size = 2*(label_count+1)+1;
  vector<size_t> flat_label_vec;
  flat_label_vec.resize(flat_vec_size);
  flat_label_vec[0] = label_count;
 
  //fill in fwd offsets
  size_t current_offset = flat_vec_size;  
  for (size_t i = 0; i < labels.size(); i++) {
    auto& label = labels[i];
    size_t label_size = label.size(); 
    flat_label_vec[i+1] = current_offset; 
    current_offset += label_size;

    flat_vec_size += (label_size*2); 
  }
  flat_label_vec[label_count+1] = current_offset;

  //fill in back offsets
  current_offset = flat_vec_size;  
  for (size_t i = 0; i < labels_back.size(); i++) {
    auto& label = labels_back[i];
    size_t label_size = label.size(); 
    flat_label_vec[i+1+label_count+1] = current_offset; 
    current_offset += label_size;

    flat_vec_size += (label_size*2); 
  }
  flat_label_vec[(label_count+1)+label_count+1] = current_offset;      

  flat_label_vec.resize(flat_vec_size);   

  //copy info into vec (fwd)
  size_t jump_to_dist = flat_label_vec[label_count+1] - flat_label_vec[1];
  for (size_t i = 0; i < labels.size(); i++) {
    auto& label = labels[i];
    size_t hub_offset = flat_label_vec[i+1];
    for (size_t j = 0; j < label.size(); j++) {
      flat_label_vec[hub_offset+j] = label[j].hub;
      flat_label_vec[hub_offset+j+jump_to_dist] = label[j].dist;
    }
  }
 
  //copy info into vec (back)
  jump_to_dist = flat_label_vec[(label_count+1)+label_count+1] - flat_label_vec[label_count+1+1];
  for (size_t i = 0; i < labels_back.size(); i++) {
    auto& label = labels_back[i];
    size_t hub_offset = flat_label_vec[1+label_count+1+i];
    for (size_t j = 0; j < label.size(); j++) {
      flat_label_vec[hub_offset+j] = label[j].hub;
      flat_label_vec[hub_offset+j+jump_to_dist] = label[j].dist;
    }
  }  

  return flat_label_vec;
}

void write_to_csv(CHOverlay& ov, string out_path) {
  ofstream out(out_path);
  out << "source,target" << endl;
  auto [edge_start, edge_end] = edges(ov);

  std::for_each(edge_start, edge_end, [&](const auto& e) {
    out << source(e, ov) << "," << target(e, ov) << endl; 
  });
}

void write_to_gr(CHOverlay& ov, string out_path) {
  ofstream out(out_path);
  stringstream out_sstr;
  out_sstr << "p tww " << num_vertices(ov) << " " << num_edges(ov) << endl;
  auto [edge_start, edge_end] = edges(ov);

  std::for_each(edge_start, edge_end, [&](const auto& e) {
    out_sstr << source(e, ov)+1 << " " << target(e, ov)+1 << endl; 
  });
  out << out_sstr.str();
} 

vector<CHOverlay::vertex_descriptor> read_node_order(string in_path) {
  vector<CHOverlay::vertex_descriptor> ordered_nodes;
  ifstream in(in_path);
  string node_string;
  while (getline(in, node_string)) {
    ordered_nodes.push_back(stoi(node_string));
  }

  return ordered_nodes;
}
} 
