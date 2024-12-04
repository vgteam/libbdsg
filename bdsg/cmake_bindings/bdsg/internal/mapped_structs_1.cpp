#include <bdsg/internal/mapped_structs.hpp>
#include <bdsg/internal/packed_structs.hpp>
#include <ios>
#include <istream>
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
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_bdsg_internal_mapped_structs_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::CompatBackend file:bdsg/internal/mapped_structs.hpp line:1144
		pybind11::class_<bdsg::CompatBackend, std::shared_ptr<bdsg::CompatBackend>> cl(M("bdsg"), "CompatBackend", "Type enum value for selecting data structures that use memory-mappable data\n structures but with the standard allocator.");
		cl.def( pybind11::init( [](){ return new bdsg::CompatBackend(); } ) );
	}
	{ // bdsg::MappedBackend file:bdsg/internal/mapped_structs.hpp line:1155
		pybind11::class_<bdsg::MappedBackend, std::shared_ptr<bdsg::MappedBackend>> cl(M("bdsg"), "MappedBackend", "Type enum value for selecting data structures that use YOMO memory mapping\n and the YOMO allocator.\n\n They can safely exist outside of mapped memory, but are probably slower\n there since YOMO's internal tables still need to be consulted when following\n pointers.");
		cl.def( pybind11::init( [](){ return new bdsg::MappedBackend(); } ) );
	}
	{ // bdsg::VectorFor file:bdsg/internal/mapped_structs.hpp line:1161
		pybind11::class_<bdsg::VectorFor<bdsg::MappedBackend>, std::shared_ptr<bdsg::VectorFor<bdsg::MappedBackend>>> cl(M("bdsg"), "VectorFor_bdsg_MappedBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::VectorFor<bdsg::MappedBackend>(); } ) );
	}
	{ // bdsg::IntVectorFor file:bdsg/internal/mapped_structs.hpp line:1167
		pybind11::class_<bdsg::IntVectorFor<bdsg::MappedBackend>, std::shared_ptr<bdsg::IntVectorFor<bdsg::MappedBackend>>> cl(M("bdsg"), "IntVectorFor_bdsg_MappedBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::IntVectorFor<bdsg::MappedBackend>(); } ) );
	}
	{ // bdsg::VectorFor file:bdsg/internal/mapped_structs.hpp line:1177
		pybind11::class_<bdsg::VectorFor<bdsg::CompatBackend>, std::shared_ptr<bdsg::VectorFor<bdsg::CompatBackend>>> cl(M("bdsg"), "VectorFor_bdsg_CompatBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::VectorFor<bdsg::CompatBackend>(); } ) );
	}
	{ // bdsg::IntVectorFor file:bdsg/internal/mapped_structs.hpp line:1183
		pybind11::class_<bdsg::IntVectorFor<bdsg::CompatBackend>, std::shared_ptr<bdsg::IntVectorFor<bdsg::CompatBackend>>> cl(M("bdsg"), "IntVectorFor_bdsg_CompatBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::IntVectorFor<bdsg::CompatBackend>(); } ) );
	}
	{ // bdsg::PackedDeque file:bdsg/internal/packed_structs.hpp line:327
		pybind11::class_<bdsg::PackedDeque<bdsg::STLBackend>, std::shared_ptr<bdsg::PackedDeque<bdsg::STLBackend>>> cl(M("bdsg"), "PackedDeque_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedDeque<bdsg::STLBackend>(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedDeque<bdsg::STLBackend> const &o){ return new bdsg::PackedDeque<bdsg::STLBackend>(o); } ) );
		cl.def("assign", (class bdsg::PackedDeque<struct bdsg::STLBackend> & (bdsg::PackedDeque<bdsg::STLBackend>::*)(const class bdsg::PackedDeque<struct bdsg::STLBackend> &)) &bdsg::PackedDeque<bdsg::STLBackend>::operator=, "C++: bdsg::PackedDeque<bdsg::STLBackend>::operator=(const class bdsg::PackedDeque<struct bdsg::STLBackend> &) --> class bdsg::PackedDeque<struct bdsg::STLBackend> &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("set", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &, const unsigned long &)) &bdsg::PackedDeque<bdsg::STLBackend>::set, "C++: bdsg::PackedDeque<bdsg::STLBackend>::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &) const) &bdsg::PackedDeque<bdsg::STLBackend>::get, "C++: bdsg::PackedDeque<bdsg::STLBackend>::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append_front", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<bdsg::STLBackend>::append_front, "C++: bdsg::PackedDeque<bdsg::STLBackend>::append_front(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("append_back", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<bdsg::STLBackend>::append_back, "C++: bdsg::PackedDeque<bdsg::STLBackend>::append_back(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop_front", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<bdsg::STLBackend>::pop_front, "C++: bdsg::PackedDeque<bdsg::STLBackend>::pop_front() --> void");
		cl.def("pop_back", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<bdsg::STLBackend>::pop_back, "C++: bdsg::PackedDeque<bdsg::STLBackend>::pop_back() --> void");
		cl.def("reserve", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<bdsg::STLBackend>::reserve, "C++: bdsg::PackedDeque<bdsg::STLBackend>::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<bdsg::STLBackend>::size, "C++: bdsg::PackedDeque<bdsg::STLBackend>::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<bdsg::STLBackend>::empty, "C++: bdsg::PackedDeque<bdsg::STLBackend>::empty() const --> bool");
		cl.def("clear", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<bdsg::STLBackend>::clear, "C++: bdsg::PackedDeque<bdsg::STLBackend>::clear() --> void");
		cl.def("memory_usage", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<bdsg::STLBackend>::memory_usage, "C++: bdsg::PackedDeque<bdsg::STLBackend>::memory_usage() const --> unsigned long");
	}
	{ // bdsg::PackedSet file:bdsg/internal/packed_structs.hpp line:411
		pybind11::class_<bdsg::PackedSet<bdsg::STLBackend>, std::shared_ptr<bdsg::PackedSet<bdsg::STLBackend>>> cl(M("bdsg"), "PackedSet_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedSet<bdsg::STLBackend>(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedSet<bdsg::STLBackend> const &o){ return new bdsg::PackedSet<bdsg::STLBackend>(o); } ) );
		cl.def("assign", (class bdsg::PackedSet<struct bdsg::STLBackend> & (bdsg::PackedSet<bdsg::STLBackend>::*)(const class bdsg::PackedSet<struct bdsg::STLBackend> &)) &bdsg::PackedSet<bdsg::STLBackend>::operator=, "C++: bdsg::PackedSet<bdsg::STLBackend>::operator=(const class bdsg::PackedSet<struct bdsg::STLBackend> &) --> class bdsg::PackedSet<struct bdsg::STLBackend> &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("insert", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedSet<bdsg::STLBackend>::insert, "C++: bdsg::PackedSet<bdsg::STLBackend>::insert(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("find", (bool (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &) const) &bdsg::PackedSet<bdsg::STLBackend>::find, "C++: bdsg::PackedSet<bdsg::STLBackend>::find(const unsigned long &) const --> bool", pybind11::arg("value"));
		cl.def("remove", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedSet<bdsg::STLBackend>::remove, "C++: bdsg::PackedSet<bdsg::STLBackend>::remove(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("size", (unsigned long (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<bdsg::STLBackend>::size, "C++: bdsg::PackedSet<bdsg::STLBackend>::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<bdsg::STLBackend>::empty, "C++: bdsg::PackedSet<bdsg::STLBackend>::empty() const --> bool");
		cl.def("set_load_factors", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(double, double)) &bdsg::PackedSet<bdsg::STLBackend>::set_load_factors, "C++: bdsg::PackedSet<bdsg::STLBackend>::set_load_factors(double, double) --> void", pybind11::arg("min_load_factor"), pybind11::arg("max_load_factor"));
		cl.def("max_load_factor", (double (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<bdsg::STLBackend>::max_load_factor, "C++: bdsg::PackedSet<bdsg::STLBackend>::max_load_factor() const --> double");
		cl.def("min_load_factor", (double (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<bdsg::STLBackend>::min_load_factor, "C++: bdsg::PackedSet<bdsg::STLBackend>::min_load_factor() const --> double");
	}
}
