//
//  packed_structs.hpp
//  
// Contains implementations of classic data structures converted into
// bit-packed integer vectors
//

#ifndef BDSG_PACKED_STRUCTS_HPP_INCLUDED
#define BDSG_PACKED_STRUCTS_HPP_INCLUDED

#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <sdsl/int_vector.hpp>

namespace bdsg {
    
using namespace std;
    
/*
 * A dynamic integer vector that maintains integers in bit-compressed form.
 * Automatically adjusts bit-width for entries depending on input data.
 */
class PackedVector {
public:
    /// Constructor (starts empty)
    PackedVector();
    
    /// Construct from contents in a stream
    PackedVector(istream& in);
    
    /// Move constructor
    PackedVector(PackedVector&& other) = default;
    /// Move assignment operator
    PackedVector& operator=(PackedVector&& other) = default;
    
    /// Copy constructor
    PackedVector(const PackedVector& other) = default;
    /// Copy assignment operator
    PackedVector& operator=(const PackedVector& other) = default;
        
    /// Destructor
    ~PackedVector();
    
    /// Clear current contents and load from contents in a stream
    void deserialize(istream& in);
    
    /// Output contents to a stream
    void serialize(ostream& out) const ;
    
    /// Set the i-th value
    inline void set(const size_t& i, const uint64_t& value);
        
    /// Returns the i-th value
    inline uint64_t get(const size_t& i) const;
        
    /// Add a value to the end
    inline void append(const uint64_t& value);
        
    /// Remove the last value
    inline void pop();
    
    /// Either shrink the vector or grow the vector to the new size. New
    /// entries created by growing are filled with 0.
    inline void resize(const size_t& new_size);
    
    /// If necessary, expand capacity so that the given number of entries can
    /// be included in the vector without reallocating. Never shrinks capacity.
    inline void reserve(const size_t& future_size);
        
    /// Returns the number of values.
    inline size_t size() const;
        
    /// Returns true if there are no entries and false otherwise.
    inline bool empty() const;

    /// Clears the backing vector.
    inline void clear();
    
    /// Reports the amount of memory consumed by this object in bytes.
    size_t memory_usage() const;
    
    /// Returns true if the contents are identical (but not necessarily storage
    /// parameters, such as pointer to data, capacity, bit width, etc.).
    inline bool operator==(const PackedVector& other) const;
        
private:
        
    // the underlying vector representation
    sdsl::int_vector<> vec;
    // tracker for number of values
    size_t filled = 0;
    // geometric expansion factor
    static const double factor;
};

/*
 * A dynamic integer vector that provides better compression when values in the
 * integer vector either 1) do not vary much from their neighbors or 2) are 0.
 * Compression is also optimized for vectors that are mostly (but not necessarily
 * exclusively) increasing.
 */
class PagedVector {
public:
    
    /// Construct and set page size (starts empty)
    PagedVector(size_t page_size);
    
    /// Construct from contents in a stream
    PagedVector(istream& in);
    
    /// Move constructor
    PagedVector(PagedVector&& other) = default;
    /// Move assignment operator
    PagedVector& operator=(PagedVector&& other) = default;
    
    // Destructor
    ~PagedVector();
    
    /// Clear current contents and load from contents in a stream
    void deserialize(istream& in);
    
    /// Output contents to a stream
    void serialize(ostream& out) const ;
    
    /// Set the i-th value
    inline void set(const size_t& i, const uint64_t& value);
    
    /// Returns the i-th value
    inline uint64_t get(const size_t& i) const;
    
    /// Add a value to the end
    inline void append(const uint64_t& value);
    
    /// Remove the last value
    inline void pop();
    
    /// Either shrink the vector or grow the vector to the new size. New
    /// entries created by growing are filled with 0.
    inline void resize(const size_t& new_size);
    
    /// If necessary, expand capacity so that the given number of entries can
    /// be included in the vector without reallocating. Never shrinks capacity.
    inline void reserve(const size_t& future_size);
    
    /// Returns the number of values
    inline size_t size() const;
    
    /// Returns true if there are no entries and false otherwise
    inline bool empty() const;
    
    /// Clears the backing vector
    inline void clear();
    
    /// Returns the page width of the vector
    inline size_t page_width() const;
    
