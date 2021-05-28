#include <bdsg/internal/mapped_structs.hpp>
#include <sstream> // __str__

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

void bind_bdsg_internal_mapped_structs(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::STLBackend file:bdsg/internal/mapped_structs.hpp line:75
		pybind11::class_<bdsg::STLBackend, std::shared_ptr<bdsg::STLBackend>> cl(M("bdsg"), "STLBackend", "Collection of templates for data structures that use the STL and SDSL.");
		cl.def( pybind11::init( [](){ return new bdsg::STLBackend(); } ) );
	}
}
