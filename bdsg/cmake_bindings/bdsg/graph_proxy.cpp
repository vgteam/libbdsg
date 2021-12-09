#include <bdsg/graph_proxy.hpp>
#include <bdsg/internal/base_packed_graph.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <bdsg/internal/wang_hash.hpp>
#include <functional>
#include <handlegraph/types.hpp>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <string>
#include <utility>
#include <vector>

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

void bind_bdsg_graph_proxy(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::GraphProxy file:bdsg/graph_proxy.hpp line:647
		pybind11::class_<bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>, std::shared_ptr<bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>>> cl(M("bdsg"), "GraphProxy_bdsg_BasePackedGraph_bdsg_STLBackend_t", "");
		cl.def("assign", (struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > & (bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend>>::*)(const struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &)) &bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=, "C++: bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::STLBackend> >::operator=(const struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &) --> struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::STLBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // bdsg::GraphProxy file:bdsg/graph_proxy.hpp line:647
		pybind11::class_<bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>, std::shared_ptr<bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>>> cl(M("bdsg"), "GraphProxy_bdsg_BasePackedGraph_bdsg_MappedBackend_t", "");
		cl.def("assign", (struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > & (bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend>>::*)(const struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &)) &bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=, "C++: bdsg::GraphProxy<bdsg::BasePackedGraph<bdsg::MappedBackend> >::operator=(const struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &) --> struct bdsg::GraphProxy<class bdsg::BasePackedGraph<struct bdsg::MappedBackend> > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// bdsg::wang_hash_64(unsigned long) file:bdsg/internal/wang_hash.hpp line:10
	M("bdsg").def("wang_hash_64", (unsigned long (*)(unsigned long)) &bdsg::wang_hash_64, "Thomas Wang's integer hash function. In many implementations, std::hash\n is identity function for integers, which leads to performance issues.\n\nC++: bdsg::wang_hash_64(unsigned long) --> unsigned long", pybind11::arg("key"));

}
