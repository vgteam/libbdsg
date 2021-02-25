//
//  mapped_structs.hpp
//  
// Contains implementations of classic data structures for use in a memory-mapped area.
//

#ifndef BDSG_MAPPED_STRUCTS_HPP_INCLUDED
#define BDSG_MAPPED_STRUCTS_HPP_INCLUDED

#include <cstdio>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <functional>
#include <limits>

#include <map>
#include <unordered_map>
#include <vector>
#include <shared_mutex>

#include <endian.h>

namespace bdsg {
    
using namespace std;

/**
 * Wrapper that stores a number in a defined endian-ness.
 */
template<typename T>
class big_endian {
public:
    big_endian();
    big_endian(const T& value);
    operator T () const;
    big_endian<T>& operator=(const T& x);
protected:
    char storage[sizeof(T)];
};

/**
 * You Only Map Once: mapped memory with safe allocation by objects in the mapped memory.
 *
 * YOMO provides an interconnected system for file-backed memory mapping and
 * allocation, so that objects whose *this is in a mapped memory segment can
 * safely allocate more memory backed by the file the object exists in.
 * Allocating more memory will not unmap memory already allocated.
 */
namespace yomo {

/**
 * Pointer to an object of type T, which lives at an address mapped by a
 * yomo::Allocator (or otherwise in a yomo::Manager's chain), and is itself
 * stored at an address mapped by a yomo::Allocator (or otherwise in the same
 * yomo::Manager chain).
 *
 * We interpret constness as applying to the value of the pointer and not the
 * pointed-to object. If you want a pointer to a const object, you need a
 * Pointer<const T>.
 */
template<typename T>
class Pointer {
public:
    /// Be constructable.
    /// Constructs as a pointer that equals nullptr.
    Pointer() = default;
    
    Pointer(T* destination);
    
    // Be a good pointer
    operator bool () const;
    T& operator*() const;
    T* operator->() const;
    operator T* () const;
    Pointer<T>& operator=(T* addr);
    T* operator+(size_t items) const;
    
    // Expose the address of the pointed-to object not through manual operator
    // calling.
    T* get() const;
    
protected:
    /// Stores the destination position in the chain, or max size_t for null.
    big_endian<size_t> position = std::numeric_limits<size_t>::max();
};

/**
 * Global manager of mapped memory segments. Talked to by pointers in memory
 * segments to figure out where they actually point to.
 *
 * The manager manages one or more "chains", each potentially corresponding to
 * a file. The chains are made up of mapped memory "segments", and each segment
 * can be mapped at a different base address.
 *
 * When a file is initially mapped, it is mapped as a single segment.
 * Additional segments may be mapped later to fulfill allocations from
 * yomo::Allocator<T> instances stored in the chain.
 */
class Manager {

public:

    using chainid_t = intptr_t;
    static const chainid_t NO_CHAIN = 0;

    /**
     * Create a chain not backed by any file. The given prefix data will occur
     * before the chain allocator data structures.
     */
    static chainid_t create_chain(const std::string& prefix = "");
    
    /**
     * Create a chain by mapping all of the given open file.
     *
     * Modifications to the chain will affect the file, and it will grow as
     * necessary.
     *
     * The Manager will not take ownership of the file descriptor.
     *
     * If the file is nonempty, data after the length of the passed prefix must
     * contain the chain allocator data structures. If it is empty, the prefix
     * and the chain allocator data structures will be written to it.
     */
    static chainid_t create_chain(int fd, const std::string& prefix = "");
    
    /**
     * Return a chain which has the same stored data as the given chain, but
     * for which modification of the chain will not modify any backing file on
     * disk. The chain returned may be the same chain as the given chain.
     *
     * Not thread safe with concurrent modificatons to the source chain.
     */
    static chainid_t get_dissociated_chain(chainid_t chain);
    
