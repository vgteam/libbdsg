#include <bdsg/overlays/path_position_overlays.hpp>
#include <bdsg/overlays/subgraph_overlay.hpp>
#include <functional>
#include <handlegraph/deletable_handle_graph.hpp>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/mutable_path_handle_graph.hpp>
#include <handlegraph/mutable_path_metadata.hpp>
#include <handlegraph/path_handle_graph.hpp>
#include <handlegraph/path_metadata.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

// bdsg::MutablePositionOverlay file:bdsg/overlays/path_position_overlays.hpp line:261
struct PyCallBack_bdsg_MutablePositionOverlay : public bdsg::MutablePositionOverlay {
	using bdsg::MutablePositionOverlay::MutablePositionOverlay;

	struct handlegraph::handle_t create_handle(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePositionOverlay::create_handle(a0);
	}
	struct handlegraph::handle_t create_handle(const std::string & a0, const long long & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePositionOverlay::create_handle(a0, a1);
	}
	void destroy_handle(const struct handlegraph::handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "destroy_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::destroy_handle(a0);
	}
	void create_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "create_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::create_edge(a0, a1);
	}
	void destroy_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "destroy_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::destroy_edge(a0, a1);
	}
	void clear() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "clear");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::clear();
	}
	struct handlegraph::handle_t apply_orientation(const struct handlegraph::handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "apply_orientation");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePositionOverlay::apply_orientation(a0);
	}
	class std::vector<handlegraph::handle_t> divide_handle(const struct handlegraph::handle_t & a0, const class std::vector<unsigned long> & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "divide_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::handle_t>>::value) {
				static pybind11::detail::override_caster_t<class std::vector<handlegraph::handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::handle_t>>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<class std::vector<handlegraph::handle_t>>(std::move(o));
		}
		return MutablePositionOverlay::divide_handle(a0, a1);
	}
	void optimize(bool a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "optimize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::optimize(a0);
	}
	bool apply_ordering(const class std::vector<handlegraph::handle_t> & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "apply_ordering");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return MutablePositionOverlay::apply_ordering(a0, a1);
	}
	void set_id_increment(const long long & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "set_id_increment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::set_id_increment(a0);
	}
	void increment_node_ids(long long a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "increment_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::increment_node_ids(a0);
	}
	void reassign_node_ids(const class std::function<long long (const long long &)> & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "reassign_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::reassign_node_ids(a0);
	}
	void destroy_path(const struct handlegraph::path_handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "destroy_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::destroy_path(a0);
	}
	struct handlegraph::path_handle_t create_path_handle(const std::string & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "create_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return MutablePositionOverlay::create_path_handle(a0, a1);
	}
	struct handlegraph::step_handle_t append_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "append_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return MutablePositionOverlay::append_step(a0, a1);
	}
	struct handlegraph::step_handle_t prepend_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "prepend_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return MutablePositionOverlay::prepend_step(a0, a1);
	}
	using _binder_ret_0 = struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>;
	_binder_ret_0 rewrite_segment(const struct handlegraph::step_handle_t & a0, const struct handlegraph::step_handle_t & a1, const class std::vector<handlegraph::handle_t> & a2) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "rewrite_segment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<_binder_ret_0>::value) {
				static pybind11::detail::override_caster_t<_binder_ret_0> caster;
				return pybind11::detail::cast_ref<_binder_ret_0>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<_binder_ret_0>(std::move(o));
		}
		return MutablePositionOverlay::rewrite_segment(a0, a1, a2);
	}
	void set_circularity(const struct handlegraph::path_handle_t & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "set_circularity");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePositionOverlay::set_circularity(a0, a1);
	}
	bool has_node(long long a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_sequence(a0);
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_degree(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_edge(a0, a1);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::override_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return PositionOverlay::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_subsequence(a0, a1, a2);
	}
	unsigned long get_node_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_node_count();
	}
	long long min_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::min_node_id();
	}
	long long max_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return PositionOverlay::max_node_id();
	}
	unsigned long get_path_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_path_count();
	}
	bool has_path(const std::string & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_path(a0);
	}
	struct handlegraph::path_handle_t get_path_handle(const std::string & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PositionOverlay::get_path_handle(a0);
	}
	std::string get_path_name(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PositionOverlay::get_path_name(a0);
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::get_is_circular(a0);
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_step_count(a0);
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_handle_of_step(a0);
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_begin(a0);
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_end(a0);
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_back(a0);
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::path_front_end(a0);
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_next_step(a0);
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PositionOverlay::has_previous_step(a0);
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_next_step(a0);
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_previous_step(a0);
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PositionOverlay::get_path_handle_of_step(a0);
	}
	unsigned long get_path_length(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_path_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_path_length(a0);
	}
	unsigned long get_position_of_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_position_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PositionOverlay::get_position_of_step(a0);
	}
	struct handlegraph::step_handle_t get_step_at_position(const struct handlegraph::path_handle_t & a0, const unsigned long & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_step_at_position");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PositionOverlay::get_step_at_position(a0, a1);
	}
	struct handlegraph::handle_t get_underlying_handle(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_underlying_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PositionOverlay::get_underlying_handle(a0);
	}
	bool for_each_step_position_on_handle(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &, const bool &, const unsigned long &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "for_each_step_position_on_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathPositionHandleGraph::for_each_step_position_on_handle(a0, a1);
	}
	unsigned long get_step_count(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathHandleGraph::get_step_count(a0);
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "for_each_path_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::for_each_path_handle_impl\"");
	}
	bool for_each_step_on_handle_impl(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "for_each_step_on_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::for_each_step_on_handle_impl\"");
	}
	class std::vector<handlegraph::step_handle_t> steps_of_handle(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "steps_of_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::step_handle_t>>::value) {
				static pybind11::detail::override_caster_t<class std::vector<handlegraph::step_handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::step_handle_t>>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<class std::vector<handlegraph::step_handle_t>>(std::move(o));
		}
		return PathHandleGraph::steps_of_handle(a0, a1);
	}
	bool is_empty(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "is_empty");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraph::is_empty(a0);
	}
	unsigned long get_edge_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_edge_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_total_length");
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
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "follow_edges_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "for_each_handle_impl");
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
	enum handlegraph::PathSense get_sense(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_sense");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::PathSense>::value) {
				static pybind11::detail::override_caster_t<enum handlegraph::PathSense> caster;
				return pybind11::detail::cast_ref<enum handlegraph::PathSense>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<enum handlegraph::PathSense>(std::move(o));
		}
		return PathMetadata::get_sense(a0);
	}
	std::string get_sample_name(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_sample_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PathMetadata::get_sample_name(a0);
	}
	std::string get_locus_name(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_locus_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PathMetadata::get_locus_name(a0);
	}
	unsigned long get_haplotype(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_haplotype");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathMetadata::get_haplotype(a0);
	}
	unsigned long get_phase_block(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_phase_block");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathMetadata::get_phase_block(a0);
	}
	using _binder_ret_1 = struct std::pair<unsigned long, unsigned long>;
	_binder_ret_1 get_subrange(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "get_subrange");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<_binder_ret_1>::value) {
				static pybind11::detail::override_caster_t<_binder_ret_1> caster;
				return pybind11::detail::cast_ref<_binder_ret_1>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<_binder_ret_1>(std::move(o));
		}
		return PathMetadata::get_subrange(a0);
	}
	bool for_each_step_of_sense_impl(const struct handlegraph::handle_t & a0, const enum handlegraph::PathSense & a1, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "for_each_step_of_sense_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathMetadata::for_each_step_of_sense_impl(a0, a1, a2);
	}
	struct handlegraph::handle_t change_sequence(const struct handlegraph::handle_t & a0, const std::string & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "change_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraph::change_sequence(a0, a1);
	}
	void destroy_paths(const class std::vector<handlegraph::path_handle_t> & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "destroy_paths");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathHandleGraph::destroy_paths(a0);
	}
	struct handlegraph::path_handle_t rename_path(const struct handlegraph::path_handle_t & a0, const std::string & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "rename_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return MutablePathHandleGraph::rename_path(a0, a1);
	}
	void pop_front_step(const struct handlegraph::path_handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "pop_front_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathHandleGraph::pop_front_step(a0);
	}
	void pop_back_step(const struct handlegraph::path_handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "pop_back_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathHandleGraph::pop_back_step(a0);
	}
	struct handlegraph::path_handle_t create_path(const enum handlegraph::PathSense & a0, const std::string & a1, const std::string & a2, const unsigned long & a3, const unsigned long & a4, const struct std::pair<unsigned long, unsigned long> & a5, bool a6) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "create_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2, a3, a4, a5, a6);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return MutablePathMetadata::create_path(a0, a1, a2, a3, a4, a5, a6);
	}
	void increment_node_ids(long a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "increment_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutableHandleGraph::increment_node_ids(a0);
	}
	struct handlegraph::handle_t truncate_handle(const struct handlegraph::handle_t & a0, bool a1, unsigned long a2) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePositionOverlay *>(this), "truncate_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return DeletableHandleGraph::truncate_handle(a0, a1, a2);
	}
};

