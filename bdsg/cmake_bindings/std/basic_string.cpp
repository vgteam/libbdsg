#include <functional>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>

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

void bind_std_basic_string(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::hash file:bits/basic_string.h line:4414
		pybind11::class_<std::hash<std::string>, std::shared_ptr<std::hash<std::string>>> cl(M("std"), "hash_std_string_t", "");
		cl.def( pybind11::init( [](){ return new std::hash<std::string>(); } ) );
		cl.def( pybind11::init( [](std::hash<std::string> const &o){ return new std::hash<std::string>(o); } ) );
	}
	{ // std::hash file:bits/functional_hash.h line:102
		pybind11::class_<std::hash<handlegraph::PathSense>, std::shared_ptr<std::hash<handlegraph::PathSense>>> cl(M("std"), "hash_handlegraph_PathSense_t", "");
		cl.def( pybind11::init( [](){ return new std::hash<handlegraph::PathSense>(); } ) );
		cl.def( pybind11::init( [](std::hash<handlegraph::PathSense> const &o){ return new std::hash<handlegraph::PathSense>(o); } ) );
	}
}
