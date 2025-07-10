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
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
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
		cl.def("assign", (class bdsg::PackedDeque<> & (bdsg::PackedDeque<bdsg::STLBackend>::*)(const class bdsg::PackedDeque<> &)) &bdsg::PackedDeque<>::operator=, "C++: bdsg::PackedDeque<>::operator=(const class bdsg::PackedDeque<> &) --> class bdsg::PackedDeque<> &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("set", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &, const unsigned long &)) &bdsg::PackedDeque<>::set, "C++: bdsg::PackedDeque<>::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &) const) &bdsg::PackedDeque<>::get, "C++: bdsg::PackedDeque<>::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append_front", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<>::append_front, "C++: bdsg::PackedDeque<>::append_front(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("append_back", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<>::append_back, "C++: bdsg::PackedDeque<>::append_back(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop_front", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<>::pop_front, "C++: bdsg::PackedDeque<>::pop_front() --> void");
		cl.def("pop_back", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<>::pop_back, "C++: bdsg::PackedDeque<>::pop_back() --> void");
		cl.def("reserve", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedDeque<>::reserve, "C++: bdsg::PackedDeque<>::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<>::size, "C++: bdsg::PackedDeque<>::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<>::empty, "C++: bdsg::PackedDeque<>::empty() const --> bool");
		cl.def("clear", (void (bdsg::PackedDeque<bdsg::STLBackend>::*)()) &bdsg::PackedDeque<>::clear, "C++: bdsg::PackedDeque<>::clear() --> void");
		cl.def("memory_usage", (unsigned long (bdsg::PackedDeque<bdsg::STLBackend>::*)() const) &bdsg::PackedDeque<>::memory_usage, "C++: bdsg::PackedDeque<>::memory_usage() const --> unsigned long");
	}
	{ // bdsg::PackedSet file:bdsg/internal/packed_structs.hpp line:411
		pybind11::class_<bdsg::PackedSet<bdsg::STLBackend>, std::shared_ptr<bdsg::PackedSet<bdsg::STLBackend>>> cl(M("bdsg"), "PackedSet_bdsg_STLBackend_t", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedSet<bdsg::STLBackend>(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedSet<bdsg::STLBackend> const &o){ return new bdsg::PackedSet<bdsg::STLBackend>(o); } ) );
		cl.def("assign", (class bdsg::PackedSet<> & (bdsg::PackedSet<bdsg::STLBackend>::*)(const class bdsg::PackedSet<> &)) &bdsg::PackedSet<>::operator=, "C++: bdsg::PackedSet<>::operator=(const class bdsg::PackedSet<> &) --> class bdsg::PackedSet<> &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("insert", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedSet<>::insert, "C++: bdsg::PackedSet<>::insert(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("find", (bool (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &) const) &bdsg::PackedSet<>::find, "C++: bdsg::PackedSet<>::find(const unsigned long &) const --> bool", pybind11::arg("value"));
		cl.def("remove", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(const unsigned long &)) &bdsg::PackedSet<>::remove, "C++: bdsg::PackedSet<>::remove(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("size", (unsigned long (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<>::size, "C++: bdsg::PackedSet<>::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<>::empty, "C++: bdsg::PackedSet<>::empty() const --> bool");
		cl.def("set_load_factors", (void (bdsg::PackedSet<bdsg::STLBackend>::*)(double, double)) &bdsg::PackedSet<>::set_load_factors, "C++: bdsg::PackedSet<>::set_load_factors(double, double) --> void", pybind11::arg("min_load_factor"), pybind11::arg("max_load_factor"));
		cl.def("max_load_factor", (double (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<>::max_load_factor, "C++: bdsg::PackedSet<>::max_load_factor() const --> double");
		cl.def("min_load_factor", (double (bdsg::PackedSet<bdsg::STLBackend>::*)() const) &bdsg::PackedSet<>::min_load_factor, "C++: bdsg::PackedSet<>::min_load_factor() const --> double");
	}
}
