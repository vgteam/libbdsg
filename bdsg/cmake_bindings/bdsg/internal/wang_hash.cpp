#include <bdsg/internal/wang_hash.hpp>

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

void bind_bdsg_internal_wang_hash(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// bdsg::wang_hash_64(unsigned long) file:bdsg/internal/wang_hash.hpp line:10
	M("bdsg").def("wang_hash_64", (unsigned long (*)(unsigned long)) &bdsg::wang_hash_64, "Thomas Wang's integer hash function. In many implementations, std::hash\n is identity function for integers, which leads to performance issues.\n\nC++: bdsg::wang_hash_64(unsigned long) --> unsigned long", pybind11::arg("key"));

}
