#include <bdsg/internal/mapped_structs.hpp>
#include <bdsg/internal/packed_structs.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sdsl/int_vector.hpp>
#include <sstream> // __str__
#include <streambuf>
#include <string>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <bdsg/internal/binder_hook_compile.hpp>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_internal_mapped_structs_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::CompatBackend file:bdsg/internal/mapped_structs.hpp line:1144
		pybind11::class_<bdsg::CompatBackend, std::shared_ptr<bdsg::CompatBackend>> cl(M("bdsg"), "CompatBackend", "Type enum value for selecting data structures that use memory-mappable data\n structures but with the standard allocator.");
		cl.def( pybind11::init( [](){ return new bdsg::CompatBackend(); } ) );
	}
	{ // bdsg::MappedBackend file:bdsg/internal/mapped_structs.hpp line:1155
		pybind11::class_<bdsg::MappedBackend, std::shared_ptr<bdsg::MappedBackend>> cl(M("bdsg"), "MappedBackend", "Type enum value for selecting data structures that use YOMO memory mapping\n and the YOMO allocator.\n\n They can safely exist outside of mapped memory, but are probably slower\n there since YOMO's internal tables still need to be consulted when following\n pointers.");
		cl.def( pybind11::init( [](){ return new bdsg::MappedBackend(); } ) );
	}
}
