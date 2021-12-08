#include <bdsg/graph_proxy.hpp>
#include <bdsg/internal/base_packed_graph.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <functional>
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

// bdsg::HandleGraphProxy file:bdsg/graph_proxy.hpp line:61
struct PyCallBack_bdsg_HandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t : public bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> {
	using bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::HandleGraphProxy;

	bool has_node(long long a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_node");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_handle");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_is_reverse");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "flip");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_length");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_sequence");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_node_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "min_node_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "max_node_id");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_degree");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "has_edge");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_edge_count");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_total_length");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_base");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get_subsequence");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "follow_edges_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "for_each_handle_impl");
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
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>> *>(this), "get");
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
};

void bind_bdsg_graph_proxy(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::AbstractGraphProxy file:bdsg/graph_proxy.hpp line:25
		pybind11::class_<bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, std::shared_ptr<bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>>> cl(M("bdsg"), "AbstractGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def("assign", (struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::AbstractGraphProxy file:bdsg/graph_proxy.hpp line:25
		pybind11::class_<bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>, std::shared_ptr<bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>>> cl(M("bdsg"), "AbstractGraphProxy_bdsg_BasePackedGraph_bdsg_MappedBackend_t", "");
		cl.def("assign", (struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > & (bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &)) &bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=, "C++: bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=(const struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &) --> struct bdsg::AbstractGraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::HandleGraphProxy file:bdsg/graph_proxy.hpp line:61
		pybind11::class_<bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, std::shared_ptr<bdsg::HandleGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>>, PyCallBack_bdsg_HandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t, bdsg::AbstractGraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, handlegraph::HandleGraph> cl(M("bdsg"), "HandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_bdsg_HandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t(); } ) );
		cl.def(pybind11::init<PyCallBack_bdsg_HandleGraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t const &>());
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
	}
}
