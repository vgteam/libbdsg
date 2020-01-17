#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_handle_graph.hpp>
#include <handlegraph/mutable_path_mutable_handle_graph.hpp>
#include <handlegraph/path_handle_graph.hpp>
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
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

// handlegraph::PathHandleGraph file:handlegraph/path_handle_graph.hpp line:20
struct PyCallBack_handlegraph_PathHandleGraph : public handlegraph::PathHandleGraph {
	using handlegraph::PathHandleGraph::PathHandleGraph;

	unsigned long get_path_count() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_path_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_path_count\"");
	}
	bool has_path(const class std::__cxx11::basic_string<char> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "has_path");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::has_path\"");
	}
	struct handlegraph::path_handle_t get_path_handle(const class std::__cxx11::basic_string<char> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_path_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_path_handle\"");
	}
	class std::__cxx11::basic_string<char> get_path_name(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_path_name");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::__cxx11::basic_string<char>>::value) {
				static pybind11::detail::overload_caster_t<class std::__cxx11::basic_string<char>> caster;
				return pybind11::detail::cast_ref<class std::__cxx11::basic_string<char>>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::__cxx11::basic_string<char>>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_path_name\"");
	}
	bool get_is_circular(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_is_circular");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_is_circular\"");
	}
	unsigned long get_step_count(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_step_count");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned long>::value) {
				static pybind11::detail::overload_caster_t<unsigned long> caster;
				return pybind11::detail::cast_ref<unsigned long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<unsigned long>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_step_count\"");
	}
	struct handlegraph::handle_t get_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_handle_of_step\"");
	}
	struct handlegraph::path_handle_t get_path_handle_of_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_path_handle_of_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::path_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::path_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::path_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::path_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_path_handle_of_step\"");
	}
	struct handlegraph::step_handle_t path_begin(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "path_begin");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::path_begin\"");
	}
	struct handlegraph::step_handle_t path_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "path_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::path_end\"");
	}
	struct handlegraph::step_handle_t path_back(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "path_back");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::path_back\"");
	}
	struct handlegraph::step_handle_t path_front_end(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "path_front_end");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::path_front_end\"");
	}
	bool has_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "has_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::has_next_step\"");
	}
	bool has_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "has_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::has_previous_step\"");
	}
	struct handlegraph::step_handle_t get_next_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_next_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_next_step\"");
	}
	struct handlegraph::step_handle_t get_previous_step(const struct handlegraph::step_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_previous_step");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::step_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::step_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::step_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::step_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"PathHandleGraph::get_previous_step\"");
	}
	bool for_each_path_handle_impl(const class std::function<bool (const struct handlegraph::path_handle_t &)> & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "for_each_path_handle_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "for_each_step_on_handle_impl");
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
	bool is_empty(const struct handlegraph::path_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "is_empty");
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
	bool has_node(long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "has_node");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_handle");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_is_reverse");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "flip");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_length");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_sequence");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_node_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "min_node_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "max_node_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_degree");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "has_edge");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_base");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "get_subsequence");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "follow_edges_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::PathHandleGraph *>(this), "for_each_handle_impl");
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

