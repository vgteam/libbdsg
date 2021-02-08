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
#include <functional>

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
    /// Must throw if allocation did not happen.
    std::function<char*(size_t)> resize;
    // TODO: stick a mutex in here?
};

// We work with "reference" types which have a ::body_t that exists in the
// memory mapping, an offset, and possibly other fields that exist outside it.
// The reference type can have members that e.g. allocate, while the body
// cannot. The reference can be null.
// These are meant to be passed around as values, and should expose some
// accessor methods that operate on the body.

template<typename Derived>
class base_ref_t {
public:
    /// The MappingContext that the referenced object lives in, or nullptr if we are null. 
    MappingContext* context;
    /// The offset in the context that the referenced object exists at.
    size_t offset;
    
    // Derived must provide a body_t.
    // Also we can't inherit multiple levels.
    
    /// Get a reference object to the existing body at the given offset.
    base_ref_t(MappingContext* context, size_t offset);
    
    /// Allocate and construct a new body at some available offset and get a
    /// reference object to it.
    base_ref_t(MappingContext* context);
    
    /// Get a null reference.
    base_ref_t();
    
    /// Be truthy if not null, and false otherwise
    operator bool () const;
    
    // TODO: we can't have helpers that return Dervied::body_t, even as a
    // pointer, because Derived inherits us and C++ thinks it isn't ready to be
    // used when we're doing our types. So we use a void pointer
    void* get_body();
    const void* get_body() const;
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
    /// Be constructable.
    /// Constructs as a pointer that equals nullptr.
    offset_ptr();
    
    // Be a good pointer
    operator bool () const; // TODO: why doesn't this work as an implicit conversion?
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
    operator bool () const;

    /// Get a ref_t to the body pointed to. If the pointer is null, the ref is also null.
    ref_t get(MappingContext* context);
    
    /// Get a ref_t to the body pointed to, plus a given offset in bodies of the same size.
    /// If the pointer is null, the ref is also null.
    ref_t get_at(MappingContext* context, size_t index);
    
    /// Set this pointer to point to the body of the given ref_t
    offset_to<ref_t>& operator=(const ref_t& other);
    
    /// Determine if this pointer points to the body of the given ref.
    bool operator==(const ref_t& other) const;
    /// Determine if this pointer does not point to the body of the given ref.
    bool operator!=(const ref_t& other) const;
};

/**
 * Entry in the list of allocated or free blocks in the arena allocator.
 */
class ArenaAllocatorBlockRef : public base_ref_t<ArenaAllocatorBlockRef> {
public:
    struct body_t {
        /// Next block. Only used when block is free.
        offset_to<ArenaAllocatorBlockRef> prev;
        /// Previous block. Only used when block is free.
        offset_to<ArenaAllocatorBlockRef> next;
        /// Size fo the block in bytes, not counting this header. Used for free
        /// and allocated blocks.
        big_endian<size_t> size;
    };
    
    using base_ref_t<ArenaAllocatorBlockRef>::base_ref_t;
    
    /// Get or set the previous entry in the free list.
    offset_to<ArenaAllocatorBlockRef>& prev();
    /// Get or set the next entry in the free list.
    offset_to<ArenaAllocatorBlockRef>& next();
    /// Get or set the size of the block, after the header.
    big_endian<size_t>& size();
    /// Get the offset in the context of the first byte of memory we manage.
    size_t get_user_data() const;
    /// Get a ref to the block managing the data starting at the given byte.
    static ArenaAllocatorBlockRef get_from_data(MappingContext* context, size_t user_data);
    
    /// Split the block, keeping first_bytes bytes and giving the rest to a new
    /// subsequent block, which is wired up and returned. Assumes the block is
    /// free.
    ArenaAllocatorBlockRef split(size_t first_bytes);
    
    /// Remove this block from the free list. Returns the blocks before and
    /// after it, which is has wired together. If this was the first or last
    /// block (or both), the appropriate return value will be a null ref.
    pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> detach();
    
    /// Attach this block to the free list, between the given blocks, which may
    /// be null.
    void attach(ArenaAllocatorBlockRef& left, ArenaAllocatorBlockRef& right);
    
