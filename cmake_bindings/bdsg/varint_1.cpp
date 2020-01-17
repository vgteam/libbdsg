#include <bdsg/varint.hpp>
#include <iterator>
#include <memory>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_varint_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::msbvarint::length(unsigned long) file:bdsg/varint.hpp line:254
	M("bdsg::msbvarint").def("length", (unsigned long (*)(unsigned long)) &bdsg::msbvarint::length, "C++: bdsg::msbvarint::length(unsigned long) --> unsigned long", pybind11::arg("n"));

	// bdsg::msbvarint::length(const unsigned long *, unsigned long) file:bdsg/varint.hpp line:269
	M("bdsg::msbvarint").def("length", (unsigned long (*)(const unsigned long *, unsigned long)) &bdsg::msbvarint::length, "C++: bdsg::msbvarint::length(const unsigned long *, unsigned long) --> unsigned long", pybind11::arg("v"), pybind11::arg("n"));

	// bdsg::msbvarint::encode(unsigned long, unsigned char *) file:bdsg/varint.hpp line:281
	M("bdsg::msbvarint").def("encode", (unsigned char * (*)(unsigned long, unsigned char *)) &bdsg::msbvarint::encode, "C++: bdsg::msbvarint::encode(unsigned long, unsigned char *) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("n"), pybind11::arg("ptr"));

	// bdsg::msbvarint::encode(const unsigned long *, unsigned char *, unsigned long) file:bdsg/varint.hpp line:294
	M("bdsg::msbvarint").def("encode", (unsigned char * (*)(const unsigned long *, unsigned char *, unsigned long)) &bdsg::msbvarint::encode, "C++: bdsg::msbvarint::encode(const unsigned long *, unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("in"), pybind11::arg("ptr"), pybind11::arg("c"));

	// bdsg::msbvarint::decode(unsigned long *, unsigned char *) file:bdsg/varint.hpp line:305
	M("bdsg::msbvarint").def("decode", (unsigned char * (*)(unsigned long *, unsigned char *)) &bdsg::msbvarint::decode, "C++: bdsg::msbvarint::decode(unsigned long *, unsigned char *) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("out"), pybind11::arg("ptr"));

	// bdsg::msbvarint::decode(unsigned long *, unsigned char *, unsigned long) file:bdsg/varint.hpp line:318
	M("bdsg::msbvarint").def("decode", (unsigned char * (*)(unsigned long *, unsigned char *, unsigned long)) &bdsg::msbvarint::decode, "C++: bdsg::msbvarint::decode(unsigned long *, unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("out"), pybind11::arg("ptr"), pybind11::arg("c"));

	// bdsg::msbvarint::seek(unsigned char *, unsigned long) file:bdsg/varint.hpp line:325
	M("bdsg::msbvarint").def("seek", (unsigned char * (*)(unsigned char *, unsigned long)) &bdsg::msbvarint::seek, "C++: bdsg::msbvarint::seek(unsigned char *, unsigned long) --> unsigned char *", pybind11::return_value_policy::automatic, pybind11::arg("ptr"), pybind11::arg("n"));

	// bdsg::msbvarint::bytes(unsigned char *, unsigned long) file:bdsg/varint.hpp line:335
	M("bdsg::msbvarint").def("bytes", (unsigned long (*)(unsigned char *, unsigned long)) &bdsg::msbvarint::bytes, "C++: bdsg::msbvarint::bytes(unsigned char *, unsigned long) --> unsigned long", pybind11::arg("ptr"), pybind11::arg("n"));

}
