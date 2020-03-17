#ifndef BDSG_UTILITY_HPP_INCLUDED
#define BDSG_UTILITY_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

namespace bdsg {

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

inline int dna_as_int(char c) {
    switch (c) {
        case 'A':
            return 1;
        case 'T':
            return 2;
        case 'C':
            return 3;
        case 'G':
            return 4;
        default:
            return 5;
    }
}
    
inline char int_as_dna(int i) {
    switch (i) {
        case 1:
            return 'A';
        case 2:
            return 'T';
        case 3:
            return 'C';
        case 4:
            return 'G';
        default:
            return 'N';
    }
}
    
// Convert a quantity in bytes to a human-friendly string
string format_memory(size_t s);

/// Return the number of threads that OMP will produce for a parallel section.
/// TODO: Assumes that this is the same for every parallel section.
int get_thread_count(void);

}

#endif
