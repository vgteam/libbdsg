#ifndef LANDMARK_HPP
#define LANDMARK_HPP

#include <span>
#include <ranges>
#include <boost/heap/priority_queue.hpp>
#include <boost/multi_array.hpp>
#include <bdsg/hash_graph.hpp>
#include <sdsl/vectors.hpp>
//#include <catch2/catch_session.hpp>
//#include <catch2/catch_test_macros.hpp>

using namespace std;
namespace bdsg {  
//inf implementation is largest possible int
#define INF_INT numeric_limits<int>::max()
#define DIST_NBITS 32
#define DIST_UINT uint32_t
#define ARR2D_OFFSET 1

/// Allow promoting a DIST_UINT to a different type, translating infinities to the type's max limit.
template<typename OtherInt>
OtherInt promote_distance(DIST_UINT val) {
    if (val == INF_INT) {
        return std::numeric_limits<OtherInt>::max();
    }
    return (OtherInt) val;
}

/// Allow demoting a DIST_UINT from a different type, translating infinities
/// from the type's max limit and erroring on unrepresentably large values.
template<typename OtherInt>
DIST_UINT demote_distance(OtherInt val) {
    if (val == std::numeric_limits<OtherInt>::max()) {
        return INF_INT;
    }
    if (val > (OtherInt) INF_INT) {
        throw std::overflow_error("Cannot store excessively wide value " + std::to_string(val) + " in " + std::to_string(DIST_NBITS) + " bits for hub labeling");
    }
    return (DIST_UINT) val;
}

/// Sum two distances, propagating infinities.
/// Does not check for overlfow.
/// TODO: We're not really sure if our distances are ints or uints and we freely mix them when we shouldn't.
int addInt(int a, int b);

typedef int NodeId;
typedef int NodesideId;
typedef enum EnterDir {OTHER_NODESIDE=0,OTHER_NODE=1} EnterDir;
typedef boost::multi_array<int, 2> Array2D;
typedef unordered_map<NodesideId, int> Ball;
class SdslArray2D {
private:
  vector<sdsl::int_vector<DIST_NBITS>> arr2d;
  uint8_t offset = ARR2D_OFFSET;
  static DIST_UINT get_inf() {
    bitset<DIST_NBITS> uint_bits;
    uint_bits.reset().flip(DIST_NBITS - 1);
    return static_cast<DIST_UINT>(uint_bits.to_ulong()); 
  }
  DIST_UINT Inf_UInt = 0; 
  DIST_UINT row_count = 0;
  DIST_UINT col_count = 0; 
  static int toOut(DIST_UINT entry) {
    if (entry == 0) {
      return INF_INT;
    } else {
      return static_cast<int>(entry-ARR2D_OFFSET);
    } 
  }
public:
  using size_type = DIST_UINT;
  SdslArray2D(int nrow, int ncol) {
    row_count = nrow; col_count = ncol; 
    arr2d.resize(row_count);
    for (DIST_UINT i = 0; i < row_count; i++) {
      sdsl::int_vector<DIST_NBITS> sdsl_row(col_count, Inf_UInt, DIST_NBITS);
      arr2d[i] = std::move(sdsl_row);
    }
  }                                   

  SdslArray2D(Array2D& a) {
    auto ashape = a.shape();
    row_count = ashape[0]; col_count = ashape[1];
    arr2d.resize(row_count);
    for (DIST_UINT i = 0; i < row_count; i++) {
      sdsl::int_vector<DIST_NBITS> sdsl_row(col_count, Inf_UInt, DIST_NBITS);
      for (DIST_UINT j = 0; j < col_count; j++) {
        int entry = a[i][j];
        if (entry == INF_INT) {
          sdsl_row[j] = Inf_UInt;
        } else {
          sdsl_row[j] = static_cast<DIST_UINT>(entry)+offset;
        }
      }
      arr2d[i] = std::move(sdsl_row);
    }
  }

  sdsl::int_vector<DIST_NBITS>& operator[](int i) {
    return arr2d[i];
  }

  DIST_UINT get(int i, int j) {
    DIST_UINT entry = arr2d[i][j];
    if (entry == 0) {
      return INF_INT;
    } else {
      return entry-offset;
    }
  }

  void set(int i, int j, DIST_UINT val) {
    if (val == INF_INT) {
      arr2d[i][j] = 0;
    } else {
      arr2d[i][j] = val+offset;
    }
  }

  size_type serialize(ostream& out, sdsl::structure_tree_node* prev_node=nullptr, string structure_name="SdslArr2d") const {
    sdsl::structure_tree_node* arr_node = sdsl::structure_tree::add_child(prev_node, structure_name, sdsl::util::class_name(*this));
    size_type bytes_count = 0;
    DIST_UINT nrows = arr2d.size();
    bytes_count += sdsl::write_member(nrows, out, arr_node, "nrows");
    for (auto& row: arr2d) {
      bytes_count += row.serialize(out, arr_node, "arr2d_row");
    }
    return bytes_count;
  }

