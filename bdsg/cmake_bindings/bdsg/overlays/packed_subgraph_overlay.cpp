#include <bdsg/overlays/packed_subgraph_overlay.hpp>
#include <bdsg/overlays/path_position_overlays.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/path_handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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
#include <bdsg/internal/binder_hook_compile.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

// bdsg::PackedSubgraphOverlay file:bdsg/overlays/packed_subgraph_overlay.hpp line:27
struct PyCallBack_bdsg_PackedSubgraphOverlay : public bdsg::PackedSubgraphOverlay {
	using bdsg::PackedSubgraphOverlay::PackedSubgraphOverlay;

	bool has_node(long long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PackedSubgraphOverlay::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PackedSubgraphOverlay::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PackedSubgraphOverlay::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PackedSubgraphOverlay::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PackedSubgraphOverlay::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PackedSubgraphOverlay::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PackedSubgraphOverlay::get_sequence(a0);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::overload_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return PackedSubgraphOverlay::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PackedSubgraphOverlay::get_subsequence(a0, a1, a2);
	}
	unsigned long get_node_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PackedSubgraphOverlay::get_node_count();
	}
	long long min_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PackedSubgraphOverlay::min_node_id();
	}
	long long max_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PackedSubgraphOverlay::max_node_id();
	}
	struct handlegraph::handle_t get_underlying_handle(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_underlying_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PackedSubgraphOverlay::get_underlying_handle(a0);
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_degree");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "has_edge");
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
	unsigned long get_edge_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_edge_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_edge_count();
	}
	unsigned long get_total_length() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "get_total_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_total_length();
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "follow_edges_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PackedSubgraphOverlay *>(this), "for_each_handle_impl");
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

// bdsg::PositionOverlay file:bdsg/overlays/path_position_overlays.hpp line:30
struct PyCallBack_bdsg_PositionOverlay : public bdsg::PositionOverlay {
	using bdsg::PositionOverlay::PositionOverlay;

	bool has_node(long long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_sequence(a0);
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_degree(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_edge(a0, a1);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::overload_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return PositionOverlay::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_subsequence(a0, a1, a2);
	}
	unsigned long get_node_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_node_count();
	}
	long long min_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::min_node_id();
	}
	long long max_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::max_node_id();
	}
	unsigned long get_path_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_path_count();
	}
	bool has_path(const std::string & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_path(a0);
	}
	struct handlegraph::path_handle_t get_path_handle(const std::string & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PositionOverlay::get_path_handle(a0);
	}
	std::string get_path_name(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_path_name(a0);
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::get_is_circular(a0);
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_step_count(a0);
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_handle_of_step(a0);
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_begin(a0);
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_end(a0);
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_back(a0);
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_front_end(a0);
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_next_step(a0);
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_previous_step(a0);
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_next_step(a0);
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_previous_step(a0);
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PositionOverlay::get_path_handle_of_step(a0);
	}
	unsigned long get_path_length(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_path_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_path_length(a0);
	}
	unsigned long get_position_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_position_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_position_of_step(a0);
	}
	struct handlegraph::step_handle_t get_step_at_position(const struct handlegraph::path_handle_t & a0, const unsigned long & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_step_at_position");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_step_at_position(a0, a1);
	}
	struct handlegraph::handle_t get_underlying_handle(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_underlying_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_underlying_handle(a0);
	}
	bool for_each_step_position_on_handle(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &, const bool &, const unsigned long &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "for_each_step_position_on_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathPositionHandleGraph::for_each_step_position_on_handle(a0, a1);
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "for_each_path_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::for_each_path_handle_impl\"");
	}
	bool for_each_step_on_handle_impl(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "for_each_step_on_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::for_each_step_on_handle_impl\"");
	}
	class std::vector<handlegraph::step_handle_t> steps_of_handle(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "steps_of_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::step_handle_t>>::value) {
				static pybind11::detail::overload_caster_t<class std::vector<handlegraph::step_handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::step_handle_t>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::vector<handlegraph::step_handle_t>>(std::move(o));
		}
		return PathHandleGraph::steps_of_handle(a0, a1);
	}
	bool is_empty(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "is_empty");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraph::is_empty(a0);
	}
	unsigned long get_edge_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_edge_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_edge_count();
	}
	unsigned long get_total_length() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "get_total_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraph::get_total_length();
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "follow_edges_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::PositionOverlay *>(this), "for_each_handle_impl");
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

