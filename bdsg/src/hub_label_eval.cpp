#include "bdsg/hub_label_eval.hpp" // BINDER_IGNORE because hub_label_eval.hpp is C++20 + Boost and is not bound
#include "bdsg/shp.hpp" // BINDER_IGNORE because shp.hpp is C++20 + Boost and is not bound
#include <boost/property_map/function_property_map.hpp> // BINDER_IGNORE

#include <chrono>
#include <queue>
#include <random>
#include <sstream>
#include <stdexcept>

namespace bdsg {

using eval_clock = std::chrono::steady_clock;

static double ms_since(eval_clock::time_point start) {
  return std::chrono::duration<double, std::milli>(eval_clock::now() - start)
      .count();
}

PackedHubLabels::PackedHubLabels(const vector<vector<HubRecord>> &labels,
                                 const vector<vector<HubRecord>> &labels_back)
    : packed(pack_labels(labels, labels_back)) {
  for (const auto &side : {&labels, &labels_back}) {
    for (const auto &label : *side) {
      entries += label.size();
      max_entries = std::max(max_entries, label.size());
    }
  }
}

DIST_UINT PackedHubLabels::query(NODE_UINT s, NODE_UINT t) const {
  return hhl_query(packed.begin(), s, t);
}

LabelingMethod hub_order_method(const std::string &name,
                                std::function<HubOrder(CHOverlay &)> order) {
  return {name, [order](const CHOverlay &original, BuildTimes &times) {
            CHOverlay ov = original;
            auto start = eval_clock::now();
            HubOrder hub_order = order(ov);
            times.order_ms = ms_since(start);

            start = eval_clock::now();
            vector<vector<HubRecord>> labels(num_vertices(ov)),
                labels_back(num_vertices(ov));
            create_labels(labels, labels_back, ov, hub_order);
            auto built = std::make_unique<PackedHubLabels>(labels, labels_back);
            times.label_ms = ms_since(start);
            return built;
          }};
}

LabelingMethod highway_method(const std::string &name,
                              const phl::HighwayLabelingOptions &options) {
  return {name, [options](const CHOverlay &ov, BuildTimes &times) {
            auto start = eval_clock::now();
            auto built = std::make_unique<PackedHighwayLabels>(
                phl::HighwayLabeling(ov, options));
            times.label_ms = ms_since(start);
            return built;
          }};
}

vector<LabelingMethod> default_labeling_methods() {
  return {
      hub_order_method("ch", contraction_hierarchy_order),
      hub_order_method("shp",
                       [](CHOverlay &ov) {
                         // Same weights as shp_hub_order(): charge each
                         // vertex's seqlen on the way into it.
                         auto weights = boost::make_function_property_map<
                             CHOverlay::edge_descriptor, DIST_UINT>(
                             [&ov](const CHOverlay::edge_descriptor &e) {
                               return ov[e].weight + ov[target(e, ov)].seqlen;
                             });
                         auto shp = shp::shp_order(ov, weights);
                         return HubOrder(shp.order.begin(), shp.order.end());
                       }),
      highway_method("phl", {}),
      highway_method("phl-noseed", {.seed_endpoints = false}),
      highway_method("phl-nomirror", {.mirror_roots = false}),
      highway_method("phl-allstops", {.stop_fraction = 1}),
  };
}

vector<LabelingMethod> labeling_methods_by_name(const std::string &names) {
  vector<LabelingMethod> all = default_labeling_methods();
  vector<LabelingMethod> chosen;
  std::stringstream ss(names);
  std::string name;
  while (std::getline(ss, name, ',')) {
    if (name.empty()) {
      continue;
    }
    auto found = std::find_if(all.begin(), all.end(), [&](const auto &m) {
      return m.name == name;
    });
    if (found == all.end()) {
      throw std::invalid_argument("Unknown labeling method: " + name);
    }
    chosen.push_back(*found);
  }
  return chosen;
}

vector<DIST_UINT> reference_distances(const CHOverlay &ov, size_t s) {
  vector<DIST_UINT> dist(num_vertices(ov), INF_INT);
  std::priority_queue<pair<DIST_UINT, size_t>, vector<pair<DIST_UINT, size_t>>,
                      greater<pair<DIST_UINT, size_t>>>
      q;
  for (auto e : boost::make_iterator_range(out_edges(s, ov))) {
    auto t = target(e, ov);
    if (ov[e].weight < dist[t]) {
      dist[t] = ov[e].weight;
      q.emplace(dist[t], t);
    }
  }
  while (!q.empty()) {
    auto [d, v] = q.top();
    q.pop();
    if (d > dist[v] || v == s) {
      continue;
    }
    for (auto e : boost::make_iterator_range(out_edges(v, ov))) {
      auto t = target(e, ov);
      DIST_UINT nd = d + ov[v].seqlen + ov[e].weight;
      if (nd < dist[t]) {
        dist[t] = nd;
        q.emplace(nd, t);
      }
    }
  }
  return dist;
}

vector<EvalResult> evaluate(const CHOverlay &ov,
                            const vector<LabelingMethod> &methods,
                            const EvalOptions &options) {
  size_t n = num_vertices(ov);
  std::mt19937_64 rng(options.seed);

  // Pick the sources to check and get their true distances once, for all
  // methods.
  vector<size_t> sources;
  if (n <= options.full_verify_limit) {
    for (size_t s = 0; s < n; s++) {
      sources.push_back(s);
    }
  } else if (n > 0) {
    std::uniform_int_distribution<size_t> pick(0, n - 1);
    for (size_t i = 0; i < options.verify_sources; i++) {
      sources.push_back(pick(rng));
    }
  }
  vector<vector<DIST_UINT>> truth;
  for (size_t s : sources) {
    truth.push_back(reference_distances(ov, s));
  }

  vector<pair<NODE_UINT, NODE_UINT>> pairs;
  if (n > 0) {
    std::uniform_int_distribution<NODE_UINT> pick(0, n - 1);
    for (size_t i = 0; i < options.query_pairs; i++) {
      pairs.emplace_back(pick(rng), pick(rng));
    }
  }

  vector<EvalResult> results;
  for (const auto &method : methods) {
    EvalResult result;
    result.method = method.name;
    result.vertices = n;
    result.edges = num_edges(ov);
    auto labels = method.build(ov, result.times);
    result.entries = labels->entry_count();
    result.max_label = labels->max_label_entries();
    result.words = labels->stored_words();

    for (size_t i = 0; i < sources.size(); i++) {
      for (size_t t = 0; t < n; t++) {
        if (labels->query(sources[i], t) != truth[i][t]) {
          result.mismatches++;
        }
        result.verified_pairs++;
      }
    }

    // Keep the results live so the queries can't be optimized out.
    volatile DIST_UINT sink = 0;
    auto start = eval_clock::now();
    for (auto [s, t] : pairs) {
      sink = sink + labels->query(s, t);
    }
    if (!pairs.empty()) {
      result.query_ns = ms_since(start) * 1e6 / pairs.size();
    }
    results.push_back(result);
  }
  return results;
}

void write_tsv(std::ostream &out, const std::string &graph_label,
               const vector<EvalResult> &results, bool header) {
  if (header) {
    out << "graph\tmethod\tvertices\tedges\torder_ms\tlabel_ms\tentries"
           "\tavg_label\tmax_label\twords\tquery_ns\tverified_pairs"
           "\tmismatches\n";
  }
  for (const auto &r : results) {
    double avg = r.vertices == 0 ? 0 : (double)r.entries / (2.0 * r.vertices);
    out << graph_label << '\t' << r.method << '\t' << r.vertices << '\t'
        << r.edges << '\t' << r.times.order_ms << '\t' << r.times.label_ms
        << '\t' << r.entries << '\t' << avg << '\t' << r.max_label << '\t'
        << r.words << '\t' << r.query_ns << '\t' << r.verified_pairs << '\t'
        << r.mismatches << '\n';
  }
}

CHOverlay grid_test_overlay(size_t width, size_t height, uint64_t seed) {
  CHOverlay ov(width * height);
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<DIST_UINT> weight(1, 10);
  for (size_t v = 0; v < width * height; v++) {
    ov[v].seqlen = 0;
  }
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      size_t v = y * width + x;
      for (size_t u : {x + 1 < width ? v + 1 : v, y + 1 < height ? v + width : v}) {
        if (u != v) {
          ov[add_edge(v, u, ov).first].weight = weight(rng);
          ov[add_edge(u, v, ov).first].weight = weight(rng);
        }
      }
    }
  }
  return ov;
}