  void load(istream& in) {
    DIST_UINT nrows;
    sdsl::read_member(nrows, in);
    arr2d.resize(nrows);
    for (DIST_UINT i = 0; i < nrows; i++) {
      arr2d[i].load(in);
    }
    row_count = nrows; col_count = arr2d[0].size();
  }

  void bit_compress() {
    for (auto& row: arr2d) {
      sdsl::util::bit_compress(row);
    }
  }

  array<DIST_UINT, 2> shape() {
    array<DIST_UINT, 2> shape_arr = {row_count, col_count};
    return shape_arr;
  }

  auto col_view(DIST_UINT col) {
    auto get_col_entry = [col] (sdsl::int_vector<DIST_NBITS>& row) {
                           return row[col];
                         };

    auto col_itr = ranges::views::transform(ranges::views::transform(arr2d, get_col_entry), toOut);
    return col_itr; 
  }
};

/*
following functions assume input graph node ids go from 1...N
*/

/*
Converts a node_id to a, or both nodeside_ids
left and return_both should not both be True
*/
//using bool for indicating side just like for direction in follow_edges
NodesideId node_to_nodeside(NodeId node_id, bool left);

array<NodesideId, 2> get_node_nodesides(NodeId node_id);

NodeId nodeside_to_node(NodesideId nodeside_id);

tuple<NodeId,bool> nodeside_to_node_tuple(NodesideId nodeside_id);

bool nodeside_left (NodesideId nodeside_id);

NodesideId other_nodeside(int nodeside_id);

NodesideId handle_entry_nodeside(const handle_t& handle, HashGraph& g);

string nodeside_string(NodesideId nodeside);


/// landmark selection

/*
returns ids of nodesides with largest degree
*/
//helper
int nodeside_degree(NodesideId ns_id, HashGraph& g);

vector<NodesideId> get_highest_degree_nodesides(bdsg::HashGraph& g, int top_num, int nodeside_count);

vector<NodeId> get_landmark_nodes(bdsg::HashGraph& g, int top_ns_num, int nodeside_count);

/*
Notes:
- modifies the table passed in
*/
void fill_other_nodeside_dists(SdslArray2D& dist_table, HashGraph& g); 

/*
trying insert-Dijkstra over decrease-key dijkstra
see https://stackoverflow.com/questions/9255620/why-does-dijkstras-algorithm-use-decrease-key

start is the id of the origin node
dijkstra goes from start node to other nodes' nodesides
*/ 
typedef tuple<int,EnterDir,NodesideId> QueueObj; 
bool pqcomp1(const QueueObj& o1, const QueueObj& o2);                    

typedef std::priority_queue<QueueObj, vector<QueueObj>, function<bool(const QueueObj&, const QueueObj&)>> PriorityQueue;

PriorityQueue initQueue(
  int nodeside_count, 
  NodesideId start_ns
);
  

/*
Called when new nodeside is discovered.
If path to the nodeside is the shortest found so far:
  update path_lengths and add new queue entry for it
*/
bool discover_nodeside(NodesideId new_nodeside, EnterDir enter_direction, int new_len, vector<vector<int>>& path_lengths, PriorityQueue& nodeside_queue); 


SdslArray2D dijkstra(bdsg::HashGraph& g, NodesideId start, NodesideId stop_ns = -1, int stop_dist = INF_INT, bool ball_ver = false); 

/*
notes:
 - lm_nodes should not be empty
*/
SdslArray2D get_lm2ns_dist_table(vector<NodeId>& lm_nodes, HashGraph& g);

void get_closest_lm_ind(SdslArray2D& dist_table, vector<int>& ind_vec, vector<int>& min_dist_vec, int nodeside_count);

/*
based off https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/msr-tr-2009-84.pdf [Chen et al. (2009)]
*/
Ball get_ball_contents(NodesideId ns, int dist_limit, HashGraph& g);


vector<Ball> find_balls(vector<int>& closest_lm_dist_vec, HashGraph& g, int min_ball_size = 0);  
 
typedef struct OracleInfo {
  vector<NodeId>& lm_nodes;
  vector<int>& closest_lm_inds;
  vector<int>& closest_lm_dists;
  vector<Ball>& balls;
  SdslArray2D& lm_sides_to_ns;
} OracleInfo;


/*
query algorithm from Chen et al. (2009)
with modifications for our purpose
*/

int oracle_query(NodesideId source, NodesideId target, OracleInfo& oracle, HashGraph& g);

}

#endif
