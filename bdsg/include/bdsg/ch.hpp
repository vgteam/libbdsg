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

namespace bdsg { 
NODE_UINT bgid(const handle_t& h, bdsg::HashGraph& hg);
 
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

CHOverlay make_boost_graph(bdsg::HashGraph& hg);
CHOverlay make_boost_graph(bdsg::SnarlDistanceIndex::TemporaryDistanceIndex& temp_index, pair<bdsg::SnarlDistanceIndex::temp_record_t, size_t>& snarl_index, SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord& temp_snarl_record, vector<pair<SnarlDistanceIndex::temp_record_t, size_t>>& all_children, const HandleGraph* graph);

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

  std::cerr << "Found " << v1_end_itr - v1_start_itr << " labels for vertex 1" << std::endl;

  auto v2_start_itr = next(start_itr, *v2_start_bound_itr); 
  auto v2_end_itr = next(start_itr, *v2_end_bound_itr);

  std::cerr << "Found " << v2_end_itr - v2_start_itr << " labels for vertex 2" << std::endl;

  auto v1_range = ranges::subrange<ItrType>(v1_start_itr, v1_end_itr);
  auto v2_range = ranges::subrange<ItrType>(v2_start_itr, v2_end_itr); 

  auto& key_vec = v1_range.size() < v2_range.size() ? v1_range : v2_range; 
  auto& search_vec = v1_range.size() < v2_range.size() ? v2_range : v1_range;
 
  auto search_start_itr = search_vec.begin();
  auto search_end_itr = search_vec.end(); 
  DIST_UINT min_dist = INF_INT;
  for (auto it = key_vec.begin(); it < key_vec.end(); it++) {
    cerr << "Performing key query" << endl;
    auto k = *it;
    auto k_dist_itr = get_dist_itr(start_itr, it); 
    cerr << "Distance for k " << k << " is " << *k_dist_itr << ", at: " << distance(start_itr,k_dist_itr) << endl;
    cerr << "searching for " << k << " between " << distance(start_itr,search_start_itr) << " & " << distance(start_itr,search_end_itr) << endl;
    search_start_itr = lower_bound(search_start_itr, search_end_itr, k); 
    if (search_start_itr == search_end_itr) {
      std::cerr << "No more search results possible" << std::endl; 
      return min_dist;
    } 
    if (*search_start_itr == k) {
      cerr << "match found, key: " << *search_start_itr << ", at " << distance(start_itr,search_start_itr) << endl;
      auto dist_itr = get_dist_itr(start_itr, search_start_itr);
      DIST_UINT d = *(dist_itr) + *(k_dist_itr);
      cerr << "dist for key is: " << *dist_itr << ", at " << distance(start_itr,dist_itr) << endl; 
      cerr << "total dist is: " << d << endl;
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
 */
template <typename ItrType>
DIST_UINT hhl_query(ItrType start_itr, size_t rank1, size_t rank2) {
  size_t label_count = *start_itr;

  std::cerr << "Making hub label query on " << label_count << " labels" << std::endl;

  // Bounds start after the label count, and at the rank of the first
  // vertex past there we find the start bound for the first vertex.
  auto start_index_1 = 1+rank1;

  std::cerr << "Start bound for forward label for rank " << rank1 << " is at index " << start_index_1 << " past there" << std::endl;

  // And there's a final end value for the first set of labels before we go on
  // to the bounds where we would find the start bound for the second vertex.
  auto start_index_2 = 1+label_count+1+rank2;

  std::cerr << "Start bound for reverse label for rank " << rank2 << " is at index " << start_index_2 << " past there" << std::endl;
  
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
