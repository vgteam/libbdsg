#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/path_handle_graph.hpp>
#include <handlegraph/snarl_decomposition.hpp>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <utility>

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

// handlegraph::SnarlDecomposition file:handlegraph/snarl_decomposition.hpp line:37
struct PyCallBack_handlegraph_SnarlDecomposition : public handlegraph::SnarlDecomposition {
	using handlegraph::SnarlDecomposition::SnarlDecomposition;

	struct handlegraph::net_handle_t get_root() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_root\"");
	}
	bool is_root(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "is_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::is_root\"");
	}
	bool is_snarl(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "is_snarl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::is_snarl\"");
	}
	bool is_chain(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "is_chain");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::is_chain\"");
	}
	bool is_node(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "is_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::is_node\"");
	}
	bool is_sentinel(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "is_sentinel");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::is_sentinel\"");
	}
	struct handlegraph::net_handle_t get_net(const struct handlegraph::handle_t & a0, const class handlegraph::HandleGraph * a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_net");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_net\"");
	}
	struct handlegraph::handle_t get_handle(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_handle\"");
	}
	struct handlegraph::net_handle_t get_parent(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_parent");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_parent\"");
	}
	struct handlegraph::net_handle_t get_bound(const struct handlegraph::net_handle_t & a0, bool a1, bool a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_bound");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_bound\"");
	}
	struct handlegraph::net_handle_t flip(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::flip\"");
	}
	struct handlegraph::net_handle_t canonical(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "canonical");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::canonical\"");
	}
	enum handlegraph::SnarlDecomposition::endpoint_t starts_at(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "starts_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::override_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::starts_at\"");
	}
	enum handlegraph::SnarlDecomposition::endpoint_t ends_at(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "ends_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::override_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::ends_at\"");
	}
	bool for_each_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "for_each_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::for_each_child_impl\"");
	}
	bool for_each_traversal_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "for_each_traversal_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::for_each_traversal_impl\"");
	}
	bool follow_net_edges_impl(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1, bool a2, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a3) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "follow_net_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2, a3);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::follow_net_edges_impl\"");
	}
	bool for_each_tippy_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "for_each_tippy_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_tippy_child_impl(a0, a1);
	}
	struct handlegraph::net_handle_t get_parent_traversal(const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "get_parent_traversal");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"SnarlDecomposition::get_parent_traversal\"");
	}
	bool for_each_traversal_start_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "for_each_traversal_start_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_start_impl(a0, a1);
	}
	bool for_each_traversal_end_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const handlegraph::SnarlDecomposition *>(this), "for_each_traversal_end_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_end_impl(a0, a1);
	}
};