void bind_bdsg_overlays_packed_subgraph_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::PackedSubgraphOverlay file:bdsg/overlays/packed_subgraph_overlay.hpp line:27
		pybind11::class_<bdsg::PackedSubgraphOverlay, std::shared_ptr<bdsg::PackedSubgraphOverlay>, PyCallBack_bdsg_PackedSubgraphOverlay, handlegraph::ExpandingOverlayGraph> cl(M("bdsg"), "PackedSubgraphOverlay", "");
		cl.def( pybind11::init<const class handlegraph::HandleGraph *>(), pybind11::arg("graph") );

		cl.def( pybind11::init( [](){ return new bdsg::PackedSubgraphOverlay(); }, [](){ return new PyCallBack_bdsg_PackedSubgraphOverlay(); } ) );
		cl.def( pybind11::init( [](PyCallBack_bdsg_PackedSubgraphOverlay const &o){ return new PyCallBack_bdsg_PackedSubgraphOverlay(o); } ) );
		cl.def( pybind11::init( [](bdsg::PackedSubgraphOverlay const &o){ return new bdsg::PackedSubgraphOverlay(o); } ) );
		cl.def("add_node", (void (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &)) &bdsg::PackedSubgraphOverlay::add_node, "Add a node from the parent graph to the subgraph. Handle must\n come from the parent graph. Has no effect if the handle is already in\n the subgraph.\n\nC++: bdsg::PackedSubgraphOverlay::add_node(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("remove_node", (void (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &)) &bdsg::PackedSubgraphOverlay::remove_node, "Remove a node from the subgraph. Handle must come from the\n parent graph. Has no effect if the handle is not in the subgraph.\n\nC++: bdsg::PackedSubgraphOverlay::remove_node(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("has_node", (bool (bdsg::PackedSubgraphOverlay::*)(long long) const) &bdsg::PackedSubgraphOverlay::has_node, "Method to check if a node exists by ID\n\nC++: bdsg::PackedSubgraphOverlay::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::PackedSubgraphOverlay const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::PackedSubgraphOverlay::*)(const long long &, bool) const) &bdsg::PackedSubgraphOverlay::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: bdsg::PackedSubgraphOverlay::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::get_id, "Get the ID from a handle\n\nC++: bdsg::PackedSubgraphOverlay::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::get_is_reverse, "Get the orientation of a handle\n\nC++: bdsg::PackedSubgraphOverlay::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: bdsg::PackedSubgraphOverlay::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::get_length, "Get the length of a node\n\nC++: bdsg::PackedSubgraphOverlay::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::get_sequence, "Get the sequence of a node, presented in the handle's local forward orientation.\n\nC++: bdsg::PackedSubgraphOverlay::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_base", (char (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &, unsigned long) const) &bdsg::PackedSubgraphOverlay::get_base, "Returns one base of a handle's sequence, in the orientation of the\n handle.\n\nC++: bdsg::PackedSubgraphOverlay::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &bdsg::PackedSubgraphOverlay::get_subsequence, "Returns a substring of a handle's sequence, in the orientation of the\n handle. If the indicated substring would extend beyond the end of the\n handle's sequence, the return value is truncated to the sequence's end.\n\nC++: bdsg::PackedSubgraphOverlay::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("get_node_count", (unsigned long (bdsg::PackedSubgraphOverlay::*)() const) &bdsg::PackedSubgraphOverlay::get_node_count, "Return the number of nodes in the graph\n\nC++: bdsg::PackedSubgraphOverlay::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::PackedSubgraphOverlay::*)() const) &bdsg::PackedSubgraphOverlay::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::PackedSubgraphOverlay::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::PackedSubgraphOverlay::*)() const) &bdsg::PackedSubgraphOverlay::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::PackedSubgraphOverlay::max_node_id() const --> long long");
		cl.def("get_underlying_handle", (struct handlegraph::handle_t (bdsg::PackedSubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PackedSubgraphOverlay::get_underlying_handle, "Returns the handle in the underlying graph that corresponds to a handle in the\n overlay\n\nC++: bdsg::PackedSubgraphOverlay::get_underlying_handle(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("assign", (class bdsg::PackedSubgraphOverlay & (bdsg::PackedSubgraphOverlay::*)(const class bdsg::PackedSubgraphOverlay &)) &bdsg::PackedSubgraphOverlay::operator=, "C++: bdsg::PackedSubgraphOverlay::operator=(const class bdsg::PackedSubgraphOverlay &) --> class bdsg::PackedSubgraphOverlay &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::PositionOverlay file:bdsg/overlays/path_position_overlays.hpp line:30
		pybind11::class_<bdsg::PositionOverlay, std::shared_ptr<bdsg::PositionOverlay>, PyCallBack_bdsg_PositionOverlay, handlegraph::PathPositionHandleGraph, handlegraph::ExpandingOverlayGraph> cl(M("bdsg"), "PositionOverlay", "");
		cl.def( pybind11::init<class handlegraph::PathHandleGraph *>(), pybind11::arg("graph") );

		cl.def( pybind11::init( [](){ return new bdsg::PositionOverlay(); }, [](){ return new PyCallBack_bdsg_PositionOverlay(); } ) );
		cl.def( pybind11::init( [](PyCallBack_bdsg_PositionOverlay const &o){ return new PyCallBack_bdsg_PositionOverlay(o); } ) );
		cl.def( pybind11::init( [](bdsg::PositionOverlay const &o){ return new bdsg::PositionOverlay(o); } ) );
		cl.def("has_node", (bool (bdsg::PositionOverlay::*)(long long) const) &bdsg::PositionOverlay::has_node, "Method to check if a node exists by ID\n\nC++: bdsg::PositionOverlay::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::PositionOverlay const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::PositionOverlay::*)(const long long &, bool) const) &bdsg::PositionOverlay::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: bdsg::PositionOverlay::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::get_id, "Get the ID from a handle\n\nC++: bdsg::PositionOverlay::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::get_is_reverse, "Get the orientation of a handle\n\nC++: bdsg::PositionOverlay::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: bdsg::PositionOverlay::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::get_length, "Get the length of a node\n\nC++: bdsg::PositionOverlay::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::get_sequence, "Get the sequence of a node, presented in the handle's local forward orientation.\n\nC++: bdsg::PositionOverlay::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_degree", (unsigned long (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::PositionOverlay::get_degree, "Get the number of edges on the right (go_left = false) or left (go_left\n = true) side of the given handle. The default implementation is O(n) in\n the number of edges returned, but graph implementations that track this\n information more efficiently can override this method.\n\nC++: bdsg::PositionOverlay::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("has_edge", (bool (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::has_edge, "Returns true if there is an edge that allows traversal from the left\n handle to the right handle. By default O(n) in the number of edges\n on left, but can be overridden with more efficient implementations.\n\nC++: bdsg::PositionOverlay::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("get_base", (char (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &, unsigned long) const) &bdsg::PositionOverlay::get_base, "Returns one base of a handle's sequence, in the orientation of the\n handle.\n\nC++: bdsg::PositionOverlay::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &bdsg::PositionOverlay::get_subsequence, "Returns a substring of a handle's sequence, in the orientation of the\n handle. If the indicated substring would extend beyond the end of the\n handle's sequence, the return value is truncated to the sequence's end.\n\nC++: bdsg::PositionOverlay::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("get_node_count", (unsigned long (bdsg::PositionOverlay::*)() const) &bdsg::PositionOverlay::get_node_count, "Return the number of nodes in the graph\n\nC++: bdsg::PositionOverlay::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::PositionOverlay::*)() const) &bdsg::PositionOverlay::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::PositionOverlay::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::PositionOverlay::*)() const) &bdsg::PositionOverlay::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::PositionOverlay::max_node_id() const --> long long");
		cl.def("get_path_count", (unsigned long (bdsg::PositionOverlay::*)() const) &bdsg::PositionOverlay::get_path_count, "Returns the number of paths stored in the graph\n\nC++: bdsg::PositionOverlay::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (bdsg::PositionOverlay::*)(const std::string &) const) &bdsg::PositionOverlay::has_path, "Determine if a path name exists and is legal to get a path handle for.\n\nC++: bdsg::PositionOverlay::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (bdsg::PositionOverlay::*)(const std::string &) const) &bdsg::PositionOverlay::get_path_handle, "Look up the path handle for the given path name.\n The path with that name must exist.\n\nC++: bdsg::PositionOverlay::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::get_path_name, "Look up the name of a path from a handle to it\n\nC++: bdsg::PositionOverlay::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::get_is_circular, "Look up whether a path is circular\n\nC++: bdsg::PositionOverlay::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::get_step_count, "Returns the number of node steps in the path\n\nC++: bdsg::PositionOverlay::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::get_handle_of_step, "Get a node handle (node ID and orientation) from a handle to an step on a path\n\nC++: bdsg::PositionOverlay::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::path_begin, "Get a handle to the first step, or in a circular path to an arbitrary step\n considered \"first\". If the path is empty, returns the past-the-last step\n returned by path_end.\n\nC++: bdsg::PositionOverlay::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::path_end, "Get a handle to a fictitious position past the end of a path. This position is\n return by get_next_step for the final step in a path in a non-circular path.\n Note that get_next_step will *NEVER* return this value for a circular path.\n\nC++: bdsg::PositionOverlay::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::path_back, "Get a handle to the last step, which will be an arbitrary step in a circular path that\n we consider \"last\" based on our construction of the path. If the path is empty\n then the implementation must return the same value as path_front_end().\n\nC++: bdsg::PositionOverlay::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::path_front_end, "Get a handle to a fictitious position before the beginning of a path. This position is\n return by get_previous_step for the first step in a path in a non-circular path.\n Note: get_previous_step will *NEVER* return this value for a circular path.\n\nC++: bdsg::PositionOverlay::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::has_next_step, "Returns true if the step is not the last step in a non-circular path.\n\nC++: bdsg::PositionOverlay::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::has_previous_step, "Returns true if the step is not the first step in a non-circular path.\n\nC++: bdsg::PositionOverlay::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::get_next_step, "Returns a handle to the next step on the path. If the given step is the final step\n of a non-circular path, returns the past-the-last step that is also returned by\n path_end. In a circular path, the \"last\" step will loop around to the \"first\" (i.e.\n the one returned by path_begin).\n Note: to iterate over each step one time, even in a circular path, consider\n for_each_step_in_path.\n\nC++: bdsg::PositionOverlay::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::get_previous_step, "Returns a handle to the previous step on the path. If the given step is the first\n step of a non-circular path, this method has undefined behavior. In a circular path,\n it will loop around from the \"first\" step (i.e. the one returned by path_begin) to\n the \"last\" step.\n Note: to iterate over each step one time, even in a circular path, consider\n for_each_step_in_path.\n\nC++: bdsg::PositionOverlay::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::get_path_handle_of_step, "Returns a handle to the path that an step is on\n\nC++: bdsg::PositionOverlay::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_length", (unsigned long (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PositionOverlay::get_path_length, "Returns the length of a path measured in bases of sequence.\n\nC++: bdsg::PositionOverlay::get_path_length(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_position_of_step", (unsigned long (bdsg::PositionOverlay::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PositionOverlay::get_position_of_step, "Returns the position along the path of the beginning of this step measured in\n bases of sequence. In a circular path, positions start at the step returned by\n path_begin().\n\nC++: bdsg::PositionOverlay::get_position_of_step(const struct handlegraph::step_handle_t &) const --> unsigned long", pybind11::arg("step"));
		cl.def("get_step_at_position", (struct handlegraph::step_handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::path_handle_t &, const unsigned long &) const) &bdsg::PositionOverlay::get_step_at_position, "Returns the step at this position, measured in bases of sequence starting at\n the step returned by path_begin(). If the position is past the end of the\n path, returns path_end().\n\nC++: bdsg::PositionOverlay::get_step_at_position(const struct handlegraph::path_handle_t &, const unsigned long &) const --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("position"));
		cl.def("get_underlying_handle", (struct handlegraph::handle_t (bdsg::PositionOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::PositionOverlay::get_underlying_handle, "Returns the handle in the underlying graph that corresponds to a handle in the\n overlay\n\nC++: bdsg::PositionOverlay::get_underlying_handle(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("assign", (class bdsg::PositionOverlay & (bdsg::PositionOverlay::*)(const class bdsg::PositionOverlay &)) &bdsg::PositionOverlay::operator=, "C++: bdsg::PositionOverlay::operator=(const class bdsg::PositionOverlay &) --> class bdsg::PositionOverlay &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
