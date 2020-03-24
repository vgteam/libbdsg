#include <handlegraph/types.hpp>
#include <sstream> // __str__

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>
#include <bdsg/bindings.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_handlegraph_types(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::handle_t file:handlegraph/types.hpp line:34
		pybind11::class_<handlegraph::handle_t, std::shared_ptr<handlegraph::handle_t>> cl(M("handlegraph"), "handle_t", "Represents the internal id of a node traversal");
		cl.def( pybind11::init( [](handlegraph::handle_t const &o){ return new handlegraph::handle_t(o); } ) );
		cl.def( pybind11::init( [](){ return new handlegraph::handle_t(); } ) );
		cl.def("assign", (struct handlegraph::handle_t & (handlegraph::handle_t::*)(const struct handlegraph::handle_t &)) &handlegraph::handle_t::operator=, "C++: handlegraph::handle_t::operator=(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::path_handle_t file:handlegraph/types.hpp line:40
		pybind11::class_<handlegraph::path_handle_t, std::shared_ptr<handlegraph::path_handle_t>> cl(M("handlegraph"), "path_handle_t", "Represents the internal id of a path entity");
		cl.def( pybind11::init( [](){ return new handlegraph::path_handle_t(); } ) );
		cl.def( pybind11::init( [](handlegraph::path_handle_t const &o){ return new handlegraph::path_handle_t(o); } ) );
		cl.def("assign", (struct handlegraph::path_handle_t & (handlegraph::path_handle_t::*)(const struct handlegraph::path_handle_t &)) &handlegraph::path_handle_t::operator=, "C++: handlegraph::path_handle_t::operator=(const struct handlegraph::path_handle_t &) --> struct handlegraph::path_handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::step_handle_t file:handlegraph/types.hpp line:43
		pybind11::class_<handlegraph::step_handle_t, std::shared_ptr<handlegraph::step_handle_t>> cl(M("handlegraph"), "step_handle_t", "A step handle is an opaque reference to a single step of an oriented node on a path in a graph");
		cl.def( pybind11::init( [](){ return new handlegraph::step_handle_t(); } ) );
		cl.def( pybind11::init( [](handlegraph::step_handle_t const &o){ return new handlegraph::step_handle_t(o); } ) );
		cl.def("assign", (struct handlegraph::step_handle_t & (handlegraph::step_handle_t::*)(const struct handlegraph::step_handle_t &)) &handlegraph::step_handle_t::operator=, "C++: handlegraph::step_handle_t::operator=(const struct handlegraph::step_handle_t &) --> struct handlegraph::step_handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
