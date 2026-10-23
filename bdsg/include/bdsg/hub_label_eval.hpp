/*
Sandbox for comparing distance labeling methods over CHOverlay graphs, without
going through the distance index. Each method builds and queries labels in its
own format, so methods that don't store hub/distance pairs can be compared too.
*/
#ifndef BDSG_HUB_LABEL_EVAL_HPP_INCLUDED
#define BDSG_HUB_LABEL_EVAL_HPP_INCLUDED

#include "bdsg/ch.hpp" // BINDER_IGNORE because ch.hpp is C++20 + Boost and is not bound

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace bdsg {

/**
 * Labels built by some labeling method for one CHOverlay.
 *
 * Nothing here fixes a label format: each method keeps whatever it needs and
 * answers queries itself.
 */
class BuiltLabels {
public:
  virtual ~BuiltLabels() = default;

  /// Get the distance from s to t with hub labeling semantics: the seqlens of
  /// the vertices strictly between the ends count, s to itself is the shortest
  /// cycle through s, and unreachable is INF_INT.
  virtual DIST_UINT query(NODE_UINT s, NODE_UINT t) const = 0;

  /// Get the total number of label entries, of whatever kind the method uses.
  virtual size_t entry_count() const = 0;

  /// Get the largest number of entries in one vertex's forward or backward
  /// label.
  virtual size_t max_label_entries() const = 0;

  /// Get the number of integer slots a stored version of these labels would
  /// take. This is the measure to compare between methods, since entries of
  /// different methods can be different widths.
  virtual size_t stored_words() const = 0;
};

/// Build times for a method, in milliseconds. Methods that don't have separate
/// ordering and labeling stages report everything as label_ms.
struct BuildTimes {
  double order_ms = 0;
  double label_ms = 0;
};

/// A named way to build labels for a CHOverlay.
struct LabelingMethod {
  std::string name;
  std::function<std::unique_ptr<BuiltLabels>(const CHOverlay &, BuildTimes &)>
      build;
};

/// Hub labels as stored in the distance index, queried with hhl_query().
class PackedHubLabels : public BuiltLabels {
public:
  PackedHubLabels(const vector<vector<HubRecord>> &labels,
                  const vector<vector<HubRecord>> &labels_back);

  DIST_UINT query(NODE_UINT s, NODE_UINT t) const override;
  size_t entry_count() const override { return entries; }
  size_t max_label_entries() const override { return max_entries; }
  size_t stored_words() const override { return packed.size(); }

private:
  vector<size_t> packed;
  size_t entries = 0;
  size_t max_entries = 0;
};

/**
 * Make a labeling method that finds a hub order and then builds labels for it
 * with create_labels(). The ordering function gets its own copy of the
 * overlay, which it may change as long as it stays valid input for
 * create_labels() (as contraction_hierarchy_order() does).
 */
LabelingMethod hub_order_method(const std::string &name,
                                std::function<HubOrder(CHOverlay &)> order);

/// Get all the labeling methods the sandbox knows about.
vector<LabelingMethod> default_labeling_methods();

/// Get labeling methods by name from a comma-separated list. Throws on unknown
/// names.
vector<LabelingMethod> labeling_methods_by_name(const std::string &names);

/// Get hub labeling distances from s to every vertex of ov the slow way, by
/// Dijkstra, with the same semantics as BuiltLabels::query().
vector<DIST_UINT> reference_distances(const CHOverlay &ov, size_t s);

struct EvalOptions {
  /// Check all pairs against Dijkstra when the overlay has at most this many
  /// vertices.
  size_t full_verify_limit = 2000;
  /// Otherwise check this many random sources against all targets.
  size_t verify_sources = 50;
  /// Number of random queries to time.
  size_t query_pairs = 100000;
  uint64_t seed = 0;
};

struct EvalResult {
  std::string method;
  size_t vertices = 0;
  size_t edges = 0;
  BuildTimes times;
  size_t entries = 0;
  size_t max_label = 0;
  size_t words = 0;
  double query_ns = 0;
  size_t verified_pairs = 0;
  size_t mismatches = 0;
};

/**
 * Build labels for ov with each method, time queries, and check them against
 * Dijkstra. Mismatches are counted rather than asserted, so one broken method
 * doesn't hide the others.
 */
vector<EvalResult> evaluate(const CHOverlay &ov,
                            const vector<LabelingMethod> &methods,
                            const EvalOptions &options = EvalOptions());

/// Write results as tab-separated rows, labelled with graph_label, optionally
/// after a header row.
void write_tsv(std::ostream &out, const std::string &graph_label,
               const vector<EvalResult> &results, bool header);

/// Make a bidirected width x height grid CHOverlay with random weights 1-10.
CHOverlay grid_test_overlay(size_t width, size_t height, uint64_t seed);

/// Make a HashGraph shaped like a pangenome: a backbone chain with bubbles
/// skipping a few nodes ahead, and occasional inversions.
HashGraph pangenome_like_test_hash_graph(size_t nodes, uint64_t seed);

/// Make a random HashGraph with random sequence lengths and random edges
/// between random orientations, including self-loops and reversing
/// self-loops.
HashGraph random_test_hash_graph(size_t nodes, size_t edges, uint64_t seed);

} // namespace bdsg

#endif
