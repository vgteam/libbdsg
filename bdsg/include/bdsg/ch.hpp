/*
file for quickly playing around with stuff    
*/
#include "landmark.hpp"
#include "hublabel.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp> 
#include <boost/graph/graphviz.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <bdsg/snarl_distance_index.hpp>

//#define debug_binary_intersection
//#define debug_hhl_query

namespace bdsg {

/**
 * For a handle graph indexed with HHL, get the HHL rank ("Boost graph ID") for
 * an orientation of a node, as a source or destination.
 */
NODE_UINT bgid(const handle_t& h, const bdsg::HashGraph& hg);

/**
 * For a net graph indexed with HHL, get the HHL rank for an orientation of a
 * net graph element (snarl start node, snarl end node, child node, child
 * chain), as either the source or destination of a query.
 *
 * Snarl start nodes and snarl end nodes are handled so that "forward"
 * orientation runs along the snarl, regardless of the orientation that the
 * underlying handle graph node is in as a snarl boundary.
 *
 * Child chains and nodes are also handled so that "forward" orientation is the
 * orientation the thing has in the snarl. So if a node is reversed in the
 * snarl, asking about forward is actually asking about that node in its local
 * reverse orientation.
 *
 * For net graphs, we need to distinguish between source and destination status
 * to allow turning around within a child chain without traversing the full
 * length of the chain. Each child chain needs to be represented by a subgraph
 * with different in and out "port" nodes in each orientation.
 */
NODE_UINT bgid(size_t net_rank, bool is_reverse, bool is_source);

/**
 * For a handle or net graph indexed with HHL, take the HHL rank of an orientation of
 * a node and get that of the opposite orientation of a node.
 *
 * For handle graphs, ranks are the same for source and destination.
 *
 * For net graphs, ranks differ between source and destination "ports" for a
 * net graph element; this also swaps source and destination status.
 */
NODE_UINT rev_bgid(NODE_UINT n);


typedef struct NodeProp { 
  DIST_UINT seqlen;
  DIST_UINT max_out = 0;
  NODE_UINT contracted_neighbors = 0;
  NODE_UINT level = 0;
  NODE_UINT arc_cover = 1; 
  bool contracted = false;
  NODE_UINT new_id;
} NodeProp;

typedef struct EdgeProp { 
  bool contracted = false;
  DIST_UINT weight = 0;
  NODE_UINT arc_cover = 1;
  bool ori = true;
} EdgeProp; 

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NodeProp, EdgeProp> CHOverlay;
typedef boost::filtered_graph<CHOverlay, function<bool(CHOverlay::edge_descriptor)>> ContractedGraph;

/**
 * Build the intermediate hub labeling computation data structure ("Boost
 * graph") from a HashGraph.
 *
 * The nodes in the graph must have dense node IDs starting at 1.
 *
 * For later queries, orientations of nodes are assigned ranks as provided by
 * the bgid() function.
 */
CHOverlay make_boost_graph(const bdsg::HashGraph& hg);
/**
 * Build the intermediate hub labeling computation data structure ("Boost
 * graph") for the net graph of a snarl in a TemporaryDistanceIndex.
 *
 * all_children must contain the child chains and nodes of the snarl, as well as the bounding nodes of the snarl, in any order.
 *
 * For later queries, orientations of children or the snarl boundary nodes are assigned query ranks based on their snarl distance index rank.
 *
 * The snarl distance index ranks are 0 and 1 for the start and end nodes of the snarl, and the rank_in_parent field of the temporary index for each child.
 */
CHOverlay make_boost_graph(const bdsg::SnarlDistanceIndex::TemporaryDistanceIndex& temp_index, const SnarlDistanceIndex::temp_record_ref_t& snarl_index, const SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record, const vector<pair<SnarlDistanceIndex::temp_record_t, size_t>>& all_children, const HandleGraph* graph);

int edge_diff(ContractedGraph::vertex_descriptor nid, ContractedGraph& ch, CHOverlay& ov, vector<DIST_UINT>& node_dists, int hop_limit);

void contract(CHOverlay::vertex_descriptor nid, ContractedGraph& ch, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<bool>& shouldnt_contract, int hop_limit);

void make_contraction_hierarchy(CHOverlay& ov);
 
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

DIST_UINT binary_intersection_ch(vector<HubRecord>& v1, vector<HubRecord>& v2);
/*
 * Do binary intersection to find shared labels for two vertices.
 *
 * start_itr should point to the first slot of the packed label data returned
 * by pack_labels(), which is the label count.
 *
 * start_bound_index variables are relative to start_itr, and give the
 * positions of the stored start bounds for the two labels; the stored end
 * bounds will be in the slots after.
 */
template <typename ItrType>
DIST_UINT binary_intersection_ch(ItrType start_itr, size_t v1_start_bound_index, size_t v2_start_bound_index) {
  auto v1_start_bound_itr = next(start_itr, v1_start_bound_index);
  auto v1_end_bound_itr = next(v1_start_bound_itr, 1);
  auto v2_start_bound_itr = next(start_itr, v2_start_bound_index);
  auto v2_end_bound_itr = next(v2_start_bound_itr, 1);

  auto v1_start_itr = next(start_itr, *v1_start_bound_itr); 
  auto v1_end_itr = next(start_itr, *v1_end_bound_itr);

#ifdef debug_binary_intersection
  std::cerr << "Found " << v1_end_itr - v1_start_itr << " labels for vertex 1" << std::endl;
#endif

  auto v2_start_itr = next(start_itr, *v2_start_bound_itr); 
  auto v2_end_itr = next(start_itr, *v2_end_bound_itr);

#ifdef debug_binary_intersection
  std::cerr << "Found " << v2_end_itr - v2_start_itr << " labels for vertex 2" << std::endl;
#endif

  auto v1_range = ranges::subrange<ItrType>(v1_start_itr, v1_end_itr);
  auto v2_range = ranges::subrange<ItrType>(v2_start_itr, v2_end_itr); 

  auto& key_vec = v1_range.size() < v2_range.size() ? v1_range : v2_range; 
  auto& search_vec = v1_range.size() < v2_range.size() ? v2_range : v1_range;
 
  auto search_start_itr = search_vec.begin();
  auto search_end_itr = search_vec.end(); 
  DIST_UINT min_dist = INF_INT;
  for (auto it = key_vec.begin(); it < key_vec.end(); it++) {
#ifdef debug_binary_intersection
    cerr << "Performing key query" << endl;
#endif
    auto k = *it;
    auto k_dist_itr = get_dist_itr(start_itr, it);
#ifdef debug_binary_intersection
    cerr << "Distance for k " << k << " is " << *k_dist_itr << ", at: " << distance(start_itr,k_dist_itr) << endl;
    cerr << "searching for " << k << " between " << distance(start_itr,search_start_itr) << " & " << distance(start_itr,search_end_itr) << endl;
#endif
    search_start_itr = lower_bound(search_start_itr, search_end_itr, k); 
    if (search_start_itr == search_end_itr) {
#ifdef debug_binary_intersection
      std::cerr << "No more search results possible" << std::endl;
#endif
      return min_dist;
    } 
    if (*search_start_itr == k) {
#ifdef debug_binary_intersection
      cerr << "match found, key: " << *search_start_itr << ", at " << distance(start_itr,search_start_itr) << endl;
#endif
      auto dist_itr = get_dist_itr(start_itr, search_start_itr);
      DIST_UINT d = *(dist_itr) + *(k_dist_itr);
#ifdef debug_binary_intersection
      cerr << "dist for key is: " << *dist_itr << ", at " << distance(start_itr,dist_itr) << endl; 
      cerr << "total dist is: " << d << endl;
#endif
      min_dist = min(min_dist, d);
    }
  }  
  return min_dist; 
}  

/**
 * Query stored hub label data for a minimum distance.
 *
 * start_itr should point to the first slot of the packed label data returned
 * by pack_labels(), which is the label count.
 *
 * The rank space covers both orientations of each node.
 *
 * Returns the minimum distance from the end of the node orientation at rank1
 * to the start of the node orientation at rank2. (If working in a net graph in
 * a SnarlDistanceIndex, these "nodes" may really be child chains.)
 *
 * If rank1 == rank2, returns the minimum distance around that cycle, if any.
 *
 * If there is no known path between the given nodes, returns INF_INT.
 */
template <typename ItrType>
DIST_UINT hhl_query(ItrType start_itr, size_t rank1, size_t rank2) {
  size_t label_count = *start_itr;

#ifdef debug_hhl_query
  std::cerr << "Making hub label query on " << label_count << " labels" << std::endl;
#endif

  // Bounds start after the label count, and at the rank of the first
  // vertex past there we find the start bound for the first vertex.
  auto start_index_1 = 1+rank1;

#ifdef debug_hhl_query
  std::cerr << "Start bound for forward label for rank " << rank1 << " is at index " << start_index_1 << " past there" << std::endl;
#endif

  // And there's a final end value for the first set of labels before we go on
  // to the bounds where we would find the start bound for the second vertex.
  auto start_index_2 = 1+label_count+1+rank2;

#ifdef debug_hhl_query
  std::cerr << "Start bound for reverse label for rank " << rank2 << " is at index " << start_index_2 << " past there" << std::endl;
#endif
  
  DIST_UINT dist = binary_intersection_ch(start_itr, start_index_1, start_index_2);


  return dist; 
}      

void down_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void down_dijk_rev(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void test_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void test_dijk_rev(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void create_labels(vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev, CHOverlay& ov);

/**
 * Puts hub labels in a flat vector form
 * 
 * Structure:
 * - offsets are relative to start of flat vector
 * - extra offset in each of fwd and back offset sets at the end so that end of ranges can be found
 * -- subtracting the extra offset by the first offset of its set gets the distance to the corresponding dist of a hub
 *
 * The layout is:
 * label count | start offsets (fwd) | start offsets (back) | fwd label hubs | fwd label dists | back label hubs | back label dists 
*/
vector<size_t> pack_labels(const vector<vector<HubRecord>>& labels, const vector<vector<HubRecord>>& labels_back); 

//not necessary stuff
void write_to_csv(CHOverlay& ov, string out_path);

void write_to_gr(CHOverlay& ov, string out_path); 

vector<CHOverlay::vertex_descriptor> read_node_order(string in_path);
}
