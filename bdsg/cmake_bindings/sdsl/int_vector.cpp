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

void bind_sdsl_int_vector(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // sdsl::int_vector file:sdsl/int_vector.hpp line:289
		pybind11::class_<sdsl::int_vector<'\x00'>, std::shared_ptr<sdsl::int_vector<'\x00'>>> cl(M("sdsl"), "int_vector_'\x00'_t", "");
		{ // sdsl::int_vector<'\x00'>::raw_wrapper file:sdsl/int_vector.hpp line:612
			auto & enclosing_class = cl;
			pybind11::class_<sdsl::int_vector<'\x00'>::raw_wrapper, std::shared_ptr<sdsl::int_vector<'\x00'>::raw_wrapper>> cl(enclosing_class, "raw_wrapper", "");
			cl.def( pybind11::init<const class sdsl::int_vector<'\x00'> &>(), pybind11::arg("_vec") );

			cl.def( pybind11::init( [](sdsl::int_vector<'\x00'>::raw_wrapper const &o){ return new sdsl::int_vector<'\x00'>::raw_wrapper(o); } ) );
		}

		cl.def( pybind11::init( [](unsigned long const & a0, unsigned long const & a1){ return new sdsl::int_vector<'\x00'>(a0, a1); } ), "doc" , pybind11::arg("size"), pybind11::arg("default_value"));
		cl.def( pybind11::init<unsigned long, unsigned long, unsigned char>(), pybind11::arg("size"), pybind11::arg("default_value"), pybind11::arg("intWidth") );

		cl.def( pybind11::init( [](){ return new sdsl::int_vector<'\x00'>(); } ), "doc" );
		cl.def( pybind11::init<unsigned long>(), pybind11::arg("size") );

		cl.def( pybind11::init( [](sdsl::int_vector<'\x00'> const &o){ return new sdsl::int_vector<'\x00'>(o); } ) );
		cl.def_readonly("raw", &sdsl::int_vector<'\x00'>::raw);
		cl.def("empty", (bool (sdsl::int_vector<'\x00'>::*)() const) &sdsl::int_vector<'\x00'>::empty, "C++: sdsl::int_vector<'\x00'>::empty() const --> bool");
		cl.def("swap", (void (sdsl::int_vector<'\x00'>::*)(class sdsl::int_vector<'\x00'> &)) &sdsl::int_vector<'\x00'>::swap, "C++: sdsl::int_vector<'\x00'>::swap(class sdsl::int_vector<'\x00'> &) --> void", pybind11::arg("v"));
		cl.def("resize", (void (sdsl::int_vector<'\x00'>::*)(const unsigned long)) &sdsl::int_vector<'\x00'>::resize, "C++: sdsl::int_vector<'\x00'>::resize(const unsigned long) --> void", pybind11::arg("size"));
		cl.def("bit_resize", (void (sdsl::int_vector<'\x00'>::*)(const unsigned long)) &sdsl::int_vector<'\x00'>::bit_resize, "C++: sdsl::int_vector<'\x00'>::bit_resize(const unsigned long) --> void", pybind11::arg("size"));
		cl.def("size", (unsigned long (sdsl::int_vector<'\x00'>::*)() const) &sdsl::int_vector<'\x00'>::size, "C++: sdsl::int_vector<'\x00'>::size() const --> unsigned long");
		cl.def_static("max_size", (unsigned long (*)()) &sdsl::int_vector<'\x00'>::max_size, "C++: sdsl::int_vector<'\x00'>::max_size() --> unsigned long");
		cl.def("bit_size", (unsigned long (sdsl::int_vector<'\x00'>::*)() const) &sdsl::int_vector<'\x00'>::bit_size, "C++: sdsl::int_vector<'\x00'>::bit_size() const --> unsigned long");
		cl.def("capacity", (unsigned long (sdsl::int_vector<'\x00'>::*)() const) &sdsl::int_vector<'\x00'>::capacity, "C++: sdsl::int_vector<'\x00'>::capacity() const --> unsigned long");
		cl.def("data", (unsigned long * (sdsl::int_vector<'\x00'>::*)()) &sdsl::int_vector<'\x00'>::data, "C++: sdsl::int_vector<'\x00'>::data() --> unsigned long *", pybind11::return_value_policy::automatic);
		cl.def("get_int", [](sdsl::int_vector<'\x00'> const &o, unsigned long const & a0) -> unsigned long { return o.get_int(a0); }, "", pybind11::arg("idx"));
		cl.def("get_int", (unsigned long (sdsl::int_vector<'\x00'>::*)(unsigned long, const unsigned char) const) &sdsl::int_vector<'\x00'>::get_int, "C++: sdsl::int_vector<'\x00'>::get_int(unsigned long, const unsigned char) const --> unsigned long", pybind11::arg("idx"), pybind11::arg("len"));
		cl.def("set_int", [](sdsl::int_vector<'\x00'> &o, unsigned long const & a0, unsigned long const & a1) -> void { return o.set_int(a0, a1); }, "", pybind11::arg("idx"), pybind11::arg("x"));
		cl.def("set_int", (void (sdsl::int_vector<'\x00'>::*)(unsigned long, unsigned long, const unsigned char)) &sdsl::int_vector<'\x00'>::set_int, "C++: sdsl::int_vector<'\x00'>::set_int(unsigned long, unsigned long, const unsigned char) --> void", pybind11::arg("idx"), pybind11::arg("x"), pybind11::arg("len"));
		cl.def("width", (unsigned char (sdsl::int_vector<'\x00'>::*)() const) &sdsl::int_vector<'\x00'>::width, "C++: sdsl::int_vector<'\x00'>::width() const --> unsigned char");
		cl.def("width", (void (sdsl::int_vector<'\x00'>::*)(unsigned char)) &sdsl::int_vector<'\x00'>::width, "C++: sdsl::int_vector<'\x00'>::width(unsigned char) --> void", pybind11::arg("new_width"));
		cl.def("__getitem__", (class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> > (sdsl::int_vector<'\x00'>::*)(const unsigned long &)) &sdsl::int_vector<'\x00'>::operator[], "C++: sdsl::int_vector<'\x00'>::operator[](const unsigned long &) --> class sdsl::int_vector_reference<class sdsl::int_vector<'\x00'> >", pybind11::arg("idx"));
		cl.def("assign", (class sdsl::int_vector<'\x00'> & (sdsl::int_vector<'\x00'>::*)(const class sdsl::int_vector<'\x00'> &)) &sdsl::int_vector<'\x00'>::operator=, "C++: sdsl::int_vector<'\x00'>::operator=(const class sdsl::int_vector<'\x00'> &) --> class sdsl::int_vector<'\x00'> &", pybind11::return_value_policy::automatic, pybind11::arg("v"));
		cl.def("__eq__", (bool (sdsl::int_vector<'\x00'>::*)(const class sdsl::int_vector<'\x00'> &) const) &sdsl::int_vector<'\x00'>::operator==, "C++: sdsl::int_vector<'\x00'>::operator==(const class sdsl::int_vector<'\x00'> &) const --> bool", pybind11::arg("v"));
		cl.def("__ne__", (bool (sdsl::int_vector<'\x00'>::*)(const class sdsl::int_vector<'\x00'> &) const) &sdsl::int_vector<'\x00'>::operator!=, "C++: sdsl::int_vector<'\x00'>::operator!=(const class sdsl::int_vector<'\x00'> &) const --> bool", pybind11::arg("v"));
		cl.def("flip", (void (sdsl::int_vector<'\x00'>::*)()) &sdsl::int_vector<'\x00'>::flip, "C++: sdsl::int_vector<'\x00'>::flip() --> void");
	}
}
