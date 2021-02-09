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
#include <limits>

#include <endian.h>

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
    // If so we'd also need to protect any math involving base_address, or even
    // the addresses of any things in the arena, or even accesses to any things
    // in the arena...
};

// We work with "reference" types which have a ::body_t that exists in the
// memory mapping at a certain context-relative position, and possibly other
// fields that exist outside it.
// The reference type can have members that e.g. allocate, while the body
// cannot. The reference can be null.
// These are meant to be passed around as values, and should expose some
// accessor methods that operate on the body.

template<typename Derived>
class base_ref_t {
public:
    /// The MappingContext that the referenced object lives in, or nullptr if we are null. 
    MappingContext* context;
    /// The position in the context that the referenced object exists at.
    size_t position;
    
    // Derived must provide a body_t.
    // Also we can't inherit multiple levels.
    
    // Pointer templates expect implementations fo these constructors:
    
    /// Get a reference object to the existing body at the given position.
    base_ref_t(MappingContext* context, size_t position);
    
    /// Allocate and construct a new body at some available position and get a
    /// reference object to it.
    base_ref_t(MappingContext* context);
    
    /// Get a null reference.
    base_ref_t();
    
    // Copy and move
    base_ref_t(const base_ref_t<Derived>& other) = default;
    base_ref_t(base_ref_t<Derived>&& other) = default;
    base_ref_t<Derived>& operator=(const base_ref_t<Derived>& other) = default;
    base_ref_t<Derived>& operator=(base_ref_t<Derived>&& other) = default;
    
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
template<typename T, int BitWidth = std::numeric_limits<T>::digits>
class big_endian {
public:
    big_endian() = default;
    big_endian(const T& value);
    operator T () const;
    big_endian& operator=(const T& x);
protected:
    char storage[sizeof(T)];
};

template<typename T, int BitWidth>
big_endian<T, BitWidth>::big_endian(const T& value) {
    *this = value;
}

// Implement endian converters templated for all integral types of the
// appropriate size for the conversion functions.

// We can't partially specialize the members any way that I can work out; we
// have to do the whole class, and we have to do it before the template is
// instantiated.

template<typename T>
class big_endian<T, 64> {
public:
    operator T () const {
        return (T)be64toh(*((const T*)this->storage));
    }
    
    big_endian& operator=(const T& x) {
        *((T*)this->storage) = (T)htobe64(x);
        return *this;
    }   
};

template<typename T>
class big_endian<T, 32> {
public:
    operator T () const {
        return (T)be32toh(*((const T*)this->storage));
    }
    
    big_endian& operator=(const T& x) {
        *((T*)this->storage) = (T)htobe32(x);
        return *this;
    }   
};

/**
 * Offset pointer to a primitive type, in the memory mapping.
 * Exists in the memory mapping.
 */
template<typename T>
class offset_ptr {
protected:
    /// Offset from our own storage location to what we point to, or max
    /// possible value if null.
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
    offset_ptr<T>& operator=(const T* addr);
    // TODO: how do we make the const version of us a pointer to a const thing
    // that we can set to a const pointer? FOr now assignment vanishes away
    // constness.
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
class offset_to : public offset_ptr<typename ref_t::body_t> {
public:
    /// Get a ref_t to the body pointed to. If the pointer is null, the ref is also null.
    ref_t get(MappingContext* context);
    
    /// Get a ref_t to the body pointed to. If the pointer is null, the ref is also null.
    const ref_t get(MappingContext* context) const;
    
    /// Get a ref_t to the body pointed to, plus a given offset in bodies of the same size.
    /// If the pointer is null, the ref is also null.
    ref_t get_at(MappingContext* context, size_t index);
    
    /// Get a ref_t to the body pointed to, plus a given offset in bodies of the same size.
    /// If the pointer is null, the ref is also null.
    const ref_t get_at(MappingContext* context, size_t index) const;
    
    /// Set this pointer to point to the body of the given ref_t
    offset_to<ref_t>& operator=(const ref_t& other);
    