    /// Reports the amount of memory consumed by this object in bytes
    size_t memory_usage() const;
    
private:
    
    PagedVector();
    
    inline uint64_t to_diff(const uint64_t& value, const uint64_t& page) const;
    inline uint64_t from_diff(const uint64_t& diff, const uint64_t& page) const;
    
    // TODO: is there a way to const this and still allow copy/move constructors?
    size_t page_size = 64;
    
    // The number of entries filled so far
    size_t filled = 0;
    
    // Evenly spaced entries from the vector
    PackedVector anchors;
    // All entries in the vector expressed as a difference from the preceding page value
    vector<PackedVector> pages;
};

/*
 * A dynamic integer vector with similar compression properties to the PagedVector,
 * but better memory usage if the vector may be very small (relative to the page
 * size.
 */
class RobustPagedVector {
public:
    /// Construct and set page size (starts empty)
    RobustPagedVector(size_t page_size);
    
    /// Construct from contents in a stream
    RobustPagedVector(istream& in);
    
    /// Move constructor
    RobustPagedVector(RobustPagedVector&& other) = default;
    /// Move assignment operator
    RobustPagedVector& operator=(RobustPagedVector&& other) = default;
    
    // Destructor
    ~RobustPagedVector();
    
    /// Clear current contents and load from contents in a stream
    void deserialize(istream& in);
    
    /// Output contents to a stream
    void serialize(ostream& out) const;
    
    /// Set the i-th value
    inline void set(const size_t& i, const uint64_t& value);
    
    /// Returns the i-th value
    inline uint64_t get(const size_t& i) const;
    
    /// Add a value to the end
    inline void append(const uint64_t& value);
    
    /// Remove the last value
    inline void pop();
    
    /// Either shrink the vector or grow the vector to the new size. New
    /// entries created by growing are filled with 0.
    inline void resize(const size_t& new_size);
    
    /// If necessary, expand capacity so that the given number of entries can
    /// be included in the vector without reallocating. Never shrinks capacity.
    inline void reserve(const size_t& future_size);
    
    /// Returns the number of values
    inline size_t size() const;
    
    /// Returns true if there are no entries and false otherwise
    inline bool empty() const;
    
    /// Clears the backing vector
    inline void clear();
    
    /// Returns the page width of the vector
    inline size_t page_width() const;
    
    /// Reports the amount of memory consumed by this object in bytes
    size_t memory_usage() const;
    
private:
    
    RobustPagedVector();
    
    /// The first page_size entries go in this vector
    PackedVector first_page;
    
    /// All entries beyond page_size go in this vector
    PagedVector latter_pages;
};

/*
 * A deque implementation that maintains integers in bit-compressed form, with the bit
 * width automatically adjusted to the entries.
 */
class PackedDeque {
public:
    /// Construct empty
    PackedDeque(void);
    /// Construct from contents in a stream
    PackedDeque(istream& in);
    
    /// Move constructor
    PackedDeque(PackedDeque&& other) = default;
    /// Move assignment operator
    PackedDeque& operator=(PackedDeque&& other) = default;
    
    /// Destructor
    ~PackedDeque(void);
    
    /// Clear current contents and load from contents in a stream
    void deserialize(istream& in);
    
    /// Output contents to a stream
    void serialize(ostream& out) const ;
    
    /// Set the i-th value
    inline void set(const size_t& i, const uint64_t& value);
    
    /// Returns the i-th value
    inline uint64_t get(const size_t& i) const;
    
    /// Add a value to the front
    inline void append_front(const uint64_t& value);
    
    /// Add a value to the back
    inline void append_back(const uint64_t& value);
    
    /// Remove the front value
    inline void pop_front();
    
    /// Remove the back value
    inline void pop_back();
    
    /// If necessary, expand capacity so that the given number of entries can
    /// be included in the deque without reallocating. Never shrinks capacity.
    inline void reserve(const size_t& future_size);
    
    /// Returns the number of values
    inline size_t size() const;
    
    /// Returns true if there are no entries and false otherwise
    inline bool empty() const;
    
    /// Empty the contents
    inline void clear();
    
    /// Reports the amount of memory consumed by this object in bytes.
    size_t memory_usage() const;
    
private:
    
    inline void contract();
    
    inline size_t internal_index(const size_t& i) const;
    
    PackedVector vec;
    
