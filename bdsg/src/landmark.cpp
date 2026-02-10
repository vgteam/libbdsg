#include "bdsg/landmark.hpp"

using namespace std;
namespace bdsg {  

int addInt(int a, int b) {
  if (a == INF_INT || b == INF_INT) {
    return INF_INT;
  }                   
  return a+b;
}

bool pqcomp1(const QueueObj& o1, const QueueObj& o2) {
  return get<0>(o1) > get<0>(o2);
};

/*
following functions assume input graph node ids go from 1...N 
*/

/*
Converts a node_id to a nodeside_id
*/
//using bool for indicating side just like for direction in follow_edges
NodesideId node_to_nodeside(NodeId node_id, bool left) {
  NodesideId ns_id = (node_id-1)*2; 

  if (!left) {
    ns_id += 1;
  }

  return ns_id;
}

array<NodesideId, 2> get_node_nodesides(NodeId node_id) {
  NodesideId ns_id = (node_id-1)*2; 
  array<NodesideId, 2> both = {ns_id, ns_id+1};
  return both;
}

NodeId nodeside_to_node(NodesideId nodeside_id) {
  return nodeside_id / 2 + 1;
}

tuple<NodeId,bool> nodeside_to_node_tuple(NodesideId nodeside_id) {
  tuple<NodeId,bool> t = {nodeside_id / 2 + 1, (nodeside_id%2==0)}; 
  return t;
} 

bool nodeside_left (NodesideId nodeside_id) {
  return nodeside_id%2==0;
}

NodesideId other_nodeside(int nodeside_id) {
  return nodeside_id ^ 1;
}

NodesideId handle_entry_nodeside(const handle_t& handle, HashGraph& g) {
  return node_to_nodeside(g.get_id(handle), !g.get_is_reverse(handle));
}

string nodeside_string(NodesideId nodeside) {
  bool is_left = nodeside_left(nodeside);
  string dir = is_left ? "left" : "right";
  NodeId node = nodeside_to_node(nodeside);
 
  stringstream stream;
  stream << node << " (" << dir << ")";
  return stream.str();
}

/// landmark selection

/*
returns ids of nodesides with largest degree
*/
//helper
int nodeside_degree(NodesideId ns_id, HashGraph& g) {
  tuple<NodeId,bool> t = nodeside_to_node_tuple(ns_id);
  NodeId node_id = get<0>(t);
  bool is_left = get<1>(t);
  handle_t node_handle = g.get_handle(node_id);
  return g.get_degree(node_handle, is_left);
}

vector<NodesideId> get_highest_degree_nodesides(bdsg::HashGraph& g, int top_num, int nodeside_count) {
  vector<int> ns_degrees; ns_degrees.resize(nodeside_count);
  vector<NodesideId> ns; ns.resize(nodeside_count);
  for (int i = 0; i < nodeside_count; i++) {
    ns_degrees[i]=nodeside_degree(i, g);
    ns[i] = i;
  } 
  
  std::sort(ns.begin(), ns.end(), [&ns_degrees](NodesideId n1, NodesideId n2){
                                    return ns_degrees[n1] > ns_degrees[n2];
                                  });
  
  auto top_span = span(ns).subspan(0, top_num);
  vector<NodesideId> top_deg_ns(top_span.begin(), top_span.end()); 
  return top_deg_ns;
}

vector<NodeId> get_landmark_nodes(bdsg::HashGraph& g, int top_ns_num, int nodeside_count) {
  vector<NodesideId> highest_deg_ns = get_highest_degree_nodesides(g, top_ns_num, nodeside_count);
  vector<NodeId> possible_lm; possible_lm.resize(top_ns_num);
  ranges::transform(highest_deg_ns.begin(), highest_deg_ns.end(), possible_lm.begin(), nodeside_to_node);
  sort(possible_lm.begin(),possible_lm.end());
  auto new_lm_end_it = unique(possible_lm.begin(), possible_lm.end());
  possible_lm.resize(distance(possible_lm.begin(), new_lm_end_it));
  
  return possible_lm;
}

/*
Notes:
- modifies the table passed in
*/
void fill_other_nodeside_dists(SdslArray2D& dist_table, HashGraph& g) {
  auto table_shape = dist_table.shape();
  for (DIST_UINT i = 0; i < table_shape[0]; i++) {
    for (DIST_UINT ns = 0; ns < table_shape[1]; ns++) {
      if (dist_table.get(i,ns) != INF_INT) {
        NodesideId other_ns = other_nodeside(ns);
        NodeId node = nodeside_to_node(ns);
        dist_table.set(i, other_ns, min(addInt(dist_table.get(i,ns), g.get_length(g.get_handle(node))), static_cast<int>(dist_table.get(i,other_ns)))); 
      }
    }
  }
}  
 
/*
trying insert-Dijkstra over decrease-key dijkstra
see https://stackoverflow.com/questions/9255620/why-does-dijkstras-algorithm-use-decrease-key

start is the id of the origin node
dijkstra goes from start node to other nodes' nodesides
*/ 

PriorityQueue initQueue(
  int nodeside_count, 
  NodesideId start_ns
) {
  vector<QueueObj> init_objs; init_objs.resize(2);
  /*
  for (int i = 0; i < nodeside_count; i++) {
    init_objs[i] = make_tuple(INF_INT, OTHER_NODESIDE, i);
    init_objs[i+nodeside_count] = make_tuple(INF_INT, OTHER_NODE, i);
  }*/
  
  //NodesideId start_node_other_ns = other_nodeside(start_ns);
  //treating start_ns as arrived to from OTHER_NODESIDE so exit is to another node
  //don't exit out other direction, that's covered by the other nodeside of the node
  init_objs[0] = make_tuple(0, OTHER_NODESIDE, start_ns);
  //dummy queue obj
  init_objs[1] = make_tuple(INF_INT, OTHER_NODE, start_ns);
  /*
  for (QueueObj qo: init_objs) {
    auto [a,b,c] = qo;
    nodeside_queue.push(qo);
  }     */
  PriorityQueue nodeside_queue{pqcomp1, move(init_objs)};
  return nodeside_queue;
}

/*
Called when new nodeside is discovered.
If path to the nodeside is the shortest found so far:
  update path_lengths and add new queue entry for it
*/
bool discover_nodeside(NodesideId new_nodeside, EnterDir enter_direction, int new_len, vector<vector<int>>& path_lengths, PriorityQueue& nodeside_queue) {
  
  if (new_len < path_lengths[enter_direction][new_nodeside]) {
    nodeside_queue.push({new_len, enter_direction, new_nodeside});
    path_lengths[enter_direction][new_nodeside] = new_len;
  } 
  return true;
}; 

//arguments after second are optional
SdslArray2D dijkstra(bdsg::HashGraph& g, NodesideId start, NodesideId stop_ns, int stop_dist, bool ball_ver) {
  //code in this function based off https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Pseudocode and its subsections
  int nodeside_count = g.get_node_count()*2; 

  #define LEFT_PATH true 
  #define RIGHT_PATH false

  NodesideId start_ns = start;
  //handle_t start_node_handle = g.get_handle(nodeside_to_node(start_ns));


  int inf = INF_INT;
  
  /*auto is_start = [start_ns](NodesideId ns) {
    return (ns == start_ns);
  }; */

  //Need to see each nodeside twice; need to keep track of two distances, one when entering from other nodeside
  //the other when entering from a completely different node
  //variables called enter_direction keep track of direction of entry
  PriorityQueue nodeside_queue = initQueue(nodeside_count, start_ns);
 
  //matrix to store minimum distances found so far
  vector<vector<int>> path_lengths(2);
  for (auto& v: path_lengths) {
    v.resize(nodeside_count);
    fill(v.begin(), v.end(), inf);
  }

  path_lengths[OTHER_NODESIDE][start_ns] = 0;

  vector<vector<bool>> which_path(2); 
  
  //keep track of which nodesides have self-loops
  vector<bool> self_loop; self_loop.resize(nodeside_count);
  fill(self_loop.begin(), self_loop.end(), false);
  
  
  //Stop conditions:
  // - empty queue
  // - stop_ns visited from the OTHER_NODE direction
  // - stop_dist reached or exceeded by next queue item
  auto not_stop_cond = [stop_ns,stop_dist] (const QueueObj& top_item) {
    bool not_stop_ns = !(get<1>(top_item) == OTHER_NODE && get<2>(top_item) == stop_ns); 
    bool not_stop_dist = (get<0>(top_item) <= stop_dist) && (get<0>(top_item) != INF_INT);
    return not_stop_ns && not_stop_dist;
  };
  while ((!nodeside_queue.empty()) && not_stop_cond(nodeside_queue.top())) {
    QueueObj item = nodeside_queue.top(); nodeside_queue.pop();
    int cur_dist = get<0>(item);
    EnterDir enter_direction = get<1>(item); 
    NodesideId cur_nodeside = get<2>(item);
    
    //gotta have this since priorities of C++ priority queue elements can't be updated
    if (cur_dist != path_lengths[enter_direction][cur_nodeside]){
      continue;
    }

    NodeId node_id = nodeside_to_node(cur_nodeside); 

    
    
    if (enter_direction == OTHER_NODESIDE) {
      //got here from the other nodeside of cur_nodeside's node
      handle_t handle = g.get_handle(node_id, nodeside_left(cur_nodeside));
      
      g.follow_edges(handle, false, [&g,cur_nodeside,&handle,cur_dist,&self_loop,&path_lengths,&nodeside_queue](const handle_t& new_h){
                                        NodesideId new_nodeside = handle_entry_nodeside(new_h,g);
                                        if (new_nodeside == cur_nodeside) {
                                          self_loop[new_nodeside] = true;
                                        }
                                        
                                        return discover_nodeside(
                                          new_nodeside, OTHER_NODE, 
                                          cur_dist, 
                                          path_lengths, nodeside_queue
                                        );
                                    });
      
    } 
    else {
      //came here from a nodeside of a node that =/= cur_nodeside's node
      handle_t handle = g.get_handle(node_id, !nodeside_left(cur_nodeside));  
      int handle_len = g.get_length(handle);

      NodesideId other_ns = other_nodeside(cur_nodeside);
      //handle_t cur_handle = self_loop[cur_nodeside] ? g.flip(handle):handle;
      if (other_ns == cur_nodeside) {
        self_loop[other_ns] = true;
      } 
      discover_nodeside(
        other_ns, OTHER_NODESIDE,
        cur_dist == INF_INT ? INF_INT : cur_dist+handle_len, 
        path_lengths, nodeside_queue
      );
    }
    
  } 
  
  SdslArray2D res(1, nodeside_count);
  for (int i = 0; i < nodeside_count; i++) {
    res.set(0, i, path_lengths[OTHER_NODE][i]);
  }
  
  if (ball_ver) {
    //fill in OTHER_NODESIDE dists for balls
    fill_other_nodeside_dists(res, g);
  }  
  return res;
}

/*
dijkstra edge cases
*/

/*
notes:
 - lm_nodes should not be empty
*/
SdslArray2D get_lm2ns_dist_table(vector<NodeId>& lm_nodes, HashGraph& g) { 
  size_t nodeside_count = g.get_node_count()*2;
  //rows: one row for each landmark nodeside (two rows per landmark)
  //cols: one column for each nodeside of the graph
  SdslArray2D table(lm_nodes.size()*2, nodeside_count);

  for (size_t i = 0; i < lm_nodes.size(); i++) {
    NodeId lm_node = lm_nodes[i];
    array<NodesideId,2> lm_nodesides = get_node_nodesides(lm_node);

    for (size_t ns_i = 0; ns_i < lm_nodesides.size(); ns_i++) {
      SdslArray2D dists = dijkstra(g, lm_nodesides[ns_i]);
    
      size_t row = i*2+ns_i;
      for (size_t col = 0; col < nodeside_count; col++) {
        table.set(row, col, dists.get(0,col));
      }
    }
  }
  
  return table;
}

void get_closest_lm_ind(SdslArray2D& dist_table, vector<int>& ind_vec, vector<int>& min_dist_vec, int nodeside_count) {
  //typedef boost::multi_array_types::index_range index_range;
  ind_vec.resize(nodeside_count); 
  min_dist_vec.resize(nodeside_count);
  for (int ns = 0; ns < nodeside_count; ns++) {
    //Array2D::index_gen ind_gen;
    //Array2D::array_view<1>::type ns_col_view = dist_table[ind_gen[index_range(0,dist_table.shape()[0])][ns]];
    auto ns_col_view = dist_table.col_view(ns);
    auto min_itr = min_element(ns_col_view.begin(), ns_col_view.end());
    ind_vec[ns] = distance(ns_col_view.begin(), min_itr) / 2;
    int min_dist = *min_itr;
    min_dist_vec[ns] = min_dist;
  }
}
/*
TEST_CASE("simple get_closest_lm_ind test","") {
  HashGraph gt;
  handle_t h = gt.create_handle("A"); 
  handle_t h2 = gt.create_handle("AC");
  handle_t h3 = gt.create_handle("ACG");
  handle_t h4 = gt.create_handle("ACGT");
  handle_t h5 = gt.create_handle("ACGTA");
  handle_t h6 = gt.create_handle("ACGTAC");

  gt.create_edge(h,h2); gt.create_edge(h,h3);
  gt.create_edge(h2,h4); gt.create_edge(h3,h5);
  gt.create_edge(h4,h6); gt.create_edge(h5,h6);
  gt.create_edge(h,h6);

  int nodeside_count = gt.get_node_count()*2;

  NodeId l1 = gt.get_id(h);
  NodeId l2 = gt.get_id(h6);
  vector<NodeId> lm_nodes = {l1, l2};
  Array2D dist_table = get_lm2ns_dist_table(lm_nodes, gt);
 
  vector<int> closest_lm_ind_vec; closest_lm_ind_vec.resize(nodeside_count);
  vector<int> closest_lm_dist_vec; closest_lm_dist_vec.resize(nodeside_count);
  get_closest_lm_ind(dist_table, closest_lm_ind_vec, closest_lm_dist_vec, nodeside_count);

  vector<int> ind_ans_key = {0,1,0,1,0,1,0,1,0,1,0,0}; 
  for (int i = 0; i<closest_lm_ind_vec.size(); i++) {
    int cind = closest_lm_ind_vec[i]; 
    REQUIRE(closest_lm_ind_vec[i] == ind_ans_key[i]);
  }

  vector<int> dist_ans_key = {INF_INT,0,0,4,0,5,2,0,3,0,0,INF_INT};
  for (int i = 0; i<closest_lm_dist_vec.size(); i++) {
    REQUIRE(closest_lm_dist_vec[i] == dist_ans_key[i]); 
  } 

} */

/*
based off https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/msr-tr-2009-84.pdf [Chen et al. (2009)]
*/
Ball get_ball_contents(NodesideId ns, int dist_limit, HashGraph& g) {
  //last param is whether to fill in OTHER_NODESIDE dists or not
  SdslArray2D dists = dijkstra(g, ns, -1, dist_limit, false);

  Ball ball; 
  for (size_t ns = 0; ns < g.get_node_count()*2; ns++) {
    int dist = dists.get(0,ns);
    if (dist != INF_INT && dist <= dist_limit) {
      ball[ns] = dist;
    }
  }
  return ball;
}
/* 
TEST_CASE("get_ball_contents - hexagonal graph","[get_ball]") {
  HashGraph gt;
  handle_t h = gt.create_handle("A"); 
  handle_t h2 = gt.create_handle("AC");
  handle_t h3 = gt.create_handle("ACG");
  handle_t h4 = gt.create_handle("ACGT");
  handle_t h5 = gt.create_handle("ACGTA");
  handle_t h6 = gt.create_handle("ACGTAC");
  gt.create_edge(h,h2); gt.create_edge(h,h3);
  gt.create_edge(h2,h4); gt.create_edge(h3,h5);
  gt.create_edge(h4,h6); gt.create_edge(h5,h6);
  gt.create_edge(h,h6);
  int nodeside_count = gt.get_node_count()*2;

  NodesideId start_ns = node_to_nodeside(gt.get_id(h2), false); 
  int dist_limit = 2;
  Ball ball = get_ball_contents(start_ns, dist_limit, gt);
  REQUIRE(ball.size() == 1);
  
  NodesideId left4 = node_to_nodeside(gt.get_id(h4), true); 
  REQUIRE(ball.contains(left4));
  REQUIRE(ball[left4] == 0);
                       
} */

vector<Ball> find_balls(vector<int>& closest_lm_dist_vec, HashGraph& g, int min_ball_size) {  
  int nodeside_count = g.get_node_count()*2; 
  vector<Ball> balls; balls.resize(nodeside_count);
  for (int i = 0; i < nodeside_count; i++) {
    int dist_limit = max(min_ball_size,closest_lm_dist_vec[i]); 
    balls[i] = get_ball_contents(i, dist_limit, g);
  }
  return balls;
}   

/*
query algorithm from Chen et al. (2009)
with modifications for our purpose
*/

int oracle_query(NodesideId source, NodesideId target, OracleInfo& oracle, HashGraph& g) {
  #define ball_list oracle.balls
  #define lm_node_vec oracle.lm_nodes
  #define closest_lm_ind oracle.closest_lm_inds
  #define lm_dist oracle.closest_lm_dists
  #define lm_to_ns oracle.lm_sides_to_ns
  
  if (ball_list[source].contains(target)) {
    return ball_list[source][target];
  }

  if (ball_list[target].contains(source)) {
    return ball_list[target][source];
  }
  
  auto get_guess = [&](NodesideId ns) { 
    //typedef boost::multi_array_types::index_range index_range;
    
    NodeId lm = lm_node_vec[closest_lm_ind[ns]];
    //auto lm_nodesides = get_node_nodesides(lm);
    int lm_length = g.get_length(g.get_handle(lm));
    int closest_lm_ns_ind = closest_lm_ind[ns]*2;

    //Array2D::index_gen ind_gen; 

    //2 x nodeside_count table 
    //each row is a nodeside of the landmark
    //col = nodeside id
    //auto lm_table = lm_to_ns[ind_gen[index_range(closest_lm_ns_ind, closest_lm_ns_ind+2)][index_range()]];
  
    //generate all possible distances
    vector<int> s;
    for (int a: {lm_to_ns.get(closest_lm_ns_ind, source), lm_to_ns.get(closest_lm_ns_ind+1, source)}) {
      for (int b: {lm_to_ns.get(closest_lm_ns_ind, target), lm_to_ns.get(closest_lm_ns_ind+1, target)}) {
        int pos_dist=addInt(a,b);
        s.push_back(pos_dist);
      }
    }
    
    //in case landmark has a self-loop
    s[0] = addInt(s[0], addInt(lm_to_ns.get(closest_lm_ns_ind+1,node_to_nodeside(lm,false)), lm_length));
    s[3] = addInt(s[3], addInt(lm_to_ns.get(closest_lm_ns_ind, node_to_nodeside(lm,true)), lm_length));
    
    int min_dist = addInt(*min_element(s.begin(), s.end()), lm_length);
    
    return min_dist;
  };

  return min(get_guess(target), get_guess(source));
}


void printMArray(Array2D& arr) {
  auto sh = arr.shape();
  int num_rows = sh[0]; int num_cols = sh[1];
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      if (arr[i][j] == INF_INT) {
        cerr << "inf ";
      } else {
        cerr << arr[i][j] << " ";
      }
    }
    cerr << endl;
  } 
} 


}