    /// Defragment and coalesce adjacent free blocks in the contiguous run this
    /// block is part of, if any. Returns the first and last blocks in the run;
    /// the last block's header will be in the free space of the first block,
    /// unless the last block is the first block.
    pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> coalesce();
    
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
    };

    using pointer = size_t;
    using const_pointer = size_t;
    using size_type = size_t;
    
    /// When using an ArenaAllocator, where should we expect the offset of the first thing allocated to be?
    /// Some header space might be necessary before it for the allocator to work.
    static const size_t reserved_bytes = sizeof(body_t);
    
    /**
     * Make an allocator in the given MappingContext, or connect to the existing one.
     * Resizes the context to be big enough to hold the allocator, if it isn't already.
     * We assume allocators are only allowed at 0, one per context.
     */
    ArenaAllocatorRef(MappingContext* context);
    
    /**
     * Copy an allocator.
     */
    ArenaAllocatorRef(const ArenaAllocatorRef& other) = default;
    
    /**
     * Copy an allocator, but change the allocated type.
     */
    template<typename U>
    ArenaAllocatorRef(const ArenaAllocatorRef<U>& alloc);
    
    /**
     * Allocate the given number of items. Ought to be near the hint.
     */
    pointer allocate(size_type n, const_pointer hint = 0);
    
    /**
     * Deallocate the given number of items. Must be the same number as were allocated.
     */
    void deallocate(pointer p, size_type n);
    
};

/**
 * Reference type for a vector of primitive items, directly stored in the mapping.
 * Note that destructors won't run when things are just unmapped, and
 * constructors won't run when things are mapped in.
 */
template<typename item_t>
class MappedVectorRef : public base_ref_t<MappedVectorRef<item_t>> {
public:
    struct body_t {
        big_endian<size_t> length;
        big_endian<size_t> reserved_length;
        offset_ptr<item_t> first;
    };
    
    using base_ref_t<MappedVectorRef<item_t>>::base_ref_t;
    
    size_t size() const;
    void resize(size_t new_size);
    item_t& at(size_t index);
    const item_t& at(size_t index) const;
    