    size_t begin_idx = 0;
    size_t filled = 0;
    static const double factor;
};
    
/*
 * A hash set that maintains integers in bit-compressed form, with the bit
 * width automatically adjusted to the entries. It is designed to have the
 * best compression when the values are similar to each other in scale,
 * except possibly for 0, which can be used as a sentinel.
 */
class PackedSet {
public:
    
    /// Constructor
    PackedSet();
    /// Desctructor
    ~PackedSet() = default;
    
    /// Move constructor
    PackedSet(PackedSet&& other) = default;
    /// Move assignment operator
    PackedSet& operator=(PackedSet&& other) = default;
    
    /// Copy constructor
    PackedSet(const PackedSet& other) = default;
    /// Copy assignment operator
    PackedSet& operator=(const PackedSet& other) = default;
    
    /// Forward declaration
    class iterator;
    
    /// Insert a value into the set. Has no effect if the value is already in the set.
    inline void insert(const uint64_t& value);
    
    /// Returns true if the value is in the set, else false.
    inline bool find(const uint64_t& value) const;
    
    /// Remove a value into the set. Has no effect if the value is not in the set.
    inline void remove(const uint64_t& value);
    
    /// Returns the number of values in the set
    inline size_t size() const;
    
    /// Returns the number of values in the set
    inline bool empty() const;
    
    /// Set the minimum and maxium load factors of the hash table
    inline void set_load_factors(double min_load_factor, double max_load_factor);
    
    /// Get the maximum load factor of the hash table
    inline double max_load_factor() const;
    
    /// Get the minimum load factor of the hash table
    inline double min_load_factor() const;
    
    /// Iterator to the first item in the set
    iterator begin() const;
    
    /// Iterator to the past-the-last item in the set
    iterator end() const;
    
    /*
     * An iterator class for the PackedSet
     */
    class iterator{
    public:
        iterator(const iterator& other) = default;
        iterator() = delete;
        ~iterator() = default;
        iterator& operator=(const iterator& other) = default;
        iterator& operator++();
        uint64_t operator*() const;
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        
    private:
        
        iterator(const PackedSet* iteratee);
        iterator(const PackedSet* iteratee, size_t i);
        
        const PackedSet* iteratee;
        
        // the index in the hash table
        size_t i = 0;
        
        friend class PackedSet;
    };
    
private:
    
    /// Internal function that returns the index of either either the null seninel
    /// or the diff, whichever comes first in linear probing
    inline size_t locate(const uint64_t& diff, const PackedVector& _table) const;
    
    /// Move up or down to the next size in the size schedule and rehash entries
    void rehash(bool shrink);
    
    /// Execute hash function for a given table
    inline size_t hash(const uint64_t& diff, const PackedVector& table) const;
    
    /// Convert a value to a difference from an anchor
    inline uint64_t to_diff(const uint64_t& value, const uint64_t& _anchor) const;
    
    /// Convert a difference from an anchor to a value
    inline uint64_t from_diff(const uint64_t& diff, const uint64_t& _anchor) const;
    
    /// Compute the optimal anchor for the current table entries
    inline uint64_t optimal_anchor() const;
    
    /// The table where the entries are stored
    PackedVector table;
    
    /// PRNG used to generate universal hash functions
    default_random_engine gen;
    
    /// A value that we greedily choose from the input to anchor differences
    uint64_t anchor = 0;
        
    /// Coefficients of a degree 4 polynomial over Z_p
    size_t coefs[5] = {0, 0, 0, 0, 0};
    
    /// Index of the current size within the schedule of sizes
    size_t schedule_val = 0;
    
    /// Minimum load factor on the array
    double min_load = 0.33;
    
    /// Maximum load factor on the array
    double max_load = 0.67;
    
    /// Number of items in the set
    size_t num_items = 0;
    
