#include <bdsg/internal/is_single_stranded.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <bdsg/internal/binder_hook_compile.hpp>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_bdsg_internal_is_single_stranded(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::algorithms::is_single_stranded(const class handlegraph::HandleGraph *) file:bdsg/internal/is_single_stranded.hpp line:25
	M("bdsg::algorithms").def("is_single_stranded", (bool (*)(const class handlegraph::HandleGraph *)) &bdsg::algorithms::is_single_stranded, "Returns true if the graph contains no reversing edges (i.e. edges that connected\n the locally forward orientation of a node to the locally reverse orientation of\n of another node).\n\nC++: bdsg::algorithms::is_single_stranded(const class handlegraph::HandleGraph *) --> bool", pybind11::arg("graph"));

	// bdsg::algorithms::single_stranded_orientation(const class handlegraph::HandleGraph *) file:bdsg/internal/is_single_stranded.hpp line:32
	M("bdsg::algorithms").def("single_stranded_orientation", (class std::vector<handlegraph::handle_t> (*)(const class handlegraph::HandleGraph *)) &bdsg::algorithms::single_stranded_orientation, "Returns a vector of handles where the orientation of each handle indicates an\n orientation that could be used to convert the graph into a single-stranded graph.\n That is, if all of the reverse handles (or all of the forward handles) were swapped\n in orientation, the graph would contain no reversing edges. Returns an empty vector\n if there is no such combination of node orientations (also if graph has no nodes).\n\nC++: bdsg::algorithms::single_stranded_orientation(const class handlegraph::HandleGraph *) --> class std::vector<handlegraph::handle_t>", pybind11::arg("graph"));

}