    // TODO: reserve(), push_back()
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


///////////////////////////////////////////////////////////////////////////////////


// Implementations

template<typename T>
ArenaAllocatorRef<T>::ArenaAllocatorRef(MappingContext* context) : base_ref_t<ArenaAllocatorRef<T>>(context, 0) {
    // We declared ourselves to be at 0.
    if (context->size < reserved_bytes) {
        // The body didn't exist yet.
        // Make room for the body
        context->base_address = context->resize(reserved_bytes);
        context->size = reserved_bytes;
        // Run its constructor
        new ((void*) context->base_address) body_t;
    }
    // Otherwise we assume the allocator was already there.
    // We totally ignore the different template parameters.
    // TODO: be a base class of all the templated derived classes?
}

template<typename T>
template<typename U>
ArenaAllocatorRef<T>::ArenaAllocatorRef(const ArenaAllocatorRef<U>& alloc): base_ref_t<ArenaAllocatorRef<T>>(alloc.context, alloc.offset) {
    // Nothing to do to change the type; just steal the state.
}

template<typename T>
auto ArenaAllocatorRef<T>::allocate(size_type n, const_pointer hint) -> pointer {
    // Find our body
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    
    // How much user space do we need?
    // TODO: alignment???
    size_t user_bytes = n * sizeof(T);
    // And how much space do we need with block overhead?
    size_t block_bytes = user_bytes + sizeof(ArenaAllocatorBlockRef::body_t);
    
    // This will hold a ref to the free block we found or made that is big enough to hold this item.
    // Starts null if there is no first_free.
    ArenaAllocatorBlockRef found = body.first_free.get(this->context);
    while (found && found.size() < user_bytes) {
        // Won't fit here. Try the next place.
        found = found.next().get(this->context);
    }
   
    if (!found) {
        // We have no free memory big enough.
        
        // Work out where new free memory will start
        size_t new_free = this->context->size;
        
        // Double the arena, or add space for the data we need to allocate, whichever is bigger.
        size_t new_bytes = max(this->context->size, block_bytes);
        this->context->base_address = this->context->resize(this->context->size + new_bytes);
        this->context->size += new_bytes;
        
        // Create a new block
        new ((void*) (this->context->base_address + new_free)) ArenaAllocatorBlockRef::body_t;
        
        // Refer to it and say we found it.
        found = ArenaAllocatorBlockRef(this->context, new_free);
        
        // Initialize it. Leave prev and next pointers null because it is the only free block.
        found.size() = user_bytes;
        
        // Put it in the linked list as head and tail.
        body.first_free = found;
        body.last_free = found;
    }
    
    // Now we can allocate memory.
    
    if (found.size() > block_bytes) {
        // We could break the user data off of this block and have some space left over.
        // TODO: use a min block size here instead.
        
        // So split the block.
        ArenaAllocatorBlockRef second = found.split(user_bytes);
        if (body.last_free == found) {
            // And fix up the end of the linked list
            body.last_free = second;
        }
    }
    
    // Now we have a free block of the right size. Make it not free.
    auto connected = found.detach();
    if (body.first_free == found) {
        // This was the first free block
        body.first_free = connected.first;
    }
    if (body.last_free == found) {
        // This was the last free block 
        body.last_free = connected.second;
    }
    
    // Give out the address of its data
    return found.get_user_data();
}


template<typename T>
void ArenaAllocatorRef<T>::deallocate(pointer p, size_type n) {
    // Find our body
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    
    // Find the block
    ArenaAllocatorBlockRef found = ArenaAllocatorBlockRef::get_from_data(this->context, p);
    
    // Find the block in the free list after it, if any
    ArenaAllocatorBlockRef right = body.first_free;
    while(right && right.offset < found.offset) {
        right = right.next();
    }
    ArenaAllocatorBlockRef left;
    if (!right) {
        // The new block should be the last block in the list.
        // So it comes after the existing last block, if any.
        left = body.last_free;
    } else {
        // The new block comes between right and its predecessor, if any
        left = right.prev();
    }
    
    // Wire in the block
    found.attach(left, right);
    
    // Update haed and tail
    if (body.last_free == left) {
        body.last_free = found;
    }
    if (body.first_free == right) {
        body.first_free = found;
    }
    
    // Defragment.
    auto bounds = found.coalesce();
    // We can't need to update the first free when defragmenting, but we may
    // need to update the last free.
    if (body.last_free == bounds.second) {
        body.last_free = bounds.first;
    }
}

template<typename item_t>
size_t MappedVectorRef<item_t>::size() const {
    auto& body = *reinterpret_cast<const body_t*>(this->get_body());
    return body.length;
}

template<typename item_t>
void MappedVectorRef<item_t>::resize(size_t new_size) {
    if (new_size == size()) {
        // Nothing to do!
        return;
    }

    // Get the body
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    
    // Where is the vector going?
    item_t* new_first = nullptr;
    
    if (new_size > body.reserved_length) {
        // Get the allocator
        // TODO: if we put a mutex in the ref, this won't work.
        ArenaAllocatorRef<item_t> alloc(this->context);

        // TODO: we can't deal with a simultaneous reallocation/move of the context
        // while we're working.

        // Allocate space for the new data, and get the position in the context
        size_t new_first_pos = alloc.allocate(new_size);
        
        // Convert to memory address
        new_first = (item_t*) (this->context->base_address + new_first_pos);
        
        if (size() > 0) {
            // Get memory address of old data
            item_t* old_first = &*body.first;
            
            for (size_t i = 0; i < size() && i < new_size; i++) {
                // Run move constructors
                new (new_first + i) item_t(std::move(*(old_first + i)));
                // And destructors
                (old_first + i)->~item_t();
            }
        }
        
        // Record the new reserved length
        body.reserved_length = new_size;
    } else {
        // Just run in place
        new_first = &*body.first;
    }
    
    for (size_t i = size(); i < new_size; i++) {
        // For anything aded on, just run constructor
        new (new_first + i) item_t();
    }
    
    if (new_size < size()) {
        // We shrank, and we had at least one item.
        item_t* old_first = &*body.first;
        for (size_t i = new_size; i < size(); i++) {
            // For anything trimmed off, just run destructors.
            (old_first + i)->~item_t();
        }
    }
    
    // Record the new length
    body.length = new_size;
}

template<typename item_t>
item_t& MappedVectorRef<item_t>::at(size_t index) {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    
    if (index <= size()) {
        // TODO: throw the right type here.
        throw std::runtime_error("Cannot get " + std::to_string(index) +
                                 " in vector of length " + std::to_string(size()));
    }
    
    return *(body.first + index);
}

template<typename item_t>
const item_t& MappedVectorRef<item_t>::at(size_t index) const {
    // Just run non-const at and constify result
    return const_cast<MappedVectorRef<item_t>*>(this)->at(index);
}



}

#endif
