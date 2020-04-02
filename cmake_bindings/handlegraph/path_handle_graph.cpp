#include <handlegraph/path_handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <sstream> // __str__

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>
#include <bdsg/internal/binder_hook_compile.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::PathForEachSocket file:handlegraph/path_handle_graph.hpp line:197
		pybind11::class_<handlegraph::PathForEachSocket, std::shared_ptr<handlegraph::PathForEachSocket>> cl(M("handlegraph"), "PathForEachSocket", "An auxilliary class that enables for each loops over paths. Not intended to\n constructed directly. Instead, use the PathHandleGraph's scan_path method.");
		{ // handlegraph::PathForEachSocket::iterator file:handlegraph/path_handle_graph.hpp line:211
			auto & enclosing_class = cl;
			pybind11::class_<handlegraph::PathForEachSocket::iterator, std::shared_ptr<handlegraph::PathForEachSocket::iterator>> cl(enclosing_class, "iterator", "Iterator object over path");
			cl.def( pybind11::init( [](handlegraph::PathForEachSocket::iterator const &o){ return new handlegraph::PathForEachSocket::iterator(o); } ) );
			cl.def("assign", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &)) &handlegraph::PathForEachSocket::iterator::operator=, "C++: handlegraph::PathForEachSocket::iterator::operator=(const class handlegraph::PathForEachSocket::iterator &) --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
			cl.def("plus_plus", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)()) &handlegraph::PathForEachSocket::iterator::operator++, "C++: handlegraph::PathForEachSocket::iterator::operator++() --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic);
			cl.def("__mul__", (struct handlegraph::handle_t (handlegraph::PathForEachSocket::iterator::*)() const) &handlegraph::PathForEachSocket::iterator::operator*, "C++: handlegraph::PathForEachSocket::iterator::operator*() const --> struct handlegraph::handle_t");
			cl.def("__eq__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator==, "C++: handlegraph::PathForEachSocket::iterator::operator==(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
			cl.def("__ne__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator!=, "C++: handlegraph::PathForEachSocket::iterator::operator!=(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
		}

		cl.def( pybind11::init( [](handlegraph::PathForEachSocket const &o){ return new handlegraph::PathForEachSocket(o); } ) );
		cl.def("begin", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::begin, "C++: handlegraph::PathForEachSocket::begin() const --> class handlegraph::PathForEachSocket::iterator");
		cl.def("end", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::end, "C++: handlegraph::PathForEachSocket::end() const --> class handlegraph::PathForEachSocket::iterator");
	}
}
