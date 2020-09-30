#ifndef BDSG_ALGORITHMS_EADES_ALGORITHM_HPP_INCLUDED
#define BDSG_ALGORITHMS_EADES_ALGORITHM_HPP_INCLUDED

/**
 * \file eades_algorithm.hpp
 *
 * Implements the Eades, Lin, Smyth (1993) algorithm for computing a heuristic
 * solution to the minimum feedback arc set.
 */

#include <handlegraph/handle_graph.hpp>
#include "bdsg/internal/is_single_stranded.hpp"

#include <vector>
#include <list>
#include <unordered_map>
#include <deque>

namespace bdsg {
namespace algorithms {

using namespace std;
using namespace handlegraph;

/// Returns a layout of handles that has a small number of edges that point backward
/// along the layout (i.e. feedback arcs). Only valid for graphs that have a single
/// stranded orientation. Consider checking this property with
/// algorithms::single_stranded_orientation.
vector<handle_t> eades_algorithm(const HandleGraph* graph);

}
}

#endif
