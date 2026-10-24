#include "bdsg/phl.hpp" // BINDER_IGNORE because phl.hpp is C++20 + Boost and is not bound

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <queue>
#include <span>
#include <tuple>
#include <unordered_set>

// #define debug_phl

namespace bdsg {
namespace phl {

namespace {

/// The distance when no walk is known.
constexpr uint64_t UNREACHED = std::numeric_limits<uint64_t>::max();

/// The tree index of a missing parent or child.
constexpr size_t NO_VERTEX = std::numeric_limits<size_t>::max();

/// One of a vertex's two labels. The Out label has distances from the vertex
/// to highway vertices, and the In label has distances from highway vertices
/// to it.
enum class Side { Out, In };

/// Get the label that a search in direction dir fills. A Forward search from
/// a highway vertex finds distances from it, so it fills In labels.
Side side_filled_by(Direction dir) {
  return dir == Direction::Forward ? Side::In : Side::Out;
}

/**
 * Get the shortest distance through one highway that two label groups give
 *
 * The groups hold entries in increasing position, with values biased as
 * HighwayLabeling describes. An out entry pairs only with in entries at its
 * own position or later, since a walk can only go forward along a highway.
 * For each in entry, we keep the best out entry at or before it.
 *
 * Group can be any type with size(), and with operator[] giving an entry
 * with position and value members.
 *
 * @param out The out label group of the source.
 * @param in The in label group of the destination.
 * @param length The total length of the highway.
 * @returns The shortest distance, or UNREACHED if no pair is in order.
 */
template <typename Group>
uint64_t shortest_through_highway(const Group &out, const Group &in,
                                  uint64_t length) {
  uint64_t best = UNREACHED;
  uint64_t best_out = UNREACHED;
  size_t next_out = 0;
  for (size_t i = 0; i < in.size(); i++) {
    auto in_entry = in[i];
    for (; next_out < out.size() && out[next_out].position <= in_entry.position;
         next_out++) {
      best_out = std::min(best_out, out[next_out].value);
    }
    if (best_out != UNREACHED) {
      best = std::min(best, best_out + in_entry.value - length);
    }
  }
  return best;
}

/// A StoredEntry is one label entry read back from label slots.
struct StoredEntry {
  uint64_t position;
  uint64_t value;
};

/// A StoredGroup is a view of the entries of one group in label slots, which
/// alternate positions and values.
struct StoredGroup {
  const uint64_t *slots;
  size_t count;

  size_t size() const { return count; }
  StoredEntry operator[](size_t i) const {
    return {slots[2 * i], slots[2 * i + 1]};
  }
};

/// An Arc is one edge of an overlay, seen from one end. It holds the vertex at
/// the other end, and the weight of the edge plus the seqlen of the vertex the
/// edge enters.
struct Arc {
  NODE_UINT neighbor;
  uint64_t weight;
};

/**
 * An adjacency table of the original edges of a CHOverlay
 *
 * An ArcTable lists the out-edges and in-edges of each vertex as arcs. Each
 * arc includes the seqlen of the vertex its edge enters, so the length of a
 * walk counts the seqlen of every vertex on it but the first.
 */
class ArcTable {
public:
  explicit ArcTable(const CHOverlay &ov);

  size_t vertex_count() const { return seqlens.size(); }

  /// Get the arcs a search in direction dir follows from v: v's out-edges
  /// for Forward, and its in-edges for Backward.
  std::span<const Arc> arcs(NODE_UINT v, Direction dir) const {
    const ArcList &list = lists[(size_t)dir];
    return std::span<const Arc>(list.arcs.data() + list.starts[v],
                                list.starts[v + 1] - list.starts[v]);
  }

  DIST_UINT seqlen(NODE_UINT v) const { return seqlens[v]; }

  /// Get v's in-degree times its out-degree, as SHP does.
  uint64_t degree(NODE_UINT v) const {
    return arcs(v, Direction::Forward).size() *
           arcs(v, Direction::Backward).size();
  }

private:
  /// An ArcList is the arcs of every vertex in one direction, with vertex
  /// v's arcs at [starts[v], starts[v + 1]).
  struct ArcList {
    std::vector<size_t> starts;
    std::vector<Arc> arcs;
  };

