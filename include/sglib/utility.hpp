#ifndef SGLIB_UTILITY_HPP_INCLUDED
#define SGLIB_UTILITY_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>

namespace sglib {

using namespace std;

char reverse_complement(const char& c);
string reverse_complement(const string& seq);
void reverse_complement_in_place(string& seq);
/// Return True if the given string is entirely Ns of either case, and false
/// otherwise.
bool is_all_n(const string& seq);

bool allATGC(const string& s);
bool allATGCN(const string& s);
string nonATGCNtoN(const string& s);
// Convert ASCII-encoded DNA to upper case
string toUppercase(const string& s);
// Convert a quantity in bytes to a human-friendly string
string format_memory(size_t s);

}

#endif
