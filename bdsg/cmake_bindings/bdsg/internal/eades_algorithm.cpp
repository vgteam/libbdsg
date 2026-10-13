#include <bdsg/internal/eades_algorithm.hpp>
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

void bind_bdsg_internal_eades_algorithm(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::algorithms::eades_algorithm(const class handlegraph::HandleGraph *) file:bdsg/internal/eades_algorithm.hpp line:29
	M("bdsg::algorithms").def("eades_algorithm", (class std::vector<handlegraph::handle_t> (*)(const class handlegraph::HandleGraph *)) &bdsg::algorithms::eades_algorithm, "Returns a layout of handles that has a small number of edges that point backward\n along the layout (i.e. feedback arcs). Only valid for graphs that have a single\n stranded orientation. Consider checking this property with\n algorithms::single_stranded_orientation.\n\nC++: bdsg::algorithms::eades_algorithm(const class handlegraph::HandleGraph *) --> class std::vector<handlegraph::handle_t>", pybind11::arg("graph"));

}