  /// The ArcList for each Direction.
  std::array<ArcList, 2> lists;
  std::vector<DIST_UINT> seqlens;
};

ArcTable::ArcTable(const CHOverlay &ov) {
  size_t n = num_vertices(ov);
  for (size_t v = 0; v < n; v++) {
    seqlens.push_back(ov[v].seqlen);
  }
  for (ArcList &list : lists) {
    list.starts.assign(n + 1, 0);
  }
  ArcList &forward = lists[(size_t)Direction::Forward];
  ArcList &backward = lists[(size_t)Direction::Backward];
  for (auto e : boost::make_iterator_range(edges(ov))) {
    if (ov[e].ori) {
      forward.starts[source(e, ov) + 1]++;
      backward.starts[target(e, ov) + 1]++;
    }
  }
  for (ArcList &list : lists) {
    for (size_t v = 0; v < n; v++) {
      list.starts[v + 1] += list.starts[v];
    }
    list.arcs.resize(list.starts[n]);
  }
  std::array<std::vector<size_t>, 2> next = {forward.starts, backward.starts};
  for (auto e : boost::make_iterator_range(edges(ov))) {
    if (ov[e].ori) {
      NODE_UINT from = source(e, ov);
      NODE_UINT to = target(e, ov);
      uint64_t weight = (uint64_t)ov[e].weight + seqlens[to];
      forward.arcs[next[0][from]++] = {to, weight};
      backward.arcs[next[1][to]++] = {from, weight};
    }
  }
}

/// A HighwayStop is one highway vertex, with its offset: the distance along
/// the highway's walk from the first highway vertex.
struct HighwayStop {
  NODE_UINT vertex;
  uint64_t offset;
};

/// A Highway is a walk cut for highway labeling. It is stored as its stops,
/// the walk vertices that vertices are labeled with, in walk order. The walk
/// never visits a vertex twice.
struct Highway {
  std::vector<HighwayStop> stops;

  uint64_t length() const { return stops.back().offset; }
};

/// A DraftEntry is one entry of a label under construction: a highway
/// number, a position along that highway, and a value biased as
/// HighwayLabeling describes.
struct DraftEntry {
  uint32_t highway;
  uint32_t position;
  uint64_t value;
};

/// Get the end of the group of draft label entries that starts at start: the
/// index of the first entry after it for another highway, or the label size.
size_t group_end(std::span<const DraftEntry> label, size_t start) {
  size_t end = start;
  while (end < label.size() && label[end].highway == label[start].highway) {
    end++;
  }
  return end;
}

/**
 * A highway labeling under construction
 *
 * A DraftLabeling gets highways one at a time, and entries only for the
 * newest highway. It answers distance queries from the entries so far.
 *
 * Each label keeps its entries in increasing highway number, and within a
 * highway in increasing position, as HighwayLabeling stores them.
 */
class DraftLabeling {
public:
  explicit DraftLabeling(size_t vertex_count) : labels(vertex_count) {}

  /// Add a highway of the given total length, and get its number.
  uint32_t add_highway(uint64_t length) {
    highway_lengths.push_back(length);
    return highway_lengths.size() - 1;
  }

  /// Add an entry for the newest highway to one of v's labels. Entries may
  /// come in any order of position.
  void add_entry(NODE_UINT v, Side side, uint32_t position, uint64_t value);

  /// Get the shortest distance from s to t that the entries so far give, or
  /// UNREACHED.
  uint64_t distance(NODE_UINT s, NODE_UINT t) const;

  const std::vector<DraftEntry> &label(NODE_UINT v, Side side) const {
    return side == Side::Out ? labels[v].out : labels[v].in;
  }

  const std::vector<uint64_t> &lengths() const { return highway_lengths; }

private:
  /// A VertexLabels is the two labels of one vertex.
  struct VertexLabels {
    std::vector<DraftEntry> out;
    std::vector<DraftEntry> in;
  };

