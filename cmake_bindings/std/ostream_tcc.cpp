#include <cwchar>
#include <ios>
#include <istream>
#include <locale>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

void bind_std_ostream_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::basic_ostream file:bits/ostream.tcc line:359
		pybind11::class_<std::ostream, std::shared_ptr<std::ostream>> cl(M("std"), "ostream", "");
		{ // std::basic_ostream<char, std::char_traits<char> >::sentry file:ostream line:96
			auto & enclosing_class = cl;
			pybind11::class_<std::basic_ostream<char, std::char_traits<char> >::sentry, std::shared_ptr<std::basic_ostream<char, std::char_traits<char> >::sentry>> cl(enclosing_class, "sentry", "");
			cl.def( pybind11::init<std::ostream &>(), pybind11::arg("__os") );

		}

		cl.def( pybind11::init<class std::basic_streambuf<char> *>(), pybind11::arg("__sb") );

		cl.def("put", (std::ostream & (std::ostream::*)(char)) &std::basic_ostream<char, std::char_traits<char> >::put, "C++: std::basic_ostream<char, std::char_traits<char> >::put(char) --> std::ostream &", pybind11::return_value_policy::automatic, pybind11::arg("__c"));
		cl.def("_M_write", (void (std::ostream::*)(const char *, long)) &std::basic_ostream<char, std::char_traits<char> >::_M_write, "C++: std::basic_ostream<char, std::char_traits<char> >::_M_write(const char *, long) --> void", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("write", (std::ostream & (std::ostream::*)(const char *, long)) &std::basic_ostream<char, std::char_traits<char> >::write, "C++: std::basic_ostream<char, std::char_traits<char> >::write(const char *, long) --> std::ostream &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("flush", (std::ostream & (std::ostream::*)()) &std::basic_ostream<char, std::char_traits<char> >::flush, "C++: std::basic_ostream<char, std::char_traits<char> >::flush() --> std::ostream &", pybind11::return_value_policy::automatic);
		cl.def("tellp", (class std::fpos<__mbstate_t> (std::ostream::*)()) &std::basic_ostream<char, std::char_traits<char> >::tellp, "C++: std::basic_ostream<char, std::char_traits<char> >::tellp() --> class std::fpos<__mbstate_t>");
		cl.def("seekp", (std::ostream & (std::ostream::*)(class std::fpos<__mbstate_t>)) &std::basic_ostream<char, std::char_traits<char> >::seekp, "C++: std::basic_ostream<char, std::char_traits<char> >::seekp(class std::fpos<__mbstate_t>) --> std::ostream &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("seekp", (std::ostream & (std::ostream::*)(long, enum std::_Ios_Seekdir)) &std::basic_ostream<char, std::char_traits<char> >::seekp, "C++: std::basic_ostream<char, std::char_traits<char> >::seekp(long, enum std::_Ios_Seekdir) --> std::ostream &", pybind11::return_value_policy::automatic, pybind11::arg(""), pybind11::arg(""));
	}
	{ // std::basic_istream file:bits/istream.tcc line:1048
		pybind11::class_<std::istream, std::shared_ptr<std::istream>> cl(M("std"), "istream", "");
		{ // std::basic_istream<char, std::char_traits<char> >::sentry file:istream line:107
			auto & enclosing_class = cl;
			pybind11::class_<std::basic_istream<char, std::char_traits<char> >::sentry, std::shared_ptr<std::basic_istream<char, std::char_traits<char> >::sentry>> cl(enclosing_class, "sentry", "");
			cl.def( pybind11::init( [](class std::basic_istream<char> & a0){ return new std::basic_istream<char, std::char_traits<char> >::sentry(a0); } ), "doc" , pybind11::arg("__is"));
			cl.def( pybind11::init<class std::basic_istream<char> &, bool>(), pybind11::arg("__is"), pybind11::arg("__noskipws") );

		}

		cl.def( pybind11::init<class std::basic_streambuf<char> *>(), pybind11::arg("__sb") );

		cl.def("gcount", (long (std::istream::*)() const) &std::basic_istream<char, std::char_traits<char> >::gcount, "C++: std::basic_istream<char, std::char_traits<char> >::gcount() const --> long");
		cl.def("get", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get() --> int");
		cl.def("get", (class std::basic_istream<char> & (std::istream::*)(char &)) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get(char &) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__c"));
		cl.def("get", (class std::basic_istream<char> & (std::istream::*)(char *, long, char)) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get(char *, long, char) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("get", (class std::basic_istream<char> & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get(char *, long) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("get", (class std::basic_istream<char> & (std::istream::*)(class std::basic_streambuf<char> &, char)) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get(class std::basic_streambuf<char> &, char) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__sb"), pybind11::arg("__delim"));
		cl.def("get", (class std::basic_istream<char> & (std::istream::*)(class std::basic_streambuf<char> &)) &std::basic_istream<char, std::char_traits<char> >::get, "C++: std::basic_istream<char, std::char_traits<char> >::get(class std::basic_streambuf<char> &) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__sb"));
		cl.def("getline", (class std::basic_istream<char> & (std::istream::*)(char *, long, char)) &std::basic_istream<char, std::char_traits<char> >::getline, "C++: std::basic_istream<char, std::char_traits<char> >::getline(char *, long, char) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("getline", (class std::basic_istream<char> & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char> >::getline, "C++: std::basic_istream<char, std::char_traits<char> >::getline(char *, long) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("ignore", (class std::basic_istream<char> & (std::istream::*)(long, int)) &std::basic_istream<char, std::char_traits<char> >::ignore, "C++: std::basic_istream<char, std::char_traits<char> >::ignore(long, int) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("ignore", (class std::basic_istream<char> & (std::istream::*)(long)) &std::basic_istream<char, std::char_traits<char> >::ignore, "C++: std::basic_istream<char, std::char_traits<char> >::ignore(long) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__n"));
		cl.def("ignore", (class std::basic_istream<char> & (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::ignore, "C++: std::basic_istream<char, std::char_traits<char> >::ignore() --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic);
		cl.def("peek", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::peek, "C++: std::basic_istream<char, std::char_traits<char> >::peek() --> int");
		cl.def("read", (class std::basic_istream<char> & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char> >::read, "C++: std::basic_istream<char, std::char_traits<char> >::read(char *, long) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("readsome", (long (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char> >::readsome, "C++: std::basic_istream<char, std::char_traits<char> >::readsome(char *, long) --> long", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("putback", (class std::basic_istream<char> & (std::istream::*)(char)) &std::basic_istream<char, std::char_traits<char> >::putback, "C++: std::basic_istream<char, std::char_traits<char> >::putback(char) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg("__c"));
		cl.def("unget", (class std::basic_istream<char> & (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::unget, "C++: std::basic_istream<char, std::char_traits<char> >::unget() --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic);
		cl.def("sync", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::sync, "C++: std::basic_istream<char, std::char_traits<char> >::sync() --> int");
		cl.def("tellg", (class std::fpos<__mbstate_t> (std::istream::*)()) &std::basic_istream<char, std::char_traits<char> >::tellg, "C++: std::basic_istream<char, std::char_traits<char> >::tellg() --> class std::fpos<__mbstate_t>");
		cl.def("seekg", (class std::basic_istream<char> & (std::istream::*)(class std::fpos<__mbstate_t>)) &std::basic_istream<char, std::char_traits<char> >::seekg, "C++: std::basic_istream<char, std::char_traits<char> >::seekg(class std::fpos<__mbstate_t>) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		cl.def("seekg", (class std::basic_istream<char> & (std::istream::*)(long, enum std::_Ios_Seekdir)) &std::basic_istream<char, std::char_traits<char> >::seekg, "C++: std::basic_istream<char, std::char_traits<char> >::seekg(long, enum std::_Ios_Seekdir) --> class std::basic_istream<char> &", pybind11::return_value_policy::automatic, pybind11::arg(""), pybind11::arg(""));
	}
}
