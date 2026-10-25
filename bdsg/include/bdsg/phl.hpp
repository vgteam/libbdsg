/*
Pruned Highway Labeling (PHL) for directed weighted graphs, after Akiba,
Iwata, Kawarabayashi, Kawata, "Fast Shortest-path Distance Queries on Road
Networks by Pruned Highway Labeling", ALENEX 2014, and the authors' code
(github.com/kawatea/pruned-highway-labeling).

PHL cuts the graph into walks called highways, and labels each vertex with its
distances to and from vertices on highways. Two labels answer a query through
any highway they share, by adding the distance along the highway between the
two highway vertices.

The paper's graphs are undirected road networks, and it picks highways by road
class. Here the graph is directed, and highways are picked greedily. From a
root, we grow pruned shortest path trees in both directions, and the highway
is the walk through the root that leads to the most vertices whose distances
the labels cannot give yet. Roots come from the snarl endpoints first, then
from branches off highways already cut, then from high-degree vertices.

In a directed graph a query can only use a highway in its own direction. So a
vertex can need several entries for one highway, one for each place a walk to
it leaves the highway.

This is a sandbox method: it builds labels in its own flat format, for
comparison with hub labeling (see hub_label_eval.hpp), and is not used by the
distance index.
*/
#ifndef BDSG_PHL_HPP_INCLUDED
#define BDSG_PHL_HPP_INCLUDED

#include "bdsg/ch.hpp" // BINDER_IGNORE because ch.hpp is C++20 + Boost and is not bound

#include <cstdint>
#include <vector>

namespace bdsg {
namespace phl {

/// Which way a search follows edges: Forward follows out-edges, Backward
/// follows in-edges.
enum class Direction { Forward, Backward };

/// Options for building a HighwayLabeling.
struct HighwayLabelingOptions {
  /// Whether to grow the first highways from the snarl endpoints. These are
  /// the vertices that make_boost_graph() gives a snarl's boundary ports. If
  /// false, or if those vertices have no edges, we start from the
  /// highest-degree vertex.
  bool seed_endpoints = true;
  /// Whether to try each highway root's other orientation as the next root.
  /// make_boost_graph() numbers a vertex's other orientation by flipping the
  /// lowest bit of its number.
  bool mirror_roots = true;
  /// The largest share of a highway vertex's uncovered vertices that its
  /// successor on the highway's walk may lead to. A walk vertex whose
  /// successor leads to more has nothing much branching off it, so it is
  /// skipped as a highway vertex and left for later highways, as in the
  /// authors' code, which uses 0.95. At 1 every walk vertex is a highway
  /// vertex.
  double stop_fraction = 0.95;
};

/**
 * Highway labels for every vertex of a CHOverlay, stored flat
 *
 * A HighwayLabeling answers distance queries between the overlay's vertices.
 *
 * Highways are numbered in the order they were picked. Each vertex has an out
 * label, with its distances to highway vertices, and an in label, with the
 * distances from highway vertices to it. A label is a run of groups, one per
 * highway, in increasing highway number. A group is a run of entries, each a
 * key and a distance, in increasing key.
 *
 * All labels are stored in one array of slots. A group takes one header slot,
 * holding the highway number shifted left by count_bits plus the number of
 * entries, followed by two slots for each entry, its key and its distance.
 *
 * The key of a highway vertex is its offset, the distance along the highway
 * from the first highway vertex, shifted left by tie_bits, plus the number of
 * highway vertices before it at the same offset. Edges can have weight 0, so
 * several highway vertices can share an offset, and the low bits say which
 * comes first. A query pairs an out entry only with in entries at or after it
 * along the highway, and gets the walk's length from the two distances and the
 * difference of the offsets.
 *
 * A vertex v whose out-edges all go to one other vertex u can have a derived
 * out label. Every walk from v starts with the edge to u, so a query can use
 * u's out label instead. A derived label takes three slots: a header with an
 * entry count of 0, then u, then the weight of the edge. Likewise, a vertex
 * whose in-edges all come from one vertex can have a derived in label. A
 * derived label always names a vertex whose label on that side is stored.
 *
 * Distances inside the labels charge each edge its weight plus the seqlen of
 * the vertex it enters. query() takes the destination's seqlen back off, so it
 * counts only the seqlens of vertices strictly between the ends, as
 * hhl_query() does.
 */
class HighwayLabeling {
public:
  /// Pick highways for ov and label its vertices with them.
  HighwayLabeling(const CHOverlay &ov,
                  const HighwayLabelingOptions &options = {});

  /// Get the distance from s to t, as hhl_query() would. The seqlens of
  /// vertices strictly between the ends count. From s to itself it is the
  /// shortest cycle through s, and to an unreachable t it is INF_INT.
  DIST_UINT query(NODE_UINT s, NODE_UINT t) const;

  /// Get the number of highways picked.
  size_t highway_count() const { return highways; }

  /// Get the total number of (key, distance) entries in stored labels.
  size_t entry_count() const { return entries; }

  /// Get the largest number of entries in one stored out or in label.
  size_t max_label_entries() const { return max_entries; }

  /// Get the number of integer slots these labels would take if stored one
  /// number per slot, as the distance index stores hub labels.
  size_t stored_words() const;

private:
  /// Get the distance from s to t through the labels, including t's seqlen,
  /// or UINT64_MAX if the labels have no walk. Follows derived labels to the
  /// stored ones they point to.
  uint64_t label_distance(NODE_UINT s, NODE_UINT t) const;

  /// Get the distance from s to t through the highways that s's out label and
  /// t's in label share, or UINT64_MAX if they give no walk.
  /// @pre Both labels are stored, not derived.
  uint64_t stored_distance(NODE_UINT s, NODE_UINT t) const;

  /// Get the vertex and the edge weight, two slots in a row, that the label
  /// at [start, end) of label_slots is derived through. Return nullptr if the
  /// label is stored.
  const uint64_t *derived_through(size_t start, size_t end) const;

  /// Where each vertex's labels start in label_slots. The out label of v is
  /// [label_starts[2v], label_starts[2v+1]) and its in label is
  /// [label_starts[2v+1], label_starts[2v+2]).
  std::vector<size_t> label_starts;
  /// The groups of all labels, in the layout described above.
  std::vector<uint64_t> label_slots;
  /// The number of low bits of a group header that hold its entry count.
  unsigned count_bits = 0;
  /// The number of low bits of a key that order highway vertices at the same
  /// offset.
  unsigned tie_bits = 0;
  /// The number of highways picked.
  size_t highways = 0;
  /// The shortest cycle through each vertex, or INF_INT if none.
  std::vector<DIST_UINT> cycle_lengths;
  /// Each vertex's seqlen, or empty if all are 0.
  std::vector<DIST_UINT> seqlens;

  /// The total number of entries in stored labels.
  size_t entries = 0;
  /// The largest number of entries in one stored label.
  size_t max_entries = 0;
};

} // namespace phl
} // namespace bdsg

#endif
