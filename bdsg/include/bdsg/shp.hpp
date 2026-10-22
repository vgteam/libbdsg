/*
SHP (Significant-path based Hub Pushing) hub ordering for directed weighted
graphs, from Li, U, Yiu, Kou, "An Experimental Study on Hub Labeling based
Shortest Path Algorithms", PVLDB 11(4), 2017. This follows the authors'
reference code (directed_weighted_sigpath in src/coverage_ordering.h of
github.com/yb47438/An-Experimental-Study-on-Hub-Labeling-based-Shortest-Path-Algorithms),
which is more specific than the paper.

SHP picks each next hub from the pruned shortest path trees produced by pushing
the previous hub, so ordering and pruned hub pushing happen together, and the
labels built along the way are returned with the order.

Header-only and generic over any Boost BidirectionalGraph + VertexListGraph
with a vertex_index map and non-negative edge weights. Boost is only used for
graph access.
*/
#ifndef BDSG_SHP_HPP_INCLUDED
#define BDSG_SHP_HPP_INCLUDED

#include <boost/graph/graph_traits.hpp> // BINDER_IGNORE
#include <boost/graph/properties.hpp> // BINDER_IGNORE
#include <boost/property_map/property_map.hpp> // BINDER_IGNORE
#include <boost/range/iterator_range.hpp> // BINDER_IGNORE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <random>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace bdsg {
namespace shp {

/// Infinite distance for a weight type.
template <typename Weight> constexpr Weight infinity() {
  if constexpr (std::numeric_limits<Weight>::has_infinity) {
    return std::numeric_limits<Weight>::infinity();
  } else {
    return std::numeric_limits<Weight>::max();
  }
}

/// Add two distances, saturating at infinity.
template <typename Weight> Weight saturating_add(Weight a, Weight b) {
  constexpr Weight inf = infinity<Weight>();
  if (a == inf || b == inf) {
    return inf;
  }
  if constexpr (std::is_integral_v<Weight>) {
    if (a > inf - b) {
      return inf;
    }
  }
  return a + b;
}

/**
 * Options for shp_order().
 *
 * By default SHP ranks vertices by the paper's directed degree,
 * indeg * outdeg. Alternatively it can rank them by how many descendants they
 * have in shortest path trees from a sample of random roots, as in Delling et
 * al., "Robust Exact Distance Queries on Massive Networks" (2014). That can
 * tell vertices apart in graphs where almost every vertex has the same degree.
 *
 * As in Delling et al., the trees are kept up to date: when a vertex becomes
 * a hub, its subtrees are removed from every tree, since the hub covers the
 * shortest paths through it. As the trees shrink they say less, so new trees
 * are grown, over the vertices that are not hubs yet, whenever the trees'
 * total size falls below refresh_fraction of their starting total, scaled by
 * the fraction of vertices that are not hubs yet.
 */
struct ShpOptions {
  enum class Score { Degree, SampledTrees };
  /// Score used to pick the first hub and to sort the fallback list.
  Score initial_score = Score::Degree;
  /// Score used to pick a hub from a significant path.
  Score path_score = Score::Degree;
  /// Number of random roots for sampled trees, in each direction, to start
  /// with. All vertices are used as roots if there are no more than this
  /// many. At most 4 * samples trees are kept at once.
  size_t samples = 32;
  /// Grow new trees when the trees' total size falls below this fraction of
  /// the starting total (scaled by the fraction of vertices not yet hubs).
  double refresh_fraction = 0.5;
  /// Seed for root sampling and for breaking ties in the fallback list.
  std::uint64_t seed = 0;
};

/**
 * Hub order and hub labels produced by shp_order() or pruned_hub_labels().
 *
 * All per-vertex vectors are indexed by the graph's vertex_index. Labels
 * store hubs by rank, and are appended in increasing rank order, so each list
 * is sorted for merge-join queries.
 */
template <class Graph, class WeightMap> struct ShpResult {
  using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
  using Weight = typename boost::property_traits<WeightMap>::value_type;
  using Label = std::vector<std::pair<std::uint32_t, Weight>>;

  /// order[i] is the vertex with rank i (rank 0 is the most important hub).
  std::vector<Vertex> order;
  /// rank[v] is the rank of v.
  std::vector<std::uint32_t> rank;
  /// L_out[v] holds (rank(h), dist(v->h)). Includes (rank(v), 0).
  std::vector<Label> L_out;
  /// L_in[v] holds (rank(h), dist(h->v)). Includes (rank(v), 0).
  std::vector<Label> L_in;
  /// Number of hubs after the first that SHP took from the fallback list
  /// because the significant paths were too short to pick from.
  size_t fallback_picks = 0;
  /// Number of sampled trees grown after the starting ones.
  size_t trees_added = 0;

  /// Average label size, (sum |L_out| + sum |L_in|) / (2n).
  double average_label_size() const {
    if (rank.empty()) {
      return 0;
    }
    size_t total = 0;
    for (size_t v = 0; v < rank.size(); v++) {
      total += L_out[v].size() + L_in[v].size();
    }
    return (double)total / (2.0 * rank.size());
  }

  /// Distance from s to t (vertex indexes) through the labels, or infinity.
  Weight query(size_t s, size_t t) const {
    Weight best = infinity<Weight>();
    auto a = L_out[s].begin();
    auto b = L_in[t].begin();
    while (a != L_out[s].end() && b != L_in[t].end()) {
      if (a->first < b->first) {
        ++a;
      } else if (b->first < a->first) {
        ++b;
      } else {
        best = std::min(best, saturating_add(a->second, b->second));
        ++a;
        ++b;
      }
    }
    return best;
  }
};

namespace detail {

enum class Direction { Forward, Backward };

/**
 * Shortest path trees from sampled roots, from which vertices' subtrees can be
 * removed, scoring each vertex by its live descendants.
 *
 * Trees in pangenome graphs can be very deep, so instead of walking up to
 * ancestors to update their counts, each tree is laid out in DFS order, where
 * a subtree is a range, and live vertices are counted with a Fenwick tree
 * over that order.
 */
class SampledTrees {
public:
  explicit SampledTrees(size_t n) : memberships(n), local(n, NONE) {}

