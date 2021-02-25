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

void bind_sdsl_int_vector_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // sdsl::int_vector_reference file:sdsl/int_vector.hpp line:634
		pybind11::class_<sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>, std::shared_ptr<sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>>> cl(M("sdsl"), "int_vector_reference_sdsl_int_vector_'\x00'_t", "");
		cl.def( pybind11::init<unsigned long *, unsigned char, unsigned char>(), pybind11::arg("word"), pybind11::arg("offset"), pybind11::arg("len") );

		cl.def( pybind11::init( [](sdsl::int_vector_reference<sdsl::int_vector<'\x00'>> const &o){ return new sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>(o); } ) );
		cl.def("assign", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(unsigned long)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator=, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator=(unsigned long) --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic, pybind11::arg("x"));
		cl.def("assign", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(const class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator=, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator=(const class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &) --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic, pybind11::arg("x"));
		cl.def("plus_plus", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)()) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator++, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator++() --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic);
		cl.def("plus_plus", (unsigned long (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(int)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator++, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator++(int) --> unsigned long", pybind11::arg(""));
		cl.def("minus_minus", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)()) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator--, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator--() --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic);
		cl.def("minus_minus", (unsigned long (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(int)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator--, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator--(int) --> unsigned long", pybind11::arg(""));
		cl.def("__iadd__", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(const unsigned long)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator+=, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator+=(const unsigned long) --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic, pybind11::arg("x"));
		cl.def("__isub__", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > & (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(const unsigned long)) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator-=, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator-=(const unsigned long) --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &", pybind11::return_value_policy::automatic, pybind11::arg("x"));
		cl.def("__eq__", (bool (sdsl::int_vector_reference<sdsl::int_vector<'\x00'>>::*)(const class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &) const) &sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator==, "C++: sdsl::int_vector_reference<sdsl::int_vector<'\x00'> >::operator==(const class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > &) const --> bool", pybind11::arg("x"));
	}
}