    /// Determine if this pointer points to the body of the given ref.
    bool operator==(const ref_t& other) const;
    /// Determine if this pointer does not point to the body of the given ref.
    bool operator!=(const ref_t& other) const;
};

/**
 * Entry in the list of free blocks in the arena allocator. Also sits before
 * allocated blocks, to remember the blocks' sizes.
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
    
    // Body member accessors
    
    // I tried to give out references to the backing fields, but getting const
    // correctness on the fields was difficult, and providing the context to
    // use the fields was awkward. So we just pass around refs by value for
    // anything that's a pointer.
    
    /// Get the previous entry in the free list.
    ArenaAllocatorBlockRef get_prev() const;
    
    /// Set the previous entry in the free list.
    void set_prev(const ArenaAllocatorBlockRef& prev);
    
    /// Get the next entry in the free list.
    ArenaAllocatorBlockRef get_next() const;
    
    /// Set the next entry in the free list.
    void set_next(const ArenaAllocatorBlockRef& next);
    
    /// Get the size of the block, after the header.
    const big_endian<size_t>& size() const;
    
    /// Get or set the size of the block, after the header. You can assign to the result.
    big_endian<size_t>& size();
    
    /// Get the position in the context of the first byte of memory we manage.
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
    
protected:
    
    /// Return true if this block comes immediately before the other one, with
    /// no space between them.
    bool immediately_before(const ArenaAllocatorBlockRef& other) const;
    
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
    
    /// When using an ArenaAllocator, where should we expect the position of the first thing allocated to be?
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

template<typename Derived>
base_ref_t<Derived>::base_ref_t(MappingContext* context, size_t position) : 
    context(context), position(position) {
    
    // Nothing to do
};

template<typename Derived>
base_ref_t<Derived>::base_ref_t(MappingContext* context) : context(context) {
    // Grab an allocator
    ArenaAllocatorRef<typename Derived::body_t> alloc(context);
    
    // Allocate one body
    position = alloc.allocate(1);
};

template<typename Derived>
base_ref_t<Derived>::base_ref_t() : context(nullptr), position(0) {
    // Nothing to do
}

template<typename Derived>
void* base_ref_t<Derived>::get_body() {
    if (!context) {
        throw std::runtime_error("Trying to follow a null reference");
    }
    return (void*)(context->base_address + position);
}

template<typename Derived>
const void* base_ref_t<Derived>::get_body() const {
    if (!context) {
        throw std::runtime_error("Trying to follow a null reference");
    }
    return (const void*)(context->base_address + position);
}

template<typename Derived>
base_ref_t<Derived>::operator bool () const {
    return (context != nullptr);
}

////////////////////

template<typename T>
offset_ptr<T>::offset_ptr() : offset(std::numeric_limits<size_t>::max()) {
    // Nothing to do!
}

template<typename T>
offset_ptr<T>::operator bool () const {
    return (offset != std::numeric_limits<size_t>::max());
}

template<typename T>
T& offset_ptr<T>::operator*() {
    return *(this->operator->());
}

template<typename T>
const T& offset_ptr<T>::operator*() const {
    return *(this->operator->());
}

template<typename T>
T* offset_ptr<T>::operator->() {
    if (offset == std::numeric_limits<size_t>::max()) {
        throw std::runtime_error("Null pointer dereference");
    }
    return (T*) (((char*) this) + offset);
}

template<typename T>
const T* offset_ptr<T>::operator->() const {
    if (offset == std::numeric_limits<size_t>::max()) {
        throw std::runtime_error("Null pointer dereference");
    }
    return (const T*) (((const char*) this) + offset);
}

template<typename T>
offset_ptr<T>& offset_ptr<T>::operator=(const T* addr) {
    if (addr) {
        // Represent real value as as a difference
        offset = (size_t) (((const char*) addr) - ((const char*) this));
    } else {
        // Represent null specially
        offset = std::numeric_limits<size_t>::max();
    }
}

template<typename T>
T* offset_ptr<T>::operator+(size_t items) {
    return this->operator->() + items;
}

template<typename T>
const T* offset_ptr<T>::operator+(size_t items) const {
    return this->operator->() + items;
}

template<typename T>
offset_ptr<T>& offset_ptr<T>::operator=(const offset_ptr<T>& other) {
    // Delegate to assignment based on cuttent absolute memory address of the
    // destination.
    *this = other.operator->();
}

template<typename T>
offset_ptr<T>& offset_ptr<T>::operator=(offset_ptr<T>&& other) {
    *this = other.operator->();
}

template<typename T>
offset_ptr<T>::offset_ptr(const offset_ptr<T>& other) {
    *this = other.operator->();
}

template<typename T>
offset_ptr<T>::offset_ptr(offset_ptr<T>&& other) {
    *this = other.operator->();
}

////////////////////


template<typename ref_t>
ref_t offset_to<ref_t>::get(MappingContext* context) {
    if (*this) {
        // We aren't null.
        // Get our position in the context
        size_t our_position = ((char*) this) - context->base_address;
        // Apply our offset and create a reference in the context
        return ref_t(context, our_position + this->offset);
    } else {
        // Return a null ref. Null refs need no context.
        return ref_t();
    }
}

template<typename ref_t>
const ref_t offset_to<ref_t>::get(MappingContext* context) const {
    if (*this) {
        // We aren't null.
        // Get our position in the context
        size_t our_position = ((const char*) this) - context->base_address;
        // Apply our offset and create a reference in the context
        return ref_t(context, our_position + this->offset);
    } else {
        // Return a null ref. Null refs need no context.
        return ref_t();
    }
}

template<typename ref_t>
ref_t offset_to<ref_t>::get_at(MappingContext* context, size_t index) {
    if (*this) {
        // Get our position in the context
        size_t our_position = ((char*) this) - context->base_address;
        // Apply our offset and the index offset and create a reference in the context
        return ref_t(context, our_position + this->offset + index * sizeof(typename ref_t::body_t));
    } else {
        // Return a null ref. Null refs need no context.
        return ref_t();
    }
}

template<typename ref_t>
const ref_t offset_to<ref_t>::get_at(MappingContext* context, size_t index) const {
    if (*this) {
        // Get our position in the context
        size_t our_position = ((const char*) this) - context->base_address;
        // Apply our offset and the index offset and create a reference in the context
        return ref_t(context, our_position + this->offset + index * sizeof(typename ref_t::body_t));
    } else {
        // Return a null ref. Null refs need no context.
        return ref_t();
    }
}
    
template<typename ref_t>
offset_to<ref_t>& offset_to<ref_t>::operator=(const ref_t& other) {
    if (other) {
        // Not null
        size_t our_position = ((char*) this) - other.context->base_address;
        this->offset = other.position - our_position;
    } else {
        // Become null
        this->offset = std::numeric_limits<size_t>::max();
    }
    
    return *this;
}

template<typename ref_t>
bool offset_to<ref_t>::operator==(const ref_t& other) const {
    if (!*this || !other) {
        // If either is null, they're only equal if they're both null
        return !*this && !other;
    }
    
    // Otherwise, we're equal if we point at equal places.
    size_t dest_position = (((char*) this) - other.context->base_address) + this->offset;
    return dest_position == other.position;
}

template<typename ref_t>
bool offset_to<ref_t>::operator!=(const ref_t& other) const {
    return !(this == other);
}

////////////////////

// ArenaAllocatorBlockRef isn't a template so it lives in the CPP.

////////////////////

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
ArenaAllocatorRef<T>::ArenaAllocatorRef(const ArenaAllocatorRef<U>& alloc): base_ref_t<ArenaAllocatorRef<T>>(alloc.context, alloc.position) {
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
        found = found.get_next();
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
    while(right && right.position < found.position) {
        right = right.get_next();
    }
    ArenaAllocatorBlockRef left;
    if (!right) {
        // The new block should be the last block in the list.
        // So it comes after the existing last block, if any.
        left = body.last_free;
    } else {
        // The new block comes between right and its predecessor, if any
        left = right.get_prev();
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

////////////////////

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