    /**
     * Return a chain which has the same stored data as the given chain, but
     * for which modification of the chain will modify the open file with the
     * given file descriptor. The chain returned may be the same chain as the
     * given chain.
     *
     * The Manager will not take ownership of the file descriptor.
     *
     * Not thread safe with concurrent modificatons to the source chain.
     */
    static chainid_t get_associated_chain(chainid_t chain, int fd);
    
    /**
     * Destroy the given chain and unmap all of its memory, and close any
     * associated file.
     */
    static void destroy_chain(chainid_t chain);
    
    /**
     * Get the chain that contains the given address, or NO_CHAIN if the
     * address is outside all current chains.
     */
    static chainid_t get_chain(const void* address);
    
    /**
     * Get the address of the given byte from the start of the chain.
     * If a length is provided, throws if the given length of bytes from
     * position are not contiguous in memory.
     */
    static void* get_address_in_chain(chainid_t chain, size_t position, size_t length = 0);
    
    /**
     * Get the position of the given byte in the chain it is in, along with the
     * identifier for that chain.
     * If a length is provided, throws if the given length of bytes from
     * position are not contiguous in memory.
     */
    static std::pair<chainid_t, size_t> get_chain_and_position(const void* address, size_t length = 0);
    
    /**
     * Find the address of the given position in the chain that the given address exists in.
     */
    static void* get_address_in_same_chain(const void* here, size_t position);
    
    /**
     * Find the position of the given address in the chain that here exists in.
     */
    static size_t get_position_in_same_chain(const void* here, const void* address);
    
    /**
     * Allocate the given number of bytes from the given chain.
     */
    static void* allocate_from(chainid_t chain, size_t bytes);
    
    /**
     * Allocate the given number of bytes from the chain containing the given
     * address.
     */
    static void* allocate_from_same_chain(void* here, size_t bytes);
    
    /**
     * Free the given allocated block in the chain to which it belongs.
     */
    static void deallocate(void* address);
    
    /**
     * Find the mapped address of the first thing allocated in the chain, given
     * that it was allocated with the given size.
     *
     * That first allocated thing must exist and not be deallocated.
     */
    static void* find_first_allocation(chainid_t chain, size_t bytes);
    
    /**
     * Dump information about free and allocated memory.
     * Not thread safe!
     */
    static void dump(chainid_t chain);  
    
protected:
    
    
    struct LinkRecord;
    
    /**
     * This occurs inside the chains and represents the header of some free or
     * allocated memory.
     */
    struct AllocatorBlock {
        /// Previous block. Only used when block is free. Null if allocated.
        Pointer<AllocatorBlock> prev;
        /// Next block. Only used when block is free. Null if allocated.
        Pointer<AllocatorBlock> next;
        /// Size fo the block in bytes, not counting this header. Used for free
        /// and allocated blocks.
        big_endian<size_t> size;
        
        /// Get the address of the first byte of memory we manage.
        void* get_user_data() const;
        /// Get the address of the block managing the data starting at the given byte.
        static AllocatorBlock* get_from_data(void* user_data);
        
        /// Split the block, keeping first_bytes bytes and giving the rest to a new
        /// subsequent block, which is wired up and returned. Assumes the block is
        /// free.
        AllocatorBlock* split(size_t first_bytes);
        
        /// Remove this block from the free list. Returns the blocks before and
        /// after it, which is has wired together. If this was the first or last
        /// block (or both), the appropriate return value will be null.
        std::pair<AllocatorBlock*, AllocatorBlock*> detach();
        
        /// Attach this block to the free list, between the given blocks, which may
        /// be null.
        void attach(AllocatorBlock* left, AllocatorBlock* right);
        
        /// Defragment and coalesce adjacent free blocks in the contiguous run this
        /// block is part of, if any. Returns the first and last blocks in the run;
        /// the last block's header will be in the free space of the first block,
        /// unless the last block is the first block.
        std::pair<AllocatorBlock*, AllocatorBlock*> coalesce();
        
    protected:
        
        /// Return true if this block comes immediately before the other one, with
        /// no space between them.
        bool immediately_before(const AllocatorBlock* other) const;
    };

