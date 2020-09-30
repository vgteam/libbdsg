#include <handlegraph/serializable_handle_graph.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

void bind_handlegraph_serializable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::SerializableHandleGraph file:handlegraph/serializable_handle_graph.hpp line:18
		pybind11::class_<handlegraph::SerializableHandleGraph, std::shared_ptr<handlegraph::SerializableHandleGraph>> cl(M("handlegraph"), "SerializableHandleGraph", "");
		cl.def("get_magic_number", (unsigned int (handlegraph::SerializableHandleGraph::*)() const) &handlegraph::SerializableHandleGraph::get_magic_number, "Returns a number that is specific to the serialized implementation for type\n checking. Does not depend on the contents of any particular instantiation\n (i.e. behaves as if static, but cannot be static and virtual).\n\nC++: handlegraph::SerializableHandleGraph::get_magic_number() const --> unsigned int");
		cl.def("serialize", (void (handlegraph::SerializableHandleGraph::*)(const std::string &) const) &handlegraph::SerializableHandleGraph::serialize, "Write the contents of this graph to a named file. Makes sure to include\n a leading magic number.\n\nC++: handlegraph::SerializableHandleGraph::serialize(const std::string &) const --> void", pybind11::arg("filename"));
		cl.def("deserialize", (void (handlegraph::SerializableHandleGraph::*)(const std::string &)) &handlegraph::SerializableHandleGraph::deserialize, "Sets the contents of this graph to the contents of a serialized graph from\n a file. The serialized graph must be from the same implementation of the\n HandleGraph interface as is calling deserialize(). Can only be called on an\n empty graph.\n\nC++: handlegraph::SerializableHandleGraph::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (class handlegraph::SerializableHandleGraph & (handlegraph::SerializableHandleGraph::*)(const class handlegraph::SerializableHandleGraph &)) &handlegraph::SerializableHandleGraph::operator=, "C++: handlegraph::SerializableHandleGraph::operator=(const class handlegraph::SerializableHandleGraph &) --> class handlegraph::SerializableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
