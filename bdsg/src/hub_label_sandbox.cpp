/*
Compare distance labeling methods on synthetic graphs or HashGraph files,
printing one TSV row per graph and method. See hub_label_eval.hpp.
*/
#include "bdsg/hub_label_eval.hpp" // BINDER_IGNORE because hub_label_eval.hpp is C++20 + Boost and is not bound

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace bdsg;
using namespace std;

static void usage(const char *name) {
  cerr << "usage: " << name << " [options]" << endl
       << "  --methods LIST         comma-separated labeling methods [all]"
       << endl
       << "  --synthetic KIND:SIZE:SEED" << endl
       << "                         grid (SIZE x SIZE), pangenome (SIZE nodes)"
       << endl
       << "                         or random (SIZE nodes, 2 x SIZE edges);"
       << endl
       << "                         may repeat" << endl
       << "  --hg FILE              HashGraph with dense node IDs from 1; may "
          "repeat"
       << endl
       << "  --full-verify-limit N  check all pairs up to N vertices [2000]"
       << endl
       << "  --verify-sources N     else check N random sources [50]" << endl
       << "  --query-pairs N        random queries to time [100000]" << endl
       << "  --seed N               seed for verification and query picks [0]"
       << endl
       << "Methods: ";
  for (const auto &m : default_labeling_methods()) {
    cerr << m.name << " ";
  }
  cerr << endl;
}

static CHOverlay synthetic_overlay(const string &spec) {
  size_t first = spec.find(':');
  size_t second = spec.find(':', first == string::npos ? first : first + 1);
  if (first == string::npos || second == string::npos) {
    throw invalid_argument("Synthetic graph must be KIND:SIZE:SEED: " + spec);
  }
  string kind = spec.substr(0, first);
  size_t size = stoull(spec.substr(first + 1, second - first - 1));
  uint64_t seed = stoull(spec.substr(second + 1));
  if (kind == "grid") {
    return grid_test_overlay(size, size, seed);
  } else if (kind == "pangenome") {
    return make_boost_graph(pangenome_like_test_hash_graph(size, seed));
  } else if (kind == "random") {
    return make_boost_graph(random_test_hash_graph(size, size * 2, seed));
  }
  throw invalid_argument("Unknown synthetic graph kind: " + kind);
}

static CHOverlay hash_graph_overlay(const string &filename) {
  if (!ifstream(filename)) {
    throw invalid_argument("Cannot open HashGraph " + filename);
  }
  HashGraph graph;
  graph.deserialize(filename);
  if (graph.get_node_count() > 0 &&
      (graph.min_node_id() != 1 ||
       graph.max_node_id() != (nid_t)graph.get_node_count())) {
    throw invalid_argument("HashGraph " + filename +
                           " does not have dense node IDs from 1");
  }
  return make_boost_graph(graph);
}

int main(int argc, char **argv) {
  vector<LabelingMethod> methods = default_labeling_methods();
  vector<pair<string, string>> graphs; // (kind, spec)
  EvalOptions options;

  try {
    for (int i = 1; i < argc; i++) {
      string arg = argv[i];
      if (arg == "-h" || arg == "--help") {
        usage(argv[0]);
        return 0;
      }
      if (i + 1 >= argc) {
        usage(argv[0]);
        return 1;
      }
      string value = argv[++i];
      if (arg == "--methods") {
        methods = labeling_methods_by_name(value);
      } else if (arg == "--synthetic") {
        graphs.emplace_back("synthetic", value);
      } else if (arg == "--hg") {
        graphs.emplace_back("hg", value);
      } else if (arg == "--full-verify-limit") {
        options.full_verify_limit = stoull(value);
      } else if (arg == "--verify-sources") {
        options.verify_sources = stoull(value);
      } else if (arg == "--query-pairs") {
        options.query_pairs = stoull(value);
      } else if (arg == "--seed") {
        options.seed = stoull(value);
      } else {
        usage(argv[0]);
        return 1;
      }
    }
    if (graphs.empty()) {
      usage(argv[0]);
      return 1;
    }

    bool header = true;
    size_t mismatches = 0;
    for (const auto &[kind, spec] : graphs) {
      CHOverlay ov =
          kind == "hg" ? hash_graph_overlay(spec) : synthetic_overlay(spec);
      auto results = evaluate(ov, methods, options);
      write_tsv(cout, spec, results, header);
      header = false;
      for (const auto &r : results) {
        mismatches += r.mismatches;
      }
    }
    if (mismatches > 0) {
      cerr << "error: " << mismatches << " distances did not match Dijkstra"
           << endl;
      return 2;
    }
  } catch (const exception &e) {
    cerr << "error: " << e.what() << endl;
    return 1;
  }
  return 0;
}