  /// The labels of each vertex.
  std::vector<VertexLabels> labels;
  /// The total length of each highway, by number.
  std::vector<uint64_t> highway_lengths;
};

void DraftLabeling::add_entry(NODE_UINT v, Side side, uint32_t position,
                              uint64_t value) {
  std::vector<DraftEntry> &entries =
      side == Side::Out ? labels[v].out : labels[v].in;
  uint32_t highway = highway_lengths.size() - 1;
  // Out label searches add entries in decreasing position, so we insert
  // within the newest highway's group, which is last in the label.
  auto at = entries.end();
  while (at != entries.begin() && (at - 1)->highway == highway &&
         (at - 1)->position > position) {
    --at;
  }
  entries.insert(at, {highway, position, value});
}

uint64_t DraftLabeling::distance(NODE_UINT s, NODE_UINT t) const {
  std::span<const DraftEntry> out = labels[s].out;
  std::span<const DraftEntry> in = labels[t].in;
  uint64_t best = UNREACHED;
  size_t i = 0;
  size_t k = 0;
  while (i < out.size() && k < in.size()) {
    if (out[i].highway < in[k].highway) {
      i = group_end(out, i);
    } else if (out[i].highway > in[k].highway) {
      k = group_end(in, k);
    } else {
      size_t i_end = group_end(out, i);
      size_t k_end = group_end(in, k);
      best = std::min(best, shortest_through_highway(
                                out.subspan(i, i_end - i),
                                in.subspan(k, k_end - k),
                                highway_lengths[out[i].highway]));
      i = i_end;
      k = k_end;
    }
  }
  return best;
}

/**
 * A labeler that adds the entries for a new highway to a DraftLabeling
 *
 * A HighwayLabeler fills in labels with one Dijkstra search from all the
 * highway vertices at once, and out labels with one search into all of them,
 * as the authors' code does for its undirected labels. The queue key of an
 * item is the value its entry would get, so entries come out in order of
 * value.
 *
 * Searches stay off the vertices of earlier highways, as in pruned landmark
 * labeling. They do go through the new highway's own vertices. In a directed
 * graph the shortest walk from a highway vertex can go back to an earlier one
 * by edges off the highway, and that walk would be lost otherwise.
 *
 * A search prunes a vertex when the labels already give its distance. It also
 * skips an entry that an earlier entry for the same vertex beats. All entries
 * and all prunes stand for lengths of real walks, so queries can never come
 * out too short. Exactness rests on the paper's pruning argument, adapted to
 * directed highways, and the tests check it against Dijkstra.
 */
class HighwayLabeler {
public:
  /// Make a labeler that adds entries to draft, and keeps its searches off
  /// the vertices that on_highway marks.
  HighwayLabeler(const ArcTable &arcs, DraftLabeling &draft,
                 const std::vector<bool> &on_highway)
      : arcs(arcs), draft(draft), on_highway(on_highway),
        searched_by(arcs.vertex_count(), 0),
        last_position(arcs.vertex_count(), 0) {}

  /// Add highway to the draft and label vertices with it.
  /// @pre The stops of highway are not marked in on_highway yet.
  void label(const Highway &highway);

private:
  /// Fill the labels that searches in direction dir fill, for the newest
  /// highway.
  void search(const Highway &highway, Direction dir);

  /// Check whether v's last entry from the current search beats an entry at
  /// position. The last entry has no larger a value, since entries come out
  /// in order of value. If it is also no earlier in the search's direction
  /// along the highway, every query that could use the new entry does at
  /// least as well with the last one.
  bool beaten(NODE_UINT v, Direction dir, uint32_t position) const {
    if (searched_by[v] != search_number) {
      return false;
    }
    return dir == Direction::Forward ? last_position[v] >= position
                                     : last_position[v] <= position;
  }

  const ArcTable &arcs;
  DraftLabeling &draft;
  /// Whether each vertex is on an earlier highway.
  const std::vector<bool> &on_highway;

