#include <bdsg/internal/packed_structs.hpp>
#include <cwchar>
#include <ios>
#include <istream>
#include <ostream>
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
#include <bdsg/binder_hook_compile.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_internal_packed_structs(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::PackedVector file:bdsg/internal/packed_structs.hpp line:27
		pybind11::class_<bdsg::PackedVector, std::shared_ptr<bdsg::PackedVector>> cl(M("bdsg"), "PackedVector", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedVector(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedVector const &o){ return new bdsg::PackedVector(o); } ) );
		cl.def("assign", (class bdsg::PackedVector & (bdsg::PackedVector::*)(const class bdsg::PackedVector &)) &bdsg::PackedVector::operator=, "Copy assignment operator\n\nC++: bdsg::PackedVector::operator=(const class bdsg::PackedVector &) --> class bdsg::PackedVector &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("set", (void (bdsg::PackedVector::*)(const unsigned long &, const unsigned long &)) &bdsg::PackedVector::set, "Set the i-th value\n\nC++: bdsg::PackedVector::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::PackedVector::*)(const unsigned long &) const) &bdsg::PackedVector::get, "Returns the i-th value\n\nC++: bdsg::PackedVector::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append", (void (bdsg::PackedVector::*)(const unsigned long &)) &bdsg::PackedVector::append, "Add a value to the end\n\nC++: bdsg::PackedVector::append(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop", (void (bdsg::PackedVector::*)()) &bdsg::PackedVector::pop, "Remove the last value\n\nC++: bdsg::PackedVector::pop() --> void");
		cl.def("resize", (void (bdsg::PackedVector::*)(const unsigned long &)) &bdsg::PackedVector::resize, "Either shrink the vector or grow the vector to the new size. New\n entries created by growing are filled with 0.\n\nC++: bdsg::PackedVector::resize(const unsigned long &) --> void", pybind11::arg("new_size"));
		cl.def("reserve", (void (bdsg::PackedVector::*)(const unsigned long &)) &bdsg::PackedVector::reserve, "If necessary, expand capacity so that the given number of entries can\n be included in the vector without reallocating. Never shrinks capacity.\n\nC++: bdsg::PackedVector::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::PackedVector::*)() const) &bdsg::PackedVector::size, "Returns the number of values.\n\nC++: bdsg::PackedVector::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedVector::*)() const) &bdsg::PackedVector::empty, "Returns true if there are no entries and false otherwise.\n\nC++: bdsg::PackedVector::empty() const --> bool");
		cl.def("clear", (void (bdsg::PackedVector::*)()) &bdsg::PackedVector::clear, "Clears the backing vector.\n\nC++: bdsg::PackedVector::clear() --> void");
		cl.def("memory_usage", (unsigned long (bdsg::PackedVector::*)() const) &bdsg::PackedVector::memory_usage, "Reports the amount of memory consumed by this object in bytes.\n\nC++: bdsg::PackedVector::memory_usage() const --> unsigned long");
		cl.def("__eq__", (bool (bdsg::PackedVector::*)(const class bdsg::PackedVector &) const) &bdsg::PackedVector::operator==, "Returns true if the contents are identical (but not necessarily storage\n parameters, such as pointer to data, capacity, bit width, etc.).\n\nC++: bdsg::PackedVector::operator==(const class bdsg::PackedVector &) const --> bool", pybind11::arg("other"));
	}
	{ // bdsg::PagedVector file:bdsg/internal/packed_structs.hpp line:106
		pybind11::class_<bdsg::PagedVector, std::shared_ptr<bdsg::PagedVector>> cl(M("bdsg"), "PagedVector", "");
		cl.def( pybind11::init<unsigned long>(), pybind11::arg("page_size") );

		cl.def("set", (void (bdsg::PagedVector::*)(const unsigned long &, const unsigned long &)) &bdsg::PagedVector::set, "Set the i-th value\n\nC++: bdsg::PagedVector::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::PagedVector::*)(const unsigned long &) const) &bdsg::PagedVector::get, "Returns the i-th value\n\nC++: bdsg::PagedVector::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append", (void (bdsg::PagedVector::*)(const unsigned long &)) &bdsg::PagedVector::append, "Add a value to the end\n\nC++: bdsg::PagedVector::append(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop", (void (bdsg::PagedVector::*)()) &bdsg::PagedVector::pop, "Remove the last value\n\nC++: bdsg::PagedVector::pop() --> void");
		cl.def("resize", (void (bdsg::PagedVector::*)(const unsigned long &)) &bdsg::PagedVector::resize, "Either shrink the vector or grow the vector to the new size. New\n entries created by growing are filled with 0.\n\nC++: bdsg::PagedVector::resize(const unsigned long &) --> void", pybind11::arg("new_size"));
		cl.def("reserve", (void (bdsg::PagedVector::*)(const unsigned long &)) &bdsg::PagedVector::reserve, "If necessary, expand capacity so that the given number of entries can\n be included in the vector without reallocating. Never shrinks capacity.\n\nC++: bdsg::PagedVector::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::PagedVector::*)() const) &bdsg::PagedVector::size, "Returns the number of values\n\nC++: bdsg::PagedVector::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PagedVector::*)() const) &bdsg::PagedVector::empty, "Returns true if there are no entries and false otherwise\n\nC++: bdsg::PagedVector::empty() const --> bool");
		cl.def("clear", (void (bdsg::PagedVector::*)()) &bdsg::PagedVector::clear, "Clears the backing vector\n\nC++: bdsg::PagedVector::clear() --> void");
		cl.def("page_width", (unsigned long (bdsg::PagedVector::*)() const) &bdsg::PagedVector::page_width, "Returns the page width of the vector\n\nC++: bdsg::PagedVector::page_width() const --> unsigned long");
		cl.def("memory_usage", (unsigned long (bdsg::PagedVector::*)() const) &bdsg::PagedVector::memory_usage, "Reports the amount of memory consumed by this object in bytes\n\nC++: bdsg::PagedVector::memory_usage() const --> unsigned long");
	}
	{ // bdsg::RobustPagedVector file:bdsg/internal/packed_structs.hpp line:188
		pybind11::class_<bdsg::RobustPagedVector, std::shared_ptr<bdsg::RobustPagedVector>> cl(M("bdsg"), "RobustPagedVector", "");
		cl.def( pybind11::init<unsigned long>(), pybind11::arg("page_size") );

		cl.def("set", (void (bdsg::RobustPagedVector::*)(const unsigned long &, const unsigned long &)) &bdsg::RobustPagedVector::set, "Set the i-th value\n\nC++: bdsg::RobustPagedVector::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::RobustPagedVector::*)(const unsigned long &) const) &bdsg::RobustPagedVector::get, "Returns the i-th value\n\nC++: bdsg::RobustPagedVector::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append", (void (bdsg::RobustPagedVector::*)(const unsigned long &)) &bdsg::RobustPagedVector::append, "Add a value to the end\n\nC++: bdsg::RobustPagedVector::append(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop", (void (bdsg::RobustPagedVector::*)()) &bdsg::RobustPagedVector::pop, "Remove the last value\n\nC++: bdsg::RobustPagedVector::pop() --> void");
		cl.def("resize", (void (bdsg::RobustPagedVector::*)(const unsigned long &)) &bdsg::RobustPagedVector::resize, "Either shrink the vector or grow the vector to the new size. New\n entries created by growing are filled with 0.\n\nC++: bdsg::RobustPagedVector::resize(const unsigned long &) --> void", pybind11::arg("new_size"));
		cl.def("reserve", (void (bdsg::RobustPagedVector::*)(const unsigned long &)) &bdsg::RobustPagedVector::reserve, "If necessary, expand capacity so that the given number of entries can\n be included in the vector without reallocating. Never shrinks capacity.\n\nC++: bdsg::RobustPagedVector::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::RobustPagedVector::*)() const) &bdsg::RobustPagedVector::size, "Returns the number of values\n\nC++: bdsg::RobustPagedVector::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::RobustPagedVector::*)() const) &bdsg::RobustPagedVector::empty, "Returns true if there are no entries and false otherwise\n\nC++: bdsg::RobustPagedVector::empty() const --> bool");
		cl.def("clear", (void (bdsg::RobustPagedVector::*)()) &bdsg::RobustPagedVector::clear, "Clears the backing vector\n\nC++: bdsg::RobustPagedVector::clear() --> void");
		cl.def("page_width", (unsigned long (bdsg::RobustPagedVector::*)() const) &bdsg::RobustPagedVector::page_width, "Returns the page width of the vector\n\nC++: bdsg::RobustPagedVector::page_width() const --> unsigned long");
		cl.def("memory_usage", (unsigned long (bdsg::RobustPagedVector::*)() const) &bdsg::RobustPagedVector::memory_usage, "Reports the amount of memory consumed by this object in bytes\n\nC++: bdsg::RobustPagedVector::memory_usage() const --> unsigned long");
	}
	{ // bdsg::PackedDeque file:bdsg/internal/packed_structs.hpp line:260
		pybind11::class_<bdsg::PackedDeque, std::shared_ptr<bdsg::PackedDeque>> cl(M("bdsg"), "PackedDeque", "");
		cl.def( pybind11::init( [](){ return new bdsg::PackedDeque(); } ) );
		cl.def("set", (void (bdsg::PackedDeque::*)(const unsigned long &, const unsigned long &)) &bdsg::PackedDeque::set, "Set the i-th value\n\nC++: bdsg::PackedDeque::set(const unsigned long &, const unsigned long &) --> void", pybind11::arg("i"), pybind11::arg("value"));
		cl.def("get", (unsigned long (bdsg::PackedDeque::*)(const unsigned long &) const) &bdsg::PackedDeque::get, "Returns the i-th value\n\nC++: bdsg::PackedDeque::get(const unsigned long &) const --> unsigned long", pybind11::arg("i"));
		cl.def("append_front", (void (bdsg::PackedDeque::*)(const unsigned long &)) &bdsg::PackedDeque::append_front, "Add a value to the front\n\nC++: bdsg::PackedDeque::append_front(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("append_back", (void (bdsg::PackedDeque::*)(const unsigned long &)) &bdsg::PackedDeque::append_back, "Add a value to the back\n\nC++: bdsg::PackedDeque::append_back(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("pop_front", (void (bdsg::PackedDeque::*)()) &bdsg::PackedDeque::pop_front, "Remove the front value\n\nC++: bdsg::PackedDeque::pop_front() --> void");
		cl.def("pop_back", (void (bdsg::PackedDeque::*)()) &bdsg::PackedDeque::pop_back, "Remove the back value\n\nC++: bdsg::PackedDeque::pop_back() --> void");
		cl.def("reserve", (void (bdsg::PackedDeque::*)(const unsigned long &)) &bdsg::PackedDeque::reserve, "If necessary, expand capacity so that the given number of entries can\n be included in the deque without reallocating. Never shrinks capacity.\n\nC++: bdsg::PackedDeque::reserve(const unsigned long &) --> void", pybind11::arg("future_size"));
		cl.def("size", (unsigned long (bdsg::PackedDeque::*)() const) &bdsg::PackedDeque::size, "Returns the number of values\n\nC++: bdsg::PackedDeque::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedDeque::*)() const) &bdsg::PackedDeque::empty, "Returns true if there are no entries and false otherwise\n\nC++: bdsg::PackedDeque::empty() const --> bool");
		cl.def("clear", (void (bdsg::PackedDeque::*)()) &bdsg::PackedDeque::clear, "Empty the contents\n\nC++: bdsg::PackedDeque::clear() --> void");
		cl.def("memory_usage", (unsigned long (bdsg::PackedDeque::*)() const) &bdsg::PackedDeque::memory_usage, "Reports the amount of memory consumed by this object in bytes.\n\nC++: bdsg::PackedDeque::memory_usage() const --> unsigned long");
	}
	{ // bdsg::PackedSet file:bdsg/internal/packed_structs.hpp line:334
		pybind11::class_<bdsg::PackedSet, std::shared_ptr<bdsg::PackedSet>> cl(M("bdsg"), "PackedSet", "");
		{ // bdsg::PackedSet::iterator file:bdsg/internal/packed_structs.hpp line:379
			auto & enclosing_class = cl;
			pybind11::class_<bdsg::PackedSet::iterator, std::shared_ptr<bdsg::PackedSet::iterator>> cl(enclosing_class, "iterator", "");
			cl.def( pybind11::init( [](bdsg::PackedSet::iterator const &o){ return new bdsg::PackedSet::iterator(o); } ) );
			cl.def("assign", (class bdsg::PackedSet::iterator & (bdsg::PackedSet::iterator::*)(const class bdsg::PackedSet::iterator &)) &bdsg::PackedSet::iterator::operator=, "C++: bdsg::PackedSet::iterator::operator=(const class bdsg::PackedSet::iterator &) --> class bdsg::PackedSet::iterator &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
			cl.def("plus_plus", (class bdsg::PackedSet::iterator & (bdsg::PackedSet::iterator::*)()) &bdsg::PackedSet::iterator::operator++, "C++: bdsg::PackedSet::iterator::operator++() --> class bdsg::PackedSet::iterator &", pybind11::return_value_policy::automatic);
			cl.def("__mul__", (unsigned long (bdsg::PackedSet::iterator::*)() const) &bdsg::PackedSet::iterator::operator*, "C++: bdsg::PackedSet::iterator::operator*() const --> unsigned long");
			cl.def("__eq__", (bool (bdsg::PackedSet::iterator::*)(const class bdsg::PackedSet::iterator &) const) &bdsg::PackedSet::iterator::operator==, "C++: bdsg::PackedSet::iterator::operator==(const class bdsg::PackedSet::iterator &) const --> bool", pybind11::arg("other"));
			cl.def("__ne__", (bool (bdsg::PackedSet::iterator::*)(const class bdsg::PackedSet::iterator &) const) &bdsg::PackedSet::iterator::operator!=, "C++: bdsg::PackedSet::iterator::operator!=(const class bdsg::PackedSet::iterator &) const --> bool", pybind11::arg("other"));
		}

		cl.def( pybind11::init( [](){ return new bdsg::PackedSet(); } ) );
		cl.def( pybind11::init( [](bdsg::PackedSet const &o){ return new bdsg::PackedSet(o); } ) );
		cl.def("assign", (class bdsg::PackedSet & (bdsg::PackedSet::*)(const class bdsg::PackedSet &)) &bdsg::PackedSet::operator=, "Copy assignment operator\n\nC++: bdsg::PackedSet::operator=(const class bdsg::PackedSet &) --> class bdsg::PackedSet &", pybind11::return_value_policy::automatic, pybind11::arg("other"));
		cl.def("insert", (void (bdsg::PackedSet::*)(const unsigned long &)) &bdsg::PackedSet::insert, "Insert a value into the set. Has no effect if the value is already in the set.\n\nC++: bdsg::PackedSet::insert(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("find", (bool (bdsg::PackedSet::*)(const unsigned long &) const) &bdsg::PackedSet::find, "Returns true if the value is in the set, else false.\n\nC++: bdsg::PackedSet::find(const unsigned long &) const --> bool", pybind11::arg("value"));
		cl.def("remove", (void (bdsg::PackedSet::*)(const unsigned long &)) &bdsg::PackedSet::remove, "Remove a value into the set. Has no effect if the value is not in the set.\n\nC++: bdsg::PackedSet::remove(const unsigned long &) --> void", pybind11::arg("value"));
		cl.def("size", (unsigned long (bdsg::PackedSet::*)() const) &bdsg::PackedSet::size, "Returns the number of values in the set\n\nC++: bdsg::PackedSet::size() const --> unsigned long");
		cl.def("empty", (bool (bdsg::PackedSet::*)() const) &bdsg::PackedSet::empty, "Returns the number of values in the set\n\nC++: bdsg::PackedSet::empty() const --> bool");
		cl.def("set_load_factors", (void (bdsg::PackedSet::*)(double, double)) &bdsg::PackedSet::set_load_factors, "Set the minimum and maxium load factors of the hash table\n\nC++: bdsg::PackedSet::set_load_factors(double, double) --> void", pybind11::arg("min_load_factor"), pybind11::arg("max_load_factor"));
		cl.def("max_load_factor", (double (bdsg::PackedSet::*)() const) &bdsg::PackedSet::max_load_factor, "Get the maximum load factor of the hash table\n\nC++: bdsg::PackedSet::max_load_factor() const --> double");
		cl.def("min_load_factor", (double (bdsg::PackedSet::*)() const) &bdsg::PackedSet::min_load_factor, "Get the minimum load factor of the hash table\n\nC++: bdsg::PackedSet::min_load_factor() const --> double");
		cl.def("begin", (class bdsg::PackedSet::iterator (bdsg::PackedSet::*)() const) &bdsg::PackedSet::begin, "Iterator to the first item in the set\n\nC++: bdsg::PackedSet::begin() const --> class bdsg::PackedSet::iterator");
		cl.def("end", (class bdsg::PackedSet::iterator (bdsg::PackedSet::*)() const) &bdsg::PackedSet::end, "Iterator to the past-the-last item in the set\n\nC++: bdsg::PackedSet::end() const --> class bdsg::PackedSet::iterator");
	}
}