  /// Add a tree, given its vertices in an order where each comes after its
  /// parent (such as Dijkstra settle order, root first), and each non-root
  /// vertex's parent.
  void add(Direction dir, const std::vector<size_t> &order,
           const std::vector<size_t> &parent) {
    std::uint32_t m = order.size();
    std::uint32_t t = trees.size();
    trees.emplace_back();
    Tree &tree = trees.back();
    tree.dir = dir;

    // Children in compressed form, by local index.
    for (std::uint32_t i = 0; i < m; i++) {
      local[order[i]] = i;
    }
    std::vector<std::uint32_t> child_start(m + 1, 0);
    for (std::uint32_t i = 1; i < m; i++) {
      child_start[local[parent[order[i]]] + 1]++;
    }
    for (std::uint32_t i = 0; i < m; i++) {
      child_start[i + 1] += child_start[i];
    }
    std::vector<std::uint32_t> children(m > 0 ? m - 1 : 0);
    std::vector<std::uint32_t> fill(child_start.begin(), child_start.end() - 1);
    for (std::uint32_t i = 1; i < m; i++) {
      children[fill[local[parent[order[i]]]]++] = i;
    }

    // DFS order times, without recursion.
    tree.by_time.resize(m);
    std::vector<std::uint32_t> tin(m);
    std::vector<std::pair<std::uint32_t, std::uint32_t>> stack;
    std::uint32_t time = 0;
    if (m > 0) {
      stack.emplace_back(0, child_start[0]);
      tin[0] = time;
      tree.by_time[time++] = order[0];
    }
    while (!stack.empty()) {
      auto &[i, next_child] = stack.back();
      if (next_child == child_start[i + 1]) {
        // All of i's subtree has times by now.
        memberships[order[i]].push_back({t, tin[i], time - 1});
        stack.pop_back();
        continue;
      }
      std::uint32_t c = children[next_child++];
      tin[c] = time;
      tree.by_time[time++] = order[c];
      stack.emplace_back(c, child_start[c]);
    }

    for (std::uint32_t i = 0; i < m; i++) {
      local[order[i]] = NONE;
    }

    // Everything starts live. Build the Fenwick tree for all ones in O(m).
    tree.fenwick.assign(m + 1, 0);
    for (std::uint32_t i = 1; i <= m; i++) {
      tree.fenwick[i] += 1;
      std::uint32_t up = i + (i & -i);
      if (up <= m) {
        tree.fenwick[up] += tree.fenwick[i];
      }
    }
    tree.next_live.resize(m + 1);
    for (std::uint32_t i = 0; i <= m; i++) {
      tree.next_live[i] = i;
    }
    tree.live = m;
    total_live += m;
    if (m > 0) {
      live_trees++;
    }
  }