  /// The number of the current search, counting from 1.
  uint32_t search_number = 0;
  /// The number of the last search that gave each vertex an entry.
  std::vector<uint32_t> searched_by;
  /// The position of the last entry the current search gave each vertex,
  /// where searched_by says it gave one.
  std::vector<uint32_t> last_position;
};

void HighwayLabeler::label(const Highway &highway) {
  draft.add_highway(highway.length());
  search(highway, Direction::Forward);
  search(highway, Direction::Backward);
}

void HighwayLabeler::search(const Highway &highway, Direction dir) {
  search_number++;
  Side side = side_filled_by(dir);
  uint64_t length = highway.length();
  // Start a search at each stop, with the value of the stop's own entry.
  auto start_value = [&](uint32_t position) {
    uint64_t offset = highway.stops[position].offset;
    return dir == Direction::Forward ? offset : length - offset;
  };

  // Queue items are (value, tiebreak, vertex, position). Among equal values,
  // the entry furthest along the highway in the search's direction comes out
  // first, so that it beats the others. Otherwise ties would give a vertex an
  // entry at every highway position that reaches it by the same walk.
  uint32_t last = highway.stops.size() - 1;
  auto tiebreak = [&](uint32_t position) {
    return dir == Direction::Forward ? last - position : position;
  };
  using Item = std::tuple<uint64_t, uint32_t, NODE_UINT, uint32_t>;
  std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;
  for (uint32_t position = 0; position <= last; position++) {
    queue.emplace(start_value(position), tiebreak(position),
                  highway.stops[position].vertex, position);
  }
  while (!queue.empty()) {
    auto [value, ignored, v, position] = queue.top();
    queue.pop();
    if (beaten(v, dir, position)) {
      continue;
    }
    NODE_UINT start = highway.stops[position].vertex;
    if (v != start) {
      uint64_t distance = value - start_value(position);
      uint64_t known = dir == Direction::Forward ? draft.distance(start, v)
                                                 : draft.distance(v, start);
      if (known <= distance) {
        continue;
      }
    }
    draft.add_entry(v, side, position, value);
    searched_by[v] = search_number;
    last_position[v] = position;
    for (const Arc &arc : arcs.arcs(v, dir)) {
      if (!on_highway[arc.neighbor] &&
          !beaten(arc.neighbor, dir, position)) {
        queue.emplace(value + arc.weight, tiebreak(position), arc.neighbor,
                      position);
      }
    }
  }
}

/// A TreeVertex is one vertex of a CoverageTree. It holds its parent and
/// heaviest child as indexes into the tree, its distance from the root, and its
/// coverage.
struct TreeVertex {
  NODE_UINT vertex;
  size_t parent;
  uint64_t distance;
  uint64_t coverage;
  size_t heaviest_child;
};

/**
 * A pruned shortest path tree from one root, in one direction
 *
 * A vertex of a CoverageTree is uncovered if it is not the root and the labels
 * cannot give its distance from the root yet. The coverage of a vertex is the
 * number of uncovered vertices in its subtree. The search that grew the tree
 * did not go past covered vertices.
 *
 * The tree keeps its vertices in the order the search reached them, root
 * first, so each comes after its parent.
 */
class CoverageTree {
public:
  explicit CoverageTree(std::vector<TreeVertex> vertices)
      : vertices(std::move(vertices)) {}

  /// Get the number of uncovered vertices in the tree.
  uint64_t coverage() const { return vertices.front().coverage; }

  /// Get the heaviest path, root first. It starts at the root and goes, at
  /// each vertex, to the child with the most coverage, until no child has
  /// any.
  std::vector<TreeVertex> heaviest_path() const;

