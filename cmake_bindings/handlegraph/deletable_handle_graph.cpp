#include <handlegraph/deletable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <sstream> // __str__
#include <utility>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_handlegraph_deletable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::DeletableHandleGraph file:handlegraph/deletable_handle_graph.hpp line:16
		pybind11::class_<handlegraph::DeletableHandleGraph, std::shared_ptr<handlegraph::DeletableHandleGraph>, handlegraph::MutableHandleGraph> cl(M("handlegraph"), "DeletableHandleGraph", "");
		cl.def("destroy_handle", (void (handlegraph::DeletableHandleGraph::*)(const struct handlegraph::handle_t &)) &handlegraph::DeletableHandleGraph::destroy_handle, "C++: handlegraph::DeletableHandleGraph::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("destroy_edge", (void (handlegraph::DeletableHandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &handlegraph::DeletableHandleGraph::destroy_edge, "C++: handlegraph::DeletableHandleGraph::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (handlegraph::DeletableHandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &handlegraph::DeletableHandleGraph::destroy_edge, "C++: handlegraph::DeletableHandleGraph::destroy_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("clear", (void (handlegraph::DeletableHandleGraph::*)()) &handlegraph::DeletableHandleGraph::clear, "C++: handlegraph::DeletableHandleGraph::clear() --> void");
		cl.def("assign", (class handlegraph::DeletableHandleGraph & (handlegraph::DeletableHandleGraph::*)(const class handlegraph::DeletableHandleGraph &)) &handlegraph::DeletableHandleGraph::operator=, "C++: handlegraph::DeletableHandleGraph::operator=(const class handlegraph::DeletableHandleGraph &) --> class handlegraph::DeletableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::MutablePathDeletableHandleGraph file:handlegraph/mutable_path_deletable_handle_graph.hpp line:17
		pybind11::class_<handlegraph::MutablePathDeletableHandleGraph, std::shared_ptr<handlegraph::MutablePathDeletableHandleGraph>, handlegraph::MutablePathMutableHandleGraph, handlegraph::DeletableHandleGraph> cl(M("handlegraph"), "MutablePathDeletableHandleGraph", "");
		cl.def("assign", (class handlegraph::MutablePathDeletableHandleGraph & (handlegraph::MutablePathDeletableHandleGraph::*)(const class handlegraph::MutablePathDeletableHandleGraph &)) &handlegraph::MutablePathDeletableHandleGraph::operator=, "C++: handlegraph::MutablePathDeletableHandleGraph::operator=(const class handlegraph::MutablePathDeletableHandleGraph &) --> class handlegraph::MutablePathDeletableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