    /// Let the iterator access the internals
    friend class iterator;
};
    
    
/// Inline functions
    
/////////////////////
/// PackedVector
/////////////////////
    
inline void PackedVector::set(const size_t& i, const uint64_t& value) {
    assert(i < filled);
        
    uint8_t width = vec.width();
    uint64_t mask = std::numeric_limits<uint64_t>::max() << width;
    while (mask & value) {
        width++;
        mask = std::numeric_limits<uint64_t>::max() << width;
    }
        
    if (width > vec.width()) {
        sdsl::int_vector<> wider_vec;
        wider_vec.width(width);
        wider_vec.resize(vec.size());
        for (size_t i = 0; i < filled; i++) {
            wider_vec[i] = vec[i];
        }
        vec = std::move(wider_vec);
    }
        
    vec[i] = value;
}
    
inline uint64_t PackedVector::get(const size_t& i) const {
    assert(i < filled);
    return vec[i];
}
    
inline void PackedVector::append(const uint64_t& value) {
    resize(filled + 1);
    set(filled - 1, value);
}
    
inline void PackedVector::pop() {
    resize(filled - 1);
}
    
inline void PackedVector::resize(const size_t& new_size) {
    if (new_size < filled) {
        size_t shrink_capacity = vec.size() / (factor * factor);
        if (new_size < shrink_capacity) {
            sdsl::int_vector<> tmp;
            tmp.width(vec.width());
            tmp.resize(new_size);
            for (size_t i = 0; i < new_size; i++) {
                tmp[i] = vec[i];
            }
            vec = std::move(tmp);
        }
    }
    else if (new_size > vec.size()) {
        size_t old_capacity = vec.size();
        size_t new_capacity = std::max<size_t>(size_t(vec.size() * factor) + 1, new_size);
        reserve(new_capacity);
        for (size_t i = old_capacity; i < vec.size(); i++) {
            vec[i] = 0;
        }
    }
    filled = new_size;
}
    
inline void PackedVector::reserve(const size_t& future_size) {
    if (future_size > vec.size()) {
        sdsl::int_vector<> tmp;
        tmp.width(vec.width());
        tmp.resize(future_size);
        for (size_t i = 0; i < filled; i++) {
            tmp[i] = vec[i];
        }
        vec = std::move(tmp);
    }
}
    
inline size_t PackedVector::size() const {
    return filled;
}
    
inline bool PackedVector::empty() const {
    return filled == 0;
}

inline void PackedVector::clear() {
    vec.resize(0);
    vec.width(1);
    filled = 0;
}
    
inline bool PackedVector::operator==(const PackedVector& other) const {
    if (size() != other.size()) {
        return false;
    }
    for (size_t i = 0; i < size(); ++i) {
        if (get(i) != other.get(i)) {
            return false;
        }
    }
    
    return true;
}
    
/////////////////////
/// PackedDeque
/////////////////////
    
    
inline size_t PackedDeque::internal_index(const size_t& i) const {
    assert(i < filled);
    return i < vec.size() - begin_idx ? begin_idx + i : i - (vec.size() - begin_idx);
}

inline void PackedDeque::set(const size_t& i, const uint64_t& value) {
    return vec.set(internal_index(i), value);
}

inline uint64_t PackedDeque::get(const size_t& i) const {
    return vec.get(internal_index(i));
}
    
inline void PackedDeque::reserve(const size_t& future_size) {
    if (future_size > vec.size()) {
        PackedVector new_vec;
        new_vec.resize(future_size);
        
        for (size_t i = 0; i < filled; i++) {
            new_vec.set(i, get(i));
        }
        vec = std::move(new_vec);
        begin_idx = 0;
    }
}

inline void PackedDeque::append_front(const uint64_t& value) {
    // expand capacity if necessary
    if (filled == vec.size()) {
        size_t new_capacity = size_t(factor * vec.size()) + 1;
        reserve(new_capacity);
    }
    
    // update the pointer to the front
    if (begin_idx == 0) {
        begin_idx = vec.size() - 1;
    }
    else {
        begin_idx--;
    }
    // update the pointer to the back
    filled++;
    
    // set the value
    vec.set(internal_index(0), value);
}

inline void PackedDeque::append_back(const uint64_t& value) {
    // expand capacity if necessary
    if (filled == vec.size()) {
        size_t new_capacity = size_t(factor * vec.size()) + 1;
        reserve(new_capacity);
    }
    
    // update the pointer to the back
    filled++;
    
    // set the value
    vec.set(internal_index(filled - 1), value);
}
    
inline void PackedDeque::contract() {
    size_t shrink_capacity = vec.size() / (factor * factor);
    if (filled <= shrink_capacity) {
        PackedVector new_vec;
        new_vec.resize(filled);
        for (size_t i = 0; i < filled; i++) {
            new_vec.set(i, get(i));
        }
        
        vec = std::move(new_vec);
        begin_idx = 0;
    }
}

inline void PackedDeque::pop_front() {
    // update the pointer to the beginning
    begin_idx++;
    if (begin_idx == vec.size()) {
        begin_idx = 0;
    }
    // update the pointer to the end
    filled--;
    
    // shrink if necessary
    contract();
}

inline void PackedDeque::pop_back() {
    // update the pointer to the end
    filled--;
    
    // shrink if necessary
    contract();
}

inline size_t PackedDeque::size() const {
    return filled;
}

inline bool PackedDeque::empty() const {
    return filled == 0;
}
    
inline void PackedDeque::clear() {
    vec.clear();
    filled = 0;
    begin_idx = 0;
}
    
/////////////////////
/// PagedVector
/////////////////////
    
inline void PagedVector::set(const size_t& i, const uint64_t& value) {
    assert(i < filled);
    uint64_t anchor = anchors.get(i / page_size);
    if (anchor == 0) {
        // this page does not have a non-zero anchor yet, use this one
        anchors.set(i / page_size, value);
        anchor = value;
    }
    pages[i / page_size].set(i % page_size, to_diff(value, anchor));
}

inline uint64_t PagedVector::get(const size_t& i) const {
    assert(i < filled);
    return from_diff(pages[i / page_size].get(i % page_size),
                     anchors.get(i / page_size));
}

inline void PagedVector::append(const uint64_t& value) {
    if (filled == pages.size() * page_size) {
        // init a new page and a new anchor
        pages.emplace_back();
        pages.back().resize(page_size);
        anchors.append(0);
    }
    
    // use the logic in set to choose anchor and diff
    filled++;
    set(filled - 1, value);
}

inline void PagedVector::pop() {
    filled--;
    while (filled + page_size <= pages.size() * page_size) {
        // the final page is unused now, remove it
        pages.pop_back(); // TODO: this won't resize since it's an STL vector
        anchors.pop();
    }
}

inline void PagedVector::resize(const size_t& new_size) {
    if (new_size < filled) {
        // shrink down to the number of pages we would need
        size_t num_pages = new_size == 0 ? 0 : (new_size - 1) / page_size + 1;
        anchors.resize(num_pages);
        pages.resize(num_pages);
    }
    else if (new_size > filled) {
        // make sure we capacity for this many elements
        reserve(new_size);
    }
    filled = new_size;
}
    
inline void PagedVector::reserve(const size_t& future_size) {
    if (future_size > pages.size() * page_size) {
        // how many pages does this require?
        size_t num_pages = (future_size - 1) / page_size + 1;
        // note: we don't need to worry about underflow b/c previous condition
        // implies future_size > 0
        
        // expand anchor and pages vectors out to the capacity of the number of pages
        anchors.reserve(num_pages);
        pages.reserve(num_pages);
        
        // add the anchors and fixed-width pages in this
        anchors.resize(num_pages);
        while (num_pages > pages.size()) {
            pages.emplace_back();
            pages.back().resize(page_size);
        }
    }
}

inline size_t PagedVector::size() const {
    return filled;
}

inline bool PagedVector::empty() const {
    return filled == 0;
}

inline void PagedVector::clear() {
    pages.clear();
    anchors.clear();
    filled = 0;
}
inline size_t PagedVector::page_width() const {
    return page_size;
}
    
inline uint64_t PagedVector::to_diff(const uint64_t& value, const uint64_t& anchor) const {
    // leaves 0 unchanged, encodes other values as a difference from the anchor value
    // with a reversible mapping into the positive integers as follows:
    // difference  0  1  2  3 -1  4  5  6  7 -2  8  9 10 11 -3 ...
    // integer     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ...
    // the goal here is use smaller integers to maintain low bit-width, allowing 0 as
    // a sentinel. the bijection is biased to encode positive differences as smaller bit-
    // width integers since anchors are taken from the beginning of their page in the
    // vector and we expect most vectors to be mostly increasing
    
    if (value == 0) {
        return 0;
    }
    else if (value >= anchor) {
        uint64_t raw_diff = value - anchor;
        return raw_diff + raw_diff / 4 + 1;
    }
    else {
        return 5 * (anchor - value);
    }
}

inline uint64_t PagedVector::from_diff(const uint64_t& diff, const uint64_t& anchor) const {
    // convert backward from the transformation described in to_diff
    
    if (diff == 0) {
        return 0;
    }
    else if (diff % 5 == 0) {
        return anchor - diff / 5;
    }
    else {
        return anchor + diff - diff / 5 - 1;
    }
}
    
inline void RobustPagedVector::set(const size_t& i, const uint64_t& value) {
    if (i < latter_pages.page_width()) {
        first_page.set(i, value);
    }
    else {
        latter_pages.set(i - latter_pages.page_width(), value);
    }
}

inline uint64_t RobustPagedVector::get(const size_t& i) const {
    if (i < latter_pages.page_width()) {
        return first_page.get(i);
    }
    else {
        return latter_pages.get(i - latter_pages.page_width());
    }
}

inline void RobustPagedVector::append(const uint64_t& value) {
    if (first_page.size() < latter_pages.page_width()) {
        first_page.append(value);
    }
    else {
        latter_pages.append(value);
    }
}

inline void RobustPagedVector::pop() {
    if (latter_pages.empty()) {
        first_page.pop();
    }
    else {
        latter_pages.pop();
    }
}

inline void RobustPagedVector::resize(const size_t& new_size) {
    if (new_size > latter_pages.page_width()) {
        first_page.resize(latter_pages.page_width());
        latter_pages.resize(new_size - latter_pages.page_width());
    }
    else {
        first_page.resize(new_size);
        latter_pages.clear();
    }
}

inline void RobustPagedVector::reserve(const size_t& future_size) {
    if (future_size > latter_pages.page_width()) {
        first_page.reserve(latter_pages.page_width());
        latter_pages.reserve(future_size - latter_pages.page_width());
    }
    else {
        first_page.reserve(future_size);
    }
}

inline size_t RobustPagedVector::size() const {
    return first_page.size() + latter_pages.size();
}

inline bool RobustPagedVector::empty() const {
    return first_page.empty() && latter_pages.empty();
}

inline void RobustPagedVector::clear() {
    first_page.clear();
    latter_pages.clear();
}

inline size_t RobustPagedVector::page_width() const {
    return latter_pages.page_width();
}



inline size_t PackedSet::hash(const uint64_t& diff, const PackedVector& _table) const {
    // do a degree-4 mod polynomial with random coefficients, which is a 5-wise
    // independent hash function
    size_t p = _table.size();
    size_t x = 1;
    size_t hsh = 0;
    for (size_t i = 0; i < 5; ++i) {
        hsh = (hsh + ((coefs[i] * x) % p)) % p;
        x = (x * diff) % p;
    }
    return hsh;
}

inline uint64_t PackedSet::to_diff(const uint64_t& value, const uint64_t& _anchor) const {
    // encodes 0 as a 1, regardless of the anchor value. represents all other values
    // according to their difference from the anchor in the following manner:
    // difference  0 -1  1 -2  2 -3  3 -4  4 ....
    // integer     2  3  4  5  6  7  8  9 10 1...
    // the goal here is use smaller integers to maintain low bit-width, allowing 0 as
    // a sentinel value in the input, regardless of the anchor, and allowing 0 as
    // a sentinel in the output, regardless of the input
    
    if (value == 0) {
        return 1;
    }
    else if (value >= _anchor) {
        return 2 * (value - _anchor + 1);
    }
    else {
        return 2 * (_anchor - value) + 1;
    }
}

inline uint64_t PackedSet::from_diff(const uint64_t& diff, const uint64_t& _anchor) const {
    // inverse of to_diff
    if (diff == 1) {
        return 0;
    }
    else if (diff % 2 == 0) {
        return _anchor + (diff / 2) - 1;
    }
    else {
        return _anchor - (diff / 2);
    }
}

inline uint64_t PackedSet::optimal_anchor() const {
    uint64_t min_val = numeric_limits<uint64_t>::max();
    uint64_t max_val = numeric_limits<uint64_t>::min();
    for (size_t i = 0; i < table.size(); ++i) {
        uint64_t diff = table.get(i);
        if (diff >= 2) {
            // this is an encoding of a non-zero value
            uint64_t val = from_diff(diff, anchor);
            min_val = min(min_val, val);
            max_val = max(max_val, val);
        }
    }
    return min_val == numeric_limits<uint64_t>::max() ? anchor : (max_val + min_val) / 2;
}

// a precomputed list of prime numbers that approximately correspond to powers of 1.25
// except with the first handful spread out more (to speed up small allocations)
static constexpr uint64_t bdsg_packed_set_size_schedule[185] = {
    1ull, 5ull, 13ull, 23ull, 43ull, 53ull, 67ull, 83ull, 107ull, 131ull, 167ull, 211ull, 263ull,
    317ull, 409ull, 509ull, 643ull, 797ull, 1009ull, 1259ull, 1571ull, 1951ull, 2459ull, 3079ull,
    3851ull, 4813ull, 6011ull, 7523ull, 9403ull, 11743ull, 14683ull, 18367ull, 22943ull, 28697ull,
    35869ull, 44839ull, 56041ull, 70061ull, 87559ull, 109471ull, 136841ull, 171053ull, 213821ull,
    267271ull, 334093ull, 417617ull, 522017ull, 652507ull, 815663ull, 1019563ull, 1274461ull,
    1593071ull, 1991359ull, 2489197ull, 3111491ull, 3889373ull, 4861729ull, 6077119ull, 7596451ull,
    9495553ull, 11869423ull, 14836823ull, 18546019ull, 23182531ull, 28978171ull, 36222707ull,
    45278381ull, 56597993ull, 70747483ull, 88434347ull, 110542937ull, 138178681ull, 172723339ull,
    215904203ull, 269880239ull, 337350311ull, 421687909ull, 527109851ull, 658887367ull, 823609177ull,
    1029511501ull, 1286889389ull, 1608611699ull, 2010764683ull, 2513455849ull, 3141819743ull,
    3927274727ull, 4909093447ull, 6136366783ull, 7670458487ull, 9588073171ull, 11985091463ull,
    14981364313ull, 18726705383ull, 23408381719ull, 29260477199ull, 36575596517ull, 45719495621ull,
    57149369561ull, 71436711943ull, 89295889877ull, 111619862371ull, 139524828029ull, 174406035043ull,
    218007543763ull, 272509429729ull, 340636787177ull, 425795983969ull, 532244979997ull, 665306224997ull,
    831632781251ull, 1039540976543ull, 1299426220693ull, 1624282775867ull, 2030353469839ull,
    2537941837309ull, 3172427296627ull, 3965534120791ull, 4956917650967ull, 6196147063709ull,
    7745183829671ull, 9681479787061ull, 12101849733851ull, 15127312167367ull, 18909140209213ull,
    23636425261531ull, 29545531576877ull, 36931914471139ull, 46164893088917ull, 57706116361109ull,
    72132645451423ull, 90165806814311ull, 112707258517891ull, 140884073147359ull, 176105091434101ull,
    220131364292747ull, 275164205365909ull, 343955256707429ull, 429944070884257ull, 537430088605367ull,
    671787610756649ull, 839734513445857ull, 1049668141807321ull, 1312085177259179ull,
    1640106471573979ull, 2050133089467481ull, 2562666361834343ull, 3203332952292953ull,
    4004166190366189ull, 5005207737957737ull, 6256509672447187ull, 7820637090558769ull,
    9775796363198681ull, 12219745453998391ull, 15274681817498017ull, 19093352271872513ull,
    23866690339840631ull, 29833362924800803ull, 37291703656000973ull, 46614629570001271ull,
    58268286962501543ull, 72835358703127003ull, 91044198378908741ull, 113805247973635963ull,
    142256559967044949ull, 177820699958806141ull, 222275874948507727ull, 277844843685634633ull,
    347306054607043327ull, 434132568258804167ull, 542665710323505179ull, 678332137904381549ull,
    847915172380476881ull, 1059893965475596103ull, 1324867456844495087ull, 1656084321055618999ull,
    2070105401319523831ull, 2587631751649404731ull, 3234539689561756073ull, 4043174611952195011ull,
    5053968264940243967ull, 6317460331175304137ull, 7896825413969130449ull, 9871031767461412841ull,
    12338789709326766061ull};

inline void PackedSet::rehash(bool shrink) {
    
    // move to the next size in the schedule
    if (shrink) {
        if (schedule_val == 0) {
            return;
        }
        --schedule_val;
    }
    else {
        ++schedule_val;
    }
    
    // find the value that will be the best anchor to the current entries
    uint64_t new_anchor = optimal_anchor();
    PackedVector new_table;
    new_table.resize(bdsg_packed_set_size_schedule[schedule_val]);
    
    std::uniform_int_distribution<uint64_t> distr(0, new_table.size() - 1);
    for (size_t i = 0; i < 5; ++i) {
        coefs[i] = distr(gen);
    }
    
    // move the entries over to the new table
    for (size_t i = 0; i < table.size(); ++i) {
        uint64_t diff = table.get(i);
        if (diff != 0) {
            // not a null sentinel, re-encode and add into the new table
            diff = to_diff(from_diff(diff, anchor), new_anchor);
            new_table.set(locate(diff, new_table), diff);
        }
    }
    
    anchor = new_anchor;
    table = move(new_table);
}

inline size_t PackedSet::locate(const uint64_t& diff, const PackedVector& _table) const {
    // linear probing until finding the diff or a null sentinel
    size_t p = _table.size();
    size_t i = hash(diff, _table);
    while (_table.get(i) != 0 && _table.get(i) != diff) {
        i = (i + 1) % p;
    }
    return i;
}

inline void PackedSet::insert(const uint64_t& value) {
    
    // greedily choose the first non-zero anchor
    if (anchor == 0) {
        anchor = value;
    }
    
    // locate the value if it exists
    uint64_t diff = to_diff(value, anchor);
    size_t i = locate(diff, table);
    if (table.get(i) == 0) {
        // the item has not been inserted yet
        if (num_items + 1 >= max_load * table.size()) {
            // rehash so we don't exceed the max load
            rehash(false);
            // update the values that may have changed
            diff = to_diff(value, anchor);
            i = locate(diff, table);
        }
        
        
        table.set(i, diff);
        ++num_items;
    }
}

inline bool PackedSet::find(const uint64_t& value) const {
    return table.get(locate(to_diff(value, anchor), table)) != 0;
}

inline void PackedSet::remove(const uint64_t& value) {
    
    // locate the value if it exists
    uint64_t diff = to_diff(value, anchor);
    size_t i = locate(diff, table);
    
    if (table.get(i) != 0) {
        // the item exists in the table
        if (num_items - 1 <= min_load * table.size()) {
            // rehash so we don't exceed the min load
            rehash(true);
            // update the values that may have changed
            diff = to_diff(value, anchor);
            i = locate(diff, table);
        }

        // compute the linear probing distance from the hash value
        size_t p = table.size();

        // make sure we don't disrupt any linear probes by removing the value
        size_t j = (i + 1) % p;
        size_t search_dist = 1;
        while (table.get(j) != 0) {

            // how far did this value search from its hash to reach its current
            // placement?
            size_t hsh = hash(table.get(j), table);
            size_t hash_dist = j >= hsh ? j - hsh : (p - hsh) + j;

            if (hash_dist >= search_dist) {
                // this value searched past i during its placement, so we need
                // to move it up to preserve the integrity of its future searches
                table.set(i, table.get(j));

                // set up the next search
                i = j;
                search_dist = 0;
            }

            j = (j + 1) % p;
            ++search_dist;
        }

        // we fixed up any placements in this block, now we've found a position
        // that we can safely remove without disrupting any other searches
        table.set(i, 0);

        --num_items;
    }
}

inline void PackedSet::set_load_factors(double min_load_factor, double max_load_factor) {
    assert(max_load_factor > min_load_factor);
    assert(min_load_factor >= 0.0);
    assert(max_load_factor < 1.0);
    min_load = min_load_factor;
    max_load = max_load_factor;
    if (table.size() > 0) {
        while (num_items <= min_load * table.size()) {
            rehash(true);
        }
        while (num_items >= max_load * table.size()) {
            rehash(false);
        }
    }
}

inline double PackedSet::max_load_factor() const {
    return max_load;
}

inline double PackedSet::min_load_factor() const {
    return min_load;
}

inline size_t PackedSet::size() const {
    return num_items;
}

inline bool PackedSet::empty() const {
    return num_items == 0;
}
    
}



#endif /* packed_structs_hpp */