  /// Get the branches off the heaviest path, with their coverage. A branch is
  /// a vertex with coverage whose parent is on the heaviest path and which is
  /// off it.
  std::vector<std::pair<NODE_UINT, uint64_t>> branches() const;

private:
  std::vector<TreeVertex> vertices;
};

std::vector<TreeVertex> CoverageTree::heaviest_path() const {
  std::vector<TreeVertex> path;
  for (size_t x = 0; x != NO_VERTEX; x = vertices[x].heaviest_child) {
    path.push_back(vertices[x]);
  }
  return path;
}

std::vector<std::pair<NODE_UINT, uint64_t>> CoverageTree::branches() const {
  std::vector<bool> on_path(vertices.size(), false);
  for (size_t x = 0; x != NO_VERTEX; x = vertices[x].heaviest_child) {
    on_path[x] = true;
  }
  std::vector<std::pair<NODE_UINT, uint64_t>> found;
  for (size_t x = 1; x < vertices.size(); x++) {
    if (!on_path[x] && on_path[vertices[x].parent] &&
        vertices[x].coverage > 0) {
      found.emplace_back(vertices[x].vertex, vertices[x].coverage);
    }
  }
  return found;
}

/**
 * Get the highway through a root
 *
 * The walk is the heaviest path of the Backward tree into the root, reversed,
 * then the heaviest path of the Forward tree out of it. The Forward part ends
 * before the first vertex the Backward part visits, so the walk never visits
 * a vertex twice.
 *
 * As in the authors' code, a walk vertex is a stop only if something branches
 * off it. That is, its successor away from the root has at most
 * stop_fraction of its coverage. The root and both ends are always stops.
 */
Highway highway_through(const CoverageTree &into, const CoverageTree &out_of,
                        double stop_fraction) {
  std::vector<TreeVertex> before = into.heaviest_path();
  std::vector<TreeVertex> after = out_of.heaviest_path();
  std::unordered_set<NODE_UINT> visited;
  for (const TreeVertex &x : before) {
    visited.insert(x.vertex);
  }
  size_t after_end = 1;
  while (after_end < after.size() && !visited.count(after[after_end].vertex)) {
    after_end++;
  }
  after.resize(after_end);

  auto is_stop = [&](const std::vector<TreeVertex> &path, size_t i) {
    return i == 0 || i + 1 == path.size() ||
           path[i + 1].coverage <= stop_fraction * path[i].coverage;
  };
  Highway highway;
  uint64_t root_offset = before.back().distance;
  for (size_t i = before.size(); i-- > 0;) {
    if (is_stop(before, i)) {
      highway.stops.push_back(
          {before[i].vertex, root_offset - before[i].distance});
    }
  }
  for (size_t i = 1; i < after.size(); i++) {
    if (is_stop(after, i)) {
      highway.stops.push_back(
          {after[i].vertex, root_offset + after[i].distance});
    }
  }
  return highway;
}

/**
 * A scout that grows CoverageTrees from roots
 *
 * A HighwayScout grows its trees through vertices not on highways yet, and
 * asks a DraftLabeling which vertices are covered.
 */
class HighwayScout {
public:
  HighwayScout(const ArcTable &arcs, const DraftLabeling &draft,
               const std::vector<bool> &on_highway)
      : arcs(arcs), draft(draft), on_highway(on_highway),
        best_distance(arcs.vertex_count(), UNREACHED),
        tree_index(arcs.vertex_count(), NO_VERTEX) {}

  /// Grow the CoverageTree from root in direction dir.
  /// @pre root is not on a highway.
  CoverageTree explore(NODE_UINT root, Direction dir);

private:
  const ArcTable &arcs;
  const DraftLabeling &draft;
  /// Whether each vertex is on a highway.
  const std::vector<bool> &on_highway;