void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::PathHandleGraph file:handlegraph/path_handle_graph.hpp line:20
		pybind11::class_<handlegraph::PathHandleGraph, std::shared_ptr<handlegraph::PathHandleGraph>, PyCallBack_handlegraph_PathHandleGraph, handlegraph::HandleGraph> cl(M("handlegraph"), "PathHandleGraph", "");
		cl.def(pybind11::init<PyCallBack_handlegraph_PathHandleGraph const &>());
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_PathHandleGraph(); } ) );
		cl.def("get_path_count", (unsigned long (handlegraph::PathHandleGraph::*)() const) &handlegraph::PathHandleGraph::get_path_count, "C++: handlegraph::PathHandleGraph::get_path_count() const --> unsigned long");
		cl.def("has_path", (bool (handlegraph::PathHandleGraph::*)(const std::string &) const) &handlegraph::PathHandleGraph::has_path, "C++: handlegraph::PathHandleGraph::has_path(const std::string &) const --> bool", pybind11::arg("path_name"));
		cl.def("get_path_handle", (struct handlegraph::path_handle_t (handlegraph::PathHandleGraph::*)(const std::string &) const) &handlegraph::PathHandleGraph::get_path_handle, "C++: handlegraph::PathHandleGraph::get_path_handle(const std::string &) const --> struct handlegraph::path_handle_t", pybind11::arg("path_name"));
		cl.def("get_path_name", (std::string (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_path_name, "C++: handlegraph::PathHandleGraph::get_path_name(const struct handlegraph::path_handle_t &) const --> std::string", pybind11::arg("path_handle"));
		cl.def("get_is_circular", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_is_circular, "C++: handlegraph::PathHandleGraph::get_is_circular(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("get_step_count", (unsigned long (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::get_step_count, "C++: handlegraph::PathHandleGraph::get_step_count(const struct handlegraph::path_handle_t &) const --> unsigned long", pybind11::arg("path_handle"));
		cl.def("get_handle_of_step", (struct handlegraph::handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_handle_of_step, "C++: handlegraph::PathHandleGraph::get_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::handle_t", pybind11::arg("step_handle"));
		cl.def("get_path_handle_of_step", (struct handlegraph::path_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_path_handle_of_step, "C++: handlegraph::PathHandleGraph::get_path_handle_of_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::path_handle_t", pybind11::arg("step_handle"));
		cl.def("path_begin", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_begin, "C++: handlegraph::PathHandleGraph::path_begin(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_end", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_end, "C++: handlegraph::PathHandleGraph::path_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_back", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_back, "C++: handlegraph::PathHandleGraph::path_back(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("path_front_end", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::path_front_end, "C++: handlegraph::PathHandleGraph::path_front_end(const struct handlegraph::path_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("path_handle"));
		cl.def("has_next_step", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::has_next_step, "C++: handlegraph::PathHandleGraph::has_next_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("has_previous_step", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::has_previous_step, "C++: handlegraph::PathHandleGraph::has_previous_step(const struct handlegraph::step_handle_t &) const --> bool", pybind11::arg("step_handle"));
		cl.def("get_next_step", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_next_step, "C++: handlegraph::PathHandleGraph::get_next_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("get_previous_step", (struct handlegraph::step_handle_t (handlegraph::PathHandleGraph::*)(const struct handlegraph::step_handle_t &) const) &handlegraph::PathHandleGraph::get_previous_step, "C++: handlegraph::PathHandleGraph::get_previous_step(const struct handlegraph::step_handle_t &) const --> struct handlegraph::step_handle_t", pybind11::arg("step_handle"));
		cl.def("is_empty", (bool (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::is_empty, "C++: handlegraph::PathHandleGraph::is_empty(const struct handlegraph::path_handle_t &) const --> bool", pybind11::arg("path_handle"));
		cl.def("scan_path", (class handlegraph::PathForEachSocket (handlegraph::PathHandleGraph::*)(const struct handlegraph::path_handle_t &) const) &handlegraph::PathHandleGraph::scan_path, "C++: handlegraph::PathHandleGraph::scan_path(const struct handlegraph::path_handle_t &) const --> class handlegraph::PathForEachSocket", pybind11::arg("path"));
		cl.def("assign", (class handlegraph::PathHandleGraph & (handlegraph::PathHandleGraph::*)(const class handlegraph::PathHandleGraph &)) &handlegraph::PathHandleGraph::operator=, "C++: handlegraph::PathHandleGraph::operator=(const class handlegraph::PathHandleGraph &) --> class handlegraph::PathHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::PathForEachSocket file:handlegraph/path_handle_graph.hpp line:197
		pybind11::class_<handlegraph::PathForEachSocket, std::shared_ptr<handlegraph::PathForEachSocket>> cl(M("handlegraph"), "PathForEachSocket", "");
		{ // handlegraph::PathForEachSocket::iterator file:handlegraph/path_handle_graph.hpp line:211
			auto & enclosing_class = cl;
			pybind11::class_<handlegraph::PathForEachSocket::iterator, std::shared_ptr<handlegraph::PathForEachSocket::iterator>> cl(enclosing_class, "iterator", "");
			cl.def( pybind11::init( [](handlegraph::PathForEachSocket::iterator const &o){ return new handlegraph::PathForEachSocket::iterator(o); } ) );
			cl.def("assign", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &)) &handlegraph::PathForEachSocket::iterator::operator=, "C++: handlegraph::PathForEachSocket::iterator::operator=(const class handlegraph::PathForEachSocket::iterator &) --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
			cl.def("plus_plus", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)()) &handlegraph::PathForEachSocket::iterator::operator++, "C++: handlegraph::PathForEachSocket::iterator::operator++() --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic);
			cl.def("__mul__", (struct handlegraph::handle_t (handlegraph::PathForEachSocket::iterator::*)() const) &handlegraph::PathForEachSocket::iterator::operator*, "C++: handlegraph::PathForEachSocket::iterator::operator*() const --> struct handlegraph::handle_t");
			cl.def("__eq__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator==, "C++: handlegraph::PathForEachSocket::iterator::operator==(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
			cl.def("__ne__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator!=, "C++: handlegraph::PathForEachSocket::iterator::operator!=(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
		}

		cl.def( pybind11::init( [](handlegraph::PathForEachSocket const &o){ return new handlegraph::PathForEachSocket(o); } ) );
		cl.def("begin", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::begin, "C++: handlegraph::PathForEachSocket::begin() const --> class handlegraph::PathForEachSocket::iterator");
		cl.def("end", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::end, "C++: handlegraph::PathForEachSocket::end() const --> class handlegraph::PathForEachSocket::iterator");
	}
	{ // handlegraph::MutablePathHandleGraph file:handlegraph/mutable_path_handle_graph.hpp line:19
		pybind11::class_<handlegraph::MutablePathHandleGraph, std::shared_ptr<handlegraph::MutablePathHandleGraph>, handlegraph::PathHandleGraph> cl(M("handlegraph"), "MutablePathHandleGraph", "");
		cl.def("destroy_path", (void (handlegraph::MutablePathHandleGraph::*)(const struct handlegraph::path_handle_t &)) &handlegraph::MutablePathHandleGraph::destroy_path, "C++: handlegraph::MutablePathHandleGraph::destroy_path(const struct handlegraph::path_handle_t &) --> void", pybind11::arg("path"));
		cl.def("create_path_handle", [](handlegraph::MutablePathHandleGraph &o, const class std::__cxx11::basic_string<char> & a0) -> handlegraph::path_handle_t { return o.create_path_handle(a0); }, "", pybind11::arg("name"));
		cl.def("create_path_handle", (struct handlegraph::path_handle_t (handlegraph::MutablePathHandleGraph::*)(const std::string &, bool)) &handlegraph::MutablePathHandleGraph::create_path_handle, "C++: handlegraph::MutablePathHandleGraph::create_path_handle(const std::string &, bool) --> struct handlegraph::path_handle_t", pybind11::arg("name"), pybind11::arg("is_circular"));
		cl.def("append_step", (struct handlegraph::step_handle_t (handlegraph::MutablePathHandleGraph::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &handlegraph::MutablePathHandleGraph::append_step, "C++: handlegraph::MutablePathHandleGraph::append_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_append"));
		cl.def("prepend_step", (struct handlegraph::step_handle_t (handlegraph::MutablePathHandleGraph::*)(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &)) &handlegraph::MutablePathHandleGraph::prepend_step, "C++: handlegraph::MutablePathHandleGraph::prepend_step(const struct handlegraph::path_handle_t &, const struct handlegraph::handle_t &) --> struct handlegraph::step_handle_t", pybind11::arg("path"), pybind11::arg("to_prepend"));
		cl.def("set_circularity", (void (handlegraph::MutablePathHandleGraph::*)(const struct handlegraph::path_handle_t &, bool)) &handlegraph::MutablePathHandleGraph::set_circularity, "C++: handlegraph::MutablePathHandleGraph::set_circularity(const struct handlegraph::path_handle_t &, bool) --> void", pybind11::arg("path"), pybind11::arg("circular"));
		cl.def("assign", (class handlegraph::MutablePathHandleGraph & (handlegraph::MutablePathHandleGraph::*)(const class handlegraph::MutablePathHandleGraph &)) &handlegraph::MutablePathHandleGraph::operator=, "C++: handlegraph::MutablePathHandleGraph::operator=(const class handlegraph::MutablePathHandleGraph &) --> class handlegraph::MutablePathHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::MutableHandleGraph file:handlegraph/mutable_handle_graph.hpp line:19
		pybind11::class_<handlegraph::MutableHandleGraph, std::shared_ptr<handlegraph::MutableHandleGraph>, handlegraph::HandleGraph> cl(M("handlegraph"), "MutableHandleGraph", "");
		cl.def("create_handle", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const std::string &)) &handlegraph::MutableHandleGraph::create_handle, "C++: handlegraph::MutableHandleGraph::create_handle(const std::string &) --> struct handlegraph::handle_t", pybind11::arg("sequence"));
		cl.def("create_handle", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const std::string &, const long &)) &handlegraph::MutableHandleGraph::create_handle, "C++: handlegraph::MutableHandleGraph::create_handle(const std::string &, const long &) --> struct handlegraph::handle_t", pybind11::arg("sequence"), pybind11::arg("id"));
		cl.def("create_edge", (void (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &)) &handlegraph::MutableHandleGraph::create_edge, "C++: handlegraph::MutableHandleGraph::create_edge(const struct handlegraph::handle_t &, const struct handlegraph::handle_t &) --> void", pybind11::arg("left"), pybind11::arg("right"));
		cl.def("create_edge", (void (handlegraph::MutableHandleGraph::*)(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &)) &handlegraph::MutableHandleGraph::create_edge, "C++: handlegraph::MutableHandleGraph::create_edge(const struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> &) --> void", pybind11::arg("edge"));
		cl.def("apply_orientation", (struct handlegraph::handle_t (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &)) &handlegraph::MutableHandleGraph::apply_orientation, "C++: handlegraph::MutableHandleGraph::apply_orientation(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t", pybind11::arg("handle"));
		cl.def("divide_handle", (struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t> (handlegraph::MutableHandleGraph::*)(const struct handlegraph::handle_t &, unsigned long)) &handlegraph::MutableHandleGraph::divide_handle, "C++: handlegraph::MutableHandleGraph::divide_handle(const struct handlegraph::handle_t &, unsigned long) --> struct std::pair<struct handlegraph::handle_t, struct handlegraph::handle_t>", pybind11::arg("handle"), pybind11::arg("offset"));
		cl.def("optimize", [](handlegraph::MutableHandleGraph &o) -> void { return o.optimize(); }, "");
		cl.def("optimize", (void (handlegraph::MutableHandleGraph::*)(bool)) &handlegraph::MutableHandleGraph::optimize, "C++: handlegraph::MutableHandleGraph::optimize(bool) --> void", pybind11::arg("allow_id_reassignment"));
		cl.def("set_id_increment", (void (handlegraph::MutableHandleGraph::*)(const long &)) &handlegraph::MutableHandleGraph::set_id_increment, "C++: handlegraph::MutableHandleGraph::set_id_increment(const long &) --> void", pybind11::arg("min_id"));
		cl.def("assign", (class handlegraph::MutableHandleGraph & (handlegraph::MutableHandleGraph::*)(const class handlegraph::MutableHandleGraph &)) &handlegraph::MutableHandleGraph::operator=, "C++: handlegraph::MutableHandleGraph::operator=(const class handlegraph::MutableHandleGraph &) --> class handlegraph::MutableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::MutablePathMutableHandleGraph file:handlegraph/mutable_path_mutable_handle_graph.hpp line:16
		pybind11::class_<handlegraph::MutablePathMutableHandleGraph, std::shared_ptr<handlegraph::MutablePathMutableHandleGraph>, handlegraph::MutablePathHandleGraph, handlegraph::MutableHandleGraph> cl(M("handlegraph"), "MutablePathMutableHandleGraph", "");
		cl.def("assign", (class handlegraph::MutablePathMutableHandleGraph & (handlegraph::MutablePathMutableHandleGraph::*)(const class handlegraph::MutablePathMutableHandleGraph &)) &handlegraph::MutablePathMutableHandleGraph::operator=, "C++: handlegraph::MutablePathMutableHandleGraph::operator=(const class handlegraph::MutablePathMutableHandleGraph &) --> class handlegraph::MutablePathMutableHandleGraph &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
