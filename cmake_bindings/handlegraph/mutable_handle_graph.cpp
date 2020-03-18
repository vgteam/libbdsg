#include <functional>
#include <handlegraph/deletable_handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/mutable_path_mutable_handle_graph.hpp>
#include <handlegraph/serializable_handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_handlegraph_mutable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::MutableHandleGraph file:handlegraph/mutable_handle_graph.hpp line:19
		pybind11::class_<handlegraph::MutableHandleGraph, std::shared_ptr<handlegraph::MutableHandleGraph>, handlegraph::HandleGraph> cl(M("handlegraph"), "MutableHandleGraph", "This is the interface for a handle graph that supports addition of new graph material.");
		cl.def("create_handle", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const std::string &)) &handlegraph::MutableHandleGraph::create_handle, "Create a new node with the given sequence and return the handle.\n\nC++: handlegraph::MutableHandleGraph::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const std::string &, const long long &)) &handlegraph::MutableHandleGraph::create_handle, "Create a new node with the given id and sequence, then return the handle.\n\nC++: handlegraph::MutableHandleGraph::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("create_edge", (void (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &handlegraph::MutableHandleGraph::create_edge, "Create an edge connecting the given handles in the given order and orientations.\n Ignores existing edges.\n\nC++: handlegraph::MutableHandleGraph::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("create_edge", (void (handlegraph::MutableHandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &handlegraph::MutableHandleGraph::create_edge, "Convenient wrapper for create_edge.\n\nC++: handlegraph::MutableHandleGraph::create_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("apply_orientation", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &)) &handlegraph::MutableHandleGraph::apply_orientation, "Alter the node that the given handle corresponds to so the orientation\n indicated by the handle becomes the node's local forward orientation.\n Rewrites all edges pointing to the node and the node's sequence to\n reflect this. Invalidates all handles to the node (including the one\n passed). Returns a new, valid handle to the node in its new forward\n orientation. Note that it is possible for the node's ID to change.\n Does not update any stored paths. May change the ordering of the underlying\n graph.\n\nC++: handlegraph::MutableHandleGraph::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &, unsigned long)) &handlegraph::MutableHandleGraph::divide_handle, "Specialization of divide_handle for a single division point\n\nC++: handlegraph::MutableHandleGraph::divide_handle(const struct handlegraph::handle_t &, unsigned long) --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offset"));
		cl.def("optimize", [](handlegraph::MutableHandleGraph &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (handlegraph::MutableHandleGraph::*)(bool)) &handlegraph::MutableHandleGraph::optimize, "Adjust the representation of the graph in memory to improve performance.\n Optionally, allow the node IDs to be reassigned to further improve\n performance.\n Note: Ideally, this method is called one time once there is expected to be\n few graph modifications in the future.\n\nC++: handlegraph::MutableHandleGraph::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("set_id_increment", (void (handlegraph::MutableHandleGraph::*)(const long long &)) &handlegraph::MutableHandleGraph::set_id_increment, "Set a minimum id to increment the id space by, used as a hint during construction.\n May have no effect on a backing implementation.\n\nC++: handlegraph::MutableHandleGraph::set_id_increment(const long long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (handlegraph::MutableHandleGraph::*)(long long)) &handlegraph::MutableHandleGraph::increment_node_ids, "Add the given value to all node IDs.\n Has a default implementation in terms of reassign_node_ids, but can be\n implemented more efficiently in some graphs.\n\nC++: handlegraph::MutableHandleGraph::increment_node_ids(long long) --> void", pybind11::arg("increment"));
		cl.def("reassign_node_ids", (void (handlegraph::MutableHandleGraph::*)(const class std::function<long long (const long long &)> &)) &handlegraph::MutableHandleGraph::reassign_node_ids, "Renumber all node IDs using the given function, which, given an old ID, returns the new ID.\n Modifies the graph in place. Invalidates all outstanding handles.\n If the graph supports paths, they also must be updated.\n The mapping function may return 0. In this case, the input ID will\n remain unchanged. The mapping function should not return any ID for\n which it would return 0.\n\nC++: handlegraph::MutableHandleGraph::reassign_node_ids(const class std::function<long long (const long long &)> &) --> void", pybind11::arg("get_new_id"));
		cl.def("assign", (class handlegraph::MutableHandleGraph & (handlegraph::MutableHandleGraph::*)(const class handlegraph::MutableHandleGraph &)) &handlegraph::MutableHandleGraph::operator=, "C++: handlegraph::MutableHandleGraph::operator=(const class handlegraph::MutableHandleGraph &) --> class handlegraph::MutableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::MutablePathMutableHandleGraph file:handlegraph/mutable_path_mutable_handle_graph.hpp line:16
		pybind11::class_<handlegraph::MutablePathMutableHandleGraph, std::shared_ptr<handlegraph::MutablePathMutableHandleGraph>, handlegraph::MutablePathHandleGraph, handlegraph::MutableHandleGraph> cl(M("handlegraph"), "MutablePathMutableHandleGraph", "This is the interface for a graph which is mutable and which has paths which are also mutable.");
		cl.def("assign", (class handlegraph::MutablePathMutableHandleGraph & (handlegraph::MutablePathMutableHandleGraph::*)(const class handlegraph::MutablePathMutableHandleGraph &)) &handlegraph::MutablePathMutableHandleGraph::operator=, "C++: handlegraph::MutablePathMutableHandleGraph::operator=(const class handlegraph::MutablePathMutableHandleGraph &) --> class handlegraph::MutablePathMutableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::DeletableHandleGraph file:handlegraph/deletable_handle_graph.hpp line:16
		pybind11::class_<handlegraph::DeletableHandleGraph, std::shared_ptr<handlegraph::DeletableHandleGraph>, handlegraph::MutableHandleGraph> cl(M("handlegraph"), "DeletableHandleGraph", "This is the interface for a handle graph that supports both addition of new nodes and edges\n as well as deletion of nodes and edges.");
		cl.def("destroy_handle", (void (handlegraph::DeletableHandleGraph::*)(const struct handlegraph::handle_t &)) &handlegraph::DeletableHandleGraph::destroy_handle, "Remove the node belonging to the given handle and all of its edges.\n Does not update any stored paths.\n Invalidates the destroyed handle.\n May be called during serial for_each_handle iteration **ONLY** on the node being iterated.\n May **NOT** be called during parallel for_each_handle iteration.\n May **NOT** be called on the node from which edges are being followed during follow_edges.\n\nC++: handlegraph::DeletableHandleGraph::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("destroy_edge", (void (handlegraph::DeletableHandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &handlegraph::DeletableHandleGraph::destroy_edge, "Remove the edge connecting the given handles in the given order and orientations.\n Ignores nonexistent edges.\n Does not update any stored paths.\n\nC++: handlegraph::DeletableHandleGraph::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (handlegraph::DeletableHandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &handlegraph::DeletableHandleGraph::destroy_edge, "Convenient wrapper for destroy_edge.\n\nC++: handlegraph::DeletableHandleGraph::destroy_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("clear", (void (handlegraph::DeletableHandleGraph::*)()) &handlegraph::DeletableHandleGraph::clear, "Remove all nodes and edges.\n\nC++: handlegraph::DeletableHandleGraph::clear() --> void");
		cl.def("assign", (class handlegraph::DeletableHandleGraph & (handlegraph::DeletableHandleGraph::*)(const class handlegraph::DeletableHandleGraph &)) &handlegraph::DeletableHandleGraph::operator=, "C++: handlegraph::DeletableHandleGraph::operator=(const class handlegraph::DeletableHandleGraph &) --> class handlegraph::DeletableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::MutablePathDeletableHandleGraph file:handlegraph/mutable_path_deletable_handle_graph.hpp line:17
		pybind11::class_<handlegraph::MutablePathDeletableHandleGraph, std::shared_ptr<handlegraph::MutablePathDeletableHandleGraph>, handlegraph::MutablePathMutableHandleGraph, handlegraph::DeletableHandleGraph> cl(M("handlegraph"), "MutablePathDeletableHandleGraph", "This is the interface for a graph which is deletable and which has paths which are also mutable.");
		cl.def("assign", (class handlegraph::MutablePathDeletableHandleGraph & (handlegraph::MutablePathDeletableHandleGraph::*)(const class handlegraph::MutablePathDeletableHandleGraph &)) &handlegraph::MutablePathDeletableHandleGraph::operator=, "C++: handlegraph::MutablePathDeletableHandleGraph::operator=(const class handlegraph::MutablePathDeletableHandleGraph &) --> class handlegraph::MutablePathDeletableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::SerializableHandleGraph file:handlegraph/serializable_handle_graph.hpp line:18
		pybind11::class_<handlegraph::SerializableHandleGraph, std::shared_ptr<handlegraph::SerializableHandleGraph>> cl(M("handlegraph"), "SerializableHandleGraph", "");
		cl.def("get_magic_number", (unsigned int (handlegraph::SerializableHandleGraph::*)() const) &handlegraph::SerializableHandleGraph::get_magic_number, "Returns a number that is specific to the serialized implementation for type\n checking. Does not depend on the contents of any particular instantiation\n (i.e. behaves as if static, but cannot be static and virtual).\n\nC++: handlegraph::SerializableHandleGraph::get_magic_number() const --> unsigned int");
		cl.def("serialize", (void (handlegraph::SerializableHandleGraph::*)(const std::string &) const) &handlegraph::SerializableHandleGraph::serialize, "Write the contents of this graph to a named file. Makes sure to include\n a leading magic number.\n\nC++: handlegraph::SerializableHandleGraph::serialize(const std::string &) const --> void", pybind11::arg("filename"));
		cl.def("deserialize", (void (handlegraph::SerializableHandleGraph::*)(const std::string &)) &handlegraph::SerializableHandleGraph::deserialize, "Sets the contents of this graph to the contents of a serialized graph from\n a file. The serialized graph must be from the same implementation of the\n HandleGraph interface as is calling deserialize(). Can only be called on an\n empty graph.\n\nC++: handlegraph::SerializableHandleGraph::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (class handlegraph::SerializableHandleGraph & (handlegraph::SerializableHandleGraph::*)(const class handlegraph::SerializableHandleGraph &)) &handlegraph::SerializableHandleGraph::operator=, "C++: handlegraph::SerializableHandleGraph::operator=(const class handlegraph::SerializableHandleGraph &) --> class handlegraph::SerializableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
