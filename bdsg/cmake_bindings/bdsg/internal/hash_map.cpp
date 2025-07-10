#include <bdsg/hash_graph.hpp>
#include <bdsg/internal/hash_map.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <bdsg/internal/utility.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_handle_graph.hpp>
#include <handlegraph/mutable_path_metadata.hpp>
#include <handlegraph/path_handle_graph.hpp>
#include <handlegraph/path_metadata.hpp>
#include <handlegraph/serializable.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <streambuf>
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

// bdsg::HashGraph file:bdsg/hash_graph.hpp line:32
struct PyCallBack_bdsg_HashGraph : public bdsg::HashGraph {
	using bdsg::HashGraph::HashGraph;

	bool has_node(long long a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long long & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::get_handle(a0, a1);
	}
	long long get_id(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HashGraph::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HashGraph::get_length(a0);
	}
	std::string get_sequence(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HashGraph::get_sequence(a0);
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::follow_edges_impl(a0, a1, a2);
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::for_each_handle_impl(a0, a1);
	}
	unsigned long get_node_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HashGraph::get_node_count();
	}
	long long min_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HashGraph::min_node_id();
	}
	long long max_node_id() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long long>::value) {
				static pybind11::detail::override_caster_t<long long> caster;
				return pybind11::detail::cast_ref<long long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<long long>(std::move(o));
		}
		return HashGraph::max_node_id();
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HashGraph::get_degree(a0, a1);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_base");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<char>::value) {
				static pybind11::detail::override_caster_t<char> caster;
				return pybind11::detail::cast_ref<char>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<char>(std::move(o));
		}
		return HashGraph::get_base(a0, a1);
	}
	std::string get_subsequence(const struct handlegraph::handle_t & a0, unsigned long a1, unsigned long a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_subsequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HashGraph::get_subsequence(a0, a1, a2);
	}
	struct handlegraph::handle_t create_handle(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::create_handle(a0);
	}
	struct handlegraph::handle_t create_handle(const std::string & a0, const long long & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::create_handle(a0, a1);
	}
	void destroy_handle(const struct handlegraph::handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "destroy_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::destroy_handle(a0);
	}
	struct handlegraph::handle_t change_sequence(const struct handlegraph::handle_t & a0, const std::string & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "change_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::change_sequence(a0, a1);
	}
	void create_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "create_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::create_edge(a0, a1);
	}
	void destroy_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "destroy_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::destroy_edge(a0, a1);
	}
	struct handlegraph::handle_t truncate_handle(const struct handlegraph::handle_t & a0, bool a1, unsigned long a2) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "truncate_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::truncate_handle(a0, a1, a2);
	}
	void clear() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "clear");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::clear();
	}
	struct handlegraph::handle_t apply_orientation(const struct handlegraph::handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "apply_orientation");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::apply_orientation(a0);
	}
	class std::vector<handlegraph::handle_t> divide_handle(const struct handlegraph::handle_t & a0, const class std::vector<unsigned long> & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "divide_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::vector<handlegraph::handle_t>>::value) {
				static pybind11::detail::override_caster_t<class std::vector<handlegraph::handle_t>> caster;
				return pybind11::detail::cast_ref<class std::vector<handlegraph::handle_t>>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<class std::vector<handlegraph::handle_t>>(std::move(o));
		}
		return HashGraph::divide_handle(a0, a1);
	}
	void optimize(bool a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "optimize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::optimize(a0);
	}
	bool apply_ordering(const class std::vector<handlegraph::handle_t> & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "apply_ordering");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::apply_ordering(a0, a1);
	}
	unsigned long get_path_count() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HashGraph::get_path_count();
	}
	bool has_path(const std::string & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::has_path(a0);
	}
	struct handlegraph::path_handle_t get_path_handle(const std::string & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return HashGraph::get_path_handle(a0);
	}
	std::string get_path_name(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<std::string>::value) {
				static pybind11::detail::override_caster_t<std::string> caster;
				return pybind11::detail::cast_ref<std::string>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<std::string>(std::move(o));
		}
		return HashGraph::get_path_name(a0);
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::get_is_circular(a0);
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::override_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return HashGraph::get_step_count(a0);
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return HashGraph::get_handle_of_step(a0);
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return HashGraph::get_path_handle_of_step(a0);
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::path_begin(a0);
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::path_end(a0);
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::path_back(a0);
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::path_front_end(a0);
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::has_next_step(a0);
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::has_previous_step(a0);
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::get_next_step(a0);
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::get_previous_step(a0);
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "for_each_path_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::for_each_path_handle_impl(a0);
	}
	bool for_each_step_on_handle_impl(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "for_each_step_on_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return HashGraph::for_each_step_on_handle_impl(a0, a1);
	}
	void destroy_path(const struct handlegraph::path_handle_t & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "destroy_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::destroy_path(a0);
	}
	void destroy_paths(const class std::vector<handlegraph::path_handle_t> & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "destroy_paths");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::destroy_paths(a0);
	}
	struct handlegraph::path_handle_t create_path_handle(const std::string & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "create_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return HashGraph::create_path_handle(a0, a1);
	}
	struct handlegraph::step_handle_t append_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "append_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::append_step(a0, a1);
	}
	struct handlegraph::step_handle_t prepend_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "prepend_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return HashGraph::prepend_step(a0, a1);
	}
	using _binder_ret_0 = struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>;
	_binder_ret_0 rewrite_segment(const struct handlegraph::step_handle_t & a0, const struct handlegraph::step_handle_t & a1, const class std::vector<handlegraph::handle_t> & a2) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "rewrite_segment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<_binder_ret_0>::value) {
				static pybind11::detail::override_caster_t<_binder_ret_0> caster;
				return pybind11::detail::cast_ref<_binder_ret_0>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<_binder_ret_0>(std::move(o));
		}
		return HashGraph::rewrite_segment(a0, a1, a2);
	}
	void set_circularity(const struct handlegraph::path_handle_t & a0, bool a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "set_circularity");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::set_circularity(a0, a1);
	}
	void set_id_increment(const long long & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "set_id_increment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::set_id_increment(a0);
	}
	void increment_node_ids(long long a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "increment_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::increment_node_ids(a0);
	}
	void reassign_node_ids(const class std::function<long long (const long long &)> & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "reassign_node_ids");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return HashGraph::reassign_node_ids(a0);
	}
	unsigned int get_magic_number() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_magic_number");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned int>::value) {
				static pybind11::detail::override_caster_t<unsigned int> caster;
				return pybind11::detail::cast_ref<unsigned int>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned int>(std::move(o));
		}
		return HashGraph::get_magic_number();
	}
	struct handlegraph::path_handle_t rename_path(const struct handlegraph::path_handle_t & a0, const std::string & a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "rename_path");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "pop_front_step");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "pop_back_step");
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
	unsigned long get_step_count(const struct handlegraph::handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_step_count");
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
	class std::vector<handlegraph::step_handle_t> steps_of_handle(const struct handlegraph::handle_t & a0, bool a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "steps_of_handle");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "is_empty");
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
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "has_edge");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_edge_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_total_length");
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
	enum handlegraph::PathSense get_sense(const struct handlegraph::path_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_sense");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_sample_name");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_locus_name");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_haplotype");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_phase_block");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "get_subrange");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "for_each_step_of_sense_impl");
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
	struct handlegraph::path_handle_t create_path(const enum handlegraph::PathSense & a0, const std::string & a1, const std::string & a2, const unsigned long & a3, const unsigned long & a4, const struct std::pair<unsigned long, unsigned long> & a5, bool a6) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "create_path");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "increment_node_ids");
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
	void deserialize(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "deserialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return Serializable::deserialize(a0);
	}
	void serialize(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HashGraph *>(this), "serialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return Serializable::serialize(a0);
	}
};

