#include <bdsg/node.hpp>
#include <bdsg/odgi.hpp>
#include <cwchar>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <map>
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


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

// bdsg::ODGI file:bdsg/odgi.hpp line:39
struct PyCallBack_bdsg_ODGI : public bdsg::ODGI {
	using bdsg::ODGI::ODGI;

	bool has_node(long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "has_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::has_node(a0);
	}
	struct handlegraph::handle_t get_handle(const long & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::get_handle(a0, a1);
	}
	long get_id(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return ODGI::get_id(a0);
	}
	bool get_is_reverse(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_is_reverse");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::get_is_reverse(a0);
	}
	struct handlegraph::handle_t flip(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::flip(a0);
	}
	unsigned long get_length(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_length");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return ODGI::get_length(a0);
	}
	class std::__cxx11::basic_string<char> get_sequence(const struct handlegraph::handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_sequence");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::__cxx11::basic_string<char>>::value) {
				static pybind11::detail::overload_caster_t<class std::__cxx11::basic_string<char>> caster;
				return pybind11::detail::cast_ref<class std::__cxx11::basic_string<char>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::__cxx11::basic_string<char>>(std::move(o));
		}
		return ODGI::get_sequence(a0);
	}
	bool follow_edges_impl(const struct handlegraph::handle_t & a0, bool a1, const class std::function<bool (const struct handlegraph::handle_t &)> & a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "follow_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::follow_edges_impl(a0, a1, a2);
	}
	bool for_each_handle_impl(const class std::function<bool (const struct handlegraph::handle_t &)> & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "for_each_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::for_each_handle_impl(a0, a1);
	}
	unsigned long get_node_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_node_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return ODGI::get_node_count();
	}
	long min_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "min_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return ODGI::min_node_id();
	}
	long max_node_id() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "max_node_id");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::overload_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return ODGI::max_node_id();
	}
	void set_id_increment(const long & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "set_id_increment");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::set_id_increment(a0);
	}
	unsigned long get_degree(const struct handlegraph::handle_t & a0, bool a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_degree");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return ODGI::get_degree(a0, a1);
	}
	bool has_path(const class std::__cxx11::basic_string<char> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::has_path(a0);
	}
	struct handlegraph::path_handle_t get_path_handle(const class std::__cxx11::basic_string<char> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return ODGI::get_path_handle(a0);
	}
	class std::__cxx11::basic_string<char> get_path_name(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::__cxx11::basic_string<char>>::value) {
				static pybind11::detail::overload_caster_t<class std::__cxx11::basic_string<char>> caster;
				return pybind11::detail::cast_ref<class std::__cxx11::basic_string<char>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::__cxx11::basic_string<char>>(std::move(o));
		}
		return ODGI::get_path_name(a0);
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return ODGI::get_step_count(a0);
	}
	unsigned long get_path_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		return ODGI::get_path_count();
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "for_each_path_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::for_each_path_handle_impl(a0);
	}
	bool for_each_step_on_handle_impl(const struct handlegraph::handle_t & a0, const class std::function<bool (const struct handlegraph::step_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "for_each_step_on_handle_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::for_each_step_on_handle_impl(a0, a1);
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::get_handle_of_step(a0);
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::path_begin(a0);
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::path_end(a0);
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::path_back(a0);
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::path_front_end(a0);
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::has_next_step(a0);
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::has_previous_step(a0);
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::get_next_step(a0);
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::get_previous_step(a0);
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return ODGI::get_path_handle_of_step(a0);
	}
	bool is_empty(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "is_empty");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::is_empty(a0);
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::get_is_circular(a0);
	}
	void set_circularity(const struct handlegraph::path_handle_t & a0, bool a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "set_circularity");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::set_circularity(a0, a1);
	}
	struct handlegraph::handle_t create_handle(const class std::__cxx11::basic_string<char> & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::create_handle(a0);
	}
	struct handlegraph::handle_t create_handle(const class std::__cxx11::basic_string<char> & a0, const long & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "create_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::create_handle(a0, a1);
	}
	void destroy_handle(const struct handlegraph::handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "destroy_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::destroy_handle(a0);
	}
	void create_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "create_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::create_edge(a0, a1);
	}
	bool has_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "has_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return ODGI::has_edge(a0, a1);
	}
	void destroy_edge(const struct handlegraph::handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "destroy_edge");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::destroy_edge(a0, a1);
	}
	void clear() override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "clear");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::clear();
	}
	void optimize(bool a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "optimize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::optimize(a0);
	}
	struct handlegraph::handle_t apply_orientation(const struct handlegraph::handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "apply_orientation");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return ODGI::apply_orientation(a0);
	}
	void destroy_path(const struct handlegraph::path_handle_t & a0) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "destroy_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::overload_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return ODGI::destroy_path(a0);
	}
	struct handlegraph::path_handle_t create_path_handle(const class std::__cxx11::basic_string<char> & a0, bool a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "create_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		return ODGI::create_path_handle(a0, a1);
	}
	struct handlegraph::step_handle_t prepend_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "prepend_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::prepend_step(a0, a1);
	}
	struct handlegraph::step_handle_t append_step(const struct handlegraph::path_handle_t & a0, const struct handlegraph::handle_t & a1) override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "append_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		return ODGI::append_step(a0, a1);
	}
	char get_base(const struct handlegraph::handle_t & a0, unsigned long a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_base");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::ODGI *>(this), "get_subsequence");
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
};

