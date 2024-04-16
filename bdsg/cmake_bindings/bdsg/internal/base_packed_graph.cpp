#include <bdsg/internal/base_packed_graph.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <bdsg/internal/packed_structs.hpp>
#include <functional>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
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
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_bdsg_internal_base_packed_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::PackedStringCollection file:bdsg/internal/base_packed_graph.hpp line:36
		pybind11::class_<bdsg::PackedStringCollection<bdsg::STLBackend>, std::shared_ptr<bdsg::PackedStringCollection<bdsg::STLBackend>>> cl(M("bdsg"), "PackedStringCollection_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedStringCollection<bdsg::STLBackend>(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedStringCollection<bdsg::STLBackend> const &o){ return new bdsg::PackedStringCollection<bdsg::STLBackend>(o); } ) );
		cl.def("size", (long long (bdsg::PackedStringCollection<bdsg::STLBackend>::*)() const) &bdsg::PackedStringCollection<>::size, "C++: bdsg::PackedStringCollection<>::size() const --> long long");
		cl.def("decode", (std::string (bdsg::PackedStringCollection<bdsg::STLBackend>::*)(const long long &) const) &bdsg::PackedStringCollection<>::decode, "C++: bdsg::PackedStringCollection<>::decode(const long long &) const --> std::string", pybind11::arg("index"));
		cl.def("push_back", (void (bdsg::PackedStringCollection<bdsg::STLBackend>::*)(const std::string &)) &bdsg::PackedStringCollection<>::push_back, "C++: bdsg::PackedStringCollection<>::push_back(const std::string &) --> void", pybind11::arg("to_add"));
		cl.def("memory_usage", (unsigned long (bdsg::PackedStringCollection<bdsg::STLBackend>::*)() const) &bdsg::PackedStringCollection<>::memory_usage, "C++: bdsg::PackedStringCollection<>::memory_usage() const --> unsigned long");
		cl.def("assign", (class bdsg::PackedStringCollection<> & (bdsg::PackedStringCollection<bdsg::STLBackend>::*)(const class bdsg::PackedStringCollection<> &)) &bdsg::PackedStringCollection<>::operator=, "C++: bdsg::PackedStringCollection<>::operator=(const class bdsg::PackedStringCollection<> &) --> class bdsg::PackedStringCollection<> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::BasePackedGraph file:bdsg/internal/base_packed_graph.hpp line:374
		pybind11::class_<bdsg::BasePackedGraph<bdsg::STLBackend>, std::shared_ptr<bdsg::BasePackedGraph<bdsg::STLBackend>>> cl(M("bdsg"), "BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::BasePackedGraph<bdsg::STLBackend>(); } ) );
		cl.def( pybind11::init( [](bdsg::BasePackedGraph<bdsg::STLBackend> const &o){ return new bdsg::BasePackedGraph<bdsg::STLBackend>(o); } ) );
		cl.def("has_node", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(long long) const) &bdsg::BasePackedGraph<>::has_node, "C++: bdsg::BasePackedGraph<>::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::BasePackedGraph<bdsg::STLBackend> const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const long long &, bool) const) &bdsg::BasePackedGraph<>::get_handle, "C++: bdsg::BasePackedGraph<>::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::get_id, "C++: bdsg::BasePackedGraph<>::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::get_is_reverse, "C++: bdsg::BasePackedGraph<>::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::flip, "C++: bdsg::BasePackedGraph<>::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::get_length, "C++: bdsg::BasePackedGraph<>::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::get_sequence, "C++: bdsg::BasePackedGraph<>::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("follow_edges", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const) &bdsg::BasePackedGraph<>::follow_edges, "C++: bdsg::BasePackedGraph<>::follow_edges(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("go_left"), pybind11::arg("iteratee"));
		cl.def("for_each_handle", [](bdsg::BasePackedGraph<bdsg::STLBackend> const &o, const class std::function<bool (const struct handlegraph::handle_t &)> & a0) -> bool { return o.for_each_handle(a0); }, "", pybind11::arg("iteratee"));
		cl.def("for_each_handle", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const) &bdsg::BasePackedGraph<>::for_each_handle, "C++: bdsg::BasePackedGraph<>::for_each_handle(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const --> bool", pybind11::arg("iteratee"), pybind11::arg("parallel"));
		cl.def("get_edge_count", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::get_edge_count, "C++: bdsg::BasePackedGraph<>::get_edge_count() const --> unsigned long");
		cl.def("get_total_length", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::get_total_length, "C++: bdsg::BasePackedGraph<>::get_total_length() const --> unsigned long");
		cl.def("get_base", (char (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, unsigned long) const) &bdsg::BasePackedGraph<>::get_base, "C++: bdsg::BasePackedGraph<>::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &bdsg::BasePackedGraph<>::get_subsequence, "C++: bdsg::BasePackedGraph<>::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("forward", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::forward, "C++: bdsg::BasePackedGraph<>::forward(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::get_node_count, "C++: bdsg::BasePackedGraph<>::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::min_node_id, "C++: bdsg::BasePackedGraph<>::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::max_node_id, "C++: bdsg::BasePackedGraph<>::max_node_id() const --> long long");
		cl.def("get_degree", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::BasePackedGraph<>::get_degree, "C++: bdsg::BasePackedGraph<>::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("has_edge", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::has_edge, "C++: bdsg::BasePackedGraph<>::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &)) &bdsg::BasePackedGraph<>::create_handle, "C++: bdsg::BasePackedGraph<>::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &, const long long &)) &bdsg::BasePackedGraph<>::create_handle, "C++: bdsg::BasePackedGraph<>::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("destroy_handle", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::destroy_handle, "C++: bdsg::BasePackedGraph<>::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("truncate_handle", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, bool, unsigned long)) &bdsg::BasePackedGraph<>::truncate_handle, "C++: bdsg::BasePackedGraph<>::truncate_handle(const struct handlegraph::handle_t &, bool, unsigned long) --> struct handlegraph::handle_t", pybind11::arg("handle"), pybind11::arg("trunc_left"), pybind11::arg("offset"));
		cl.def("create_edge", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::create_edge, "C++: bdsg::BasePackedGraph<>::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::destroy_edge, "C++: bdsg::BasePackedGraph<>::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("clear", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)()) &bdsg::BasePackedGraph<>::clear, "C++: bdsg::BasePackedGraph<>::clear() --> void");
		cl.def("apply_orientation", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::apply_orientation, "C++: bdsg::BasePackedGraph<>::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (class std::vector<handlegraph::handle_t> (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &)) &bdsg::BasePackedGraph<>::divide_handle, "C++: bdsg::BasePackedGraph<>::divide_handle(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &) --> class std::vector<handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offsets"));
		cl.def("divide_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, unsigned long)) &bdsg::BasePackedGraph<>::divide_handle, "C++: bdsg::BasePackedGraph<>::divide_handle(const struct handlegraph::handle_t &, unsigned long) --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offset"));
		cl.def("optimize", [](bdsg::BasePackedGraph<bdsg::STLBackend> &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(bool)) &bdsg::BasePackedGraph<>::optimize, "C++: bdsg::BasePackedGraph<>::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("apply_ordering", [](bdsg::BasePackedGraph<bdsg::STLBackend> &o, const class std::vector<handlegraph::handle_t> & a0) -> bool { return o.apply_ordering(a0); }, "", pybind11::arg("order"));
		cl.def("apply_ordering", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const class std::vector<handlegraph::handle_t> &, bool)) &bdsg::BasePackedGraph<>::apply_ordering, "C++: bdsg::BasePackedGraph<>::apply_ordering(const class std::vector<handlegraph::handle_t> &, bool) --> bool", pybind11::arg("order"), pybind11::arg("compact_ids"));
		cl.def("get_path_count", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::get_path_count, "C++: bdsg::BasePackedGraph<>::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &) const) &bdsg::BasePackedGraph<>::has_path, "C++: bdsg::BasePackedGraph<>::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &) const) &bdsg::BasePackedGraph<>::get_path_handle, "C++: bdsg::BasePackedGraph<>::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_path_name, "C++: bdsg::BasePackedGraph<>::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_is_circular, "C++: bdsg::BasePackedGraph<>::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_step_count, "C++: bdsg::BasePackedGraph<>::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &) const) &bdsg::BasePackedGraph<>::get_step_count, "C++: bdsg::BasePackedGraph<>::get_step_count(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::get_handle_of_step, "C++: bdsg::BasePackedGraph<>::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::path_begin, "C++: bdsg::BasePackedGraph<>::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::path_end, "C++: bdsg::BasePackedGraph<>::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::path_back, "C++: bdsg::BasePackedGraph<>::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::path_front_end, "C++: bdsg::BasePackedGraph<>::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::has_next_step, "C++: bdsg::BasePackedGraph<>::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::has_previous_step, "C++: bdsg::BasePackedGraph<>::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::get_next_step, "C++: bdsg::BasePackedGraph<>::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::get_previous_step, "C++: bdsg::BasePackedGraph<>::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::BasePackedGraph<>::get_path_handle_of_step, "C++: bdsg::BasePackedGraph<>::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("for_each_path_handle", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const) &bdsg::BasePackedGraph<>::for_each_path_handle, "C++: bdsg::BasePackedGraph<>::for_each_path_handle(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const --> bool", pybind11::arg("iteratee"));
		cl.def("for_each_step_on_handle", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const) &bdsg::BasePackedGraph<>::for_each_step_on_handle, "C++: bdsg::BasePackedGraph<>::for_each_step_on_handle(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("iteratee"));
		cl.def("steps_of_handle", [](bdsg::BasePackedGraph<bdsg::STLBackend> const &o, const struct handlegraph::handle_t & a0) -> std::vector<handlegraph::step_handle_t> { return o.steps_of_handle(a0); }, "", pybind11::arg("handle"));
		cl.def("steps_of_handle", (class std::vector<handlegraph::step_handle_t> (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::BasePackedGraph<>::steps_of_handle, "C++: bdsg::BasePackedGraph<>::steps_of_handle(const struct handlegraph::handle_t &, bool) const --> class std::vector<handlegraph::step_handle_t>", pybind11::arg("handle"), pybind11::arg("match_orientation"));
		cl.def("is_empty", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::is_empty, "C++: bdsg::BasePackedGraph<>::is_empty(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("destroy_path", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &)) &bdsg::BasePackedGraph<>::destroy_path, "C++: bdsg::BasePackedGraph<>::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("create_path_handle", [](bdsg::BasePackedGraph<bdsg::STLBackend> &o, const std::string & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &, bool)) &bdsg::BasePackedGraph<>::create_path_handle, "C++: bdsg::BasePackedGraph<>::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("append_step", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::append_step, "C++: bdsg::BasePackedGraph<>::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::BasePackedGraph<>::prepend_step, "C++: bdsg::BasePackedGraph<>::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_prepend"));
		cl.def("rewrite_segment", (struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t> (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &)) &bdsg::BasePackedGraph<>::rewrite_segment, "C++: bdsg::BasePackedGraph<>::rewrite_segment(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &) --> struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>", pybind11::arg("segment_begin"), pybind11::arg("segment_end"), pybind11::arg("new_segment"));
		cl.def("set_circularity", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &, bool)) &bdsg::BasePackedGraph<>::set_circularity, "C++: bdsg::BasePackedGraph<>::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path"), pybind11::arg("circular"));
		cl.def("set_id_increment", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const long long &)) &bdsg::BasePackedGraph<>::set_id_increment, "C++: bdsg::BasePackedGraph<>::set_id_increment(const long long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(long long)) &bdsg::BasePackedGraph<>::increment_node_ids, "C++: bdsg::BasePackedGraph<>::increment_node_ids(long long) --> void", pybind11::arg("increment"));
		cl.def("reassign_node_ids", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const class std::function<long long (const long long &)> &)) &bdsg::BasePackedGraph<>::reassign_node_ids, "C++: bdsg::BasePackedGraph<>::reassign_node_ids(const class std::function<long long (const long long &)> &) --> void", pybind11::arg("get_new_id"));
		cl.def("get_sense", (enum handlegraph::PathSense (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_sense, "C++: bdsg::BasePackedGraph<>::get_sense(const struct handlegraph::path_handle_t &) const --> enum handlegraph::PathSense", pybind11::arg("handle"));
		cl.def("get_sample_name", (std::string (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_sample_name, "C++: bdsg::BasePackedGraph<>::get_sample_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_locus_name", (std::string (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_locus_name, "C++: bdsg::BasePackedGraph<>::get_locus_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_haplotype", (unsigned long (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_haplotype, "C++: bdsg::BasePackedGraph<>::get_haplotype(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_subrange", (struct std::pair<unsigned long, unsigned long> (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::BasePackedGraph<>::get_subrange, "C++: bdsg::BasePackedGraph<>::get_subrange(const struct handlegraph::path_handle_t &) const --> struct std::pair<unsigned long, unsigned long>", pybind11::arg("handle"));
		cl.def("create_path", [](bdsg::BasePackedGraph<bdsg::STLBackend> &o, const enum handlegraph::PathSense & a0, const std::string & a1, const std::string & a2, const unsigned long & a3, const struct std::pair<unsigned long, unsigned long> & a4) -> handlegraph::path_handle_t { return o.create_path(a0, a1, a2, a3, a4); }, "", pybind11::arg("sense"), pybind11::arg("sample"), pybind11::arg("locus"), pybind11::arg("haplotype"), pybind11::arg("subrange"));
		cl.def("create_path", (struct handlegraph::path_handle_t (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const enum handlegraph::PathSense &, const std::string &, const std::string &, const unsigned long &, const struct std::pair<unsigned long, unsigned long> &, bool)) &bdsg::BasePackedGraph<>::create_path, "C++: bdsg::BasePackedGraph<>::create_path(const enum handlegraph::PathSense &, const std::string &, const std::string &, const unsigned long &, const struct std::pair<unsigned long, unsigned long> &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("sense"), pybind11::arg("sample"), pybind11::arg("locus"), pybind11::arg("haplotype"), pybind11::arg("subrange"), pybind11::arg("is_circular"));
		cl.def("for_each_step_of_sense", (bool (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const struct handlegraph::handle_t &, const enum handlegraph::PathSense &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const) &bdsg::BasePackedGraph<>::for_each_step_of_sense, "C++: bdsg::BasePackedGraph<>::for_each_step_of_sense(const struct handlegraph::handle_t &, const enum handlegraph::PathSense &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const --> bool", pybind11::arg("visited"), pybind11::arg("sense"), pybind11::arg("iteratee"));
		cl.def("get_magic_number", (unsigned int (bdsg::BasePackedGraph<bdsg::STLBackend>::*)() const) &bdsg::BasePackedGraph<>::get_magic_number, "C++: bdsg::BasePackedGraph<>::get_magic_number() const --> unsigned int");
		cl.def("deserialize", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &)) &bdsg::BasePackedGraph<>::deserialize, "C++: bdsg::BasePackedGraph<>::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("serialize", (void (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const std::string &)) &bdsg::BasePackedGraph<>::serialize, "C++: bdsg::BasePackedGraph<>::serialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (class bdsg::BasePackedGraph<> & (bdsg::BasePackedGraph<bdsg::STLBackend>::*)(const class bdsg::BasePackedGraph<> &)) &bdsg::BasePackedGraph<>::operator=, "C++: bdsg::BasePackedGraph<>::operator=(const class bdsg::BasePackedGraph<> &) --> class bdsg::BasePackedGraph<> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