  /// Remove v's subtree from every tree where v is live.
  void remove(size_t v) {
    // Dropping a tree edits v's memberships, so wait until after the loop.
    std::vector<std::uint32_t> emptied;
    for (auto &membership : memberships[v]) {
      Tree &tree = trees[membership.tree];
      if (tree.live == 0 || !is_live(tree, membership.tin)) {
        continue;
      }
      // Kill each live time in the range, skipping dead runs.
      for (std::uint32_t i = find_live(tree, membership.tin);
           i <= membership.tout; i = find_live(tree, i + 1)) {
        fenwick_add(tree, i, -1);
        tree.next_live[i] = i + 1;
        tree.live--;
        total_live--;
      }
      assert(fenwick_prefix(tree, tree.by_time.size()) == tree.live);
      if (tree.live == 0) {
        emptied.push_back(membership.tree);
      }
    }
    for (std::uint32_t t : emptied) {
      drop(t);
    }
  }

  /// Live descendants of v (not counting v) summed over live forward trees.
  std::uint64_t forward_score(size_t v) const {
    return score(v, Direction::Forward);
  }

  /// Live descendants of v (not counting v) summed over live backward trees.
  std::uint64_t backward_score(size_t v) const {
    return score(v, Direction::Backward);
  }

  /// Live descendants of v summed over all trees.
  std::uint64_t score(size_t v) const {
    return forward_score(v) + backward_score(v);
  }

  /// Total number of live vertices across all trees.
  std::uint64_t live_total() const { return total_live; }

  /// Number of trees that still have live vertices.
  size_t live_tree_count() const { return live_trees; }

private:
  static constexpr std::uint32_t NONE =
      std::numeric_limits<std::uint32_t>::max();

  struct Tree {
    Direction dir;
    /// Vertex at each DFS time.
    std::vector<size_t> by_time;
    /// Fenwick tree over DFS times, 1-based, holding 1 for live vertices.
    std::vector<std::uint32_t> fenwick;
    /// Union-find style pointer to the next time that might be live.
    std::vector<std::uint32_t> next_live;
    std::uint64_t live = 0;
  };

  /// A vertex's place in one tree: its DFS time, and the last DFS time in
  /// its subtree.
  struct Membership {
    std::uint32_t tree;
    std::uint32_t tin;
    std::uint32_t tout;
  };

  std::vector<Tree> trees;
  std::vector<std::vector<Membership>> memberships;
  // Scratch for add(): local index of each vertex in the tree being added.
  std::vector<std::uint32_t> local;
  std::uint64_t total_live = 0;
  size_t live_trees = 0;

  std::uint64_t score(size_t v, Direction dir) const {
    std::uint64_t total = 0;
    for (auto &membership : memberships[v]) {
      const Tree &tree = trees[membership.tree];
      if (tree.dir != dir || tree.live == 0 ||
          !is_live(tree, membership.tin)) {
        continue;
      }
      // Live subtree size, minus v itself.
      total += fenwick_prefix(tree, membership.tout + 1) -
               fenwick_prefix(tree, membership.tin) - 1;
    }
    return total;
  }

  static bool is_live(const Tree &tree, std::uint32_t time) {
    return tree.next_live[time] == time;
  }

  static std::uint32_t find_live(Tree &tree, std::uint32_t time) {
    while (tree.next_live[time] != time) {
      // Path halving.
      tree.next_live[time] = tree.next_live[tree.next_live[time]];
      time = tree.next_live[time];
    }
    return time;
  }

  static void fenwick_add(Tree &tree, std::uint32_t time, int delta) {
    for (std::uint32_t i = time + 1; i < tree.fenwick.size(); i += i & -i) {
      tree.fenwick[i] += delta;
    }
  }

