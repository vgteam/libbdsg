#include <functional>
#include <handlegraph/expanding_overlay_graph.hpp>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>

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

// handlegraph::ExpandingOverlayGraph file:handlegraph/expanding_overlay_graph.hpp line:17
struct PyCallBack_handlegraph_ExpandingOverlayGraph : public handlegraph::ExpandingOverlayGraph {
	using handlegraph::ExpandingOverlayGraph::ExpandingOverlayGraph;

	struct handlegraph::handle_t get_underlying_handle(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_underlying_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"ExpandingOverlayGraph::get_underlying_handle\"");
	}
	bool has_node(long long a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::has_node\"");
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_handle\"");
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_id\"");
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_is_reverse\"");
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::flip\"");
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_length\"");
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_sequence\"");
	}
	unsigned long get_node_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::get_node_count\"");
	}
	long long min_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::min_node_id\"");
	}
	long long max_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::max_node_id\"");
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_degree(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraph::has_edge(a0, a1);
	}
	unsigned long get_edge_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_edge_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_edge_count();
	}
	unsigned long get_total_length() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_total_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_total_length();
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::override_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return HandleGraph::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HandleGraph::get_subsequence(a0, a1, a2);
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::follow_edges_impl\"");
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::ExpandingOverlayGraph *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"HandleGraph::for_each_handle_impl\"");
	}
};

void bind_handlegraph_expanding_overlay_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::ExpandingOverlayGraph file:handlegraph/expanding_overlay_graph.hpp line:17
		pybind11::class_<handlegraph::ExpandingOverlayGraph, std::shared_ptr<handlegraph::ExpandingOverlayGraph>, PyCallBack_handlegraph_ExpandingOverlayGraph, handlegraph::HandleGraph> cl(M("handlegraph"), "ExpandingOverlayGraph", "This is the interface for a graph that represents a transformation of some underlying\n HandleGraph where every node in the overlay corresponds to a node in the underlying\n graph, but where more than one node in the overlay can map to the same underlying node.");
		cl.def(pybind11::init<PyCallBack_handlegraph_ExpandingOverlayGraph const &>());
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_ExpandingOverlayGraph(); } ) );
		cl.def("get_underlying_handle", (struct handlegraph::handle_t (handlegraph::ExpandingOverlayGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::ExpandingOverlayGraph::get_underlying_handle, "Returns the handle in the underlying graph that corresponds to a handle in the\n overlay\n\nC++: handlegraph::ExpandingOverlayGraph::get_underlying_handle(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("assign", (class handlegraph::ExpandingOverlayGraph & (handlegraph::ExpandingOverlayGraph::*)(const class handlegraph::ExpandingOverlayGraph &)) &handlegraph::ExpandingOverlayGraph::operator=, "C++: handlegraph::ExpandingOverlayGraph::operator=(const class handlegraph::ExpandingOverlayGraph &) --> class handlegraph::ExpandingOverlayGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
