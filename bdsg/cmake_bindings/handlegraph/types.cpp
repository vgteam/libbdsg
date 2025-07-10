#include <handlegraph/types.hpp>
#include <sstream> // __str__

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

void bind_handlegraph_types(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::handle_t file:handlegraph/types.hpp line:39
		pybind11::class_<handlegraph::handle_t, std::shared_ptr<handlegraph::handle_t>> cl(M("handlegraph"), "handle_t", "Represents a traversal of a node in a graph in a particular direction");
		cl.def( pybind11::init( [](handlegraph::handle_t const &o){ return new handlegraph::handle_t(o); } ) );
		cl.def( pybind11::init( [](){ return new handlegraph::handle_t(); } ) );
		cl.def("assign", (struct handlegraph::handle_t & (handlegraph::handle_t::*)(const struct handlegraph::handle_t &)) &handlegraph::handle_t::operator=, "C++: handlegraph::handle_t::operator=(const struct handlegraph::handle_t &) --> struct handlegraph::handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::path_handle_t file:handlegraph/types.hpp line:45
		pybind11::class_<handlegraph::path_handle_t, std::shared_ptr<handlegraph::path_handle_t>> cl(M("handlegraph"), "path_handle_t", "Represents the internal id of a path entity");
		cl.def( pybind11::init( [](){ return new handlegraph::path_handle_t(); } ) );
		cl.def( pybind11::init( [](handlegraph::path_handle_t const &o){ return new handlegraph::path_handle_t(o); } ) );
		cl.def("assign", (struct handlegraph::path_handle_t & (handlegraph::path_handle_t::*)(const struct handlegraph::path_handle_t &)) &handlegraph::path_handle_t::operator=, "C++: handlegraph::path_handle_t::operator=(const struct handlegraph::path_handle_t &) --> struct handlegraph::path_handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// handlegraph::PathSense file:handlegraph/types.hpp line:48
	pybind11::enum_<handlegraph::PathSense>(M("handlegraph"), "PathSense", "Represents a sense that a path can have")
		.value("GENERIC", handlegraph::PathSense::GENERIC)
		.value("REFERENCE", handlegraph::PathSense::REFERENCE)
		.value("HAPLOTYPE", handlegraph::PathSense::HAPLOTYPE);

;

	{ // handlegraph::step_handle_t file:handlegraph/types.hpp line:55
		pybind11::class_<handlegraph::step_handle_t, std::shared_ptr<handlegraph::step_handle_t>> cl(M("handlegraph"), "step_handle_t", "A step handle is an opaque reference to a single step of an oriented node on a path in a graph");
		cl.def( pybind11::init( [](){ return new handlegraph::step_handle_t(); } ) );
		cl.def( pybind11::init( [](handlegraph::step_handle_t const &o){ return new handlegraph::step_handle_t(o); } ) );
		cl.def("assign", (struct handlegraph::step_handle_t & (handlegraph::step_handle_t::*)(const struct handlegraph::step_handle_t &)) &handlegraph::step_handle_t::operator=, "C++: handlegraph::step_handle_t::operator=(const struct handlegraph::step_handle_t &) --> struct handlegraph::step_handle_t &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // handlegraph::net_handle_t file:handlegraph/types.hpp line:73
		pybind11::class_<handlegraph::net_handle_t, std::shared_ptr<handlegraph::net_handle_t>> cl(M("handlegraph"), "net_handle_t", "A net handle is an opaque reference to a category of traversals of a single\n node, a chain, or the interior of a snarl, in the snarl decomposition of a\n graph.\n\n Snarls and chains are bounded by two particular points, but the traversal\n may not visit both or any of them (as is the case for traversals between\n internal tips).\n\n The handle refers to the snarl or chain itself and also a particular\n category of traversals of it. Each of the start and end of the traversal can\n be the start of the snarl/chain, the end of the snarl/chain, or some\n internal tip, for 6 distinct combinations.\n\n For single nodes, we only have forward and reverse.");
		cl.def( pybind11::init( [](){ return new handlegraph::net_handle_t(); } ) );
		cl.def( pybind11::init( [](handlegraph::net_handle_t const &o){ return new handlegraph::net_handle_t(o); } ) );
	}
}
