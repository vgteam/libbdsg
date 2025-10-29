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

void bind_std_stl_function(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::binary_function file:bits/stl_function.h line:131
		pybind11::class_<std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool>, std::shared_ptr<std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool>>> cl(M("std"), "binary_function_handlegraph_PathSense_handlegraph_PathSense_bool_t", "");
		cl.def( pybind11::init( [](){ return new std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool>(); } ) );
		cl.def( pybind11::init( [](std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool> const &o){ return new std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool>(o); } ) );
	}
	{ // std::binary_function file:bits/stl_function.h line:131
		pybind11::class_<std::binary_function<std::string,std::string,bool>, std::shared_ptr<std::binary_function<std::string,std::string,bool>>> cl(M("std"), "binary_function_std_string_std_string_bool_t", "");
		cl.def( pybind11::init( [](){ return new std::binary_function<std::string,std::string,bool>(); } ) );
		cl.def( pybind11::init( [](std::binary_function<std::string,std::string,bool> const &o){ return new std::binary_function<std::string,std::string,bool>(o); } ) );
		cl.def("assign", (struct std::binary_function<std::string, std::string, bool> & (std::binary_function<std::string,std::string,bool>::*)(const struct std::binary_function<std::string, std::string, bool> &)) &std::binary_function<std::string, std::string, bool>::operator=, "C++: std::binary_function<std::string, std::string, bool>::operator=(const struct std::binary_function<std::string, std::string, bool> &) --> struct std::binary_function<std::string, std::string, bool> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // std::equal_to file:bits/stl_function.h line:373
		pybind11::class_<std::equal_to<handlegraph::PathSense>, std::shared_ptr<std::equal_to<handlegraph::PathSense>>, std::binary_function<handlegraph::PathSense,handlegraph::PathSense,bool>> cl(M("std"), "equal_to_handlegraph_PathSense_t", "");
		cl.def( pybind11::init( [](){ return new std::equal_to<handlegraph::PathSense>(); } ) );
		cl.def( pybind11::init( [](std::equal_to<handlegraph::PathSense> const &o){ return new std::equal_to<handlegraph::PathSense>(o); } ) );
		cl.def("__call__", (bool (std::equal_to<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &, const enum handlegraph::PathSense &) const) &std::equal_to<handlegraph::PathSense>::operator(), "C++: std::equal_to<handlegraph::PathSense>::operator()(const enum handlegraph::PathSense &, const enum handlegraph::PathSense &) const --> bool", pybind11::arg("__x"), pybind11::arg("__y"));
	}
	{ // std::equal_to file:bits/stl_function.h line:373
		pybind11::class_<std::equal_to<std::string>, std::shared_ptr<std::equal_to<std::string>>, std::binary_function<std::string,std::string,bool>> cl(M("std"), "equal_to_std_string_t", "");
		cl.def( pybind11::init( [](){ return new std::equal_to<std::string>(); } ) );
		cl.def( pybind11::init( [](std::equal_to<std::string> const &o){ return new std::equal_to<std::string>(o); } ) );
		cl.def("__call__", (bool (std::equal_to<std::string>::*)(const std::string &, const std::string &) const) &std::equal_to<std::string>::operator(), "C++: std::equal_to<std::string>::operator()(const std::string &, const std::string &) const --> bool", pybind11::arg("__x"), pybind11::arg("__y"));
		cl.def("assign", (struct std::binary_function<std::string, std::string, bool> & (std::binary_function<std::string,std::string,bool>::*)(const struct std::binary_function<std::string, std::string, bool> &)) &std::binary_function<std::string, std::string, bool>::operator=, "C++: std::binary_function<std::string, std::string, bool>::operator=(const struct std::binary_function<std::string, std::string, bool> &) --> struct std::binary_function<std::string, std::string, bool> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