void bind_bdsg_internal_hash_map(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::wang_hash file:bdsg/internal/hash_map.hpp line:120
		pybind11::class_<bdsg::wang_hash<long long,void>, std::shared_ptr<bdsg::wang_hash<long long,void>>> cl(M("bdsg"), "wang_hash_long_long_void_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::wang_hash<long long,void>(); } ) );
		cl.def("__call__", (unsigned long (bdsg::wang_hash<long long,void>::*)(const long long &) const) &bdsg::wang_hash<long long>::operator(), "C++: bdsg::wang_hash<long long>::operator()(const long long &) const --> unsigned long", pybind11::arg("x"));
	}
	{ // bdsg::wang_hash file:bdsg/internal/hash_map.hpp line:120
		pybind11::class_<bdsg::wang_hash<long,void>, std::shared_ptr<bdsg::wang_hash<long,void>>> cl(M("bdsg"), "wang_hash_long_void_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::wang_hash<long,void>(); } ) );
		cl.def("__call__", (unsigned long (bdsg::wang_hash<long,void>::*)(const long &) const) &bdsg::wang_hash<long>::operator(), "C++: bdsg::wang_hash<long>::operator()(const long &) const --> unsigned long", pybind11::arg("x"));
	}
	{ // bdsg::wang_hash file:bdsg/internal/hash_map.hpp line:120
		pybind11::class_<bdsg::wang_hash<char,void>, std::shared_ptr<bdsg::wang_hash<char,void>>> cl(M("bdsg"), "wang_hash_char_void_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::wang_hash<char,void>(); } ) );
		cl.def("__call__", (unsigned long (bdsg::wang_hash<char,void>::*)(const char &) const) &bdsg::wang_hash<char>::operator(), "C++: bdsg::wang_hash<char>::operator()(const char &) const --> unsigned long", pybind11::arg("x"));
	}
	{ // bdsg::StringHashMapFor file:bdsg/internal/hash_map.hpp line:212
		pybind11::class_<bdsg::StringHashMapFor<bdsg::STLBackend>, std::shared_ptr<bdsg::StringHashMapFor<bdsg::STLBackend>>> cl(M("bdsg"), "StringHashMapFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::StringHashMapFor<bdsg::STLBackend>(); } ) );
	}
	{ // bdsg::PairHashMapFor file:bdsg/internal/hash_map.hpp line:245
		pybind11::class_<bdsg::PairHashMapFor<bdsg::STLBackend>, std::shared_ptr<bdsg::PairHashMapFor<bdsg::STLBackend>>> cl(M("bdsg"), "PairHashMapFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PairHashMapFor<bdsg::STLBackend>(); } ) );
	}
	{ // bdsg::HashSetFor file:bdsg/internal/hash_map.hpp line:295
		pybind11::class_<bdsg::HashSetFor<bdsg::STLBackend>, std::shared_ptr<bdsg::HashSetFor<bdsg::STLBackend>>> cl(M("bdsg"), "HashSetFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::HashSetFor<bdsg::STLBackend>(); } ) );
	}
	{ // bdsg::HashSetFor file:bdsg/internal/hash_map.hpp line:302
		pybind11::class_<bdsg::HashSetFor<bdsg::CompatBackend>, std::shared_ptr<bdsg::HashSetFor<bdsg::CompatBackend>>> cl(M("bdsg"), "HashSetFor_bdsg_CompatBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::HashSetFor<bdsg::CompatBackend>(); } ) );
	}
	{ // bdsg::HashSetFor file:bdsg/internal/hash_map.hpp line:309
		pybind11::class_<bdsg::HashSetFor<bdsg::MappedBackend>, std::shared_ptr<bdsg::HashSetFor<bdsg::MappedBackend>>> cl(M("bdsg"), "HashSetFor_bdsg_MappedBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::HashSetFor<bdsg::MappedBackend>(); } ) );
	}
	{ // bdsg::StringHashSetFor file:bdsg/internal/hash_map.hpp line:342
		pybind11::class_<bdsg::StringHashSetFor<bdsg::STLBackend>, std::shared_ptr<bdsg::StringHashSetFor<bdsg::STLBackend>>> cl(M("bdsg"), "StringHashSetFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::StringHashSetFor<bdsg::STLBackend>(); } ) );
	}
	{ // bdsg::PairHashSetFor file:bdsg/internal/hash_map.hpp line:375
		pybind11::class_<bdsg::PairHashSetFor<bdsg::STLBackend>, std::shared_ptr<bdsg::PairHashSetFor<bdsg::STLBackend>>> cl(M("bdsg"), "PairHashSetFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PairHashSetFor<bdsg::STLBackend>(); } ) );
	}
	// bdsg::reverse_complement(const char &) file:bdsg/internal/utility.hpp line:13
	M("bdsg").def("reverse_complement", (char (*)(const char &)) &bdsg::reverse_complement, "C++: bdsg::reverse_complement(const char &) --> char", pybind11::arg("c"));

	// bdsg::reverse_complement(const std::string &) file:bdsg/internal/utility.hpp line:14
	M("bdsg").def("reverse_complement", (std::string (*)(const std::string &)) &bdsg::reverse_complement, "C++: bdsg::reverse_complement(const std::string &) --> std::string", pybind11::arg("seq"));

	// bdsg::reverse_complement_in_place(std::string &) file:bdsg/internal/utility.hpp line:15
	M("bdsg").def("reverse_complement_in_place", (void (*)(std::string &)) &bdsg::reverse_complement_in_place, "C++: bdsg::reverse_complement_in_place(std::string &) --> void", pybind11::arg("seq"));

	// bdsg::is_all_n(const std::string &) file:bdsg/internal/utility.hpp line:18
	M("bdsg").def("is_all_n", (bool (*)(const std::string &)) &bdsg::is_all_n, "Return True if the given string is entirely Ns of either case, and false\n otherwise.\n\nC++: bdsg::is_all_n(const std::string &) --> bool", pybind11::arg("seq"));

	// bdsg::allATGC(const std::string &) file:bdsg/internal/utility.hpp line:20
	M("bdsg").def("allATGC", (bool (*)(const std::string &)) &bdsg::allATGC, "C++: bdsg::allATGC(const std::string &) --> bool", pybind11::arg("s"));

	// bdsg::allATGCN(const std::string &) file:bdsg/internal/utility.hpp line:21
	M("bdsg").def("allATGCN", (bool (*)(const std::string &)) &bdsg::allATGCN, "C++: bdsg::allATGCN(const std::string &) --> bool", pybind11::arg("s"));

	// bdsg::nonATGCNtoN(const std::string &) file:bdsg/internal/utility.hpp line:22
	M("bdsg").def("nonATGCNtoN", (std::string (*)(const std::string &)) &bdsg::nonATGCNtoN, "C++: bdsg::nonATGCNtoN(const std::string &) --> std::string", pybind11::arg("s"));

	// bdsg::toUppercase(const std::string &) file:bdsg/internal/utility.hpp line:24
	M("bdsg").def("toUppercase", (std::string (*)(const std::string &)) &bdsg::toUppercase, "C++: bdsg::toUppercase(const std::string &) --> std::string", pybind11::arg("s"));

	// bdsg::dna_as_int(char) file:bdsg/internal/utility.hpp line:26
	M("bdsg").def("dna_as_int", (int (*)(char)) &bdsg::dna_as_int, "C++: bdsg::dna_as_int(char) --> int", pybind11::arg("c"));

	// bdsg::int_as_dna(int) file:bdsg/internal/utility.hpp line:41
	M("bdsg").def("int_as_dna", (char (*)(int)) &bdsg::int_as_dna, "C++: bdsg::int_as_dna(int) --> char", pybind11::arg("i"));

	// bdsg::format_memory(unsigned long) file:bdsg/internal/utility.hpp line:57
	M("bdsg").def("format_memory", (std::string (*)(unsigned long)) &bdsg::format_memory, "C++: bdsg::format_memory(unsigned long) --> std::string", pybind11::arg("s"));

	// bdsg::get_thread_count() file:bdsg/internal/utility.hpp line:61
	M("bdsg").def("get_thread_count", (int (*)()) &bdsg::get_thread_count, "Return the number of threads that OMP will produce for a parallel section.\n TODO: Assumes that this is the same for every parallel section.\n\nC++: bdsg::get_thread_count() --> int");

	{ // bdsg::HashGraph file:bdsg/hash_graph.hpp line:32
		pybind11::class_<bdsg::HashGraph, std::shared_ptr<bdsg::HashGraph>, PyCallBack_bdsg_HashGraph, handlegraph::MutablePathDeletableHandleGraph, handlegraph::SerializableHandleGraph> cl(M("bdsg"), "HashGraph", "HashGraph is a HandleGraph implementation designed for simplicity. Nodes are\n plain C++ objects stored in a hash map, which contain C++ vectors\n representing their adjacencies.\n\n HashGraph is a good choice when fast access to or modification of a graph is\n required, but can use more memory than other graph implementations.");
		cl.def( pybind11::init( [](){ return new bdsg::HashGraph(); }, [](){ return new PyCallBack_bdsg_HashGraph(); } ) );
		cl.def( pybind11::init( [](PyCallBack_bdsg_HashGraph const &o){ return new PyCallBack_bdsg_HashGraph(o); } ) );
		cl.def( pybind11::init( [](bdsg::HashGraph const &o){ return new bdsg::HashGraph(o); } ) );
		cl.def("has_node", (bool (bdsg::HashGraph::*)(long long) const) &bdsg::HashGraph::has_node, "Method to check if a node exists by ID\n\nC++: bdsg::HashGraph::has_node(long long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::HashGraph const &o, const long long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const long long &, bool) const) &bdsg::HashGraph::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: bdsg::HashGraph::get_handle(const long long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long long (bdsg::HashGraph::*)(const struct handlegraph::handle_t &) const) &bdsg::HashGraph::get_id, "Get the ID from a handle\n\nC++: bdsg::HashGraph::get_id(const struct handlegraph::handle_t &) const --> long long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::HashGraph::*)(const struct handlegraph::handle_t &) const) &bdsg::HashGraph::get_is_reverse, "Get the orientation of a handle\n\nC++: bdsg::HashGraph::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const struct handlegraph::handle_t &) const) &bdsg::HashGraph::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: bdsg::HashGraph::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::HashGraph::*)(const struct handlegraph::handle_t &) const) &bdsg::HashGraph::get_length, "Get the length of a node\n\nC++: bdsg::HashGraph::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::HashGraph::*)(const struct handlegraph::handle_t &) const) &bdsg::HashGraph::get_sequence, "Get the sequence of a node, presented in the handle's local forward orientation.\n\nC++: bdsg::HashGraph::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("follow_edges_impl", (bool (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const) &bdsg::HashGraph::follow_edges_impl, "Loop over all the handles to next/previous (right/left) nodes. Passes\n them to a callback which returns false to stop iterating and true to\n continue. Returns true if we finished and false if we stopped early.\n\nC++: bdsg::HashGraph::follow_edges_impl(const struct handlegraph::handle_t &, bool, const class std::function<bool (const struct handlegraph::handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("go_left"), pybind11::arg("iteratee"));
		cl.def("for_each_handle_impl", [](bdsg::HashGraph const &o, const class std::function<bool (const struct handlegraph::handle_t &)> & a0) -> bool { return o.for_each_handle_impl(a0); }, "", pybind11::arg("iteratee"));
		cl.def("for_each_handle_impl", (bool (bdsg::HashGraph::*)(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const) &bdsg::HashGraph::for_each_handle_impl, "Loop over all the nodes in the graph in their local forward\n orientations, in their internal stored order. Stop if the iteratee\n returns false. Can be told to run in parallel, in which case stopping\n after a false return value is on a best-effort basis and iteration\n order is not defined.\n\nC++: bdsg::HashGraph::for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> &, bool) const --> bool", pybind11::arg("iteratee"), pybind11::arg("parallel"));
		cl.def("get_node_count", (unsigned long (bdsg::HashGraph::*)() const) &bdsg::HashGraph::get_node_count, "Return the number of nodes in the graph\n TODO: can't be node_count because XG has a field named node_count.\n\nC++: bdsg::HashGraph::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long long (bdsg::HashGraph::*)() const) &bdsg::HashGraph::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::HashGraph::min_node_id() const --> long long");
		cl.def("max_node_id", (long long (bdsg::HashGraph::*)() const) &bdsg::HashGraph::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::HashGraph::max_node_id() const --> long long");
		cl.def("get_degree", (unsigned long (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::HashGraph::get_degree, "Efficiently get the number of edges attached to one side of a handle.\n\nC++: bdsg::HashGraph::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("get_base", (char (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, unsigned long) const) &bdsg::HashGraph::get_base, "Returns one base of a handle's sequence, in the orientation of the\n handle.\n\nC++: bdsg::HashGraph::get_base(const struct handlegraph::handle_t &, unsigned long) const --> char", pybind11::arg("handle"), pybind11::arg("index"));
		cl.def("get_subsequence", (std::string (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, unsigned long, unsigned long) const) &bdsg::HashGraph::get_subsequence, "Returns a substring of a handle's sequence, in the orientation of the\n handle. If the indicated substring would extend beyond the end of the\n handle's sequence, the return value is truncated to the sequence's end.\n\nC++: bdsg::HashGraph::get_subsequence(const struct handlegraph::handle_t &, unsigned long, unsigned long) const --> std::string", pybind11::arg("handle"), pybind11::arg("index"), pybind11::arg("size"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const std::string &)) &bdsg::HashGraph::create_handle, "Create a new node with the given sequence and return the handle.\n The sequence may not be empty.\n\nC++: bdsg::HashGraph::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const std::string &, const long long &)) &bdsg::HashGraph::create_handle, "Create a new node with the given id and sequence, then return the handle.\n The sequence may not be empty.\n The ID must be strictly greater than 0.\n\nC++: bdsg::HashGraph::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("destroy_handle", (void (bdsg::HashGraph::*)(const struct handlegraph::handle_t &)) &bdsg::HashGraph::destroy_handle, "Remove the node belonging to the given handle and all of its edges.\n Destroys any paths in which the node participates.\n Invalidates the destroyed handle.\n May be called during serial for_each_handle iteration **ONLY** on the node being iterated.\n May **NOT** be called during parallel for_each_handle iteration.\n May **NOT** be called on the node from which edges are being followed during follow_edges.\n May **NOT** be called during iteration over paths, if it would destroy a path.\n May **NOT** be called during iteration along a path, if it would destroy that path.\n\nC++: bdsg::HashGraph::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("change_sequence", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, const std::string &)) &bdsg::HashGraph::change_sequence, "Change the sequence of handle to a new sequence. Returns a (possibly alterered)\n handle to the node with the new sequence. May invalidate the existing handle. Updates\n paths if called through an inheriting MutablePath interface.\n\nC++: bdsg::HashGraph::change_sequence(const struct handlegraph::handle_t &, const std::string &) --> struct handlegraph::handle_t", pybind11::arg("handle"), pybind11::arg("sequence"));
		cl.def("create_edge", (void (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::HashGraph::create_edge, "Create an edge connecting the given handles in the given order and orientations.\n Ignores existing edges.\n\nC++: bdsg::HashGraph::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::HashGraph::destroy_edge, "Remove the edge connecting the given handles in the given order and orientations.\n Ignores nonexistent edges.\n Does not update any stored paths.\n\nC++: bdsg::HashGraph::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("truncate_handle", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, bool, unsigned long)) &bdsg::HashGraph::truncate_handle, "Shorten a node by truncating either the left or right side of the node, relative to the orientation\n of the handle, starting from a given offset along the nodes sequence. Any edges on the truncated\n end of the node are deleted. Returns a (possibly altered) handle to the truncated node.\n May invalid stored paths.\n\nC++: bdsg::HashGraph::truncate_handle(const struct handlegraph::handle_t &, bool, unsigned long) --> struct handlegraph::handle_t", pybind11::arg("handle"), pybind11::arg("trunc_left"), pybind11::arg("offset"));
		cl.def("clear", (void (bdsg::HashGraph::*)()) &bdsg::HashGraph::clear, "Remove all nodes and edges. Does not update any stored paths.\n\nC++: bdsg::HashGraph::clear() --> void");
		cl.def("apply_orientation", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const struct handlegraph::handle_t &)) &bdsg::HashGraph::apply_orientation, "Alter the node that the given handle corresponds to so the orientation\n indicated by the handle becomes the node's local forward orientation.\n Rewrites all edges pointing to the node and the node's sequence to\n reflect this. Invalidates all handles to the node (including the one\n passed). Returns a new, valid handle to the node in its new forward\n orientation. Note that it is possible for the node's ID to change.\n Does not update any stored paths. May change the ordering of the underlying\n graph.\n\nC++: bdsg::HashGraph::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (class std::vector<handlegraph::handle_t> (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &)) &bdsg::HashGraph::divide_handle, "Split a handle's underlying node at the given offsets in the handle's\n orientation. Returns all of the handles to the parts. Other handles to\n the node being split may be invalidated. The split pieces stay in the\n same local forward orientation as the original node, but the returned\n handles come in the order and orientation appropriate for the handle\n passed in.\n Updates stored paths.\n\nC++: bdsg::HashGraph::divide_handle(const struct handlegraph::handle_t &, const class std::vector<unsigned long> &) --> class std::vector<handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offsets"));
		cl.def("optimize", [](bdsg::HashGraph &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (bdsg::HashGraph::*)(bool)) &bdsg::HashGraph::optimize, "Adjust the representation of the graph in memory to improve performance.\n Optionally, allow the node IDs to be reassigned to further improve\n performance.\n Note: Ideally, this method is called one time once there is expected to be\n few graph modifications in the future.\n\nC++: bdsg::HashGraph::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("apply_ordering", [](bdsg::HashGraph &o, const class std::vector<handlegraph::handle_t> & a0) -> bool { return o.apply_ordering(a0); }, "", pybind11::arg("order"));
		cl.def("apply_ordering", (bool (bdsg::HashGraph::*)(const class std::vector<handlegraph::handle_t> &, bool)) &bdsg::HashGraph::apply_ordering, "Reorder the graph's internal structure to match that given.\n This sets the order that is used for iteration in functions like for_each_handle.\n If compact_ids is true, may (but will not necessarily) compact the id space of the graph to match the ordering, from 1->|ordering|.\n In other cases, node IDs will be preserved.\n This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.\n This may invalidate outstanding handles.\n Returns true if node IDs actually were adjusted to match the given order, and false if they remain unchanged.\n\nC++: bdsg::HashGraph::apply_ordering(const class std::vector<handlegraph::handle_t> &, bool) --> bool", pybind11::arg("order"), pybind11::arg("compact_ids"));
		cl.def("get_path_count", (unsigned long (bdsg::HashGraph::*)() const) &bdsg::HashGraph::get_path_count, "Returns the number of paths stored in the graph\n\nC++: bdsg::HashGraph::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (bdsg::HashGraph::*)(const std::string &) const) &bdsg::HashGraph::has_path, "Determine if a path name exists and is legal to get a path handle for.\n\nC++: bdsg::HashGraph::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (bdsg::HashGraph::*)(const std::string &) const) &bdsg::HashGraph::get_path_handle, "Look up the path handle for the given path name.\n The path with that name must exist.\n\nC++: bdsg::HashGraph::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::get_path_name, "Look up the name of a path from a handle to it\n\nC++: bdsg::HashGraph::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::get_is_circular, "Look up whether a path is circular\n\nC++: bdsg::HashGraph::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::get_step_count, "Returns the number of node steps in the path\n\nC++: bdsg::HashGraph::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::get_handle_of_step, "Get a node handle (node ID and orientation) from a handle to a step on a path\n\nC++: bdsg::HashGraph::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::get_path_handle_of_step, "Returns a handle to the path that a step is on\n\nC++: bdsg::HashGraph::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::path_begin, "Get a handle to the first step, or in a circular path to an arbitrary step\n considered \"first\". If the path is empty, returns the past-the-last step\n returned by path_end.\n\nC++: bdsg::HashGraph::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::path_end, "Get a handle to a fictitious position past the end of a path. This position is\n return by get_next_step for the final step in a path in a non-circular path.\n Note that get_next_step will *NEVER* return this value for a circular path.\n\nC++: bdsg::HashGraph::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::path_back, "Get a handle to the last step, which will be an arbitrary step in a circular path that\n we consider \"last\" based on our construction of the path. If the path is empty\n then the implementation must return the same value as path_front_end().\n\nC++: bdsg::HashGraph::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &) const) &bdsg::HashGraph::path_front_end, "Get a handle to a fictitious position before the beginning of a path. This position is\n return by get_previous_step for the first step in a path in a non-circular path.\n Note: get_previous_step will *NEVER* return this value for a circular path.\n\nC++: bdsg::HashGraph::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::has_next_step, "Returns true if the step is not the last step in a non-circular path.\n\nC++: bdsg::HashGraph::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::has_previous_step, "Returns true if the step is not the first step in a non-circular path.\n\nC++: bdsg::HashGraph::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::get_next_step, "Returns a handle to the next step on the path. If the given step is the final step\n of a non-circular path, returns the past-the-last step that is also returned by\n path_end. In a circular path, the \"last\" step will loop around to the \"first\" (i.e.\n the one returned by path_begin).\n Note: to iterate over each step one time, even in a circular path, consider\n for_each_step_in_path.\n\nC++: bdsg::HashGraph::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &) const) &bdsg::HashGraph::get_previous_step, "Returns a handle to the previous step on the path. If the given step is the first\n step of a non-circular path, this method has undefined behavior. In a circular path,\n it will loop around from the \"first\" step (i.e. the one returned by path_begin) to\n the \"last\" step.\n Note: to iterate over each step one time, even in a circular path, consider\n for_each_step_in_path.\n\nC++: bdsg::HashGraph::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("for_each_path_handle_impl", (bool (bdsg::HashGraph::*)(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const) &bdsg::HashGraph::for_each_path_handle_impl, "Execute a function on each path in the graph\n\nC++: bdsg::HashGraph::for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> &) const --> bool", pybind11::arg("iteratee"));
		cl.def("for_each_step_on_handle_impl", (bool (bdsg::HashGraph::*)(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const) &bdsg::HashGraph::for_each_step_on_handle_impl, "Calls a function with all steps of a node on paths.\n\nC++: bdsg::HashGraph::for_each_step_on_handle_impl(const struct handlegraph::handle_t &, const class std::function<bool (const struct handlegraph::step_handle_t &)> &) const --> bool", pybind11::arg("handle"), pybind11::arg("iteratee"));
		cl.def("destroy_path", (void (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &)) &bdsg::HashGraph::destroy_path, "Destroy the given path. Invalidates handles to the path and its node steps.\n\nC++: bdsg::HashGraph::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("destroy_paths", (void (bdsg::HashGraph::*)(const class std::vector<handlegraph::path_handle_t> &)) &bdsg::HashGraph::destroy_paths, "Destroy the given set of paths. Invalidates handles to all the paths and their steps.\n\nC++: bdsg::HashGraph::destroy_paths(const class std::vector<handlegraph::path_handle_t> &) --> void", pybind11::arg("paths"));
		cl.def("create_path_handle", [](bdsg::HashGraph &o, const std::string & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (bdsg::HashGraph::*)(const std::string &, bool)) &bdsg::HashGraph::create_path_handle, "Create a path with the given name. The caller must ensure that no path\n with the given name exists already, or the behavior is undefined.\n Returns a handle to the created empty path. Handles to other paths must\n remain valid.\n\nC++: bdsg::HashGraph::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("append_step", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::HashGraph::append_step, "Append a visit to a node to the given path. Returns a handle to the new\n final step on the path which is appended. If the path is cirular, the new\n step is placed between the steps considered \"last\" and \"first\" by the\n method path_begin. Handles to prior steps on the path, and to other paths,\n must remain valid.\n\nC++: bdsg::HashGraph::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::HashGraph::prepend_step, "Prepend a visit to a node to the given path. Returns a handle to the new\n first step on the path which is appended. If the path is cirular, the new\n step is placed between the steps considered \"last\" and \"first\" by the\n method path_begin. Handles to later steps on the path, and to other paths,\n must remain valid.\n\nC++: bdsg::HashGraph::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_prepend"));
		cl.def("rewrite_segment", (struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t> (bdsg::HashGraph::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &)) &bdsg::HashGraph::rewrite_segment, "Delete a segment of a path and rewrite it as some other sequence of\n steps. Returns a pair of step_handle_t's that indicate the range of the\n new segment in the path. The segment to delete should be designated by\n the first (begin) and past-last (end) step handles.  If the step that is\n returned by path_begin is deleted, path_begin will now return the first\n step from the new segment or, in the case that the new segment is empty,\n the step used as segment_end. Empty ranges consist of two copies of the\n same step handle. Empty ranges in empty paths consist of two copies of\n the end sentinel handle for the path. Rewriting an empty range inserts\n before the provided end handle.\n\nC++: bdsg::HashGraph::rewrite_segment(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const class std::vector<handlegraph::handle_t> &) --> struct std::pair<struct handlegraph::step_handle_t, struct handlegraph::step_handle_t>", pybind11::arg("segment_begin"), pybind11::arg("segment_end"), pybind11::arg("new_segment"));
		cl.def("set_circularity", (void (bdsg::HashGraph::*)(const struct handlegraph::path_handle_t &, bool)) &bdsg::HashGraph::set_circularity, "Make a path circular or non-circular. If the path is becoming circular, the\n last step is joined to the first step. If the path is becoming linear, the\n step considered \"last\" is unjoined from the step considered \"first\" according\n to the method path_begin.\n\nC++: bdsg::HashGraph::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path"), pybind11::arg("circular"));
		cl.def("set_id_increment", (void (bdsg::HashGraph::*)(const long long &)) &bdsg::HashGraph::set_id_increment, "Set a minimum id to increment the id space by, used as a hint during construction.\n May have no effect on a backing implementation.\n\nC++: bdsg::HashGraph::set_id_increment(const long long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (bdsg::HashGraph::*)(long long)) &bdsg::HashGraph::increment_node_ids, "Add the given value to all node IDs\n\nC++: bdsg::HashGraph::increment_node_ids(long long) --> void", pybind11::arg("increment"));
		cl.def("reassign_node_ids", (void (bdsg::HashGraph::*)(const class std::function<long long (const long long &)> &)) &bdsg::HashGraph::reassign_node_ids, "Reassign all node IDs as specified by the old->new mapping function.\n\nC++: bdsg::HashGraph::reassign_node_ids(const class std::function<long long (const long long &)> &) --> void", pybind11::arg("get_new_id"));
		cl.def("get_magic_number", (unsigned int (bdsg::HashGraph::*)() const) &bdsg::HashGraph::get_magic_number, "Returns a static high-entropy number to indicate the class\n\nC++: bdsg::HashGraph::get_magic_number() const --> unsigned int");
		cl.def("assign", (class bdsg::HashGraph & (bdsg::HashGraph::*)(const class bdsg::HashGraph &)) &bdsg::HashGraph::operator=, "C++: bdsg::HashGraph::operator=(const class bdsg::HashGraph &) --> class bdsg::HashGraph &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
	}
}