    /**
     * This occurs at the start of a chain, after any prefix, and lets us find the free list.
     */
    struct AllocatorHeader {
        /// Where is the first free block of memory?
        Pointer<AllocatorBlock> first_free;
        /// Where is the last free block of memory?
        Pointer<AllocatorBlock> last_free;
    };
    
    
    /**
     * For each chain, stores each mapping's start address by chain offset position.
     * Each entry is useful for bound queries.
     */
    static unordered_map<chainid_t, map<size_t, intptr_t>> chain_space_index;
    
    /**
     * For each memory start address, what mapping does it start?
     * Useful for bound queries.
     */
    static map<intptr_t, LinkRecord> address_space_index;
    
    /**
     * Mutex for a readers-writer lock on the indexes, for allocating.
     */
    static shared_timed_mutex mutex;
    
    /**
     * How big should a link be to start with.
     */
    static constexpr size_t BASE_SIZE = 1024;
    
    /**
     * Of that, how many bytes should we allow the prefix to possibly use?
     * The allocator has to be able to keep its header within the rest of the first link.
     */
    static constexpr size_t MAX_PREFIX_SIZE = 16;
    
    /**
     * How much should we expand each new link by?
     */
    static constexpr size_t SCALE_FACTOR = 2;
   
    /**
     * Create a chain with one link and no allocator setup.
     * Block will either be the entire size of an existing file, or the given starting size.
     * Returns the chain ID and a flag for if there was data in an open file to read.
     */
    static std::pair<chainid_t, bool> open_chain(int fd = 0, size_t start_size = BASE_SIZE);
    
    /**
     * Extend the given chain to the given new total size.
     */
    static void extend_chain_to(chainid_t chain, size_t new_total_size);
    
    /**
     * Add a link into a chain. The caller must hold a write lock on the manager data structures.
     * The number of bytes must be nonzero.
     */
    static LinkRecord& add_link(LinkRecord& head, size_t new_bytes);
    
    /**
     * Create a new chain, using the given file if set, and copy data from the
     * given existing chain.
     */
    static chainid_t copy_chain(chainid_t chain, int fd = 0);
   
    /**
     * Set up the allocator data structures in the first link, assuming they
     * aren't present. Put them at the given offset, and carve them out of the
     * given amoutn of remaining space in the link. 
     */
    static void set_up_allocator_at(chainid_t chain, size_t offset, size_t space);
    
    /**
     * Connect to the allocator data structures in the first link, assuming they are present.
     */
    static void connect_allocator_at(chainid_t chain, size_t offset);
    
    /**
     * Find the allocator header in a chain that has had its allocator connected.
     */
    static AllocatorHeader* find_allocator_header(chainid_t chain);
    
    /**
     * Run the given callback with the allocator header for the given chain.
     * The allocator will be locked; only one function will be run on a given
     * chain's allocator at a time.
     */
    static void with_allocator_header(chainid_t chain,
                                      const std::function<void(AllocatorHeader*)>& callback);

};

/**
 * Allocator that allocates via the yomo::Manager from the chain in which it
 * itself occurs.
 * Still deals in normal pointers, which are safe to use when not storing them
 * in the allocated memory. Pointers in the allocated memory should be
 * yomo::Pointer<T>.
 */
template<typename T>
class Allocator {

public:
    // TODO: If we want to template these like this, we need to make them able
    // to work from the stack and be the types that allocate and deallocate
    // use, if we really want to be standard-compliant.
    using pointer = Pointer<T>;
    using const_pointer = Pointer<const T>;
    using size_type = size_t;
    using value_type = T;
    
    
    Allocator() = default;
    Allocator(const Allocator& other) = default;
    Allocator(Allocator&& other) = default;
    Allocator& operator=(const Allocator& other) = default;
    Allocator& operator=(Allocator&& other) = default;
    
    /**
     * Copy an allocator, but change the allocated type.
     */
    template<typename U>
    Allocator(const Allocator<U>& alloc);
    
    /**
     * Allocate the given number of items. Ought to be near the hint.
     */
    T* allocate(size_type n, const T* hint = nullptr);
    
