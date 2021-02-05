//
//  mapped_structs.hpp
//  
// Contains implementations of classic data structures for use in a memory-mapped area.
//

#ifndef BDSG_MAPPED_STRUCTS_HPP_INCLUDED
#define BDSG_MAPPED_STRUCTS_HPP_INCLUDED

#include <cstdio>
#include <cstdint>
#include <iostream>

namespace bdsg {
    
using namespace std;

/**
 * Context in which memory mapping happens. Needs to be passed down through all
 * the reference object constructors so that more reference objects can be
 * made.
 */
struct MappingContext {
    char* base_address;
    size_t size;
    /// When we're trying to allocate and we can't, resize. Might move the whole thing.
    std::function<char*(size_t)> resize;
    // TODO: stick a mutex in here?
};

// We work with "reference" types which have a ::body_t that exists in the
// memory mapping, an offset, and possibly other fields that exist outside it.
// The reference type can have members that e.g. allocate, while the body
// cannot.
// These are meant to be value types, and should expose some accessor methods
// that operate on the body.

template<typename Derived>
class base_ref_t {
public:
    MappingContext& context;
    size_t offset;
    
    // Derived must provide a body_t.
    // Also we can't inherit multiple levels.
    
    /// Get a reference object to the existing body at the given offset.
    base_ref_t(MappingContext& context, size_t offset);
    
    /// Allocate and construct a new body at some available offset and get a
    /// reference object to it.
    base_ref_t(MappingContext& context);
};

/**
 * Wrapper that stores a number in a defined endian-ness.
 */
template<typename T>
class big_endian {
public:
    operator T () const;
    big_endian<T>& operator=(const T& x);
protected:
    char storage[sizeof(T)];
};

/**
 * Offset pointer to a primitive type, in the memory mapping.
 * Exists in the memory mapping.
 */
template<typename T>
class offset_ptr {
private:
    big_endian<size_t> offset;
public:
    // Be constructable
    offset_ptr() = default;

    // Be a good pointer
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;
    offset_ptr& operator=(T* addr);
    T* operator+(size_t items);
    const T* operator+(size_t items) const;
    
    // Make sure to account for offset changes when copying or moving
    offset_ptr<T>& operator=(const offset_ptr<T>& other);
    offset_ptr<T>& operator=(offset_ptr<T>&& other);
    offset_ptr(const offset_ptr<T>& other);
    offset_ptr(offset_ptr<T>&& other);
};

/**
 * Offset pointer to the body of a reference type, in the memory mapping.
 * Exists in the memory mapping.
 * Makes it easy to wrap the body back up in a reference type.
 */
template<typename ref_t>
class offset_to : offset_ptr<typename ref_t::body_t> {
public:
    /// Get a ref_t to the body pointed to.
    ref_t get(MappingContext& context);
    
    /// Get a ref_t to the body pointed to, plus a given offset in bodies of the same size.
    ref_t get_at(MappingContext& context, size_t index);
    
    /// Set this pointer to point to the body of the given ref_t
    offset_to<ref_t>& operator=(const ref_t other);
};

/**
 * Entry in the list of allocated or free blocks in the arena allocator.
 */
class ArenaAllocatorBlockRef : public base_ref_t<ArenaAllocatorBlockRef> {
public:
    struct body_t {
        offset_to<ArenaAllocatorBlockRef> prev;
        offset_to<ArenaAllocatorBlockRef> next;
        big_endian<bool> is_free;
    };
};

/**
 * Allocator on top of a byte array.
 * Maintains a free list and deals in offsets in the array as an interface.
 */
template<typename T>
class ArenaAllocatorRef : public base_ref_t<ArenaAllocatorRef<T>> {
public:
    struct body_t {
        offset_to<ArenaAllocatorBlockRef> first_free;
        offset_to<ArenaAllocatorBlockRef> last_free;
        offset_to<ArenaAllocatorBlockRef> first_used;
        offset_to<ArenaAllocatorBlockRef> last_used;
    };

    using pointer = size_t;
    using const_pointer = size_t;
    using size_type = size_t;
    
    /// When using an ArenaAllocator, where should we expect the offset of the first thing allocated to be?
    /// Some header space might be necessary before it for the allocator to work.
    static const size_t reserved_bytes = sizeof(body_t);
    
    /**
     * Allocate the given number of items. Ought to be near the hint.
     */
    pointer allocate(size_type n, const_pointer hint = 0);
    
    /**
     * Deallocate the given number of items. Must be the same number as were allocated.
     */
    void deallocate(pointer p, size_type n);
    
    /**
     * Make an allocator in the given MappingContext, or connect to the existing one.
     * Resizes the context to be big enough to hold the allocator, if it isn't already.
     * We assume allocators are only allowed at 0, one per context.
     */
    ArenaAllocatorRef(MappingContext& context);
    
    /**
     * Copy an allocator.
     */
    ArenaAllocatorRef(const ArenaAllocatorRef& other) = default;
    
    /**
     * Copy an allocator, but change the allocated type.
     */
    template<typename U>
    ArenaAllocatorRef(const ArenaAllocatorRef<U>& alloc);
};

/**
 * Reference type for a vector of primitive items, directly stored in the mapping.
 */
template<typename item_t>
class MappedVectorRef : public base_ref_t<MappedVectorRef<item_t>> {
public:
    struct body_t {
        big_endian<size_t> length;
        offset_ptr<item_t> first;
    };
    
    using base_ref_t<MappedVectorRef<item_t>>::base_ref_t;
    
    size_t size() const;
    void resize(size_t new_size);
    item_t& at(size_t index);
    const item_t& at(size_t index) const;
};

/**
 * Reference type for a vector of reference type items, with bodies stored in the mapping.
 */
template<typename item_ref_t>
class MappedRefVectorRef : public base_ref_t<MappedRefVectorRef<item_ref_t>> {
public:
    struct body_t {
        big_endian<size_t> length;
        offset_to<item_ref_t> first;
    };
    
    using base_ref_t<MappedRefVectorRef<item_ref_t>>::base_ref_t;
    
    size_t size() const;
    void resize(size_t new_size);
    item_ref_t at(size_t index);
    const item_ref_t at(size_t index) const;
};

class IntVectorRef : public base_ref_t<IntVectorRef> {
public:
    struct body_t {
    };
    // TODO
};

class PackedVectorRef : public base_ref_t<PackedVectorRef> {
public:
    struct body_t {
    };
    // TODO
};

class PagedVectorRef : public base_ref_t<PagedVectorRef> {
public:
    struct body_t {
    };
    // TODO
};

}

#endif
