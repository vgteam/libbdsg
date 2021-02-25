#include <bdsg/internal/mmap_backend.hpp>
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

void bind_bdsg_internal_mmap_backend(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::MmapBackend file:bdsg/internal/mmap_backend.hpp line:21
		pybind11::class_<bdsg::MmapBackend, std::shared_ptr<bdsg::MmapBackend>, handlegraph::TriviallySerializable> cl(M("bdsg"), "MmapBackend", "Basic implementation of TriviallySerializable that uses mmap to map the file\n into memory.");
		cl.def("serialize", [](bdsg::MmapBackend &o, const std::string & a0) -> void { return o.serialize(a0); }, "", pybind11::arg("filename"));
		cl.def("deserialize", [](bdsg::MmapBackend &o, const std::string & a0) -> void { return o.deserialize(a0); }, "", pybind11::arg("filename"));
		cl.def("dissociate", (void (bdsg::MmapBackend::*)()) &bdsg::MmapBackend::dissociate, "Break the write-back link between this object and the file it was loaded\n from, if any. Future modifications to the object will not affect the\n file, although future modifications to the file may still affect the\n object.\n\n After this is called, serialized_data() may return a different address.\n Old pointers into user data are invalidated.\n\nC++: bdsg::MmapBackend::dissociate() --> void");
		cl.def("serialize", (void (bdsg::MmapBackend::*)(int)) &bdsg::MmapBackend::serialize, "Write the contents of this object to an open file descriptor. Makes\n sure to include a leading magic number. If the file is a normal file,\n future modifications to the object will affect the file until\n dissociate() is called or another normal file is associated.\n\n Assumes that the file entirely belongs to this object.\n\nC++: bdsg::MmapBackend::serialize(int) --> void", pybind11::arg("fd"));
		cl.def("deserialize", (void (bdsg::MmapBackend::*)(int)) &bdsg::MmapBackend::deserialize, "Sets the contents of this object to the contents of a serialized object\n from an open file descriptor. The serialized object must be from the\n same implementation of this interface as is calling deserialize(). Can\n only be called on an empty object  If the file is a normal writeable\n file, future modifications to the object will affect the file until\n dissociate() is called or another normal file is associated.\n\n Assumes that the file entirely belongs to this object.\n\nC++: bdsg::MmapBackend::deserialize(int) --> void", pybind11::arg("fd"));
		cl.def("assign", (class bdsg::MmapBackend & (bdsg::MmapBackend::*)(const class bdsg::MmapBackend &)) &bdsg::MmapBackend::operator=, "C++: bdsg::MmapBackend::operator=(const class bdsg::MmapBackend &) --> class bdsg::MmapBackend &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