    /**
     * Deallocate the given number of items. Must be the same number as were allocated.
     */
    void deallocate(T* p, size_type n);

};

/**
 * Interface between normally allocated objects and chain-allocated objects.
 * Pointer to an object that is allocated at the beginning of a chain, and
 * which should allocate, if it allocates, from the chain.
 * Itself lives outside the chain. Can be null.
 *
 * T must use yomo::Pointer as its pointer type and yomo::Allocator as its
 * allocator type.
 */
template<typename T>
class UniqueMappedPointer {
public:
    UniqueMappedPointer() = default;
    UniqueMappedPointer(const UniqueMappedPointer& other) = delete;
    UniqueMappedPointer(UniqueMappedPointer&& other) = default;
    UniqueMappedPointer& operator=(const UniqueMappedPointer& other) = delete;
    UniqueMappedPointer& operator=(UniqueMappedPointer&& other) = default;
    
    operator bool () const;
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;
    operator T*();
    operator const T*() const;
    
    /**
     * Get the memory address of the referenced object, or nullptr.
     */
    T* get();
    const T* get() const;

    /**
     * Make a new default-constructed T in memory, preceeded by the given
     * prefix. Forward other arguments to the constructor.
     */
    template <typename... Args>
    void construct(const std::string& prefix = "", Args&&... constructor_args);
    
    /**
     * Point to the already-constructed T saved to the file at fd by a previous
     * save() call.
     */
    void load(int fd, const std::string& prefix = "");
    
    /**
     * Break any write-back association with a backing file and move the object
     * to non-file-backed memory.
     *
     * TODO: Allow private COW mappings by adding features to MIO, to avoid a copy.
     */
    void dissociate();
    
    /**
     * Move the stored item and all associated memory into memory mapped in the
     * given file. The pointer must not be null. No move constructors are
     * called.
     */
    void save(int fd);
    
    /**
     * Free any associated memory and become empty.
     */
    void reset();
private:
    Manager::chainid_t chain = Manager::NO_CHAIN;
};

};

/**
 * A vector of primitive items, with guaranteed ABI compatibility across
 * compilers and platforms.
 */
template<typename T, typename Alloc = std::allocator<T>>
class CompatVector {
public:

    CompatVector() = default;
    
    // Because we contain a pointer, we need a destructor and copy and move
    // constructors and assignment operators.
    ~CompatVector();
    
    CompatVector(const CompatVector& other);
    CompatVector(CompatVector&& other);
    
    CompatVector& operator=(const CompatVector& other);
    CompatVector& operator=(CompatVector&& other);

    size_t size() const;
    void resize(size_t new_size);
    void reserve(size_t new_reserved_length);
    
    /**
     * Empty out the vector and free any allocated memory.
     */
    void clear();
    T& at(size_t index);
    const T& at(size_t index) const;
    
    // TODO: reserve(), push_back()
protected:
    // We keep the allocator in ourselves, so if working in a chain we allocate
    // in the right chain.
    Alloc alloc;
    big_endian<size_t> length = 0;
    big_endian<size_t> reserved_length = 0;
    typename std::allocator_traits<Alloc>::pointer first = nullptr;
    