  /// The best distance from the root found so far for each vertex, during a
  /// search. It is UNREACHED between searches.
  std::vector<uint64_t> best_distance;
  /// The index of each vertex in the tree being grown, during a search. It is
  /// NO_VERTEX between searches.
  std::vector<size_t> tree_index;
};

CoverageTree HighwayScout::explore(NODE_UINT root, Direction dir) {
  std::vector<TreeVertex> tree;
  std::vector<NODE_UINT> touched;

  // Queue items are (distance, vertex, parent's tree index).
  using Item = std::tuple<uint64_t, NODE_UINT, size_t>;
  std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;
  best_distance[root] = 0;
  touched.push_back(root);
  queue.emplace(0, root, NO_VERTEX);
  while (!queue.empty()) {
    auto [distance, v, parent] = queue.top();
    queue.pop();
    if (tree_index[v] != NO_VERTEX || distance > best_distance[v]) {
      continue;
    }
    tree_index[v] = tree.size();
    // The root counts as covered, since it is about to go on a highway.
    bool covered =
        v == root || (dir == Direction::Forward ? draft.distance(root, v)
                                                : draft.distance(v, root)) <=
                         distance;
    tree.push_back({v, parent, distance, covered ? 0u : 1u, NO_VERTEX});
    if (covered && v != root) {
      continue;
    }
    for (const Arc &arc : arcs.arcs(v, dir)) {
      NODE_UINT u = arc.neighbor;
      if (on_highway[u] || tree_index[u] != NO_VERTEX) {
        continue;
      }
      if (distance + arc.weight < best_distance[u]) {
        if (best_distance[u] == UNREACHED) {
          touched.push_back(u);
        }
        best_distance[u] = distance + arc.weight;
        queue.emplace(best_distance[u], u, tree_index[v]);
      }
    }
  }
  for (NODE_UINT v : touched) {
    best_distance[v] = UNREACHED;
    tree_index[v] = NO_VERTEX;
  }

  // Children come after their parents, so going backward each vertex's
  // coverage is final before it is added to its parent's.
  for (size_t x = tree.size() - 1; x > 0; x--) {
    TreeVertex &parent = tree[tree[x].parent];
    parent.coverage += tree[x].coverage;
    if (tree[x].coverage > 0 &&
        (parent.heaviest_child == NO_VERTEX ||
         tree[x].coverage > tree[parent.heaviest_child].coverage)) {
      parent.heaviest_child = x;
    }
  }
  return CoverageTree(std::move(tree));
}

/**
 * A builder that cuts an overlay into highways and labels vertices with them
 *
 * A HighwayBuilder cuts each highway through a root (see highway_through()).
 * The score of a root is the coverage of its Backward and Forward
 * CoverageTrees together. The builder cuts the highway of the best root
 * first, and labels vertices with each highway as it is cut.
 *
 * The builder keeps a queue of candidate roots, each with its score from
 * when its trees were last grown. Labels only grow and the vertices off
 * highways only shrink, so a score can only have gone down since. This makes
 * the queue a lazy greedy queue. The builder regrows the trees of the best
 * candidate. If the new score still beats the old score of every other
 * candidate, it cuts the highway. Otherwise it puts the candidate back with
 * the new score. The branches off the trees of each cut highway become new
 * candidates.
 *
 * The first candidates are the snarl endpoints. When the queue runs out, we
 * try the next vertex not on a highway, by decreasing degree. Every vertex is
 * tried before the builder finishes. So when it finishes, every vertex is on
 * a highway or has nothing left to cover.
 */
class HighwayBuilder {
public:
  HighwayBuilder(const ArcTable &arcs, const HighwayLabelingOptions &options);

  /// Cut all the highways and get the finished labels.
  DraftLabeling build() &&;

private:
  /// A Candidate is a possible highway root, with its score.
  struct Candidate {
    uint64_t score;
    NODE_UINT root;

    bool operator<(const Candidate &other) const {
      return score < other.score;
    }
  };

  /// Add the snarl endpoints as candidates that go first. They are the
  /// vertices make_boost_graph() gives a snarl's boundary ports.
  void add_endpoint_candidates();

  /// Add the next untried vertex not on a highway as a candidate. Returns
  /// false if there are none left.
  bool add_fallback_candidate();

  /// Cut the highway through the root of trees into and out_of, label
  /// vertices with it, and add its branches as candidates.
  void cut(const CoverageTree &into, const CoverageTree &out_of);

  const ArcTable &arcs;
  /// Whether to try each highway root's other orientation next.
  bool mirror_roots;
  /// The largest share of a stop's coverage its successor may have.
  double stop_fraction;
  /// The labels so far.
  DraftLabeling draft;
  /// Whether each vertex is on a highway already.
  std::vector<bool> on_highway;
  /// Grows trees to score candidates and find highways in.
  HighwayScout scout;
  /// Labels vertices with each new highway.
  HighwayLabeler labeler;

