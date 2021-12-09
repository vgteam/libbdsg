#include <bdsg/graph_proxy.hpp>
#include <bdsg/internal/base_packed_graph.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <functional>
#include <handlegraph/mutable_path_handle_graph.hpp>
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

// bdsg::MutablePathDeletableHandleGraphProxy file:bdsg/graph_proxy.hpp line:402
struct PyCallBack_bdsg_MutablePathDeletableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t : public bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> {
	using bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::MutablePathDeletableHandleGraphProxy;

	struct handlegraph::handle_t create_handle(const std::string & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::create_handle(a0);
	}
	struct handlegraph::handle_t create_handle(const std::string & a0, const long long & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::create_handle(a0, a1);
	}
	void create_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "create_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::create_edge(a0, a1);
	}
	struct handlegraph::handle_t apply_orientation(const struct handlegraph::handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "apply_orientation");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::apply_orientation(a0);
	}
	class std::vector<handlegraph::handle_t> divide_handle(const struct handlegraph::handle_t & a0, const class std::vector<unsigned long> & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "divide_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::handle_t>>::value) {
				static pybind11::detail::overload_caster_t<class std::vector<handlegraph::handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::handle_t>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::vector<handlegraph::handle_t>>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::divide_handle(a0, a1);
	}
	void optimize(bool a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "optimize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::optimize(a0);
	}
	void apply_ordering(const class std::vector<handlegraph::handle_t> & a0, bool a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "apply_ordering");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::apply_ordering(a0, a1);
	}
	void set_id_increment(const long long & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "set_id_increment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::set_id_increment(a0);
	}
	void increment_node_ids(long long a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "increment_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::increment_node_ids(a0);
	}
	void increment_node_ids(long a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "increment_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::increment_node_ids(a0);
	}
	void reassign_node_ids(const class std::function<long long (const long long &)> & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "reassign_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::reassign_node_ids(a0);
	}
	void destroy_path(const struct handlegraph::path_handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "destroy_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::destroy_path(a0);
	}
	struct handlegraph::path_handle_t create_path_handle(const std::string & a0, bool a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "create_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::create_path_handle(a0, a1);
	}
	struct handlegraph::step_handle_t append_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "append_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::append_step(a0, a1);
	}
	struct handlegraph::step_handle_t prepend_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "prepend_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::prepend_step(a0, a1);
	}
	using _binder_ret_0 = struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>;
	_binder_ret_0 rewrite_segment(const struct handlegraph::step_handle_t & a0, const struct handlegraph::step_handle_t & a1, const class std::vector<handlegraph::handle_t> & a2) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "rewrite_segment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<_binder_ret_0>::value) {
				static pybind11::detail::overload_caster_t<_binder_ret_0> caster;
				return pybind11::detail::cast_ref<_binder_ret_0>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<_binder_ret_0>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::rewrite_segment(a0, a1, a2);
	}
	void set_circularity(const struct handlegraph::path_handle_t & a0, bool a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "set_circularity");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::set_circularity(a0, a1);
	}
	void destroy_handle(const struct handlegraph::handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "destroy_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::destroy_handle(a0);
	}
	void destroy_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "destroy_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::destroy_edge(a0, a1);
	}
	struct handlegraph::handle_t truncate_handle(const struct handlegraph::handle_t & a0, bool a1, unsigned long a2) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "truncate_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::truncate_handle(a0, a1, a2);
	}
	void clear() override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "clear");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathDeletableHandleGraphProxy::clear();
	}
	unsigned long get_path_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathHandleGraphProxy::get_path_count();
	}
	bool has_path(const std::string & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::has_path(a0);
	}
	struct handlegraph::path_handle_t get_path_handle(const std::string & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::get_path_handle(a0);
	}
	std::string get_path_name(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return PathHandleGraphProxy::get_path_name(a0);
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::get_is_circular(a0);
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathHandleGraphProxy::get_step_count(a0);
	}
	unsigned long get_step_count(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return PathHandleGraphProxy::get_step_count(a0);
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::get_handle_of_step(a0);
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::get_path_handle_of_step(a0);
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::path_begin(a0);
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::path_end(a0);
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::path_back(a0);
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::path_front_end(a0);
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::has_next_step(a0);
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::has_previous_step(a0);
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::get_next_step(a0);
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return PathHandleGraphProxy::get_previous_step(a0);
	}
	class std::vector<handlegraph::step_handle_t> steps_of_handle(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "steps_of_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::step_handle_t>>::value) {
				static pybind11::detail::overload_caster_t<class std::vector<handlegraph::step_handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::step_handle_t>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::vector<handlegraph::step_handle_t>>(std::move(o));
		}
		return PathHandleGraphProxy::steps_of_handle(a0, a1);
	}
	bool is_empty(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "is_empty");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::is_empty(a0);
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "for_each_path_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::for_each_path_handle_impl(a0);
	}
	bool for_each_step_on_handle_impl(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "for_each_step_on_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return PathHandleGraphProxy::for_each_step_on_handle_impl(a0, a1);
	}
	bool has_node(long long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraphProxy::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HandleGraphProxy::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HandleGraphProxy::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraphProxy::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HandleGraphProxy::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraphProxy::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HandleGraphProxy::get_sequence(a0);
	}
	unsigned long get_node_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraphProxy::get_node_count();
	}
	long long min_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HandleGraphProxy::min_node_id();
	}
	long long max_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::overload_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HandleGraphProxy::max_node_id();
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraphProxy::get_degree(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraphProxy::has_edge(a0, a1);
	}
	unsigned long get_edge_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_edge_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraphProxy::get_edge_count();
	}
	unsigned long get_total_length() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_total_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HandleGraphProxy::get_total_length();
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::overload_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return HandleGraphProxy::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::overload_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HandleGraphProxy::get_subsequence(a0, a1, a2);
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraphProxy::follow_edges_impl(a0, a1, a2);
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HandleGraphProxy::for_each_handle_impl(a0, a1);
	}
	class bdsg::BasePackedGraph<struct bdsg::STLBackend> * get() override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<class bdsg::BasePackedGraph<struct bdsg::STLBackend> *>::value) {
				static pybind11::detail::overload_caster_t<class bdsg::BasePackedGraph<struct bdsg::STLBackend> *> caster;
				return pybind11::detail::cast_ref<class bdsg::BasePackedGraph<struct bdsg::STLBackend> *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class bdsg::BasePackedGraph<struct bdsg::STLBackend> *>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"AbstractGraphProxy::get\"");
	}
	struct handlegraph::path_handle_t rename_path(const struct handlegraph::path_handle_t & a0, const std::string & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "rename_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return MutablePathHandleGraph::rename_path(a0, a1);
	}
	void pop_front_step(const struct handlegraph::path_handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "pop_front_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathHandleGraph::pop_front_step(a0);
	}
	void pop_back_step(const struct handlegraph::path_handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "pop_back_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return MutablePathHandleGraph::pop_back_step(a0);
	}
};