void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::PathForEachSocket file:handlegraph/path_handle_graph.hpp line:220
		pybind11::class_<handlegraph::PathForEachSocket, std::shared_ptr<handlegraph::PathForEachSocket>> cl(M("handlegraph"), "PathForEachSocket", "An auxilliary class that enables for each loops over paths. Not intended to\n constructed directly. Instead, use the PathHandleGraph's scan_path method.");
		cl.def( pybind11::init( [](handlegraph::PathForEachSocket const &o){ return new handlegraph::PathForEachSocket(o); } ) );
		cl.def("begin", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::begin, "C++: handlegraph::PathForEachSocket::begin() const --> class handlegraph::PathForEachSocket::iterator");
		cl.def("end", (class handlegraph::PathForEachSocket::iterator (handlegraph::PathForEachSocket::*)() const) &handlegraph::PathForEachSocket::end, "C++: handlegraph::PathForEachSocket::end() const --> class handlegraph::PathForEachSocket::iterator");

		{ // handlegraph::PathForEachSocket::iterator file:handlegraph/path_handle_graph.hpp line:234
			auto & enclosing_class = cl;
			pybind11::class_<handlegraph::PathForEachSocket::iterator, std::shared_ptr<handlegraph::PathForEachSocket::iterator>> cl(enclosing_class, "iterator", "Iterator object over path");
			cl.def( pybind11::init( [](handlegraph::PathForEachSocket::iterator const &o){ return new handlegraph::PathForEachSocket::iterator(o); } ) );
			cl.def("assign", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &)) &handlegraph::PathForEachSocket::iterator::operator=, "C++: handlegraph::PathForEachSocket::iterator::operator=(const class handlegraph::PathForEachSocket::iterator &) --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
			cl.def("pre_increment", (class handlegraph::PathForEachSocket::iterator & (handlegraph::PathForEachSocket::iterator::*)()) &handlegraph::PathForEachSocket::iterator::operator++, "C++: handlegraph::PathForEachSocket::iterator::operator++() --> class handlegraph::PathForEachSocket::iterator &", pybind11::return_value_policy::automatic);
			cl.def("dereference", (struct handlegraph::handle_t (handlegraph::PathForEachSocket::iterator::*)() const) &handlegraph::PathForEachSocket::iterator::operator*, "C++: handlegraph::PathForEachSocket::iterator::operator*() const --> struct handlegraph::handle_t");
			cl.def("__eq__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator==, "C++: handlegraph::PathForEachSocket::iterator::operator==(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
			cl.def("__ne__", (bool (handlegraph::PathForEachSocket::iterator::*)(const class handlegraph::PathForEachSocket::iterator &) const) &handlegraph::PathForEachSocket::iterator::operator!=, "C++: handlegraph::PathForEachSocket::iterator::operator!=(const class handlegraph::PathForEachSocket::iterator &) const --> bool", pybind11::arg("other"));
		}

	}
	{ // handlegraph::SnarlDecomposition file:handlegraph/snarl_decomposition.hpp line:37
		pybind11::class_<handlegraph::SnarlDecomposition, std::shared_ptr<handlegraph::SnarlDecomposition>, PyCallBack_handlegraph_SnarlDecomposition> cl(M("handlegraph"), "SnarlDecomposition", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_handlegraph_SnarlDecomposition(); } ) );
		cl.def(pybind11::init<PyCallBack_handlegraph_SnarlDecomposition const &>());

		pybind11::enum_<handlegraph::SnarlDecomposition::endpoint_t>(cl, "endpoint_t", pybind11::arithmetic(), "Represents a place that a traversal can start or end. Traversals can start\n or end at the start, end, or an internal tip of the thing they traverse.")
			.value("START", handlegraph::SnarlDecomposition::START)
			.value("END", handlegraph::SnarlDecomposition::END)
			.value("TIP", handlegraph::SnarlDecomposition::TIP)
			.export_values();

		cl.def("for_each_child", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::for_each_child<std::function<bool (const handlegraph::net_handle_t &)>>, "/////////////////////////////////////////////////////////////////////////\n\nC++: handlegraph::SnarlDecomposition::for_each_child(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("parent"), pybind11::arg("iteratee"));
		cl.def("for_each_traversal", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::for_each_traversal<std::function<bool (const handlegraph::net_handle_t &)>>, "C++: handlegraph::SnarlDecomposition::for_each_traversal(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("item"), pybind11::arg("iteratee"));
		cl.def("follow_net_edges", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, bool, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::follow_net_edges<std::function<bool (const handlegraph::net_handle_t &)>>, "C++: handlegraph::SnarlDecomposition::follow_net_edges(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, bool, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("here"), pybind11::arg("graph"), pybind11::arg("go_left"), pybind11::arg("iteratee"));
		cl.def("for_each_tippy_child", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::for_each_tippy_child<std::function<bool (const handlegraph::net_handle_t &)>>, "C++: handlegraph::SnarlDecomposition::for_each_tippy_child(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("parent"), pybind11::arg("iteratee"));
		cl.def("for_each_traversal_start", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::for_each_traversal_start<std::function<bool (const handlegraph::net_handle_t &)>>, "C++: handlegraph::SnarlDecomposition::for_each_traversal_start(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("traversal"), pybind11::arg("iteratee"));
		cl.def("for_each_traversal_end", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &handlegraph::SnarlDecomposition::for_each_traversal_end<std::function<bool (const handlegraph::net_handle_t &)>>, "C++: handlegraph::SnarlDecomposition::for_each_traversal_end(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("traversal"), pybind11::arg("iteratee"));
		cl.def("get_root", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)() const) &handlegraph::SnarlDecomposition::get_root, "Get a net handle referring to a tip-to-tip traversal of the contents of the root snarl.\n TODO: Special handling for circular things in the root snarl? Circular traversal type?\n\nC++: handlegraph::SnarlDecomposition::get_root() const --> struct handlegraph::net_handle_t");
		cl.def("is_root", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::is_root, "Return true if the given handle refers to (a traversal of) the root\n snarl, and false otherwise.\n\nC++: handlegraph::SnarlDecomposition::is_root(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_snarl", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::is_snarl, "Returns true if the given net handle refers to (a traversal of) a snarl.\n\nC++: handlegraph::SnarlDecomposition::is_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_chain", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::is_chain, "Returns true if the given net handle refers to (a traversal of) a chain.\n\nC++: handlegraph::SnarlDecomposition::is_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_node", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::is_node, "Returns true if the given net handle refers to (a traversal of) a single node, and thus has a corresponding handle_t.\n\nC++: handlegraph::SnarlDecomposition::is_node(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_sentinel", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::is_sentinel, "Return true if the given net handle is a snarl bound sentinel (in either\n inward or outward orientation), and false otherwise.\n\nC++: handlegraph::SnarlDecomposition::is_sentinel(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("get_net", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const) &handlegraph::SnarlDecomposition::get_net, "Turn a handle to an oriented node into a net handle for a start-to-end or end-to-start traversal of the node, as appropriate.\n\nC++: handlegraph::SnarlDecomposition::get_net(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::net_handle_t", pybind11::arg("handle"), pybind11::arg("graph"));
		cl.def("get_handle", (struct handlegraph::handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const) &handlegraph::SnarlDecomposition::get_handle, "For a net handle to a traversal of a single node, get the handle for that node in the orientation it is traversed.\n May not be called for other net handles.\n\nC++: handlegraph::SnarlDecomposition::get_handle(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::handle_t", pybind11::arg("net"), pybind11::arg("graph"));
		cl.def("get_parent", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::get_parent, "Get the parent snarl of a chain, or the parent chain of a snarl or node.\n If the child is start-to-end or end-to-start, and the parent is a chain,\n the chain comes out facing the same way, accounting for the relative\n orientation of the child snarl or node in the chain. Otherwise,\n everything is produced as start-to-end, even if that is not actually a\n realizable traversal of a snarl or chain. May not be called on the root\n snarl.\n\n Also works on snarl boundary sentinels.\n\nC++: handlegraph::SnarlDecomposition::get_parent(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("child"));
		cl.def("get_bound", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, bool, bool) const) &handlegraph::SnarlDecomposition::get_bound, "Get the bounding handle for the snarl or chain referenced by the given\n net handle, getting the start or end facing in or out as appropriate.\n\n For snarls, returns the bounding sentinel net handles. For chains,\n returns net handles for traversals of the bounding nodes of the chain.\n\n Ignores traversal type.\n\n May not be called on traversals of individual nodes.\n\nC++: handlegraph::SnarlDecomposition::get_bound(const struct handlegraph::net_handle_t &, bool, bool) const --> struct handlegraph::net_handle_t", pybind11::arg("snarl"), pybind11::arg("get_end"), pybind11::arg("face_in"));
		cl.def("flip", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::flip, "Return a net handle to the same snarl/chain/node in the opposite orientation.\n No effect on tip-to-tip, start-to-start, or end-to-end net handles. Flips all the others.\n\nC++: handlegraph::SnarlDecomposition::flip(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("canonical", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::canonical, "Get a canonical traversal handle from any net handle. All handles to the\n same net graph element have the same canonical traversal. That canonical\n traversal must be realizable, and might not always be start-to-end or\n even consistently be the same kind of traversal for different snarls,\n chains, or nodes. Mostly useful to normalize for equality comparisons.\n\nC++: handlegraph::SnarlDecomposition::canonical(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("starts_at", (enum handlegraph::SnarlDecomposition::endpoint_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::starts_at, "Return the kind of location at which the given traversal starts.\n\nC++: handlegraph::SnarlDecomposition::starts_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("ends_at", (enum handlegraph::SnarlDecomposition::endpoint_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::ends_at, "Return the kind of location at which the given traversal ends.\n\nC++: handlegraph::SnarlDecomposition::ends_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("get_parent_traversal", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::get_parent_traversal, "Get a net handle for traversals of a the snarl or chain that contains\n the given oriented bounding node traversals or sentinels. Given two\n sentinels for a snarl, produces a net handle to a start-to-end,\n end-to-end, end-to-start, or start-to-start traversal of that snarl.\n Given handles to traversals of the bounding nodes of a chain, similarly\n produces a net handle to a traversal of the chain.\n\n For a chain, either or both handles can also be a snarl containing tips,\n for a tip-to-start, tip-to-end, start-to-tip, end-to-tip, or tip-to-tip\n traversal. Similarly, for a snarl, either or both handles can be a chain\n in the snarl that contains internal tips, or that has no edges on the\n appropriate end.\n\n May only be called if a path actually exists between the given start\n and end.\n\nC++: handlegraph::SnarlDecomposition::get_parent_traversal(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("traversal_start"), pybind11::arg("traversal_end"));
		cl.def("get_start_bound", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::get_start_bound, "Get a handle to the inward-facing traversal of the first node in a chain\n or the start boundary in a snarl.\n\n This isn't necessarily where the traversal specified by the given handle\n actually starts (it may be end to end or tip to tip, for example).\n\nC++: handlegraph::SnarlDecomposition::get_start_bound(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("parent"));
		cl.def("get_end_bound", (struct handlegraph::net_handle_t (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::get_end_bound, "Get a handle to the outward-facing traversal of the last node in a chain\n or the end boundary in a snarl.\n\n This isn't necessarily where the traversal specified by the given handle\n actually ends (it may be start to start or tip to tip, for example).\n\nC++: handlegraph::SnarlDecomposition::get_end_bound(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("parent"));
		cl.def("starts_at_start", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::starts_at_start, "Return true if the given net handle describes a category of traversal that starts at the local start of the snarl/chain/node.\n\nC++: handlegraph::SnarlDecomposition::starts_at_start(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("starts_at_end", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::starts_at_end, "Return true if the given net handle describes a category of traversal that starts at the local end of the snarl/chain/node.\n\nC++: handlegraph::SnarlDecomposition::starts_at_end(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("starts_at_tip", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::starts_at_tip, "Return true if the given net handle describes a category of traversal that starts at an internal tip in the snarl/chain. Never true for nodes.\n\nC++: handlegraph::SnarlDecomposition::starts_at_tip(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("ends_at_start", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::ends_at_start, "Return true if the given net handle describes a category of traversal that ends at the local start of the snarl/chain/node.\n\nC++: handlegraph::SnarlDecomposition::ends_at_start(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("ends_at_end", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::ends_at_end, "Return true if the given net handle describes a category of traversal that ends at the local end of the snarl/chain/node.\n\nC++: handlegraph::SnarlDecomposition::ends_at_end(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("ends_at_tip", (bool (handlegraph::SnarlDecomposition::*)(const struct handlegraph::net_handle_t &) const) &handlegraph::SnarlDecomposition::ends_at_tip, "Return true if the given net handle describes a category of traversal that ends at the an internal tip in the snarl/chain. Never true for nodes.\n\nC++: handlegraph::SnarlDecomposition::ends_at_tip(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("assign", (class handlegraph::SnarlDecomposition & (handlegraph::SnarlDecomposition::*)(const class handlegraph::SnarlDecomposition &)) &handlegraph::SnarlDecomposition::operator=, "C++: handlegraph::SnarlDecomposition::operator=(const class handlegraph::SnarlDecomposition &) --> class handlegraph::SnarlDecomposition &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