    static const int RESIZE_FACTOR = 2;
};

///////////////////////////////////////////////////////////////////////////////////


// Implementations

template<typename T>
big_endian<T>::big_endian() {
    for (auto& byte : storage) {
        byte = 0;
    }
#ifdef debug_big_endian
    std::cerr << "Zeroed " << sizeof(T) << " bytes for a big_endian at " << (intptr_t) this << std::endl;
#endif
    assert(*this == 0);
}

template<typename T>
big_endian<T>::big_endian(const T& value) {
    *this = value;
}

template<typename T>
big_endian<T>::operator T () const {
    
    static_assert(std::numeric_limits<T>::is_integer);

    // I had no luck partially specializing the conversion operator for all
    // integral types of a given width, so we switch and call only the
    // conversion that will work.
    // TODO: manually write all the specializations?
    // Note that signed types report 1 fewer bits (i.e. 63)
    switch (std::numeric_limits<T>::digits) {
    case 64:
    case 63:
        return (T)be64toh(*((const T*)storage));
        break;
    case 32:
    case 31:
        return (T)be32toh(*((const T*)storage));
        break;
    case 16:
    case 15:
        return (T)be16toh(*((const T*)storage));
        break;
    default:
        throw runtime_error("Unimplemented bit width: " + std::to_string(std::numeric_limits<T>::digits));
    }
}

template<typename T>
big_endian<T>& big_endian<T>::operator=(const T& x) {

    static_assert(std::numeric_limits<T>::is_integer);

    switch (std::numeric_limits<T>::digits) {
    case 64:
    case 63:
        *((T*)storage) = (T)htobe64(x);
        break;
    case 32:
    case 31:
        *((T*)storage) = (T)htobe32(x);
        break;
    case 16:
    case 15:
        *((T*)storage) = (T)htobe16(x);
        break;
    default:
        throw runtime_error("Unimplemented bit width: " + std::to_string(std::numeric_limits<T>::digits));
    }
    
#ifdef debug_big_endian
    std::cerr << "Set a big_endian at " << (intptr_t) this << " to " << x << std::endl;
#endif
    
    return *this;
}

////////////////////

template<typename T, typename Alloc>
CompatVector<T, Alloc>::~CompatVector() {
    if (first) {
        // We have some memory allocated to us.
        
        for (size_t i = 0; i < length; i++) {
            // Destroy everything in it.
            (first + i)->~T();
        }
        
        // Get rid of it.
        alloc.deallocate(first, reserved_length);
    }
}

template<typename T, typename Alloc>
CompatVector<T, Alloc>::CompatVector(const CompatVector& other) {
#ifdef debug_compat_vector
    std::cerr << "Copy-constructing a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    if (other.size() != 0) {
        first = alloc.allocate(other.size());
        reserved_length = other.size();
        length = other.size();
        
        for (size_t i = 0; i < other.size(); i++) {
            // Copy each item
            new (first + i) T(other.at(i));
        }
    }
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
}

template<typename T, typename Alloc>
CompatVector<T, Alloc>::CompatVector(CompatVector&& other) :
    length(other.length), reserved_length(other.reserved_length), first(other.first) {
    
#ifdef debug_compat_vector
    std::cerr << "Move-constructing a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    
    // And say they have no items or memory.
    other.length = 0;
    other.reserved_length = 0;
    other.first = nullptr;
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
}

template<typename T, typename Alloc>
CompatVector<T, Alloc>& CompatVector<T, Alloc>::operator=(const CompatVector& other) {
#ifdef debug_compat_vector
    std::cerr << "Copy-assigning a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    if (this != &other) {
        // TODO: can we economize and use copy constructors instead of
        // assignment here sometimes?
        resize(other.size);
        for (size_t i = 0; i < other.size(); i++) {
            this.at(i) = other.at(i);
        }
    }
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
    return *this;
}

template<typename T, typename Alloc>
CompatVector<T, Alloc>& CompatVector<T, Alloc>::operator=(CompatVector&& other) {
#ifdef debug_compat_vector
    std::cerr << "Move-assigning a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    if (this != &other) {
        // Get rid of our memory
        clear();
        
        // Steal their memory
        length = other.length;
        reserved_length = other.reserved_length;
        first = other.first;
        
        // And say they have no items or memory.
        other.length = 0;
        other.reserved_length = 0;
        other.first = nullptr;
    }
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
    return *this;
}

