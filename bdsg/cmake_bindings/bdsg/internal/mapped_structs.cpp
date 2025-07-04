#include <bdsg/internal/mapped_structs.hpp>
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

void bind_bdsg_internal_mapped_structs(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::STLBackend file:bdsg/internal/mapped_structs.hpp line:44
		pybind11::class_<bdsg::STLBackend, std::shared_ptr<bdsg::STLBackend>> cl(M("bdsg"), "STLBackend", "Type enum value for selecting data structures that use the STL and SDSL.");
		cl.def( pybind11::init( [](){ return new bdsg::STLBackend(); } ) );
	}
	{ // bdsg::VectorFor file:bdsg/internal/mapped_structs.hpp line:66
		pybind11::class_<bdsg::VectorFor<bdsg::STLBackend>, std::shared_ptr<bdsg::VectorFor<bdsg::STLBackend>>> cl(M("bdsg"), "VectorFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::VectorFor<bdsg::STLBackend>(); } ) );
	}
	{ // bdsg::IntVectorFor file:bdsg/internal/mapped_structs.hpp line:72
		pybind11::class_<bdsg::IntVectorFor<bdsg::STLBackend>, std::shared_ptr<bdsg::IntVectorFor<bdsg::STLBackend>>> cl(M("bdsg"), "IntVectorFor_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::IntVectorFor<bdsg::STLBackend>(); } ) );
	}
}
