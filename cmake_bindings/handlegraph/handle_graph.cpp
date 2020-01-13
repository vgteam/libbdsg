#include <cwchar>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
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

// handlegraph::HandleGraph file:handlegraph/handle_graph.hpp line:22
struct PyCallBack_handlegraph_HandleGraph : public handlegraph::HandleGraph {
	using handlegraph::HandleGraph::HandleGraph;

	bool has_node(long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::has_node\"");
	}
	struct handlegraph::handle_t get_handle(const long & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_handle\"");
	}
	long get_id(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_id\"");
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_is_reverse\"");
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::flip\"");
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_length\"");
	}
	class std::__cxx11::basic_string<char> get_sequence(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::__cxx11::basic_string<char>>::value) {
				static pybind11::detail::overload_caster_t<class std::__cxx11::basic_string<char>> caster;
				return pybind11::detail::cast_ref<class std::__cxx11::basic_string<char>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::__cxx11::basic_string<char>>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_sequence\"");
	}
	unsigned long get_node_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_node_count\"");
	}
	long min_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::min_node_id\"");
	}
	long max_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::max_node_id\"");
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_degree(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraph::has_edge(a0, a1);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::overload_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return HandleGraph::get_base(a0, a1);
	}
	class std::__cxx11::basic_string<char> get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::__cxx11::basic_string<char>>::value) {
				static pybind11::detail::overload_caster_t<class std::__cxx11::basic_string<char>> caster;
				return pybind11::detail::cast_ref<class std::__cxx11::basic_string<char>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::__cxx11::basic_string<char>>(std::move(o));
		}
		return HandleGraph::get_subsequence(a0, a1, a2);
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::follow_edges_impl\"");
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::HandleGraph *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::for_each_handle_impl\"");
	}
};

// handlegraph::SerializableHandleGraph file:handlegraph/handle_graph.hpp line:172
struct PyCallBack_handlegraph_SerializableHandleGraph : public handlegraph::SerializableHandleGraph {
	using handlegraph::SerializableHandleGraph::SerializableHandleGraph;

	void serialize(class std::basic_ostream<char> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SerializableHandleGraph *>(this), "serialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SerializableHandleGraph::serialize\"");
	}
	void deserialize(class std::basic_istream<char> & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SerializableHandleGraph *>(this), "deserialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SerializableHandleGraph::deserialize\"");
	}
};

// handlegraph::VectorizableHandleGraph file:handlegraph/handle_graph.hpp line:192
struct PyCallBack_handlegraph_VectorizableHandleGraph : public handlegraph::VectorizableHandleGraph {
	using handlegraph::VectorizableHandleGraph::VectorizableHandleGraph;

	unsigned long node_vector_offset(const long & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::VectorizableHandleGraph *>(this), "node_vector_offset");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"VectorizableHandleGraph::node_vector_offset\"");
	}
	long node_at_vector_offset(const unsigned long & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::VectorizableHandleGraph *>(this), "node_at_vector_offset");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"VectorizableHandleGraph::node_at_vector_offset\"");
	}
	unsigned long edge_index(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::VectorizableHandleGraph *>(this), "edge_index");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"VectorizableHandleGraph::edge_index\"");
	}
};