void bind_bdsg_graph_proxy_4(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::SerializableHandleGraphProxy file:bdsg/graph_proxy.hpp line:332
		pybind11::class_<bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, std::shared_ptr<bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>>, bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, handlegraph::SerializableHandleGraph> cl(M("bdsg"), "SerializableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def("get_magic_number", (unsigned int (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_magic_number, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_magic_number() const --> unsigned int");
		cl.def("deserialize", (void (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::deserialize, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("serialize", (void (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::serialize, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::serialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (class handlegraph::SerializableHandleGraph & (handlegraph::SerializableHandleGraph::*)(const class handlegraph::SerializableHandleGraph &)) &handlegraph::SerializableHandleGraph::operator=, "C++: handlegraph::SerializableHandleGraph::operator=(const class handlegraph::SerializableHandleGraph &) --> class handlegraph::SerializableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::SerializableHandleGraphProxy file:bdsg/graph_proxy.hpp line:332
		pybind11::class_<bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>, std::shared_ptr<bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>>, bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>, handlegraph::SerializableHandleGraph> cl(M("bdsg"), "SerializableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_MappedBackend_t", "");
		cl.def("get_magic_number", (unsigned int (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)() const) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::get_magic_number, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::get_magic_number() const --> unsigned int");
		cl.def("deserialize", (void (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const std::string &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::deserialize, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("serialize", (void (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const std::string &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::serialize, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::serialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > & (bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &)) &bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=, "C++: bdsg::SerializableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=(const struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &) --> struct bdsg::SerializableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > & (bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &)) &bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=, "C++: bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &) --> struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (class handlegraph::SerializableHandleGraph & (handlegraph::SerializableHandleGraph::*)(const class handlegraph::SerializableHandleGraph &)) &handlegraph::SerializableHandleGraph::operator=, "C++: handlegraph::SerializableHandleGraph::operator=(const class handlegraph::SerializableHandleGraph &) --> class handlegraph::SerializableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::MutablePathDeletableHandleGraphProxy file:bdsg/graph_proxy.hpp line:402
		pybind11::class_<bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, std::shared_ptr<bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>>, PyCallBack_bdsg_MutablePathDeletableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t, bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, handlegraph::MutablePathDeletableHandleGraph> cl(M("bdsg"), "MutablePathDeletableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_bdsg_MutablePathDeletableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t(); } ) );
		cl.def(pybind11::init<PyCallBack_bdsg_MutablePathDeletableHandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t const &>());
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &, const long long &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("create_edge", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_edge, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("apply_orientation", (struct handlegraph::handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::apply_orientation, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (class std::vector<handlegraph::handle_t> (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::divide_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::divide_handle(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &) --> class std::vector<handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offsets"));
		cl.def("optimize", [](bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(bool)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::optimize, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("apply_ordering", [](bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> &o, const class std::vector<handlegraph::handle_t> & a0) -> void { return o.apply_ordering(a0); }, "", pybind11::arg("order"));
		cl.def("apply_ordering", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const class std::vector<handlegraph::handle_t> &, bool)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::apply_ordering, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::apply_ordering(const class std::vector<handlegraph::handle_t> &, bool) --> void", pybind11::arg("order"), pybind11::arg("compact_ids"));
		cl.def("set_id_increment", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const long long &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::set_id_increment, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::set_id_increment(const long long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(long long)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::increment_node_ids, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::increment_node_ids(long long) --> void", pybind11::arg("increment"));
		cl.def("increment_node_ids", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(long)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::increment_node_ids, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::increment_node_ids(long) --> void", pybind11::arg("increment"));
		cl.def("reassign_node_ids", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const class std::function<long long (const long long &)> &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::reassign_node_ids, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::reassign_node_ids(const class std::function<long long (const long long &)> &) --> void", pybind11::arg("get_new_id"));
		cl.def("destroy_path", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_path, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("create_path_handle", [](bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> &o, const std::string & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &, bool)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_path_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("append_step", (struct handlegraph::step_handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::append_step, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::prepend_step, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_prepend"));
		cl.def("rewrite_segment", (struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t> (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::rewrite_segment, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::rewrite_segment(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &) --> struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>", pybind11::arg("segment_begin"), pybind11::arg("segment_end"), pybind11::arg("new_segment"));
		cl.def("set_circularity", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &, bool)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::set_circularity, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path"), pybind11::arg("circular"));
		cl.def("destroy_handle", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("destroy_edge", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_edge, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("truncate_handle", (struct handlegraph::handle_t (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, bool, unsigned long)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::truncate_handle, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::truncate_handle(const struct handlegraph::handle_t &, bool, unsigned long) --> struct handlegraph::handle_t", pybind11::arg("handle"), pybind11::arg("trunc_left"), pybind11::arg("offset"));
		cl.def("clear", (void (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)()) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::clear, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::clear() --> void");
		cl.def("assign", (struct bdsg::MutablePathDeletableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::MutablePathDeletableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::MutablePathDeletableHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::MutablePathDeletableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::MutablePathDeletableHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("get_path_count", (unsigned long (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_count, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_path, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const std::string &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_handle, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_name, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_is_circular, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_step_count, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_step_count, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_step_count(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_handle_of_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_handle_of_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_begin, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_end, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_back, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_front_end, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_next_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_previous_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_next_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::step_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_previous_step, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("steps_of_handle", [](bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> const &o, const struct handlegraph::handle_t & a0) -> std::vector<handlegraph::step_handle_t> { return o.steps_of_handle(a0); }, "", pybind11::arg("handle"));
		cl.def("steps_of_handle", (class std::vector<handlegraph::step_handle_t> (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::steps_of_handle, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::steps_of_handle(const struct handlegraph::handle_t &, bool) const --> class std::vector<handlegraph::step_handle_t>", pybind11::arg("handle"), pybind11::arg("match_orientation"));
		cl.def("is_empty", (bool (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::path_handle_t &) const) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::is_empty, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::is_empty(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("assign", (struct bdsg::PathHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::PathHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::PathHandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::PathHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::PathHandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("has_node", (bool (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(long long) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_node, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const long long &, bool) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_handle, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_id, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_is_reverse, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::flip, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_length, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_sequence, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_node_count, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::min_node_id, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::max_node_id, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::max_node_id() const --> long long");
		cl.def("get_degree", (unsigned long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_degree, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("has_edge", (bool (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_edge, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("get_edge_count", (unsigned long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_edge_count, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_edge_count() const --> unsigned long");
		cl.def("get_total_length", (unsigned long (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)() const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_total_length, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_total_length() const --> unsigned long");
		cl.def("get_base", (char (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, unsigned long) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_base, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_subsequence, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("assign", (struct bdsg::HandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::HandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::HandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::HandleGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("follow_edges", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const) &handlegraph::HandleGraph::follow_edges<std::function<bool (const handlegraph::handle_t &)>>, "C++: handlegraph::HandleGraph::follow_edges(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("go_left"), pybind11::arg("iteratee"));
		cl.def("for_each_handle", [](handlegraph::HandleGraph const &o, const class std::function<bool (const struct handlegraph::handle_t &)> & a0) -> bool { return o.for_each_handle(a0); }, "", pybind11::arg("iteratee"));
		cl.def("for_each_handle", (bool (handlegraph::HandleGraph::*)(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const) &handlegraph::HandleGraph::for_each_handle<std::function<bool (const handlegraph::handle_t &)>>, "C++: handlegraph::HandleGraph::for_each_handle(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const --> bool", pybind11::arg("iteratee"), pybind11::arg("parallel"));
		cl.def("for_each_edge", [](handlegraph::HandleGraph const &o, const class std::function<bool (const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)> & a0) -> bool { return o.for_each_edge(a0); }, "", pybind11::arg("iteratee"));
		cl.def("for_each_edge", (bool (handlegraph::HandleGraph::*)(const class std::function<bool (const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)> &, bool) const) &handlegraph::HandleGraph::for_each_edge<std::function<bool (const std::pair<handlegraph::handle_t, handlegraph::handle_t> &)>>, "C++: handlegraph::HandleGraph::for_each_edge(const class std::function<bool (const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)> &, bool) const --> bool", pybind11::arg("iteratee"), pybind11::arg("parallel"));
		cl.def("has_node", (bool (handlegraph::HandleGraph::*)(long long) const) &handlegraph::HandleGraph::has_node, "Method to check if a node exists by ID\n\nC++: handlegraph::HandleGraph::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](handlegraph::HandleGraph const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const long long &, bool) const) &handlegraph::HandleGraph::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: handlegraph::HandleGraph::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_id, "Get the ID from a handle\n\nC++: handlegraph::HandleGraph::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_is_reverse, "Get the orientation of a handle\n\nC++: handlegraph::HandleGraph::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: handlegraph::HandleGraph::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_length, "Get the length of a node\n\nC++: handlegraph::HandleGraph::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::get_sequence, "Get the sequence of a node, presented in the handle's local forward\n orientation.\n\nC++: handlegraph::HandleGraph::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::get_node_count, "Return the number of nodes in the graph\n\nC++: handlegraph::HandleGraph::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: handlegraph::HandleGraph::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: handlegraph::HandleGraph::max_node_id() const --> long long");
		cl.def("get_degree", (unsigned long (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, bool) const) &handlegraph::HandleGraph::get_degree, "Get the number of edges on the right (go_left = false) or left (go_left\n = true) side of the given handle. The default implementation is O(n) in\n the number of edges returned, but graph implementations that track this\n information more efficiently can override this method.\n\nC++: handlegraph::HandleGraph::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("has_edge", (bool (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::has_edge, "Returns true if there is an edge that allows traversal from the left\n handle to the right handle. By default O(n) in the number of edges\n on left, but can be overridden with more efficient implementations.\n\nC++: handlegraph::HandleGraph::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("has_edge", (bool (handlegraph::HandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const) &handlegraph::HandleGraph::has_edge, "Convenient wrapper of has_edge for edge_t argument.\n\nC++: handlegraph::HandleGraph::has_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) const --> bool", pybind11::arg("edge"));
		cl.def("get_edge_count", (unsigned long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::get_edge_count, "Return the total number of edges in the graph. If not overridden,\n counts them all in linear time.\n\nC++: handlegraph::HandleGraph::get_edge_count() const --> unsigned long");
		cl.def("get_total_length", (unsigned long (handlegraph::HandleGraph::*)() const) &handlegraph::HandleGraph::get_total_length, "Return the total length of all nodes in the graph, in bp. If not\n overridden, loops over all nodes in linear time.\n\nC++: handlegraph::HandleGraph::get_total_length() const --> unsigned long");
		cl.def("get_base", (char (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, unsigned long) const) &handlegraph::HandleGraph::get_base, "Returns one base of a handle's sequence, in the orientation of the\n handle.\n\nC++: handlegraph::HandleGraph::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &handlegraph::HandleGraph::get_subsequence, "Returns a substring of a handle's sequence, in the orientation of the\n handle. If the indicated substring would extend beyond the end of the\n handle's sequence, the return value is truncated to the sequence's end.\n By default O(n) in the size of the handle's sequence, but can be overriden.\n\nC++: handlegraph::HandleGraph::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("forward", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::forward, "Get the locally forward version of a handle\n\nC++: handlegraph::HandleGraph::forward(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("edge_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (handlegraph::HandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::edge_handle, "A pair of handles can be used as an edge. When so used, the handles have a\n canonical order and orientation.\n\nC++: handlegraph::HandleGraph::edge_handle(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("traverse_edge_handle", (struct handlegraph::handle_t (handlegraph::HandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const) &handlegraph::HandleGraph::traverse_edge_handle, "Such a pair can be viewed from either inward end handle and produce the\n outward handle you would arrive at.\n\nC++: handlegraph::HandleGraph::traverse_edge_handle(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("edge"), pybind11::arg("left"));
		cl.def("assign", (class handlegraph::HandleGraph & (handlegraph::HandleGraph::*)(const class handlegraph::HandleGraph &)) &handlegraph::HandleGraph::operator=, "C++: handlegraph::HandleGraph::operator=(const class handlegraph::HandleGraph &) --> class handlegraph::HandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("for_each_path_handle", (bool (handlegraph::PathHandleGraph::*)(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const) &handlegraph::PathHandleGraph::for_each_path_handle<std::function<bool (const handlegraph::path_handle_t &)>>, "/////////////////////////////////////////////////////////////////////////\n\nC++: handlegraph::PathHandleGraph::for_each_path_handle(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const --> bool", pybind11::arg("iteratee"));
		cl.def("for_each_step_on_handle", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const) &handlegraph::PathHandleGraph::for_each_step_on_handle<std::function<bool (const handlegraph::step_handle_t &)>>, "C++: handlegraph::PathHandleGraph::for_each_step_on_handle(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("iteratee"));
		cl.def("for_each_step_in_path", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const) &handlegraph::PathHandleGraph::for_each_step_in_path<std::function<bool (const handlegraph::step_handle_t &)>>, "C++: handlegraph::PathHandleGraph::for_each_step_in_path(const struct handlegraph::path_handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const --> bool", pybind11::arg("path"), pybind11::arg("iteratee"));
		cl.def("get_path_count", (unsigned long (handlegraph::PathHandleGraph::*)() const) &handlegraph::PathHandleGraph::get_path_count, "Returns the number of paths stored in the graph\n\nC++: handlegraph::PathHandleGraph::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (handlegraph::PathHandleGraph::*)(const std::string &) const) &handlegraph::PathHandleGraph::has_path, "Determine if a path name exists and is legal to get a path handle for.\n\nC++: handlegraph::PathHandleGraph::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (handlegraph::PathHandleGraph::*)(const std::string &) const) &handlegraph::PathHandleGraph::get_path_handle, "Look up the path handle for the given path name.\n The path with that name must exist.\n\nC++: handlegraph::PathHandleGraph::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_path_name, "Look up the name of a path from a handle to it\n\nC++: handlegraph::PathHandleGraph::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_is_circular, "Look up whether a path is circular\n\nC++: handlegraph::PathHandleGraph::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_step_count, "Returns the number of node steps in the path\n\nC++: handlegraph::PathHandleGraph::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (handlegraph::PathHandleGraph::*)(const struct handlegraph::handle_t &) const) &handlegraph::PathHandleGraph::get_step_count, "Returns the number of node steps on a handle\n\nC++: handlegraph::PathHandleGraph::get_step_count(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_handle_of_step, "Get a node handle (node ID and orientation) from a handle to an step on a path\n\nC++: handlegraph::PathHandleGraph::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_path_handle_of_step, "Returns a handle to the path that an step is on\n\nC++: handlegraph::PathHandleGraph::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_begin, "Get a handle to the first step, which will be an arbitrary step in a circular path\n that we consider \"first\" based on our construction of the path. If the path is empty,\n then the implementation must return the same value as path_end().\n\nC++: handlegraph::PathHandleGraph::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_end, "Get a handle to a fictitious position past the end of a path. This position is\n returned by get_next_step for the final step in a path in a non-circular path.\n Note: get_next_step will *NEVER* return this value for a circular path.\n\nC++: handlegraph::PathHandleGraph::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_back, "Get a handle to the last step, which will be an arbitrary step in a circular path that\n we consider \"last\" based on our construction of the path. If the path is empty\n then the implementation must return the same value as path_front_end().\n\nC++: handlegraph::PathHandleGraph::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_front_end, "Get a handle to a fictitious position before the beginning of a path. This position is\n return by get_previous_step for the first step in a path in a non-circular path.\n Note: get_previous_step will *NEVER* return this value for a circular path.\n\nC++: handlegraph::PathHandleGraph::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::has_next_step, "Returns true if the step is not the last step in a non-circular path.\n\nC++: handlegraph::PathHandleGraph::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::has_previous_step, "Returns true if the step is not the first step in a non-circular path.\n\nC++: handlegraph::PathHandleGraph::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_next_step, "Returns a handle to the next step on the path. If the given step is the final step\n of a non-circular path, this method has undefined behavior. In a circular path,\n the \"last\" step will loop around to the \"first\" step.\n\nC++: handlegraph::PathHandleGraph::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_previous_step, "Returns a handle to the previous step on the path. If the given step is the first\n step of a non-circular path, this method has undefined behavior. In a circular path,\n it will loop around from the \"first\" step (i.e. the one returned by path_begin) to\n the \"last\" step.\n\nC++: handlegraph::PathHandleGraph::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("steps_of_handle", [](handlegraph::PathHandleGraph const &o, const struct handlegraph::handle_t & a0) -> std::vector<handlegraph::step_handle_t> { return o.steps_of_handle(a0); }, "", pybind11::arg("handle"));
		cl.def("steps_of_handle", (class std::vector<handlegraph::step_handle_t> (handlegraph::PathHandleGraph::*)(const struct handlegraph::handle_t &, bool) const) &handlegraph::PathHandleGraph::steps_of_handle, "Returns a vector of all steps of a node on paths. Optionally restricts to\n steps that match the handle in orientation.\n\nC++: handlegraph::PathHandleGraph::steps_of_handle(const struct handlegraph::handle_t &, bool) const --> class std::vector<handlegraph::step_handle_t>", pybind11::arg("handle"), pybind11::arg("match_orientation"));
		cl.def("is_empty", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::is_empty, "Returns true if the given path is empty, and false otherwise\n\nC++: handlegraph::PathHandleGraph::is_empty(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("scan_path", (class handlegraph::PathForEachSocket (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::scan_path, "Returns a class with an STL-style iterator interface that can be used directly\n in a for each loop like:\n for (handle_t handle : graph->scan_path(path)) { }\n\nC++: handlegraph::PathHandleGraph::scan_path(const struct handlegraph::path_handle_t &) const --> class handlegraph::PathForEachSocket", pybind11::arg("path"));
		cl.def("assign", (class handlegraph::PathHandleGraph & (handlegraph::PathHandleGraph::*)(const class handlegraph::PathHandleGraph &)) &handlegraph::PathHandleGraph::operator=, "C++: handlegraph::PathHandleGraph::operator=(const class handlegraph::PathHandleGraph &) --> class handlegraph::PathHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("assign", (class handlegraph::MutablePathDeletableHandleGraph & (handlegraph::MutablePathDeletableHandleGraph::*)(const class handlegraph::MutablePathDeletableHandleGraph &)) &handlegraph::MutablePathDeletableHandleGraph::operator=, "C++: handlegraph::MutablePathDeletableHandleGraph::operator=(const class handlegraph::MutablePathDeletableHandleGraph &) --> class handlegraph::MutablePathDeletableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
