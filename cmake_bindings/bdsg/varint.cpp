#include <bdsg/varint.hpp>
#include <iterator>
#include <memory>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_varint(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::sqvarint::count_leading_zeros_64(unsigned long) file:bdsg/varint.hpp line:31
	M("bdsg::sqvarint").def("count_leading_zeros_64", (unsigned int (*)(unsigned long)) &bdsg::sqvarint::count_leading_zeros_64, "C++: bdsg::sqvarint::count_leading_zeros_64(unsigned long) --> unsigned int", pybind11::arg("x"));

	// bdsg::sqvarint::count_trailing_zeros_32(unsigned int) file:bdsg/varint.hpp line:39
	M("bdsg::sqvarint").def("count_trailing_zeros_32", (unsigned int (*)(unsigned int)) &bdsg::sqvarint::count_trailing_zeros_32, "C++: bdsg::sqvarint::count_trailing_zeros_32(unsigned int) --> unsigned int", pybind11::arg("x"));

	// bdsg::sqvarint::unaligned_load_u64(const unsigned char *) file:bdsg/varint.hpp line:47
	M("bdsg::sqvarint").def("unaligned_load_u64", (unsigned long (*)(const unsigned char *)) &bdsg::sqvarint::unaligned_load_u64, "C++: bdsg::sqvarint::unaligned_load_u64(const unsigned char *) --> unsigned long", pybind11::arg("p"));

	// bdsg::sqvarint::unaligned_load(const unsigned char *, unsigned char) file:bdsg/varint.hpp line:55
	M("bdsg::sqvarint").def("unaligned_load", (unsigned long (*)(const unsigned char *, unsigned char)) &bdsg::sqvarint::unaligned_load, "C++: bdsg::sqvarint::unaligned_load(const unsigned char *, unsigned char) --> unsigned long", pybind11::arg("p"), pybind11::arg("s"));

	// bdsg::sqvarint::unaligned_load_u16(const unsigned char *) file:bdsg/varint.hpp line:65
	M("bdsg::sqvarint").def("unaligned_load_u16", (unsigned short (*)(const unsigned char *)) &bdsg::sqvarint::unaligned_load_u16, "C++: bdsg::sqvarint::unaligned_load_u16(const unsigned char *) --> unsigned short", pybind11::arg("p"));

	// bdsg::sqvarint::encode(const unsigned long *, unsigned char *, unsigned long) file:bdsg/varint.hpp line:141
	M("bdsg::sqvarint").def("encode", (unsigned char * (*)(const unsigned long *, unsigned char *, unsigned long)) &bdsg::sqvarint::encode, "C++: bdsg::sqvarint::encode(const unsigned long *, unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("in"), pybind11::arg("out"), pybind11::arg("count"));

	// bdsg::sqvarint::decode(unsigned long *, unsigned char *, unsigned long) file:bdsg/varint.hpp line:166
	M("bdsg::sqvarint").def("decode", (unsigned char * (*)(unsigned long *, unsigned char *, unsigned long)) &bdsg::sqvarint::decode, "C++: bdsg::sqvarint::decode(unsigned long *, unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("out"), pybind11::arg("in"), pybind11::arg("count"));

	// bdsg::sqvarint::length(const unsigned long &) file:bdsg/varint.hpp line:183
	M("bdsg::sqvarint").def("length", (unsigned long (*)(const unsigned long &)) &bdsg::sqvarint::length, "C++: bdsg::sqvarint::length(const unsigned long &) --> unsigned long", pybind11::arg("i"));

	// bdsg::sqvarint::length(const unsigned long *, unsigned long) file:bdsg/varint.hpp line:199
	M("bdsg::sqvarint").def("length", (unsigned long (*)(const unsigned long *, unsigned long)) &bdsg::sqvarint::length, "C++: bdsg::sqvarint::length(const unsigned long *, unsigned long) --> unsigned long", pybind11::arg("v"), pybind11::arg("count"));

	// bdsg::sqvarint::seek(unsigned char *, unsigned long) file:bdsg/varint.hpp line:205
	M("bdsg::sqvarint").def("seek", (unsigned char * (*)(unsigned char *, unsigned long)) &bdsg::sqvarint::seek, "C++: bdsg::sqvarint::seek(unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("in"), pybind11::arg("count"));

	// bdsg::sqvarint::bytes(unsigned char *, unsigned long) file:bdsg/varint.hpp line:219
	M("bdsg::sqvarint").def("bytes", (unsigned long (*)(unsigned char *, unsigned long)) &bdsg::sqvarint::bytes, "C++: bdsg::sqvarint::bytes(unsigned char *, unsigned long) --> unsigned long", pybind11::arg("in"), pybind11::arg("count"));

}