HashGraph pangenome_like_test_hash_graph(size_t nodes, uint64_t seed) {
  HashGraph g;
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<size_t> length(1, 30);
  vector<handle_t> handles;
  for (size_t i = 0; i < nodes; i++) {
    handles.push_back(g.create_handle(string(length(rng), 'A')));
  }
  std::uniform_int_distribution<size_t> skip(2, 6);
  std::bernoulli_distribution bubble(0.4);
  std::bernoulli_distribution inversion(0.05);
  for (size_t i = 0; i + 1 < nodes; i++) {
    g.create_edge(handles[i], handles[i + 1]);
    size_t j = i + skip(rng);
    if (j < nodes && bubble(rng)) {
      g.create_edge(handles[i], handles[j]);
    }
    if (j < nodes && inversion(rng)) {
      g.create_edge(handles[i], g.flip(handles[j]));
      g.create_edge(g.flip(handles[j]), handles[j + 1 < nodes ? j + 1 : j]);
    }
  }
  return g;
}

HashGraph random_test_hash_graph(size_t nodes, size_t edges, uint64_t seed) {
  HashGraph g;
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<size_t> length(1, 5);
  vector<handle_t> handles;
  for (size_t i = 0; i < nodes; i++) {
    handles.push_back(g.create_handle(string(length(rng), 'A')));
  }
  std::uniform_int_distribution<size_t> pick(0, nodes - 1);
  std::bernoulli_distribution flip(0.2);
  for (size_t i = 0; i < edges; i++) {
    handle_t a = handles[pick(rng)];
    handle_t b = handles[pick(rng)];
    g.create_edge(flip(rng) ? g.flip(a) : a, flip(rng) ? g.flip(b) : b);
  }
  return g;
}

} // namespace bdsg