  /// Number of live vertices with DFS time less than end.
  static std::uint64_t fenwick_prefix(const Tree &tree, std::uint32_t end) {
    std::uint64_t total = 0;
    for (std::uint32_t i = end; i > 0; i -= i & -i) {
      total += tree.fenwick[i];
    }
    return total;
  }

  /// Free a tree with no live vertices left, and forget memberships in it.
  void drop(std::uint32_t t) {
    Tree &tree = trees[t];
    for (size_t v : tree.by_time) {
      auto &list = memberships[v];
      list.erase(std::remove_if(list.begin(), list.end(),
                                [&](const Membership &membership) {
                                  return membership.tree == t;
                                }),
                 list.end());
    }
    tree = Tree{tree.dir, {}, {}, {}, 0};
    live_trees--;
  }
};

/**
 * Runs pruned hub pushing over a graph, filling in a ShpResult, and
 * implements the SHP next-hub choice.
 *
 * Scratch arrays are allocated once and only touched entries are reset
 * between searches.
 */
template <class Graph, class WeightMap> class HubPusher {
public:
  using Result = ShpResult<Graph, WeightMap>;
  using Vertex = typename Result::Vertex;
  using Weight = typename Result::Weight;
  using Label = typename Result::Label;

  HubPusher(const Graph &g, WeightMap w)
      : g(g), w(w), index(get(boost::vertex_index, g)), n(num_vertices(g)),
        verts(n), deg(n), indeg(n), outdeg(n), used(n, false),
        dist(n, infinity<Weight>()), parent(n, NONE), state(n, UNSEEN),
        cov(n, 0), tmp(n, infinity<Weight>()) {
    for (auto v : boost::make_iterator_range(vertices(g))) {
      size_t i = get(index, v);
      verts[i] = v;
      indeg[i] = in_degree(v, g);
      outdeg[i] = out_degree(v, g);
      deg[i] = indeg[i] * outdeg[i];
      if constexpr (std::is_signed_v<Weight> ||
                    std::is_floating_point_v<Weight>) {
        for (auto e : boost::make_iterator_range(out_edges(v, g))) {
          assert(get(w, e) >= 0);
        }
      }
    }
    result.rank.assign(n, NONE_RANK);
    result.L_out.resize(n);
    result.L_in.resize(n);
  }

  /// Run SHP: choose hubs by significant paths, pushing each one.
  Result run_shp(const ShpOptions &options) {
    if (n == 0) {
      return std::move(result);
    }
    std::mt19937_64 rng(options.seed);
    using Score = ShpOptions::Score;
    if (options.initial_score == Score::SampledTrees ||
        options.path_score == Score::SampledTrees) {
      start_trees(options, rng);
    }
    trees_for_paths = options.path_score == Score::SampledTrees;
    if (options.initial_score == Score::SampledTrees) {
      make_fallback_heap(rng);
    } else {
      make_fallback_list(rng);
    }
    size_t h = next_fallback();
    SignificantPath forward_path, backward_path;
    for (std::uint32_t r = 0; r < n; r++) {
      push_hub(h, r, &forward_path, &backward_path);
      if (r + 1 == n) {
        break;
      }
      h = choose_next(h, forward_path, backward_path);
      assert(!used[h]);
    }
    return std::move(result);
  }

  /// Push hubs in a fixed order.
  Result run_fixed(const std::vector<Vertex> &order) {
    assert(order.size() == n);
    for (std::uint32_t r = 0; r < n; r++) {
      size_t h = get(index, order[r]);
      assert(!used[h]);
      push_hub(h, r);
    }
    return std::move(result);
  }

private:
  static constexpr size_t NONE = std::numeric_limits<size_t>::max();
  static constexpr std::uint32_t NONE_RANK =
      std::numeric_limits<std::uint32_t>::max();

  /// Settle states for vertices in a search.
  enum SettleState : std::uint8_t { UNSEEN, LABELED, PRUNED_LEAF };

  /// Vertices on a significant path (excluding the root), with their
  /// coverage.
  struct SignificantPath {
    std::vector<size_t> verts;
    std::vector<std::uint64_t> cov;
  };

  const Graph &g;
  WeightMap w;
  typename boost::property_map<Graph, boost::vertex_index_t>::const_type index;
  size_t n;
  Result result;

  std::vector<Vertex> verts;
  std::vector<std::uint64_t> deg;
  std::vector<std::uint64_t> indeg;
  std::vector<std::uint64_t> outdeg;
  std::vector<bool> used;

  // Per-search scratch, reset through touched.
  std::vector<Weight> dist;
  std::vector<size_t> parent;
  std::vector<SettleState> state;
  std::vector<std::uint64_t> cov;
  std::vector<size_t> touched;
  std::vector<size_t> settle_order;
  std::vector<std::pair<Weight, size_t>> heap;
  // Dense distances by hub rank from the root's opposite label.
  std::vector<Weight> tmp;

  // Sampled shortest path trees, if any score uses them, with the shuffled
  // list of vertices to take roots from and a pointer into it.
  std::unique_ptr<SampledTrees> trees;
  std::vector<size_t> roots;
  size_t next_root = 0;
  std::uint64_t initial_tree_total = 0;
  size_t max_trees = 0;
  double refresh_fraction = 0;
  size_t hubs_pushed = 0;
  // Whether hubs are picked from significant paths by sampled tree scores
  // instead of degree.
  bool trees_for_paths = false;
  // Whether the fallback list is fallback_heap instead of fallback.
  bool fallback_by_trees = false;

  // Max-heap of (score when pushed, tiebreak key, vertex), used for the
  // fallback list when it is sorted by sampled tree scores, which change.
  std::vector<std::tuple<std::uint64_t, size_t, size_t>> fallback_heap;

  // Degree-sorted fallback list and a pointer into it.
  std::vector<size_t> fallback;
  size_t fallback_pos = 0;

  /// Give h rank r and push it forward then backward. If the path pointers
  /// are set, also take the significant path of each tree, while that tree is
  /// still in the scratch arrays.
  void push_hub(size_t h, std::uint32_t r,
                SignificantPath *forward_path = nullptr,
                SignificantPath *backward_path = nullptr) {
    result.rank[h] = r;
    result.order.push_back(verts[h]);
    used[h] = true;
    // Self entries guard against zero-weight cycles pruning the root.
    result.L_out[h].emplace_back(r, 0);
    result.L_in[h].emplace_back(r, 0);
    search(h, Direction::Forward);
    if (forward_path) {
      compute_coverage(h);
      *forward_path = walk(h, Direction::Forward);
    }
    search(h, Direction::Backward);
    if (backward_path) {
      compute_coverage(h);
      *backward_path = walk(h, Direction::Backward);
    }
    hubs_pushed++;
    if (trees) {
      // h now covers the shortest paths through it, so take it and its
      // descendants out of the sampled trees, and grow more trees if they
      // have shrunk too much. This reuses the scratch arrays, so it has to
      // come after the walks.
      trees->remove(h);
      refresh_trees();
    }
  }

  void reset() {
    for (size_t v : touched) {
      dist[v] = infinity<Weight>();
      parent[v] = NONE;
      state[v] = UNSEEN;
      cov[v] = 0;
    }
    touched.clear();
    settle_order.clear();
    heap.clear();
  }

  void heap_push(Weight d, size_t v) {
    heap.emplace_back(d, v);
    std::push_heap(heap.begin(), heap.end(), std::greater<>());
  }

  /// Is d already covered by the labels, given tmp loaded for the root?
  bool covered(const Label &label, Weight d) const {
    for (auto &[r, d2] : label) {
      if (tmp[r] != infinity<Weight>() && saturating_add(tmp[r], d2) <= d) {
        return true;
      }
    }
    return false;
  }

  /// Relax the edge from u (settled at d) to t with weight we.
  void relax(size_t u, Weight d, size_t t, Weight we) {
    if (t == u || used[t]) {
      // Ignore self-loops; used vertices would be skipped on settle anyway.
      return;
    }
    Weight nd = saturating_add(d, we);
    if (nd < dist[t]) {
      if (dist[t] == infinity<Weight>()) {
        touched.push_back(t);
      }
      dist[t] = nd;
      parent[t] = u;
      heap_push(nd, t);
    }
  }

  /// Pruned Dijkstra from h. Forward follows out-edges and writes L_in;
  /// backward follows in-edges and writes L_out.
  void search(size_t h, Direction dir) {
    reset();
    bool forward = dir == Direction::Forward;
    std::vector<Label> &labels = forward ? result.L_in : result.L_out;
    const Label &root_label = forward ? result.L_out[h] : result.L_in[h];
    std::uint32_t hub_rank = result.rank[h];

    for (auto &[r, d] : root_label) {
      tmp[r] = d;
    }

    dist[h] = 0;
    touched.push_back(h);
    heap_push(0, h);
    while (!heap.empty()) {
      std::pop_heap(heap.begin(), heap.end(), std::greater<>());
      auto [d, v] = heap.back();
      heap.pop_back();
      if (state[v] != UNSEEN || d > dist[v]) {
        // Stale heap entry.
        continue;
      }
      settle_order.push_back(v);
      if (v != h) {
        // Used vertices are never relaxed into, so v is not used here.
        // Prune on settle, with <=.
        if (covered(labels[v], d)) {
          state[v] = PRUNED_LEAF;
          continue;
        }
        labels[v].emplace_back(hub_rank, d);
      }
      state[v] = LABELED;

      if (forward) {
        for (auto e : boost::make_iterator_range(out_edges(verts[v], g))) {
          relax(v, d, get(index, target(e, g)), get(w, e));
        }
      } else {
        for (auto e : boost::make_iterator_range(in_edges(verts[v], g))) {
          relax(v, d, get(index, source(e, g)), get(w, e));
        }
      }
    }

    for (auto &[r, d] : root_label) {
      tmp[r] = infinity<Weight>();
    }
  }

  /// cov(v) = number of labeled vertices in v's subtree. Pruned leaves get 0.
  void compute_coverage(size_t h) {
    for (size_t v : settle_order) {
      cov[v] = state[v] == LABELED ? 1 : 0;
    }
    for (auto it = settle_order.rbegin(); it != settle_order.rend(); ++it) {
      if (*it != h) {
        cov[parent[*it]] += cov[*it];
      }
    }
  }

  /// Walk from the root of the current tree to the child with the most
  /// coverage, keeping the first child seen on ties, until reaching a vertex
  /// with no children. The root is not included.
  SignificantPath walk(size_t h, Direction dir) const {
    SignificantPath path;
    size_t cur = h;
    while (true) {
      size_t best = NONE;
      auto consider = [&](size_t c) {
        if (c != cur && state[c] != UNSEEN && parent[c] == cur &&
            (best == NONE || cov[c] > cov[best])) {
          best = c;
        }
      };
      if (dir == Direction::Forward) {
        for (auto e : boost::make_iterator_range(out_edges(verts[cur], g))) {
          consider(get(index, target(e, g)));
        }
      } else {
        for (auto e : boost::make_iterator_range(in_edges(verts[cur], g))) {
          consider(get(index, source(e, g)));
        }
      }
      if (best == NONE) {
        return path;
      }
      path.verts.push_back(best);
      path.cov.push_back(cov[best]);
      cur = best;
    }
  }

  /// Unpruned Dijkstra from root over the vertices that are not hubs yet,
  /// adding the resulting shortest path tree to the sampled trees.
  void grow_tree(size_t root, Direction dir) {
    reset();
    dist[root] = 0;
    touched.push_back(root);
    heap_push(0, root);
    while (!heap.empty()) {
      std::pop_heap(heap.begin(), heap.end(), std::greater<>());
      auto [d, v] = heap.back();
      heap.pop_back();
      if (state[v] != UNSEEN || d > dist[v]) {
        continue;
      }
      settle_order.push_back(v);
      state[v] = LABELED;
      // relax() skips hubs, whose shortest paths are already covered.
      if (dir == Direction::Forward) {
        for (auto e : boost::make_iterator_range(out_edges(verts[v], g))) {
          relax(v, d, get(index, target(e, g)), get(w, e));
        }
      } else {
        for (auto e : boost::make_iterator_range(in_edges(verts[v], g))) {
          relax(v, d, get(index, source(e, g)), get(w, e));
        }
      }
    }
    trees->add(dir, settle_order, parent);
  }

  /// Grow a forward and a backward tree from the next unused root, if any.
  bool grow_tree_pair() {
    while (next_root < roots.size() && used[roots[next_root]]) {
      next_root++;
    }
    if (next_root == roots.size()) {
      return false;
    }
    size_t root = roots[next_root++];
    grow_tree(root, Direction::Forward);
    grow_tree(root, Direction::Backward);
    return true;
  }

  /// Grow the starting sampled trees, from up to samples random roots.
  void start_trees(const ShpOptions &options, std::mt19937_64 &rng) {
    trees = std::make_unique<SampledTrees>(n);
    roots.resize(n);
    for (size_t i = 0; i < n; i++) {
      roots[i] = i;
    }
    std::shuffle(roots.begin(), roots.end(), rng);
    max_trees = 4 * options.samples;
    refresh_fraction = options.refresh_fraction;
    for (size_t i = 0; i < std::min(options.samples, n); i++) {
      grow_tree_pair();
    }
    initial_tree_total = trees->live_total();
  }

  /// Grow new trees while the sampled trees' total size is below
  /// refresh_fraction of where it started, scaled down by the fraction of
  /// vertices that are hubs already.
  void refresh_trees() {
    double target = refresh_fraction * initial_tree_total *
                    (double)(n - hubs_pushed) / n;
    while (trees->live_total() < target &&
           trees->live_tree_count() + 2 <= max_trees && grow_tree_pair()) {
      result.trees_added += 2;
    }
  }

  /// Sort vertices by degree descending, with ties broken by a seeded
  /// shuffle.
  void make_fallback_list(std::mt19937_64 &rng) {
    fallback.resize(n);
    for (size_t i = 0; i < n; i++) {
      fallback[i] = i;
    }
    std::shuffle(fallback.begin(), fallback.end(), rng);
    std::stable_sort(fallback.begin(), fallback.end(),
                     [&](size_t a, size_t b) { return deg[a] > deg[b]; });
    fallback_pos = 0;
  }

  /// Put all vertices in a max-heap by sampled tree score, with ties broken
  /// by a seeded shuffle.
  void make_fallback_heap(std::mt19937_64 &rng) {
    std::vector<size_t> shuffled(n);
    for (size_t i = 0; i < n; i++) {
      shuffled[i] = i;
    }
    std::shuffle(shuffled.begin(), shuffled.end(), rng);
    fallback_heap.clear();
    for (size_t pos = 0; pos < n; pos++) {
      // Earlier in the shuffle wins ties.
      fallback_heap.emplace_back(trees->score(shuffled[pos]), n - pos,
                                 shuffled[pos]);
    }
    std::make_heap(fallback_heap.begin(), fallback_heap.end());
    fallback_by_trees = true;
  }

  size_t next_fallback() {
    if (!fallback_by_trees) {
      while (used[fallback[fallback_pos]]) {
        fallback_pos++;
      }
      return fallback[fallback_pos];
    }
    // Scores only go down as trees shrink, so an entry is right if its score
    // is still current. Otherwise put it back with its current score.
    while (true) {
      auto [score, key, v] = fallback_heap.front();
      if (!used[v]) {
        std::uint64_t current = trees->score(v);
        if (current == score) {
          return v;
        }
        std::pop_heap(fallback_heap.begin(), fallback_heap.end());
        fallback_heap.back() = {current, key, v};
        std::push_heap(fallback_heap.begin(), fallback_heap.end());
        continue;
      }
      std::pop_heap(fallback_heap.begin(), fallback_heap.end());
      fallback_heap.pop_back();
    }
  }

  /// Choose the next hub after pushing h, from the significant paths of its
  /// forward and backward trees.
  ///
  /// Choosing between the paths and between the road-like and complex
  /// network rules always uses degree, since the road-like threshold is
  /// calibrated for degree. Only the pick from the chosen path uses the
  /// configured path score.
  size_t choose_next(size_t h, const SignificantPath &forward_path,
                     const SignificantPath &backward_path) {
    auto max_deg = [&](const SignificantPath &p) {
      std::uint64_t m = deg[h];
      for (size_t v : p.verts) {
        m = std::max(m, deg[v]);
      }
      return m;
    };
    std::uint64_t maxdeg_f = max_deg(forward_path);
    std::uint64_t maxdeg_b = max_deg(backward_path);
    std::uint64_t maxdeg = std::max(maxdeg_f, maxdeg_b);

    std::vector<size_t> path;
    std::vector<std::uint64_t> path_cov;
    bool backward = maxdeg_f < maxdeg_b;
    if (backward) {
      // Backward path, leaf first.
      path.assign(backward_path.verts.rbegin(), backward_path.verts.rend());
      path_cov.assign(backward_path.cov.rbegin(), backward_path.cov.rend());
    } else {
      path = forward_path.verts;
      path_cov = forward_path.cov;
    }
    // One-sided score: out-degree on backward paths and in-degree on forward
    // ones. With sampled trees, descendants in forward trees stand in for
    // out-degree, and descendants in backward trees for in-degree.
    auto onedeg = [&](size_t v) -> std::uint64_t {
      if (trees_for_paths) {
        return backward ? trees->forward_score(v) : trees->backward_score(v);
      }
      return backward ? outdeg[v] : indeg[v];
    };

    if (path.size() <= 2) {
      result.fallback_picks++;
      return next_fallback();
    }

    if ((double)maxdeg / path.size() < 2.0 && path.size() > 3) {
      // Road-like: pick the vertex where coverage changes most along the
      // path, weighted by its one-sided degree. The last path vertex is never
      // chosen.
      //
      // On a reversed (backward) path, path[j+1] is the parent of path[j], so
      // the coverage drop into the parent is credited to the child. This
      // matches the reference code and is kept on purpose.
      size_t best = 0;
      std::uint64_t best_score = 0;
      for (size_t j = 0; j + 1 < path.size(); j++) {
        std::uint64_t a = path_cov[j + 1], b = path_cov[j];
        std::uint64_t score = onedeg(path[j]) * (a > b ? a - b : b - a);
        if (j == 0 || score > best_score) {
          best = j;
          best_score = score;
        }
      }
      return path[best];
    }

    // Complex network: pick the highest-scoring (by default, highest-degree)
    // vertex on the path.
    auto score = [&](size_t v) -> std::uint64_t {
      return trees_for_paths ? trees->score(v) : deg[v];
    };
    size_t best = 0;
    std::uint64_t best_score = score(path[0]);
    for (size_t j = 1; j < path.size(); j++) {
      std::uint64_t s = score(path[j]);
      if (s > best_score) {
        best = j;
        best_score = s;
      }
    }
    return path[best];
  }
};

} // namespace detail

/**
 * Compute an SHP hub order for g with edge weights w, along with the hub
 * labels built by pruned hub pushing in that order.
 *
 * g must model BidirectionalGraph and VertexListGraph and have a
 * vertex_index map. Weights must be non-negative. Self-loops are ignored in
 * searches, and parallel edges act as their minimum weight. See ShpOptions
 * for how vertices are scored.
 */
template <class Graph, class WeightMap>
ShpResult<Graph, WeightMap> shp_order(const Graph &g, WeightMap w,
                                      const ShpOptions &options) {
  return detail::HubPusher<Graph, WeightMap>(g, w).run_shp(options);
}

/**
 * Compute an SHP hub order for g with edge weights w, scoring vertices by
 * degree, with ties in the initial degree ordering broken by an RNG seeded
 * with seed.
 */
template <class Graph, class WeightMap>
ShpResult<Graph, WeightMap> shp_order(const Graph &g, WeightMap w,
                                      std::uint64_t seed = 0) {
  ShpOptions options;
  options.seed = seed;
  return shp_order(g, w, options);
}

/**
 * Build hub labels for g by pruned hub pushing in the given fixed order, the
 * same way shp_order() does.
 */
template <class Graph, class WeightMap>
ShpResult<Graph, WeightMap> pruned_hub_labels(
    const Graph &g, WeightMap w,
    const std::vector<typename boost::graph_traits<Graph>::vertex_descriptor>
        &order) {
  return detail::HubPusher<Graph, WeightMap>(g, w).run_fixed(order);
}

} // namespace shp
} // namespace bdsg

#endif
