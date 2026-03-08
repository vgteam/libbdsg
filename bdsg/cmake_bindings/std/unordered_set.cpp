#include <functional>
#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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

void bind_std_unordered_set(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::unordered_set file:bits/unordered_set.h line:104
		pybind11::class_<std::unordered_set<handlegraph::PathSense>, std::shared_ptr<std::unordered_set<handlegraph::PathSense>>> cl(M("std"), "unordered_set_handlegraph_PathSense_t", "");
		cl.def( pybind11::init( [](){ return new std::unordered_set<handlegraph::PathSense>(); } ) );
		cl.def( pybind11::init( [](unsigned long const & a0){ return new std::unordered_set<handlegraph::PathSense>(a0); } ), "doc" , pybind11::arg("__n"));
		cl.def( pybind11::init( [](unsigned long const & a0, const struct std::hash<enum handlegraph::PathSense> & a1){ return new std::unordered_set<handlegraph::PathSense>(a0, a1); } ), "doc" , pybind11::arg("__n"), pybind11::arg("__hf"));
		cl.def( pybind11::init( [](unsigned long const & a0, const struct std::hash<enum handlegraph::PathSense> & a1, const struct std::equal_to<enum handlegraph::PathSense> & a2){ return new std::unordered_set<handlegraph::PathSense>(a0, a1, a2); } ), "doc" , pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__eql"));
		cl.def( pybind11::init<unsigned long, const struct std::hash<enum handlegraph::PathSense> &, const struct std::equal_to<enum handlegraph::PathSense> &, const class std::allocator<enum handlegraph::PathSense> &>(), pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__eql"), pybind11::arg("__a") );

		cl.def( pybind11::init( [](std::unordered_set<handlegraph::PathSense> const &o){ return new std::unordered_set<handlegraph::PathSense>(o); } ) );
		cl.def( pybind11::init<const class std::allocator<enum handlegraph::PathSense> &>(), pybind11::arg("__a") );

		cl.def( pybind11::init<const class std::unordered_set<enum handlegraph::PathSense> &, const class std::allocator<enum handlegraph::PathSense> &>(), pybind11::arg("__uset"), pybind11::arg("__a") );

		cl.def( pybind11::init<unsigned long, const class std::allocator<enum handlegraph::PathSense> &>(), pybind11::arg("__n"), pybind11::arg("__a") );

		cl.def( pybind11::init<unsigned long, const struct std::hash<enum handlegraph::PathSense> &, const class std::allocator<enum handlegraph::PathSense> &>(), pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__a") );

		cl.def("assign", (class std::unordered_set<enum handlegraph::PathSense> & (std::unordered_set<handlegraph::PathSense>::*)(const class std::unordered_set<enum handlegraph::PathSense> &)) &std::unordered_set<handlegraph::PathSense>::operator=, "C++: std::unordered_set<handlegraph::PathSense>::operator=(const class std::unordered_set<enum handlegraph::PathSense> &) --> class std::unordered_set<enum handlegraph::PathSense> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("get_allocator", (class std::allocator<enum handlegraph::PathSense> (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::get_allocator, "C++: std::unordered_set<handlegraph::PathSense>::get_allocator() const --> class std::allocator<enum handlegraph::PathSense>");
		cl.def("empty", (bool (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::empty, "C++: std::unordered_set<handlegraph::PathSense>::empty() const --> bool");
		cl.def("size", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::size, "C++: std::unordered_set<handlegraph::PathSense>::size() const --> unsigned long");
		cl.def("max_size", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::max_size, "C++: std::unordered_set<handlegraph::PathSense>::max_size() const --> unsigned long");
		cl.def("insert", (struct std::pair<struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false>, bool> (std::unordered_set<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &)) &std::unordered_set<handlegraph::PathSense>::insert, "C++: std::unordered_set<handlegraph::PathSense>::insert(const enum handlegraph::PathSense &) --> struct std::pair<struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false>, bool>", pybind11::arg("__x"));
		cl.def("erase", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &)) &std::unordered_set<handlegraph::PathSense>::erase, "C++: std::unordered_set<handlegraph::PathSense>::erase(const enum handlegraph::PathSense &) --> unsigned long", pybind11::arg("__x"));
		cl.def("clear", (void (std::unordered_set<handlegraph::PathSense>::*)()) &std::unordered_set<handlegraph::PathSense>::clear, "C++: std::unordered_set<handlegraph::PathSense>::clear() --> void");
		cl.def("swap", (void (std::unordered_set<handlegraph::PathSense>::*)(class std::unordered_set<enum handlegraph::PathSense> &)) &std::unordered_set<handlegraph::PathSense>::swap, "C++: std::unordered_set<handlegraph::PathSense>::swap(class std::unordered_set<enum handlegraph::PathSense> &) --> void", pybind11::arg("__x"));
		cl.def("hash_function", (struct std::hash<enum handlegraph::PathSense> (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::hash_function, "C++: std::unordered_set<handlegraph::PathSense>::hash_function() const --> struct std::hash<enum handlegraph::PathSense>");
		cl.def("key_eq", (struct std::equal_to<enum handlegraph::PathSense> (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::key_eq, "C++: std::unordered_set<handlegraph::PathSense>::key_eq() const --> struct std::equal_to<enum handlegraph::PathSense>");
		cl.def("count", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &) const) &std::unordered_set<handlegraph::PathSense>::count, "C++: std::unordered_set<handlegraph::PathSense>::count(const enum handlegraph::PathSense &) const --> unsigned long", pybind11::arg("__x"));
		cl.def("equal_range", (struct std::pair<struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false>, struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false> > (std::unordered_set<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &)) &std::unordered_set<handlegraph::PathSense>::equal_range, "C++: std::unordered_set<handlegraph::PathSense>::equal_range(const enum handlegraph::PathSense &) --> struct std::pair<struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false>, struct std::__detail::_Node_iterator<enum handlegraph::PathSense, true, false> >", pybind11::arg("__x"));
		cl.def("bucket_count", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::bucket_count, "C++: std::unordered_set<handlegraph::PathSense>::bucket_count() const --> unsigned long");
		cl.def("max_bucket_count", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::max_bucket_count, "C++: std::unordered_set<handlegraph::PathSense>::max_bucket_count() const --> unsigned long");
		cl.def("bucket_size", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)(unsigned long) const) &std::unordered_set<handlegraph::PathSense>::bucket_size, "C++: std::unordered_set<handlegraph::PathSense>::bucket_size(unsigned long) const --> unsigned long", pybind11::arg("__n"));
		cl.def("bucket", (unsigned long (std::unordered_set<handlegraph::PathSense>::*)(const enum handlegraph::PathSense &) const) &std::unordered_set<handlegraph::PathSense>::bucket, "C++: std::unordered_set<handlegraph::PathSense>::bucket(const enum handlegraph::PathSense &) const --> unsigned long", pybind11::arg("__key"));
		cl.def("load_factor", (float (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::load_factor, "C++: std::unordered_set<handlegraph::PathSense>::load_factor() const --> float");
		cl.def("max_load_factor", (float (std::unordered_set<handlegraph::PathSense>::*)() const) &std::unordered_set<handlegraph::PathSense>::max_load_factor, "C++: std::unordered_set<handlegraph::PathSense>::max_load_factor() const --> float");
		cl.def("max_load_factor", (void (std::unordered_set<handlegraph::PathSense>::*)(float)) &std::unordered_set<handlegraph::PathSense>::max_load_factor, "C++: std::unordered_set<handlegraph::PathSense>::max_load_factor(float) --> void", pybind11::arg("__z"));
		cl.def("rehash", (void (std::unordered_set<handlegraph::PathSense>::*)(unsigned long)) &std::unordered_set<handlegraph::PathSense>::rehash, "C++: std::unordered_set<handlegraph::PathSense>::rehash(unsigned long) --> void", pybind11::arg("__n"));
		cl.def("reserve", (void (std::unordered_set<handlegraph::PathSense>::*)(unsigned long)) &std::unordered_set<handlegraph::PathSense>::reserve, "C++: std::unordered_set<handlegraph::PathSense>::reserve(unsigned long) --> void", pybind11::arg("__n"));
	}
	{ // std::unordered_set file:bits/unordered_set.h line:104
		pybind11::class_<std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>, std::shared_ptr<std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>>> cl(M("std"), "unordered_set_std_string_std_hash_std_string_std_equal_to_std_string_std_allocator_std_string_t", "");
		cl.def( pybind11::init( [](){ return new std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>(); } ) );
		cl.def( pybind11::init( [](unsigned long const & a0){ return new std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>(a0); } ), "doc" , pybind11::arg("__n"));
		cl.def( pybind11::init( [](unsigned long const & a0, const struct std::hash<std::string > & a1){ return new std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>(a0, a1); } ), "doc" , pybind11::arg("__n"), pybind11::arg("__hf"));
		cl.def( pybind11::init( [](unsigned long const & a0, const struct std::hash<std::string > & a1, const struct std::equal_to<std::string > & a2){ return new std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>(a0, a1, a2); } ), "doc" , pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__eql"));
		cl.def( pybind11::init<unsigned long, const struct std::hash<std::string > &, const struct std::equal_to<std::string > &, const class std::allocator<std::string > &>(), pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__eql"), pybind11::arg("__a") );

		cl.def( pybind11::init( [](std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >> const &o){ return new std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>(o); } ) );
		cl.def( pybind11::init<const class std::allocator<std::string > &>(), pybind11::arg("__a") );

		cl.def( pybind11::init<const class std::unordered_set<std::string > &, const class std::allocator<std::string > &>(), pybind11::arg("__uset"), pybind11::arg("__a") );

		cl.def( pybind11::init<unsigned long, const class std::allocator<std::string > &>(), pybind11::arg("__n"), pybind11::arg("__a") );

		cl.def( pybind11::init<unsigned long, const struct std::hash<std::string > &, const class std::allocator<std::string > &>(), pybind11::arg("__n"), pybind11::arg("__hf"), pybind11::arg("__a") );

		cl.def("assign", (class std::unordered_set<std::string > & (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const class std::unordered_set<std::string > &)) &std::unordered_set<std::string>::operator=, "C++: std::unordered_set<std::string>::operator=(const class std::unordered_set<std::string > &) --> class std::unordered_set<std::string > &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("get_allocator", (class std::allocator<std::string > (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::get_allocator, "C++: std::unordered_set<std::string>::get_allocator() const --> class std::allocator<std::string >");
		cl.def("empty", (bool (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::empty, "C++: std::unordered_set<std::string>::empty() const --> bool");
		cl.def("size", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::size, "C++: std::unordered_set<std::string>::size() const --> unsigned long");
		cl.def("max_size", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::max_size, "C++: std::unordered_set<std::string>::max_size() const --> unsigned long");
		cl.def("insert", (struct std::pair<struct std::__detail::_Node_iterator<std::string, true, true>, bool> (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const std::string &)) &std::unordered_set<std::string>::insert, "C++: std::unordered_set<std::string>::insert(const std::string &) --> struct std::pair<struct std::__detail::_Node_iterator<std::string, true, true>, bool>", pybind11::arg("__x"));
		cl.def("erase", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const std::string &)) &std::unordered_set<std::string>::erase, "C++: std::unordered_set<std::string>::erase(const std::string &) --> unsigned long", pybind11::arg("__x"));
		cl.def("clear", (void (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)()) &std::unordered_set<std::string>::clear, "C++: std::unordered_set<std::string>::clear() --> void");
		cl.def("swap", (void (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(class std::unordered_set<std::string > &)) &std::unordered_set<std::string>::swap, "C++: std::unordered_set<std::string>::swap(class std::unordered_set<std::string > &) --> void", pybind11::arg("__x"));
		cl.def("hash_function", (struct std::hash<std::string > (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::hash_function, "C++: std::unordered_set<std::string>::hash_function() const --> struct std::hash<std::string >");
		cl.def("key_eq", (struct std::equal_to<std::string > (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::key_eq, "C++: std::unordered_set<std::string>::key_eq() const --> struct std::equal_to<std::string >");
		cl.def("count", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const std::string &) const) &std::unordered_set<std::string>::count, "C++: std::unordered_set<std::string>::count(const std::string &) const --> unsigned long", pybind11::arg("__x"));
		cl.def("equal_range", (struct std::pair<struct std::__detail::_Node_iterator<std::string, true, true>, struct std::__detail::_Node_iterator<std::string, true, true> > (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const std::string &)) &std::unordered_set<std::string>::equal_range, "C++: std::unordered_set<std::string>::equal_range(const std::string &) --> struct std::pair<struct std::__detail::_Node_iterator<std::string, true, true>, struct std::__detail::_Node_iterator<std::string, true, true> >", pybind11::arg("__x"));
		cl.def("bucket_count", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::bucket_count, "C++: std::unordered_set<std::string>::bucket_count() const --> unsigned long");
		cl.def("max_bucket_count", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::max_bucket_count, "C++: std::unordered_set<std::string>::max_bucket_count() const --> unsigned long");
		cl.def("bucket_size", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(unsigned long) const) &std::unordered_set<std::string>::bucket_size, "C++: std::unordered_set<std::string>::bucket_size(unsigned long) const --> unsigned long", pybind11::arg("__n"));
		cl.def("bucket", (unsigned long (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(const std::string &) const) &std::unordered_set<std::string>::bucket, "C++: std::unordered_set<std::string>::bucket(const std::string &) const --> unsigned long", pybind11::arg("__key"));
		cl.def("load_factor", (float (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::load_factor, "C++: std::unordered_set<std::string>::load_factor() const --> float");
		cl.def("max_load_factor", (float (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)() const) &std::unordered_set<std::string>::max_load_factor, "C++: std::unordered_set<std::string>::max_load_factor() const --> float");
		cl.def("max_load_factor", (void (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(float)) &std::unordered_set<std::string>::max_load_factor, "C++: std::unordered_set<std::string>::max_load_factor(float) --> void", pybind11::arg("__z"));
		cl.def("rehash", (void (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(unsigned long)) &std::unordered_set<std::string>::rehash, "C++: std::unordered_set<std::string>::rehash(unsigned long) --> void", pybind11::arg("__n"));
		cl.def("reserve", (void (std::unordered_set<std::string,std::hash<std::string >,std::equal_to<std::string >,std::allocator<std::string >>::*)(unsigned long)) &std::unordered_set<std::string>::reserve, "C++: std::unordered_set<std::string>::reserve(unsigned long) --> void", pybind11::arg("__n"));
	}
}
