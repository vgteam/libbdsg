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


DIST_UINT binary_intersection_ch(vector<HubRecord>& v1, vector<HubRecord>& v2);

template <typename ItrType>
DIST_UINT hhl_query(ItrType start_itr, size_t rank1, size_t rank2);  

void down_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void down_dijk_rev(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void test_dijk(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void test_dijk_rev(int node, CHOverlay& ov, vector<DIST_UINT>& node_dists, vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev);

void create_labels(vector<vector<HubRecord>>& labels, vector<vector<HubRecord>>& labels_rev, CHOverlay& ov);

vector<size_t> pack_labels(const vector<vector<HubRecord>>& labels, const vector<vector<HubRecord>>& labels_back); 

//not necessary stuff
void write_to_csv(CHOverlay& ov, string out_path);

void write_to_gr(CHOverlay& ov, string out_path); 

vector<CHOverlay::vertex_descriptor> read_node_order(string in_path);
}
