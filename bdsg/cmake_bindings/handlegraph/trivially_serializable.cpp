#include <functional>
#include <handlegraph/trivially_serializable.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_handlegraph_trivially_serializable(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // handlegraph::TriviallySerializable file:handlegraph/trivially_serializable.hpp line:31
		pybind11::class_<handlegraph::TriviallySerializable, std::shared_ptr<handlegraph::TriviallySerializable>, handlegraph::Serializable> cl(M("handlegraph"), "TriviallySerializable", "Interface for objects that can use identical in-memory and serialized representations.\n\n The representation begins with the serialized 4-byte magic number, followed\n by user data. Length is implicit in either stream length or file size, and\n can be grown.\n\n If serialization or deserialization throws, it is safe to destroy the\n object, but not to do anything else with it.\n\n Modifying a file that an object has been loaded from or saved to, either on\n disk or through another associated object, is undefined behavior.\n To prevent modifications to an object from modifying the last file loaded or\n saved to, use dissociate().");
		cl.def("dissociate", (void (handlegraph::TriviallySerializable::*)()) &handlegraph::TriviallySerializable::dissociate, "Break the write-back link between this object and the file it was loaded\n from, if any. Future modifications to the object will not affect the\n file, although future modifications to the file may still affect the\n object.\n\nC++: handlegraph::TriviallySerializable::dissociate() --> void");
		cl.def("serialize", (void (handlegraph::TriviallySerializable::*)(const class std::function<void (const void *, unsigned long)> &) const) &handlegraph::TriviallySerializable::serialize, "Serialize as blocks of data shown to the given function. The pointer\n must not be null. The blocks must include the magic number.\n\nC++: handlegraph::TriviallySerializable::serialize(const class std::function<void (const void *, unsigned long)> &) const --> void", pybind11::arg("iteratee"));
		cl.def("serialize", (void (handlegraph::TriviallySerializable::*)(int)) &handlegraph::TriviallySerializable::serialize, "Write the contents of this object to an open file descriptor. Makes\n sure to include a leading magic number. If the file is a normal file,\n future modifications to the object will affect the file until\n dissociate() is called or another normal file is associated.\n\n Assumes that the file entirely belongs to this object.\n\nC++: handlegraph::TriviallySerializable::serialize(int) --> void", pybind11::arg("fd"));
		cl.def("deserialize", (void (handlegraph::TriviallySerializable::*)(int)) &handlegraph::TriviallySerializable::deserialize, "Sets the contents of this object to the contents of a serialized object\n from an open file descriptor. The serialized object must be from the\n same implementation of this interface as is calling deserialize(). Can\n only be called on an empty object  If the file is a normal writeable\n file, future modifications to the object will affect the file until\n dissociate() is called or another normal file is associated.\n\n Assumes that the file entirely belongs to this object.\n\nC++: handlegraph::TriviallySerializable::deserialize(int) --> void", pybind11::arg("fd"));
		cl.def("serialize", (void (handlegraph::TriviallySerializable::*)(const std::string &)) &handlegraph::TriviallySerializable::serialize, "Write the contents of this object to a named file. Makes sure to\n include a leading magic number. If the file is nonexistent or a normal\n file, future modifications to the object will affect the file until\n dissociate() is called or another normal file is associated.\n\nC++: handlegraph::TriviallySerializable::serialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("deserialize", (void (handlegraph::TriviallySerializable::*)(const std::string &)) &handlegraph::TriviallySerializable::deserialize, "Sets the contents of this object to the contents of a serialized object\n from a file. The serialized object must be from the same implementation\n of this interface as is calling deserialize(). Can only be called on an\n empty object. If the file is a normal writeable file, future\n modifications to the object will affect the file until dissociate() is\n called or another normal file is associated.\n\nC++: handlegraph::TriviallySerializable::deserialize(const std::string &) --> void", pybind11::arg("filename"));
		cl.def("assign", (class handlegraph::TriviallySerializable & (handlegraph::TriviallySerializable::*)(const class handlegraph::TriviallySerializable &)) &handlegraph::TriviallySerializable::operator=, "C++: handlegraph::TriviallySerializable::operator=(const class handlegraph::TriviallySerializable &) --> class handlegraph::TriviallySerializable &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
}