  /// The candidate roots, best score first.
  std::priority_queue<Candidate> candidates;
  /// All vertices by decreasing degree, to try when candidates run out.
  std::vector<NODE_UINT> by_degree;
  /// The next vertex of by_degree to try.
  size_t next_by_degree = 0;

#ifdef debug_phl
  size_t rescored = 0;
#endif
};

HighwayBuilder::HighwayBuilder(const ArcTable &arcs,
                               const HighwayLabelingOptions &options)
    : arcs(arcs), mirror_roots(options.mirror_roots),
      stop_fraction(options.stop_fraction), draft(arcs.vertex_count()),
      on_highway(arcs.vertex_count(), false), scout(arcs, draft, on_highway),
      labeler(arcs, draft, on_highway) {
  for (NODE_UINT v = 0; v < arcs.vertex_count(); v++) {
    by_degree.push_back(v);
  }
  std::stable_sort(by_degree.begin(), by_degree.end(),
                   [&](NODE_UINT a, NODE_UINT b) {
                     return arcs.degree(a) > arcs.degree(b);
                   });
  if (options.seed_endpoints) {
    add_endpoint_candidates();
  }
}

void HighwayBuilder::add_endpoint_candidates() {
  // Take the start of the snarl going in, the end coming in, and both of
  // them on the other strand.
  for (NODE_UINT v : {bgid(0, false, true), bgid(1, false, false),
                      bgid(1, true, true), bgid(0, true, false)}) {
    if (v < arcs.vertex_count() && (!arcs.arcs(v, Direction::Forward).empty() ||
                                    !arcs.arcs(v, Direction::Backward).empty())) {
      candidates.push({UNREACHED, v});
    }
  }
}

bool HighwayBuilder::add_fallback_candidate() {
  while (next_by_degree < by_degree.size() &&
         on_highway[by_degree[next_by_degree]]) {
    next_by_degree++;
  }
  if (next_by_degree == by_degree.size()) {
    return false;
  }
  candidates.push({UNREACHED, by_degree[next_by_degree++]});
  return true;
}

void HighwayBuilder::cut(const CoverageTree &into,
                         const CoverageTree &out_of) {
  Highway highway = highway_through(into, out_of, stop_fraction);
  labeler.label(highway);
  for (const HighwayStop &stop : highway.stops) {
    on_highway[stop.vertex] = true;
  }
  for (const CoverageTree *tree : {&into, &out_of}) {
    for (auto [v, coverage] : tree->branches()) {
      candidates.push({coverage, v});
    }
  }
}

DraftLabeling HighwayBuilder::build() && {
  while (!candidates.empty() || add_fallback_candidate()) {
    Candidate best = candidates.top();
    candidates.pop();
    if (on_highway[best.root]) {
      continue;
    }
    CoverageTree into = scout.explore(best.root, Direction::Backward);
    CoverageTree out_of = scout.explore(best.root, Direction::Forward);
    uint64_t score = into.coverage() + out_of.coverage();
    if (score == 0) {
      // The labels already give every distance a highway here would.
      continue;
    }
    if (!candidates.empty() && score < candidates.top().score) {
#ifdef debug_phl
      rescored++;
#endif
      candidates.push({score, best.root});
      continue;
    }
    cut(into, out_of);
    NODE_UINT mirror = best.root ^ 1;
    if (mirror_roots && mirror < arcs.vertex_count() && !on_highway[mirror]) {
      candidates.push({UNREACHED, mirror});
    }
  }
#ifdef debug_phl
  std::cerr << "PHL: " << draft.lengths().size() << " highways for "
            << arcs.vertex_count() << " vertices, " << rescored
            << " candidates put back after rescoring" << std::endl;
#endif
  return std::move(draft);
}

} // namespace

HighwayLabeling::HighwayLabeling(const CHOverlay &ov,
                                 const HighwayLabelingOptions &options) {
  ArcTable arcs(ov);
  DraftLabeling draft = HighwayBuilder(arcs, options).build();

  // Find how many bits the largest group's entry count needs.
  size_t n = arcs.vertex_count();
  size_t largest_group = 0;
  for (NODE_UINT v = 0; v < n; v++) {
    for (Side side : {Side::Out, Side::In}) {
      const std::vector<DraftEntry> &label = draft.label(v, side);
      for (size_t i = 0; i < label.size(); i = group_end(label, i)) {
        largest_group = std::max(largest_group, group_end(label, i) - i);
      }
    }
  }
  while (largest_group >> count_bits) {
    count_bits++;
  }

  // Copy the draft labels into label slots, one header and the entries for
  // each group.
  highway_lengths = draft.lengths();
  for (NODE_UINT v = 0; v < n; v++) {
    for (Side side : {Side::Out, Side::In}) {
      label_starts.push_back(label_slots.size());
      const std::vector<DraftEntry> &label = draft.label(v, side);
      for (size_t i = 0, end; i < label.size(); i = end) {
        end = group_end(label, i);
        label_slots.push_back(((uint64_t)label[i].highway << count_bits) |
                              (end - i));
        for (size_t k = i; k < end; k++) {
          label_slots.push_back(label[k].position);
          label_slots.push_back(label[k].value);
        }
      }
      entries += label.size();
      max_entries = std::max(max_entries, label.size());
    }
  }
  label_starts.push_back(label_slots.size());

  // Keep the seqlens only if query() has some to take off.
  bool any_seqlen = false;
  for (NODE_UINT v = 0; v < n; v++) {
    any_seqlen = any_seqlen || arcs.seqlen(v) != 0;
  }
  if (any_seqlen) {
    for (NODE_UINT v = 0; v < n; v++) {
      seqlens.push_back(arcs.seqlen(v));
    }
  }

  // Find the shortest cycle through each vertex s. The cycle leaves s by some
  // edge and comes back by the shortest walk.
  for (NODE_UINT s = 0; s < n; s++) {
    uint64_t best = UNREACHED;
    for (const Arc &arc : arcs.arcs(s, Direction::Forward)) {
      uint64_t back =
          arc.neighbor == s ? 0 : label_distance(arc.neighbor, s);
      if (back != UNREACHED) {
        best = std::min(best, arc.weight + back);
      }
    }
    cycle_lengths.push_back(best == UNREACHED
                                ? INF_INT
                                : demote_distance<uint64_t>(
                                      best - arcs.seqlen(s)));
  }
}

uint64_t HighwayLabeling::label_distance(NODE_UINT s, NODE_UINT t) const {
  size_t out = label_starts[2 * s];
  size_t out_end = label_starts[2 * s + 1];
  size_t in = label_starts[2 * t + 1];
  size_t in_end = label_starts[2 * t + 2];
  uint64_t count_mask = ((uint64_t)1 << count_bits) - 1;
  uint64_t best = UNREACHED;
  while (out < out_end && in < in_end) {
    uint64_t out_highway = label_slots[out] >> count_bits;
    uint64_t in_highway = label_slots[in] >> count_bits;
    size_t out_count = label_slots[out] & count_mask;
    size_t in_count = label_slots[in] & count_mask;
    if (out_highway == in_highway) {
      best = std::min(best, shortest_through_highway(
                                StoredGroup{&label_slots[out + 1], out_count},
                                StoredGroup{&label_slots[in + 1], in_count},
                                highway_lengths[out_highway]));
    }
    // Step past the group with the lower highway number, or both if they
    // are the same highway.
    if (out_highway <= in_highway) {
      out += 1 + 2 * out_count;
    }
    if (in_highway <= out_highway) {
      in += 1 + 2 * in_count;
    }
  }
  return best;
}

DIST_UINT HighwayLabeling::query(NODE_UINT s, NODE_UINT t) const {
  if (s == t) {
    return cycle_lengths[s];
  }
  uint64_t distance = label_distance(s, t);
  if (distance == UNREACHED) {
    return INF_INT;
  }
  if (!seqlens.empty()) {
    distance -= seqlens[t];
  }
  return demote_distance<uint64_t>(distance);
}

size_t HighwayLabeling::stored_words() const {
  // count_bits takes one more slot.
  return label_starts.size() + label_slots.size() + 1 +
         highway_lengths.size() + cycle_lengths.size() + seqlens.size();
}

} // namespace phl
} // namespace bdsg