void bind_bdsg_node(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::node_t file:bdsg/node.hpp line:23
		pybind11::class_<bdsg::node_t, std::shared_ptr<bdsg::node_t>> cl(M("bdsg"), "node_t", "A node object with the sequence, its edge lists, and paths");
		{ // bdsg::node_t::step_t file:bdsg/node.hpp line:39
			auto & enclosing_class = cl;
			pybind11::class_<bdsg::node_t::step_t, std::shared_ptr<bdsg::node_t::step_t>> cl(enclosing_class, "step_t", "");
			cl.def( pybind11::init( [](){ return new bdsg::node_t::step_t(); } ) );
			cl.def( pybind11::init<const unsigned long &, const bool &, const unsigned long &, const unsigned long &, const unsigned long &, const unsigned long &>(), pybind11::arg("id"), pybind11::arg("rev"), pybind11::arg("prev_id"), pybind11::arg("prev_rank"), pybind11::arg("next_id"), pybind11::arg("next_rank") );

			cl.def("path_id", (const unsigned long (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::path_id, "C++: bdsg::node_t::step_t::path_id() const --> const unsigned long");
			cl.def("is_rev", (const bool (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::is_rev, "C++: bdsg::node_t::step_t::is_rev() const --> const bool");
			cl.def("prev_id", (const unsigned long (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::prev_id, "C++: bdsg::node_t::step_t::prev_id() const --> const unsigned long");
			cl.def("prev_rank", (const unsigned long (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::prev_rank, "C++: bdsg::node_t::step_t::prev_rank() const --> const unsigned long");
			cl.def("next_id", (const unsigned long (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::next_id, "C++: bdsg::node_t::step_t::next_id() const --> const unsigned long");
			cl.def("next_rank", (const unsigned long (bdsg::node_t::step_t::*)() const) &bdsg::node_t::step_t::next_rank, "C++: bdsg::node_t::step_t::next_rank() const --> const unsigned long");
			cl.def("set_path_step", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_path_step, "C++: bdsg::node_t::step_t::set_path_step(const unsigned long &) --> void", pybind11::arg("i"));
			cl.def("set_path_id", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_path_id, "C++: bdsg::node_t::step_t::set_path_id(const unsigned long &) --> void", pybind11::arg("i"));
			cl.def("set_is_rev", (void (bdsg::node_t::step_t::*)(const bool &)) &bdsg::node_t::step_t::set_is_rev, "C++: bdsg::node_t::step_t::set_is_rev(const bool &) --> void", pybind11::arg("b"));
			cl.def("set_prev_id", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_prev_id, "C++: bdsg::node_t::step_t::set_prev_id(const unsigned long &) --> void", pybind11::arg("i"));
			cl.def("set_prev_rank", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_prev_rank, "C++: bdsg::node_t::step_t::set_prev_rank(const unsigned long &) --> void", pybind11::arg("i"));
			cl.def("set_next_id", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_next_id, "C++: bdsg::node_t::step_t::set_next_id(const unsigned long &) --> void", pybind11::arg("i"));
			cl.def("set_next_rank", (void (bdsg::node_t::step_t::*)(const unsigned long &)) &bdsg::node_t::step_t::set_next_rank, "C++: bdsg::node_t::step_t::set_next_rank(const unsigned long &) --> void", pybind11::arg("i"));
		}

		cl.def( pybind11::init( [](){ return new bdsg::node_t(); } ) );
		cl.def("seq_start", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::seq_start, "C++: bdsg::node_t::seq_start() const --> const unsigned long");
		cl.def("seq_bytes", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::seq_bytes, "C++: bdsg::node_t::seq_bytes() const --> const unsigned long");
		cl.def("edge_start", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::edge_start, "C++: bdsg::node_t::edge_start() const --> const unsigned long");
		cl.def("edge_count", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::edge_count, "C++: bdsg::node_t::edge_count() const --> const unsigned long");
		cl.def("edge_bytes", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::edge_bytes, "C++: bdsg::node_t::edge_bytes() const --> const unsigned long");
		cl.def("path_count", (const unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::path_count, "C++: bdsg::node_t::path_count() const --> const unsigned long");
		cl.def("set_seq_bytes", (void (bdsg::node_t::*)(const unsigned long &)) &bdsg::node_t::set_seq_bytes, "C++: bdsg::node_t::set_seq_bytes(const unsigned long &) --> void", pybind11::arg("i"));
		cl.def("set_edge_count", (void (bdsg::node_t::*)(const unsigned long &)) &bdsg::node_t::set_edge_count, "C++: bdsg::node_t::set_edge_count(const unsigned long &) --> void", pybind11::arg("i"));
		cl.def("set_edge_bytes", (void (bdsg::node_t::*)(const unsigned long &)) &bdsg::node_t::set_edge_bytes, "C++: bdsg::node_t::set_edge_bytes(const unsigned long &) --> void", pybind11::arg("i"));
		cl.def("sequence_size", (unsigned long (bdsg::node_t::*)() const) &bdsg::node_t::sequence_size, "C++: bdsg::node_t::sequence_size() const --> unsigned long");
		cl.def("sequence", (const std::string (bdsg::node_t::*)() const) &bdsg::node_t::sequence, "C++: bdsg::node_t::sequence() const --> const std::string");
		cl.def("set_sequence", (void (bdsg::node_t::*)(const std::string &)) &bdsg::node_t::set_sequence, "C++: bdsg::node_t::set_sequence(const std::string &) --> void", pybind11::arg("seq"));
		cl.def("add_edge", (void (bdsg::node_t::*)(const unsigned long &, const unsigned long &)) &bdsg::node_t::add_edge, "C++: bdsg::node_t::add_edge(const unsigned long &, const unsigned long &) --> void", pybind11::arg("relative_id"), pybind11::arg("edge_type"));
		cl.def("remove_edge", (void (bdsg::node_t::*)(const unsigned long &)) &bdsg::node_t::remove_edge, "C++: bdsg::node_t::remove_edge(const unsigned long &) --> void", pybind11::arg("rank"));
		cl.def("add_path_step", (void (bdsg::node_t::*)(const unsigned long &, const bool &, const unsigned long &, const unsigned long &, const unsigned long &, const unsigned long &)) &bdsg::node_t::add_path_step, "C++: bdsg::node_t::add_path_step(const unsigned long &, const bool &, const unsigned long &, const unsigned long &, const unsigned long &, const unsigned long &) --> void", pybind11::arg("path_id"), pybind11::arg("is_rev"), pybind11::arg("prev_id"), pybind11::arg("prev_rank"), pybind11::arg("next_id"), pybind11::arg("next_rank"));
		cl.def("add_path_step", (void (bdsg::node_t::*)(const struct bdsg::node_t::step_t &)) &bdsg::node_t::add_path_step, "C++: bdsg::node_t::add_path_step(const struct bdsg::node_t::step_t &) --> void", pybind11::arg("step"));
		cl.def("set_path_step", (void (bdsg::node_t::*)(const unsigned long &, const struct bdsg::node_t::step_t &)) &bdsg::node_t::set_path_step, "C++: bdsg::node_t::set_path_step(const unsigned long &, const struct bdsg::node_t::step_t &) --> void", pybind11::arg("rank"), pybind11::arg("step"));
		cl.def("flip_paths", (struct std::pair<class std::map<unsigned long, struct std::pair<unsigned long, bool>, struct std::less<unsigned long>, class std::allocator<struct std::pair<const unsigned long, struct std::pair<unsigned long, bool> > > >, class std::map<unsigned long, struct std::pair<unsigned long, bool>, struct std::less<unsigned long>, class std::allocator<struct std::pair<const unsigned long, struct std::pair<unsigned long, bool> > > > > (bdsg::node_t::*)(const unsigned long &, const unsigned long &)) &bdsg::node_t::flip_paths, "C++: bdsg::node_t::flip_paths(const unsigned long &, const unsigned long &) --> struct std::pair<class std::map<unsigned long, struct std::pair<unsigned long, bool>, struct std::less<unsigned long>, class std::allocator<struct std::pair<const unsigned long, struct std::pair<unsigned long, bool> > > >, class std::map<unsigned long, struct std::pair<unsigned long, bool>, struct std::less<unsigned long>, class std::allocator<struct std::pair<const unsigned long, struct std::pair<unsigned long, bool> > > > >", pybind11::arg("start_marker"), pybind11::arg("end_marker"));
		cl.def("get_path_step", (const struct bdsg::node_t::step_t (bdsg::node_t::*)(const unsigned long &) const) &bdsg::node_t::get_path_step, "C++: bdsg::node_t::get_path_step(const unsigned long &) const --> const struct bdsg::node_t::step_t", pybind11::arg("rank"));
		cl.def("remove_path_step", (void (bdsg::node_t::*)(const unsigned long &)) &bdsg::node_t::remove_path_step, "C++: bdsg::node_t::remove_path_step(const unsigned long &) --> void", pybind11::arg("rank"));
		cl.def("update_path_last_bytes", (void (bdsg::node_t::*)()) &bdsg::node_t::update_path_last_bytes, "C++: bdsg::node_t::update_path_last_bytes() --> void");
		cl.def("clear", (void (bdsg::node_t::*)()) &bdsg::node_t::clear, "C++: bdsg::node_t::clear() --> void");
		cl.def("clear_path_steps", (void (bdsg::node_t::*)()) &bdsg::node_t::clear_path_steps, "C++: bdsg::node_t::clear_path_steps() --> void");
		cl.def("serialize", (unsigned long (bdsg::node_t::*)(std::ostream &) const) &bdsg::node_t::serialize, "C++: bdsg::node_t::serialize(std::ostream &) const --> unsigned long", pybind11::arg("out"));
		cl.def("load", (void (bdsg::node_t::*)(class std::basic_istream<char> &)) &bdsg::node_t::load, "C++: bdsg::node_t::load(class std::basic_istream<char> &) --> void", pybind11::arg("in"));
		cl.def("display", (void (bdsg::node_t::*)() const) &bdsg::node_t::display, "C++: bdsg::node_t::display() const --> void");
		cl.def_static("pack_step", (unsigned long (*)(const unsigned long &, const bool &)) &bdsg::node_t::pack_step, "C++: bdsg::node_t::pack_step(const unsigned long &, const bool &) --> unsigned long", pybind11::arg("path_id"), pybind11::arg("is_rev"));
		cl.def_static("step_path_id", (unsigned long (*)(const unsigned long &)) &bdsg::node_t::step_path_id, "C++: bdsg::node_t::step_path_id(const unsigned long &) --> unsigned long", pybind11::arg("step"));
		cl.def_static("step_is_rev", (bool (*)(const unsigned long &)) &bdsg::node_t::step_is_rev, "C++: bdsg::node_t::step_is_rev(const unsigned long &) --> bool", pybind11::arg("step"));
	}
	{ // bdsg::ODGI file:bdsg/odgi.hpp line:39
		pybind11::class_<bdsg::ODGI, std::shared_ptr<bdsg::ODGI>, PyCallBack_bdsg_ODGI, handlegraph::MutablePathDeletableHandleGraph> cl(M("bdsg"), "ODGI", "");
		cl.def( pybind11::init( [](){ return new bdsg::ODGI(); }, [](){ return new PyCallBack_bdsg_ODGI(); } ) );
		cl.def("has_node", (bool (bdsg::ODGI::*)(long) const) &bdsg::ODGI::has_node, "Method to check if a node exists by ID\n\nC++: bdsg::ODGI::has_node(long) const --> bool", pybind11::arg("node_id"));
		cl.def("get_handle", [](bdsg::ODGI const &o, const long & a0) -> handlegraph::handle_t { return o.get_handle(a0); }, "", pybind11::arg("node_id"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::ODGI::*)(const long &, bool) const) &bdsg::ODGI::get_handle, "Look up the handle for the node with the given ID in the given orientation\n\nC++: bdsg::ODGI::get_handle(const long &, bool) const --> struct handlegraph::handle_t", pybind11::arg("node_id"), pybind11::arg("is_reverse"));
		cl.def("get_id", (long (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::get_id, "Get the ID from a handle\n\nC++: bdsg::ODGI::get_id(const struct handlegraph::handle_t &) const --> long", pybind11::arg("handle"));
		cl.def("get_is_reverse", (bool (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::get_is_reverse, "Get the orientation of a handle\n\nC++: bdsg::ODGI::get_is_reverse(const struct handlegraph::handle_t &) const --> bool", pybind11::arg("handle"));
		cl.def("flip", (struct handlegraph::handle_t (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::flip, "Invert the orientation of a handle (potentially without getting its ID)\n\nC++: bdsg::ODGI::flip(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("get_length", (unsigned long (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::get_length, "Get the length of a node\n\nC++: bdsg::ODGI::get_length(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_sequence", (std::string (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::get_sequence, "Get the sequence of a node, presented in the handle's local forward orientation.\n\nC++: bdsg::ODGI::get_sequence(const struct handlegraph::handle_t &) const --> std::string", pybind11::arg("handle"));
		cl.def("get_node_count", (unsigned long (bdsg::ODGI::*)() const) &bdsg::ODGI::get_node_count, "Return the number of nodes in the graph\n TODO: can't be node_count because XG has a field named node_count.\n\nC++: bdsg::ODGI::get_node_count() const --> unsigned long");
		cl.def("min_node_id", (long (bdsg::ODGI::*)() const) &bdsg::ODGI::min_node_id, "Return the smallest ID in the graph, or some smaller number if the\n smallest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::ODGI::min_node_id() const --> long");
		cl.def("max_node_id", (long (bdsg::ODGI::*)() const) &bdsg::ODGI::max_node_id, "Return the largest ID in the graph, or some larger number if the\n largest ID is unavailable. Return value is unspecified if the graph is empty.\n\nC++: bdsg::ODGI::max_node_id() const --> long");
		cl.def("set_id_increment", (void (bdsg::ODGI::*)(const long &)) &bdsg::ODGI::set_id_increment, "Set a minimum id to increment the id space by, used as a hint during construction.\n May have no effect on a backing implementation.\n\nC++: bdsg::ODGI::set_id_increment(const long &) --> void", pybind11::arg("min_id"));
		cl.def("increment_node_ids", (void (bdsg::ODGI::*)(long)) &bdsg::ODGI::increment_node_ids, "Add the given value to all node IDs\n\nC++: bdsg::ODGI::increment_node_ids(long) --> void", pybind11::arg("increment"));
		cl.def("get_degree", (unsigned long (bdsg::ODGI::*)(const struct handlegraph::handle_t &, bool) const) &bdsg::ODGI::get_degree, "Get the number of edges on the right (go_left = false) or left (go_left\n = true) side of the given handle. The default implementation is O(n) in\n the number of edges returned, but graph implementations that track this\n information more efficiently can override this method.\n\nC++: bdsg::ODGI::get_degree(const struct handlegraph::handle_t &, bool) const --> unsigned long", pybind11::arg("handle"), pybind11::arg("go_left"));
		cl.def("forward", (struct handlegraph::handle_t (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::forward, "Get the locally forward version of a handle\n\nC++: bdsg::ODGI::forward(const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("edge_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (bdsg::ODGI::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &bdsg::ODGI::edge_handle, "A pair of handles can be used as an edge. When so used, the handles have a\n canonical order and orientation.\n\nC++: bdsg::ODGI::edge_handle(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("traverse_edge_handle", (struct handlegraph::handle_t (bdsg::ODGI::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const) &bdsg::ODGI::traverse_edge_handle, "Such a pair can be viewed from either inward end handle and produce the\n outward handle you would arrive at.\n\nC++: bdsg::ODGI::traverse_edge_handle(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &, const struct handlegraph::handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("edge"), pybind11::arg("left"));
		cl.def("has_path", (bool (bdsg::ODGI::*)(const std::string &) const) &bdsg::ODGI::has_path, "Determine if a path name exists and is legal to get a path handle for.\n\nC++: bdsg::ODGI::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (bdsg::ODGI::*)(const std::string &) const) &bdsg::ODGI::get_path_handle, "Look up the path handle for the given path name.\n The path with that name must exist.\n\nC++: bdsg::ODGI::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::get_path_name, "Look up the name of a path from a handle to it\n\nC++: bdsg::ODGI::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::get_step_count, "Returns the number of node steps in the path\n\nC++: bdsg::ODGI::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_path_count", (unsigned long (bdsg::ODGI::*)() const) &bdsg::ODGI::get_path_count, "Returns the number of paths stored in the graph\n\nC++: bdsg::ODGI::get_path_count() const --> unsigned long");
		cl.def("get_step_count", (unsigned long (bdsg::ODGI::*)(const struct handlegraph::handle_t &) const) &bdsg::ODGI::get_step_count, "Returns the number of node steps on the handle\n\nC++: bdsg::ODGI::get_step_count(const struct handlegraph::handle_t &) const --> unsigned long", pybind11::arg("handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_handle_of_step, "Get a node handle (node ID and orientation) from a handle to an step on a path\n\nC++: bdsg::ODGI::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("get_path", (struct handlegraph::path_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_path, "Get a path handle (path ID) from a handle to an step on a path\n\nC++: bdsg::ODGI::get_path(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::path_begin, "Get a handle to the first step in a path.\n The path MUST be nonempty.\n\nC++: bdsg::ODGI::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::path_end, "Get a handle to a fictitious handle one past the end of the path\n\nC++: bdsg::ODGI::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::path_back, "Get a handle to the last step, which is arbitrary in the case of a circular path\n\nC++: bdsg::ODGI::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::path_front_end, "Get a handle to a fictitious handle one past the start of the path\n\nC++: bdsg::ODGI::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("is_path_front_end", (bool (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::is_path_front_end, "Returns true if the step handle is a front end magic handle\n\nC++: bdsg::ODGI::is_path_front_end(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("is_path_end", (bool (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::is_path_end, "Returns true if the step handle is an end magic handle\n\nC++: bdsg::ODGI::is_path_end(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_next_step", (bool (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::has_next_step, "Returns true if the step is not the last step on the path, else false\n\nC++: bdsg::ODGI::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::has_previous_step, "Returns true if the step is not the first step on the path, else false\n\nC++: bdsg::ODGI::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_next_step, "Returns a handle to the next step on the path\n\nC++: bdsg::ODGI::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_previous_step, "Returns a handle to the previous step on the path\n\nC++: bdsg::ODGI::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_path_handle_of_step, "Returns a handle to the path that an step is on\n\nC++: bdsg::ODGI::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("get_ordinal_rank_of_step", (unsigned long (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &) const) &bdsg::ODGI::get_ordinal_rank_of_step, "Returns the 0-based ordinal rank of a step on a path\n\nC++: bdsg::ODGI::get_ordinal_rank_of_step(const struct handlegraph::step_handle_t &) const --> unsigned long", pybind11::arg("step_handle"));
		cl.def("is_empty", (bool (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::is_empty, "Returns true if the given path is empty, and false otherwise\n\nC++: bdsg::ODGI::is_empty(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("for_each_step_in_path", (void (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &, const class std::function<void (const struct handlegraph::step_handle_t &)> &) const) &bdsg::ODGI::for_each_step_in_path, "Loop over all the steps along a path, from first through last\n\nC++: bdsg::ODGI::for_each_step_in_path(const struct handlegraph::path_handle_t &, const class std::function<void (const struct handlegraph::step_handle_t &)> &) const --> void", pybind11::arg("path"), pybind11::arg("iteratee"));
		cl.def("get_is_circular", (bool (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &) const) &bdsg::ODGI::get_is_circular, "Returns true if the path is circular\n\nC++: bdsg::ODGI::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("set_circularity", (void (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &, bool)) &bdsg::ODGI::set_circularity, "Set if the path is circular or not\n\nC++: bdsg::ODGI::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path_handle"), pybind11::arg("circular"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::ODGI::*)(const std::string &)) &bdsg::ODGI::create_handle, "Create a new node with the given sequence and return the handle.\n\nC++: bdsg::ODGI::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (bdsg::ODGI::*)(const std::string &, const long &)) &bdsg::ODGI::create_handle, "Create a new node with the given id and sequence, then return the handle.\n\nC++: bdsg::ODGI::create_handle(const std::string &, const long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("create_hidden_handle", (struct handlegraph::handle_t (bdsg::ODGI::*)(const std::string &)) &bdsg::ODGI::create_hidden_handle, "Create a \"hidden\" node which might carry parts of paths that traversed deleted portions of the graph\n\nC++: bdsg::ODGI::create_hidden_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("destroy_handle", (void (bdsg::ODGI::*)(const struct handlegraph::handle_t &)) &bdsg::ODGI::destroy_handle, "Remove the node belonging to the given handle and all of its edges.\n Does not update any stored paths.\n Invalidates the destroyed handle.\n May be called during serial for_each_handle iteration **ONLY** on the node being iterated.\n May **NOT** be called during parallel for_each_handle iteration.\n May **NOT** be called on the node from which edges are being followed during follow_edges.\n\nC++: bdsg::ODGI::destroy_handle(const struct handlegraph::handle_t &) --> void", pybind11::arg("handle"));
		cl.def("create_edge", (void (bdsg::ODGI::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::create_edge, "Create an edge connecting the given handles in the given order and orientations.\n Ignores existing edges.\n\nC++: bdsg::ODGI::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("has_edge", (bool (bdsg::ODGI::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const) &bdsg::ODGI::has_edge, "Check if an edge exists\n\nC++: bdsg::ODGI::has_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) const --> bool", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("create_edge", (void (bdsg::ODGI::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &bdsg::ODGI::create_edge, "Convenient wrapper for create_edge.\n\nC++: bdsg::ODGI::create_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("destroy_edge", (void (bdsg::ODGI::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::destroy_edge, "Remove the edge connecting the given handles in the given order and orientations.\n Ignores nonexistent edges.\n Does not update any stored paths.\n\nC++: bdsg::ODGI::destroy_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("destroy_edge", (void (bdsg::ODGI::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &bdsg::ODGI::destroy_edge, "Convenient wrapper for destroy_edge.\n\nC++: bdsg::ODGI::destroy_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("clear", (void (bdsg::ODGI::*)()) &bdsg::ODGI::clear, "Remove all nodes and edges. Does not update any stored paths.\n\nC++: bdsg::ODGI::clear() --> void");
		cl.def("clear_paths", (void (bdsg::ODGI::*)()) &bdsg::ODGI::clear_paths, "Remove all stored paths\n\nC++: bdsg::ODGI::clear_paths() --> void");
		cl.def("swap_handles", (void (bdsg::ODGI::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::swap_handles, "Swap the nodes corresponding to the given handles, in the ordering used\n by for_each_handle when looping over the graph. Other handles to the\n nodes being swapped must not be invalidated. If a swap is made while\n for_each_handle is running, it affects the order of the handles\n traversed during the current traversal (so swapping an already seen\n handle to a later handle's position will make the seen handle be visited\n again and the later handle not be visited at all).\n\nC++: bdsg::ODGI::swap_handles(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("a"), pybind11::arg("b"));
		cl.def("optimize", [](bdsg::ODGI &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (bdsg::ODGI::*)(bool)) &bdsg::ODGI::optimize, "Organize the graph for better performance and memory use\n\nC++: bdsg::ODGI::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("apply_orientation", (struct handlegraph::handle_t (bdsg::ODGI::*)(const struct handlegraph::handle_t &)) &bdsg::ODGI::apply_orientation, "Alter the node that the given handle corresponds to so the orientation\n indicated by the handle becomes the node's local forward orientation.\n Rewrites all edges pointing to the node and the node's sequence to\n reflect this. Invalidates all handles to the node (including the one\n passed). Returns a new, valid handle to the node in its new forward\n orientation. Note that it is possible for the node's ID to change.\n Does not update any stored paths. May change the ordering of the underlying\n graph.\n\nC++: bdsg::ODGI::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (bdsg::ODGI::*)(const struct handlegraph::handle_t &, unsigned long)) &bdsg::ODGI::divide_handle, "Specialization of divide_handle for a single division point\n\nC++: bdsg::ODGI::divide_handle(const struct handlegraph::handle_t &, unsigned long) --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offset"));
		cl.def("destroy_path", (void (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &)) &bdsg::ODGI::destroy_path, "Destroy the given path. Invalidates handles to the path and its node steps.\n\nC++: bdsg::ODGI::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("create_path_handle", [](bdsg::ODGI &o, const class std::__cxx11::basic_string<char> & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (bdsg::ODGI::*)(const std::string &, bool)) &bdsg::ODGI::create_path_handle, "Create a path with the given name. The caller must ensure that no path\n with the given name exists already, or the behavior is undefined.\n Returns a handle to the created empty path. Handles to other paths must\n remain valid.\n\nC++: bdsg::ODGI::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::prepend_step, "Append a visit to a node to the given path. Returns a handle to the new\n final step on the path which is appended. Handles to prior\n steps on the path, and to other paths, must remain valid.\n\nC++: bdsg::ODGI::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("append_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::append_step, "Append a visit to a node to the given path. Returns a handle to the new\n final step on the path which is appended. Handles to prior\n steps on the path, and to other paths, must remain valid.\n\nC++: bdsg::ODGI::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("insert_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::insert_step, "Insert a visit to a node to the given path between the given steps.\n Returns a handle to the new step on the path which is appended.\n\n Handles to prior steps on the path, and to other paths, must remain valid.\n\nC++: bdsg::ODGI::insert_step(const struct handlegraph::step_handle_t &, const struct handlegraph::step_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("before"), pybind11::arg("after"), pybind11::arg("to_insert"));
		cl.def("set_step", (struct handlegraph::step_handle_t (bdsg::ODGI::*)(const struct handlegraph::step_handle_t &, const struct handlegraph::handle_t &)) &bdsg::ODGI::set_step, "Set the step to the given handle, possibly re-linking and cleaning up if needed\n\nC++: bdsg::ODGI::set_step(const struct handlegraph::step_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"), pybind11::arg("handle"));
		cl.def("display", (void (bdsg::ODGI::*)() const) &bdsg::ODGI::display, "A helper function to visualize the state of the graph\n\nC++: bdsg::ODGI::display() const --> void");
		cl.def("to_gfa", (void (bdsg::ODGI::*)(std::ostream &) const) &bdsg::ODGI::to_gfa, "Convert to GFA (for debugging)\n\nC++: bdsg::ODGI::to_gfa(std::ostream &) const --> void", pybind11::arg("out"));
		cl.def("serialize", (unsigned long (bdsg::ODGI::*)(std::ostream &)) &bdsg::ODGI::serialize, "Serialize\n\nC++: bdsg::ODGI::serialize(std::ostream &) --> unsigned long", pybind11::arg("out"));
		cl.def("load", (void (bdsg::ODGI::*)(class std::basic_istream<char> &)) &bdsg::ODGI::load, "Load\n\nC++: bdsg::ODGI::load(class std::basic_istream<char> &) --> void", pybind11::arg("in"));
		cl.def("assign", (class bdsg::ODGI & (bdsg::ODGI::*)(const class bdsg::ODGI &)) &bdsg::ODGI::operator=, "C++: bdsg::ODGI::operator=(const class bdsg::ODGI &) --> class bdsg::ODGI &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