template<typename T, typename Alloc>
size_t CompatVector<T, Alloc>::size() const {
    return length;
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::reserve(size_t new_reserved_length) {
    // Find where the data is. Note that this may be null.
    T* old_first = first;
    // And how much there is
    size_t old_reserved_length = reserved_length;
    
#ifdef debug_compat_vector
    std::cerr << "Reserving vector at " << (intptr_t)this
        << " with " << old_reserved_length << " spaces at "
        << (intptr_t) old_first << " to have " << new_reserved_length  << " spaces" << std::endl;
#endif

    if (new_reserved_length > old_reserved_length) {
        // Allocate space for the new data, and get the position in the context
        T* new_first  = alloc.allocate(new_reserved_length);
        
        // Record the new reserved length
        reserved_length = new_reserved_length;
        
#ifdef debug_compat_vector
        std::cerr << "Vector data moving from " << (intptr_t) old_first
            << " to " << (intptr_t) new_first << std::endl;
#endif
        for (size_t i = 0; i < size(); i++) {
            // Move over the preserved values.
            new (new_first + i) T(std::move(*(old_first + i)));
        }
        
        for (size_t i = 0; i < size(); i++) {
            // Destroy all the original values
            (old_first + i)->~T();
        }
        
        if (old_first) {
            // Free old memory if we moved
            alloc.deallocate(old_first, old_reserved_length);
        }
        first = new_first;
    }
    
    // If it isn't growing, ignore it.
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::resize(size_t new_size) {
    // Find where the data is. Note that this may be null.
    T* old_first = first;
    // And how much there is
    size_t old_reserved_length = reserved_length;
    
#ifdef debug_compat_vector
    std::cerr << "Resizing vector at " << (intptr_t)this
        << " with " << size() << " items at "
        << (intptr_t) old_first << " to size " << new_size << std::endl;
#endif

    if (new_size == size()) {
        // Nothing to do!
        return;
    }

    if (new_size > reserved_length) {
        // We will need some more memory, and our data might move.
        // Make sure repeated calls to be bigger reallocate only a sensible number of times.
        reserve(std::max(new_size, size() * RESIZE_FACTOR));
    }
   
    // TODO: throw away excess memory when shrinking
   
    if (new_size < size()) {
        // We shrank, and we had at least one item.
#ifdef debug_compat_vector
        std::cerr << "Removing items between " << new_size << " and " << size() << std::endl;
#endif
        for (size_t i = new_size; i < size(); i++) {
            // For anything trimmed off, just run destructors.
            (first + i)->~T();
        }
    } else {
#ifdef debug_compat_vector
        std::cerr << "Adding items between " << size() << " and " << new_size << std::endl;
#endif
        for (size_t i = size(); i < new_size; i++) {
            // For anything aded on, just run constructor.
            // This will use value initialization
            // <https://stackoverflow.com/a/2418195> and zero everything if a
            // default constructor is not defined.
            new (first + i) T();
        }
    }
    
    // Record the new length
    length = new_size;
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::clear() {
    // Deconstruct anything we have.
    resize(0);
    if (first) {
        // We have some memory allocated to us.
        // Get rid of it.
        alloc.deallocate(first, reserved_length);
    }
    reserved_length = 0;
}

template<typename T, typename Alloc>
T& CompatVector<T, Alloc>::at(size_t index) {
    if (index >= size()) {
        // TODO: throw the right type here.
        throw std::runtime_error("Cannot get " + std::to_string(index) +
                                 " in vector of length " + std::to_string(size()));
    }
    
    return *(first + index);
}

template<typename T, typename Alloc>
const T& CompatVector<T, Alloc>::at(size_t index) const {
    // Just run non-const at and constify result
    return const_cast<CompatVector<T, Alloc>*>(this)->at(index);
}


namespace yomo {

template<typename T>
Pointer<T>::Pointer(T* destination) {
    *this = destination;
}

template<typename T>
Pointer<T>::operator bool () const {
    return position != std::numeric_limits<size_t>::max();
}

template<typename T>
T& Pointer<T>::operator*() const {
    return *get();
}

template<typename T>
T* Pointer<T>::operator->() const {
    return get();
}

template<typename T>
Pointer<T>::operator T* () const {
    return get();
}

template<typename T>
Pointer<T>& Pointer<T>::operator=(T* addr) {
    if (addr == nullptr) {
        // Adopt our special null value
        position = std::numeric_limits<size_t>::max();
    } else {
        // Get the position, requiring that it is in the same chain as us.
        position = Manager::get_position_in_same_chain(this, addr);
    }
    return *this;
}

template<typename T>
T* Pointer<T>::operator+(size_t items) const {
    return get() + items;
}

template<typename T>
T* Pointer<T>::get() const {
    if (position == std::numeric_limits<size_t>::max()) {
        return nullptr;
    } else {
        return (T*) Manager::get_address_in_same_chain((const void*) this, position);
    }
}



template<typename T>
template<typename U>
Allocator<T>::Allocator(const Allocator<U>& alloc) {
    // Nothing to do!
}

template<typename T>
auto Allocator<T>::allocate(size_type n, const T* hint) -> T* {
    return (T*) Manager::allocate_from_same_chain((void*) this, n * sizeof(T));
}

template<typename T>
void Allocator<T>::deallocate(T* p, size_type n) {
    Manager::deallocate((void*) p);
}


template<typename T>
UniqueMappedPointer<T>::operator bool () const {
    return chain != Manager::NO_CHAIN;
}

template<typename T>
T& UniqueMappedPointer<T>::operator*() {
    return *get();
}

template<typename T>
const T& UniqueMappedPointer<T>::operator*() const {
    return *get();
}

template<typename T>
T* UniqueMappedPointer<T>::operator->() {
    return get();
}

template<typename T>
const T* UniqueMappedPointer<T>::operator->() const {
    return get();
}

template<typename T>
UniqueMappedPointer<T>::operator T*() {
    return get();
}

template<typename T>
UniqueMappedPointer<T>::operator const T*() const {
    return get();
}

template<typename T>
T* UniqueMappedPointer<T>::get() {
    if (chain == Manager::NO_CHAIN) {
        return nullptr;
    } else {
        return (T*) Manager::find_first_allocation(chain, sizeof(T));
    }
}

template<typename T>
const T* UniqueMappedPointer<T>::get() const {
    if (chain == Manager::NO_CHAIN) {
        return nullptr;
    } else {
        return (T*) Manager::find_first_allocation(chain, sizeof(T));
    }
}

template<typename T>
template<typename... Args>
void UniqueMappedPointer<T>::construct(const std::string& prefix, Args&&... constructor_args) {
    // Drop any existing chain.
    reset();
    
    // Make a new chain
    chain = Manager::create_chain(prefix);
    // Allocate space in the cahin for the item.
    // Can't use the Allocator because we don't have a place in the chain to
    // store one.
    T* item = (T*) Manager::allocate_from(chain, sizeof(T));
    // Run the constructor.
    new (item) T(std::forward<Args>(constructor_args)...);
}

template<typename T>
void UniqueMappedPointer<T>::load(int fd, const std::string& prefix) {
    // Drop any existing chain.
    reset();

    chain = Manager::create_chain(fd, prefix);
}

template<typename T>
void UniqueMappedPointer<T>::dissociate() {
    if (chain == Manager::NO_CHAIN) {
        throw runtime_error("Cannot dissociate a null object");
    }
    // Copy the chain
    Manager::chainid_t new_chain = Manager::get_dissociated_chain(chain);
    // Get rid of the old chain
    Manager::destroy_chain(chain);
    // Adopt the new chain
    chain = new_chain;
}

template<typename T>
void UniqueMappedPointer<T>::save(int fd) {
    if (chain == Manager::NO_CHAIN) {
        throw runtime_error("Cannot save a null object");
    }
    // Copy the chain
    Manager::chainid_t new_chain = Manager::get_associated_chain(chain, fd);
    // Get rid of the old chain
    Manager::destroy_chain(chain);
    // Adopt the new chain
    chain = new_chain;
}

template<typename T>
void UniqueMappedPointer<T>::reset() {
    if (chain != Manager::NO_CHAIN) {
        Manager::destroy_chain(chain);
        chain = Manager::NO_CHAIN;
    }
}

}


}

#endif
