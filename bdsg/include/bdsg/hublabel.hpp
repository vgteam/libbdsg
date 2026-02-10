/*
header file for hub labeling stuff
*/
#include "landmark.hpp"
#include <fstream> 

namespace bdsg {

typedef unordered_map<NodesideId,DIST_UINT> NsDistMap;
typedef struct HubRecord {
  NodeId hub {};
  DIST_UINT dist {};

  HubRecord() : hub{0}, dist{INF_INT} {}  
  HubRecord(NodeId hid, DIST_UINT min_dist) : hub{hid}, dist{min_dist} {}

  auto operator<=>(const HubRecord& r2) const {
    return hub <=> r2.hub;
  }
  
  auto operator<=>(const NodeId& n) const {
    return hub <=> n;
  } 
  //based off https://uscilab.github.io/cereal/serialization_functions.html
  template<class Archive>
  void serialize(Archive& a) {
    a(hub, dist); 
  }
} HubRecord;

typedef pair<vector<HubRecord>,vector<HubRecord>> HubRecsPair; 
typedef uint32_t NODE_UINT;


//first vec stores hub nodes whose left ns the label's ns reaches
//second vec stores hub nodes whose right ns the label's ns reaches  
typedef pair<vector<NodeId>,vector<NodeId>> HubsPair;  
typedef pair<HubsPair,NsDistMap> Label; 

typedef tuple<DIST_UINT, NodesideId> QueueObj2; 
bool pqcomp2(const QueueObj2& o1, const QueueObj2& o2);                    
typedef std::priority_queue<QueueObj2, vector<QueueObj2>, function<bool(const QueueObj2&, const QueueObj2&)>> PriorityQueue2; 

typedef tuple<DIST_UINT, NodesideId, DIST_UINT> HwQueueObj; 
bool pqcomp_hw(const HwQueueObj& o1, const HwQueueObj& o2);                    
typedef std::priority_queue<HwQueueObj, vector<HwQueueObj>, function<bool(const HwQueueObj&, const HwQueueObj&)>> HwPriorityQueue; 

bool label_contains(HubRecsPair& label, NodesideId query);

DIST_UINT get_label_dist(HubRecsPair& label, NodesideId query); 

DIST_UINT hl_build_intersect(HubRecsPair& l1, HubRecsPair& l2, vector<DIST_UINT>& node_lens); 

DIST_UINT get_degree(NodeId node, bdsg::HashGraph& g);

void pruned_dijkstra(bdsg::HashGraph& g, NodesideId start, vector<HubRecsPair>& labels, vector<DIST_UINT>& path_lengths, vector<NodesideId>& rank_to_ns, vector<NODE_UINT>& ns_to_rank, vector<DIST_UINT>& rank_node_lens,int stop_dist = INF_INT);

DIST_UINT hl_query(NodesideId i, NodesideId j, vector<HubRecsPair>& labels, bdsg::HashGraph& g);
DIST_UINT hl_query(NodesideId i, NodesideId j, vector<HubRecsPair>& labels, vector<DIST_UINT>& rank_node_lens);

vector<HubRecsPair> make_labels(bdsg::HashGraph& g); 



} 