// bdsg::SubgraphOverlay file:bdsg/overlays/subgraph_overlay.hpp line:30
struct PyCallBack_bdsg_SubgraphOverlay : public bdsg::SubgraphOverlay {
	using bdsg::SubgraphOverlay::SubgraphOverlay;

	bool has_node(long long a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SubgraphOverlay::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return SubgraphOverlay::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return SubgraphOverlay::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SubgraphOverlay::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return SubgraphOverlay::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return SubgraphOverlay::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return SubgraphOverlay::get_sequence(a0);
	}
	unsigned long get_node_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return SubgraphOverlay::get_node_count();
	}
	long long min_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return SubgraphOverlay::min_node_id();
	}
	long long max_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return SubgraphOverlay::max_node_id();
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SubgraphOverlay::follow_edges_impl(a0, a1, a2);
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SubgraphOverlay::for_each_handle_impl(a0, a1);
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_degree");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "has_edge");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_edge_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_total_length");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_base");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SubgraphOverlay *>(this), "get_subsequence");
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
};

void bind_bdsg_overlays_path_position_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::MutablePositionOverlay file:bdsg/overlays/path_position_overlays.hpp line:261
		pybind11::class_<bdsg::MutablePositionOverlay, std::shared_ptr<bdsg::MutablePositionOverlay>, PyCallBack_bdsg_MutablePositionOverlay, bdsg::PositionOverlay, handlegraph::MutablePathDeletableHandleGraph> cl(M("bdsg"), "MutablePositionOverlay", "");
		cl.def( pybind11::init<class handlegraph::MutablePathDeletableHandleGraph *>(), pybind11::arg("graph") );

		cl.def( pybind11::init( [](){ return new bdsg::MutablePositionOverlay(); }, [](){ return new PyCallBack_bdsg_MutablePositionOverlay(); } ) );
		cl.def( pybind11::init( [](PyCallBack_bdsg_MutablePositionOverlay const &o){ return new PyCallBack_bdsg_MutablePositionOverlay(o); } ) );
		cl.def( pybind11::init( [](bdsg::MutablePositionOverlay const &o){ return new bdsg::MutablePositionOverlay(o); } ) );
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::MutablePositionOverlay::*)(const std::string &)) &bdsg::MutablePositionOverlay::create_handle, "Create a new node with the given sequence and return the handle.\n\nC++: bdsg::MutablePositionOverlay::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::MutablePositionOverlay::*)(const std::string &, const long long &)) &bdsg::MutablePositionOverlay::create_handle, "Create a new node with the given id and sequence, then return the handle.\n\nC++: bdsg::MutablePositionOverlay::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("destroy_handle", (void (bdsg::MutablePositionOverlay::*)(const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::destroy_handle, "Remove the node belonging to the given handle and all of its edges.\n Does not update any stored paths.\n Invalidates the destroyed handle.\n May be called during serial for_each_handle iteration **ONLY** on the node being iterated.\n May **NOT** be called during parallel for_each_handle iteration.\n May **NOT** be called on the node from which edges are being followed during follow_edges.\n\nC++: bdsg::MutablePositionOverlay::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("create_edge", (void (bdsg::MutablePositionOverlay::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::create_edge, "Create an edge connecting the given handles in the given order and orientations.\n Ignores existing edges.\n\nC++: bdsg::MutablePositionOverlay::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (bdsg::MutablePositionOverlay::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::destroy_edge, "Remove the edge connecting the given handles in the given order and orientations.\n Ignores nonexistent edges.\n Does not update any stored paths.\n\nC++: bdsg::MutablePositionOverlay::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("clear", (void (bdsg::MutablePositionOverlay::*)()) &bdsg::MutablePositionOverlay::clear, "Remove all nodes and edges. Does not update any stored paths.\n\nC++: bdsg::MutablePositionOverlay::clear() --> void");
		cl.def("apply_orientation", (struct handlegraph::handle_t (bdsg::MutablePositionOverlay::*)(const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::apply_orientation, "Alter the node that the given handle corresponds to so the orientation\n indicated by the handle becomes the node's local forward orientation.\n Rewrites all edges pointing to the node and the node's sequence to\n reflect this. Invalidates all handles to the node (including the one\n passed). Returns a new, valid handle to the node in its new forward\n orientation. Note that it is possible for the node's ID to change.\n Does not update any stored paths. May change the ordering of the underlying\n graph.\n\nC++: bdsg::MutablePositionOverlay::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (class std::vector<handlegraph::handle_t> (bdsg::MutablePositionOverlay::*)(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &)) &bdsg::MutablePositionOverlay::divide_handle, "Split a handle's underlying node at the given offsets in the handle's\n orientation. Returns all of the handles to the parts. Other handles to\n the node being split may be invalidated. The split pieces stay in the\n same local forward orientation as the original node, but the returned\n handles come in the order and orientation appropriate for the handle\n passed in.\n Updates stored paths.\n\nC++: bdsg::MutablePositionOverlay::divide_handle(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &) --> class std::vector<handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offsets"));
		cl.def("optimize", [](bdsg::MutablePositionOverlay &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (bdsg::MutablePositionOverlay::*)(bool)) &bdsg::MutablePositionOverlay::optimize, "Adjust the representation of the graph in memory to improve performance.\n Optionally, allow the node IDs to be reassigned to further improve\n performance.\n Note: Ideally, this method is called one time once there is expected to be\n few graph modifications in the future.\n\nC++: bdsg::MutablePositionOverlay::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("apply_ordering", [](bdsg::MutablePositionOverlay &o, const class std::vector<handlegraph::handle_t> & a0) -> bool { return o.apply_ordering(a0); }, "", pybind11::arg("order"));
		cl.def("apply_ordering", (bool (bdsg::MutablePositionOverlay::*)(const class std::vector<handlegraph::handle_t> &, bool)) &bdsg::MutablePositionOverlay::apply_ordering, "Reorder the graph's internal structure to match that given.\n This sets the order that is used for iteration in functions like for_each_handle.\n If compact_ids is true, may (but will not necessarily) compact the id space of the graph to match the ordering, from 1->|ordering|.\n In other cases, node IDs will be preserved.\n This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.\n This may invalidate outstanding handles.\n Returns true if node IDs actually were adjusted to match the given order, and false if they remain unchanged.\n\nC++: bdsg::MutablePositionOverlay::apply_ordering(const class std::vector<handlegraph::handle_t> &, bool) --> bool", pybind11::arg("order"), pybind11::arg("compact_ids"));
		cl.def("set_id_increment", (void (bdsg::MutablePositionOverlay::*)(const long long &)) &bdsg::MutablePositionOverlay::set_id_increment, "No-op function (required by MutableHandleGraph interface)\n\nC++: bdsg::MutablePositionOverlay::set_id_increment(const long long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (bdsg::MutablePositionOverlay::*)(long long)) &bdsg::MutablePositionOverlay::increment_node_ids, "Add the given value to all node IDs.\n Has a default implementation in terms of reassign_node_ids, but can be\n implemented more efficiently in some graphs.\n\nC++: bdsg::MutablePositionOverlay::increment_node_ids(long long) --> void", pybind11::arg("increment"));
		cl.def("reassign_node_ids", (void (bdsg::MutablePositionOverlay::*)(const class std::function<long long (const long long &)> &)) &bdsg::MutablePositionOverlay::reassign_node_ids, "Renumber all node IDs using the given function, which, given an old ID, returns the new ID.\n Modifies the graph in place. Invalidates all outstanding handles.\n If the graph supports paths, they also must be updated.\n The mapping function may return 0. In this case, the input ID will\n remain unchanged. The mapping function should not return any ID for\n which it would return 0.\n\nC++: bdsg::MutablePositionOverlay::reassign_node_ids(const class std::function<long long (const long long &)> &) --> void", pybind11::arg("get_new_id"));
		cl.def("destroy_path", (void (bdsg::MutablePositionOverlay::*)(const struct handlegraph::path_handle_t &)) &bdsg::MutablePositionOverlay::destroy_path, "Destroy the given path. Invalidates handles to the path and its node steps.\n\nC++: bdsg::MutablePositionOverlay::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("create_path_handle", [](bdsg::MutablePositionOverlay &o, const std::string & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (bdsg::MutablePositionOverlay::*)(const std::string &, bool)) &bdsg::MutablePositionOverlay::create_path_handle, "Create a path with the given name. The caller must ensure that no path\n with the given name exists already, or the behavior is undefined.\n Returns a handle to the created empty path. Handles to other paths must\n remain valid.\n\nC++: bdsg::MutablePositionOverlay::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("append_step", (struct handlegraph::step_handle_t (bdsg::MutablePositionOverlay::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::append_step, "Append a visit to a node to the given path. Returns a handle to the new\n final step on the path which is appended. Handles to prior\n steps on the path, and to other paths, must remain valid.\n\nC++: bdsg::MutablePositionOverlay::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (bdsg::MutablePositionOverlay::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePositionOverlay::prepend_step, "Prepend a visit to a node to the given path. Returns a handle to the new\n first step on the path which is appended. If the path is cirular, the new\n step is placed between the steps considered \"last\" and \"first\" by the\n method path_begin. Handles to later steps on the path, and to other paths,\n must remain valid.\n\nC++: bdsg::MutablePositionOverlay::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_prepend"));
		cl.def("rewrite_segment", (struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t> (bdsg::MutablePositionOverlay::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &)) &bdsg::MutablePositionOverlay::rewrite_segment, "WARNING: online path position indexing is inefficient for this method.\n\n Delete a segment of a path and rewrite it as some other sequence of steps. Returns a pair\n of step_handle_t's that indicate the range of the new segment in the path. The segment to\n delete should be designated by the first and the past-the-last step handle.  If the step\n that is returned by path_begin is deleted, path_begin will now return the first step from\n the new segment or, in the case that the new segment is empty, segment_end.\n\nC++: bdsg::MutablePositionOverlay::rewrite_segment(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &) --> struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>", pybind11::arg("segment_begin"), pybind11::arg("segment_end"), pybind11::arg("new_segment"));
		cl.def("set_circularity", (void (bdsg::MutablePositionOverlay::*)(const struct handlegraph::path_handle_t &, bool)) &bdsg::MutablePositionOverlay::set_circularity, "Make a path circular or non-circular. If the path is becoming circular, the\n last step is joined to the first step. If the path is becoming linear, the\n step considered \"last\" is unjoined from the step considered \"first\" according\n to the method path_begin.\n\nC++: bdsg::MutablePositionOverlay::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path"), pybind11::arg("circular"));
		cl.def("assign", (class bdsg::MutablePositionOverlay & (bdsg::MutablePositionOverlay::*)(const class bdsg::MutablePositionOverlay &)) &bdsg::MutablePositionOverlay::operator=, "C++: bdsg::MutablePositionOverlay::operator=(const class bdsg::MutablePositionOverlay &) --> class bdsg::MutablePositionOverlay &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::SubgraphOverlay file:bdsg/overlays/subgraph_overlay.hpp line:30
		pybind11::class_<bdsg::SubgraphOverlay, std::shared_ptr<bdsg::SubgraphOverlay>, PyCallBack_bdsg_SubgraphOverlay, handlegraph::HandleGraph> cl(M("bdsg"), "SubgraphOverlay", "Present a HandleGraph that is a backing HandleGraph but restricted\n to a subset of nodes.  It won't give handles to nodes not in the \n subset, but it's not bulletproof: handles from outside the subset\n won't undergo any special checks.  ");
		cl.def( pybind11::init( [](){ return new bdsg::SubgraphOverlay(); }, [](){ return new PyCallBack_bdsg_SubgraphOverlay(); } ) );
		cl.def( pybind11::init( [](PyCallBack_bdsg_SubgraphOverlay const &o){ return new PyCallBack_bdsg_SubgraphOverlay(o); } ) );
		cl.def( pybind11::init( [](bdsg::SubgraphOverlay const &o){ return new bdsg::SubgraphOverlay(o); } ) );
		cl.def("has_node", (bool (bdsg::SubgraphOverlay::*)(long long) const) &bdsg::SubgraphOverlay::has_node, "Method to check if a node exists by ID\n\nC++: bdsg::SubgraphOverlay::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::SubgraphOverlay const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::SubgraphOverlay::*)(const long long &, bool) const) &bdsg::SubgraphOverlay::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: bdsg::SubgraphOverlay::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::SubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::SubgraphOverlay::get_id, "Get the ID from a handle\n\nC++: bdsg::SubgraphOverlay::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::SubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::SubgraphOverlay::get_is_reverse, "Get the orientation of a handle\n\nC++: bdsg::SubgraphOverlay::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::SubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::SubgraphOverlay::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: bdsg::SubgraphOverlay::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::SubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::SubgraphOverlay::get_length, "Get the length of a node\n\nC++: bdsg::SubgraphOverlay::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::SubgraphOverlay::*)(const struct handlegraph::handle_t &) const) &bdsg::SubgraphOverlay::get_sequence, "Get the sequence of a node, presented in the handle's local forward\n orientation.\n\nC++: bdsg::SubgraphOverlay::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (bdsg::SubgraphOverlay::*)() const) &bdsg::SubgraphOverlay::get_node_count, "Return the number of nodes in the graph\n\nC++: bdsg::SubgraphOverlay::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::SubgraphOverlay::*)() const) &bdsg::SubgraphOverlay::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::SubgraphOverlay::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::SubgraphOverlay::*)() const) &bdsg::SubgraphOverlay::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::SubgraphOverlay::max_node_id() const --> long long");
		cl.def("assign", (class bdsg::SubgraphOverlay & (bdsg::SubgraphOverlay::*)(const class bdsg::SubgraphOverlay &)) &bdsg::SubgraphOverlay::operator=, "C++: bdsg::SubgraphOverlay::operator=(const class bdsg::SubgraphOverlay &) --> class bdsg::SubgraphOverlay &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