void bind_handlegraph_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::HandleGraph file:handlegraph/handle_graph.hpp line:22
		pybind11::class_<handlegraph::HandleGraph, std::shared_ptr<handlegraph::HandleGraph>, PyCallBack_handlegraph_HandleGraph> cl(M("handlegraph"), "HandleGraph", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_HandleGraph(); } ) );
		cl.def(pybind11::init<PyCallBack_handlegraph_HandleGraph const &>());
		cl.def("follow_edges", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const) &handlegraph::HandleGraph::follow_edges<std::function<bool (const handlegraph::handle_t &)>>, "C++: handlegraph::HandleGraph::follow_edges(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("go_left"), pybind11::arg("iteratee"));
		cl.def("for_each_handle", [](handlegraph::HandleGraph const &o, const class std::function<bool (const struct handlegraph::handle_t &)> & a0) -> bool { return o.for_each_handle(a0); }, "", pybind11::arg("iteratee"));
		cl.def("for_each_handle", (bool (handlegraph::HandleGraph::*)(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const) &handlegraph::HandleGraph::for_each_handle<std::function<bool (const handlegraph::handle_t &)>>, "C++: handlegraph::HandleGraph::for_each_handle(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const --> bool", pybind11::arg("iteratee"), pybind11::arg("parallel"));
		cl.def("has_node", (bool (handlegraph::HandleGraph::*)(long) const) &handlegraph::HandleGraph::has_node, "C++: handlegraph::HandleGraph::has_node(long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](handlegraph::HandleGraph const &o, const long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const long &, bool) const) &handlegraph::HandleGraph::get_handle, "C++: handlegraph::HandleGraph::get_handle(const long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_id, "C++: handlegraph::HandleGraph::get_id(const struct handlegraph::handle_t &) const --> long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_is_reverse, "C++: handlegraph::HandleGraph::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::flip, "C++: handlegraph::HandleGraph::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_length, "C++: handlegraph::HandleGraph::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_sequence, "C++: handlegraph::HandleGraph::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::get_node_count, "C++: handlegraph::HandleGraph::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::min_node_id, "C++: handlegraph::HandleGraph::min_node_id() const --> long");
		cl.def("max_node_id", (long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::max_node_id, "C++: handlegraph::HandleGraph::max_node_id() const --> long");
		cl.def("get_degree", (unsigned long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, bool) const) &handlegraph::HandleGraph::get_degree, "C++: handlegraph::HandleGraph::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("has_edge", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::has_edge, "C++: handlegraph::HandleGraph::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("has_edge", (bool (handlegraph::HandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const) &handlegraph::HandleGraph::has_edge, "C++: handlegraph::HandleGraph::has_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const --> bool", pybind11::arg("edge"));
		cl.def("get_base", (char (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, unsigned long) const) &handlegraph::HandleGraph::get_base, "C++: handlegraph::HandleGraph::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &handlegraph::HandleGraph::get_subsequence, "C++: handlegraph::HandleGraph::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("forward", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::forward, "C++: handlegraph::HandleGraph::forward(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("edge_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::edge_handle, "C++: handlegraph::HandleGraph::edge_handle(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("traverse_edge_handle", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::traverse_edge_handle, "C++: handlegraph::HandleGraph::traverse_edge_handle(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("edge"), pybind11::arg("left"));
		cl.def("assign", (class handlegraph::HandleGraph & (handlegraph::HandleGraph::*)(const class handlegraph::HandleGraph &)) &handlegraph::HandleGraph::operator=, "C++: handlegraph::HandleGraph::operator=(const class handlegraph::HandleGraph &) --> class handlegraph::HandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::SerializableHandleGraph file:handlegraph/handle_graph.hpp line:172
		pybind11::class_<handlegraph::SerializableHandleGraph, std::shared_ptr<handlegraph::SerializableHandleGraph>, PyCallBack_handlegraph_SerializableHandleGraph> cl(M("handlegraph"), "SerializableHandleGraph", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_SerializableHandleGraph(); } ) );
		cl.def("serialize", (void (handlegraph::SerializableHandleGraph::*)(std::ostream &) const) &handlegraph::SerializableHandleGraph::serialize, "C++: handlegraph::SerializableHandleGraph::serialize(std::ostream &) const --> void", pybind11::arg("out"));
		cl.def("deserialize", (void (handlegraph::SerializableHandleGraph::*)(class std::basic_istream<char> &)) &handlegraph::SerializableHandleGraph::deserialize, "C++: handlegraph::SerializableHandleGraph::deserialize(class std::basic_istream<char> &) --> void", pybind11::arg("in"));
		cl.def("assign", (class handlegraph::SerializableHandleGraph & (handlegraph::SerializableHandleGraph::*)(const class handlegraph::SerializableHandleGraph &)) &handlegraph::SerializableHandleGraph::operator=, "C++: handlegraph::SerializableHandleGraph::operator=(const class handlegraph::SerializableHandleGraph &) --> class handlegraph::SerializableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::VectorizableHandleGraph file:handlegraph/handle_graph.hpp line:192
		pybind11::class_<handlegraph::VectorizableHandleGraph, std::shared_ptr<handlegraph::VectorizableHandleGraph>, PyCallBack_handlegraph_VectorizableHandleGraph> cl(M("handlegraph"), "VectorizableHandleGraph", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_VectorizableHandleGraph(); } ) );
		cl.def("node_vector_offset", (unsigned long (handlegraph::VectorizableHandleGraph::*)(const long &) const) &handlegraph::VectorizableHandleGraph::node_vector_offset, "C++: handlegraph::VectorizableHandleGraph::node_vector_offset(const long &) const --> unsigned long", pybind11::arg("node_id"));
		cl.def("node_at_vector_offset", (long (handlegraph::VectorizableHandleGraph::*)(const unsigned long &) const) &handlegraph::VectorizableHandleGraph::node_at_vector_offset, "C++: handlegraph::VectorizableHandleGraph::node_at_vector_offset(const unsigned long &) const --> long", pybind11::arg("offset"));
		cl.def("edge_index", (unsigned long (handlegraph::VectorizableHandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const) &handlegraph::VectorizableHandleGraph::edge_index, "C++: handlegraph::VectorizableHandleGraph::edge_index(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const --> unsigned long", pybind11::arg("edge"));
		cl.def("assign", (class handlegraph::VectorizableHandleGraph & (handlegraph::VectorizableHandleGraph::*)(const class handlegraph::VectorizableHandleGraph &)) &handlegraph::VectorizableHandleGraph::operator=, "C++: handlegraph::VectorizableHandleGraph::operator=(const class handlegraph::VectorizableHandleGraph &) --> class handlegraph::VectorizableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
