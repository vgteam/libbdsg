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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
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
  Result run_shp(std::uint64_t seed) {
    if (n == 0) {
      return std::move(result);
    }
    make_fallback_list(seed);
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

  /// Sort vertices by deg descending, with ties broken by a seeded shuffle.
  void make_fallback_list(std::uint64_t seed) {
    fallback.resize(n);
    for (size_t i = 0; i < n; i++) {
      fallback[i] = i;
    }
    std::mt19937_64 rng(seed);
    std::shuffle(fallback.begin(), fallback.end(), rng);
    std::stable_sort(fallback.begin(), fallback.end(),
                     [&](size_t a, size_t b) { return deg[a] > deg[b]; });
    fallback_pos = 0;
  }

  size_t next_fallback() {
    while (used[fallback[fallback_pos]]) {
      fallback_pos++;
    }
    return fallback[fallback_pos];
  }

  /// Choose the next hub after pushing h, from the significant paths of its
  /// forward and backward trees.
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
    const std::vector<std::uint64_t> *onedeg;
    if (maxdeg_f < maxdeg_b) {
      // Backward path, leaf first.
      path.assign(backward_path.verts.rbegin(), backward_path.verts.rend());
      path_cov.assign(backward_path.cov.rbegin(), backward_path.cov.rend());
      onedeg = &outdeg;
    } else {
      path = forward_path.verts;
      path_cov = forward_path.cov;
      onedeg = &indeg;
    }

    if (path.size() <= 2) {
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
        std::uint64_t score = (*onedeg)[path[j]] * (a > b ? a - b : b - a);
        if (j == 0 || score > best_score) {
          best = j;
          best_score = score;
        }
      }
      return path[best];
    }

    // Complex network: pick the highest-degree vertex on the path.
    size_t best = 0;
    for (size_t j = 1; j < path.size(); j++) {
      if (deg[path[j]] > deg[path[best]]) {
        best = j;
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
 * searches, and parallel edges act as their minimum weight. Ties in the
 * initial degree ordering are broken by an RNG seeded with seed.
 */
template <class Graph, class WeightMap>
ShpResult<Graph, WeightMap> shp_order(const Graph &g, WeightMap w,
                                      std::uint64_t seed = 0) {
  return detail::HubPusher<Graph, WeightMap>(g, w).run_shp(seed);
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
