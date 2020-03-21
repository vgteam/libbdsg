#include <bdsg/is_single_stranded.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_is_single_stranded(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::algorithms::is_single_stranded(const class handlegraph::HandleGraph *) file:bdsg/is_single_stranded.hpp line:25
	M("bdsg::algorithms").def("is_single_stranded", (bool (*)(const class handlegraph::HandleGraph *)) &bdsg::algorithms::is_single_stranded, "Returns true if the graph contains no reversing edges (i.e. edges that connected\n the locally forward orientation of a node to the locally reverse orientation of\n of another node).\n\nC++: bdsg::algorithms::is_single_stranded(const class handlegraph::HandleGraph *) --> bool", pybind11::arg("graph"));

}
