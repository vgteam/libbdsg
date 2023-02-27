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
#include <climits>
#include <iostream>
#include <functional>
#include <limits>

#include <map>
#include <unordered_map>
#include <vector>
#include <shared_mutex>

// TODO: We only target little-endian systems, like x86_64 and ARM64 Linux and
// MacOS. Porting to big-endian systems will require wrapping all the numbers
// in little_endian<T> templates or something.

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    #error The libbdsg memory-mapping system must be ported to this non-little-endian machine in order to build!
#endif

// Since the library already depends on SDSL we might as well use their fast
// bit twiddling functions.
#include <sdsl/bits.hpp>
// And we need to stock our non-mmapped backend with an int vector.
#include <sdsl/int_vector.hpp>

namespace bdsg {
    
using namespace std;

/**
 * Type enum value for selecting data structures that use the STL and SDSL.
 */
struct STLBackend {
};

/**
 * Template to choose the appropriate bit-packed integer vector for a backend.
 * Exposes the resulting type at ::type.
 */
template<typename Backend>
struct IntVectorFor {
};

/**
 * Template to choose the appropriate vector for a backend. Exposes the
 * resulting template at ::type.
 */
template<typename Backend>
struct VectorFor {
};

// Implementations for the data structures for STLBackend

template<>
struct VectorFor<STLBackend> {
    template<typename Item>
    using type = std::vector<Item>;
};

template<>
struct IntVectorFor<STLBackend> {
    using type = sdsl::int_vector<>;
};

/**
 * You Only Map Once: mapped memory with safe allocation by objects in the mapped memory.
 *
 * YOMO provides an interconnected system for file-backed memory mapping and
 * allocation, so that objects whose *this is in a mapped memory segment can
 * safely allocate more memory backed by the file the object exists in.
 * Allocating more memory will not unmap memory already allocated.
 *
 * Note that you need to follow some rules in the objects you use:
 *
 * 1. Always template on an allocator and pointer type, or use yomo::Allocator
 * and yomo::Pointer.
 *
 * 2. Don't use yomo::Allocator off of the stack, only from object storage.
 *
 * 3. Don't have a vtable. Don't inherit from anything with virtual methods, or
 * have virtual methods yourself. Note that this means you can't actually
 * inherit from libhandlegraph::HandleGraph!
 */
namespace yomo {

/**
 * Pointer to an object of type T, which lives at an address mapped by a
 * yomo::Allocator (or otherwise in a yomo::Manager's chain), and is itself
 * stored at an address mapped by a yomo::Allocator (or otherwise in the same
 * yomo::Manager chain).
 *
 * Can also work outside of any chain, which lets us make things of types that
 * use this as local variables, at the cost of not being able to fail fast
 * whenever we make a mistake and wander out of a chain.
 *
 * We interpret constness as applying to the value of the pointer and not the
 * pointed-to object. If you want a pointer to a const object, you need a
 * Pointer<const T>.
 *
 * Note that you *need* to use the constructors, destructors, and assignment
 * operators! You can't just bitwise copy this!
 */
template<typename T>
class Pointer {
public:
    /// Be constructable.
    /// Constructs as a pointer that equals nullptr.
    Pointer();
    
    Pointer(T* destination);
    
    // Copy and move and comparison has to go through actual memory addresses
    Pointer(const Pointer& other);
    Pointer(Pointer&& other);
    Pointer& operator=(const Pointer& other);
    Pointer& operator=(Pointer&& other);
     
    // Be a good pointer
    operator bool () const;
    // We use this template instead of T& to let us have a Pointer<void>.
    std::add_lvalue_reference<T> operator*() const;
    T* operator->() const;
    operator T* () const;
    Pointer<T>& operator=(T* addr);
    T* operator+(size_t items) const;
    
    // Expose the address of the pointed-to object not through manual operator
    // calling.
    T* get() const;
    
protected:
    /// Stores the offset from this pointer to the pointed-to object in the
    /// chain, or the max int64_t if the pointer is null.
    int64_t offset;
    
    // TODO: Replace this hack with a C++20 atomic_ref when possible.
    // For now, we check the size, and then assume that an atomic and the
    // item behind it are bitwise compatible.
    static_assert(sizeof(std::atomic<bool>) == sizeof(bool),
        "bdsg::yomo::Pointer requires atomic access to bool");
    
    /// Stores true if the destination is known to be in the same link as the
    /// pointer, if the pointer is not null. Once two things are in the same
    /// link, they never end up in different links on subsequent loads. Needs
    /// to be mutable and atomic because it is updated on reads if we find that
    /// the destination is now in the same link.
    mutable std::atomic<bool> local;
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
     * Set this to true to enable additional self-checks on memory management
     * correctness, and false to disable them.
     */
    static bool check_chains;

    /**
     * Create a chain not backed by any file. The given prefix data will be
     * placed before the chain allocator data structures.
     */
    static chainid_t create_chain(const std::string& prefix = "");
    
    /**
     * Create a chain by mapping all of the given open file. The file must
     * begin with the given prefix, if specified, or an error will occur.
     *
     * Modifications to the chain will affect the file, and it will grow as
     * necessary. If the file is read-only, and an attempt is made to modify
     * the chain or any data in it, a memory protection error (segmentation
     * fault) will occur.
     *
     * The Manager will not take ownership of the file descriptor.
     *
     * If the file is nonempty, data after the length of the passed prefix must
     * contain the chain allocator data structures. If it is empty, the prefix
     * and the chain allocator data structures will be written to it.
     *
     * The entire file will be mapped in one contiguous link.
     */
    static chainid_t create_chain(int fd, const std::string& prefix = "");
    
    /**
     * Create a chain by calling the given function until it returns an empty
     * string, and concatenating all the results.
     *
     * The result must begin with the given prefix, if specified, or an error
     * will occur.
     *
     * All content from the iterator will be stored in one contiguous link,
     * despite the length not being known ahead of time.
     */
    static chainid_t create_chain(const std::function<std::string(void)>& iterator, const std::string& prefix = "");
    
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
     *
     * Also drops trailing free blocks and truncates them out of the backing
     * file.
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
     * For NO_CHAIN just uses an offset from address 0 in memory.
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
     * For NO_CHAIN just uses an offset from address 0 in memory.
     */
    static void* get_address_in_same_chain(const void* here, size_t position);
    
    /**
     * Find the position of the given address in the chain that here exists in.
     * For NO_CHAIN just uses an offset from address 0 in memory.
     */
    static size_t get_position_in_same_chain(const void* here, const void* address);
    
    /**
     * Find the offset from the given here to the given address, constraining
     * them to come from the same chain (or NO_CHAIN). Also returns a flag that
     * is true if the offset in the chain equals the offset in memory, and
     * false otherwise.
     */
    static std::pair<int64_t, bool> get_offset_in_same_chain(const void* here, const void* address);
    
    /**
     * Apply an offset from the given here to get an address, within the chain
     * that here is in (or NO_CHAIN). Also returns a flag that is true if the
     * offset in the chain equals the offset in memory, and false otherwise.
     */
    static std::pair<void*, bool> follow_offset_in_same_chain(const void* here, int64_t offset);
    
    /**
     * Allocate the given number of bytes from the given chain.
     * For NO_CHAIN just allocates with malloc().
     */
    static void* allocate_from(chainid_t chain, size_t bytes);
    
    /**
     * Allocate the given number of bytes from the chain containing the given
     * address.
     * For NO_CHAIN just allocates with malloc().
     */
    static void* allocate_from_same_chain(void* here, size_t bytes);
    
    /**
     * Tell the memory management subsystem that an entire chain should be
     * loaded, if possible.
     *
     * If blocking is set to true, actually read from each page in the chain,
     * or make a syscall with similar effect, before returning.
     */
    static void preload_chain(chainid_t chain, bool blocking = false);
    
    /**
     * Free the given allocated block in the chain to which it belongs.
     * For NO_CHAIN just frees with free().
     */
    static void deallocate(void* address);
    
    /**
     * Find the mapped address of the first thing allocated in the chain, given
     * that it was allocated with the given size.
     *
     * That first allocated thing must exist and not be deallocated.
     *
     * Must not be called for NO_CHAIN.
     */
    static void* find_first_allocation(chainid_t chain, size_t bytes);
    
    /**
     * Return the total number of bytes in the given chain.
     */
    static size_t get_chain_size(chainid_t chain);
    
    /**
     * Return true if the chain cna be written to, and false if it can only be
     * accessed read-only.
     */
    static bool is_chain_writable(chainid_t chain);
    
    /**
     * Get statistics about the memory in a chain. Returns all 0s if not a managed chain.
     *
     * Returns the total bytes in the chain, the number of free bytes,
     * and the number of free bytes reclaimable when the chain is closed. 
     */
    static std::tuple<size_t, size_t, size_t> get_usage(chainid_t chain);
    
    /**
     * Scan all memory regions in the given chain. Calls the iteratee with each
     * region's start address and length, in order.
     */
    static void scan_chain(chainid_t chain, const std::function<void(const void*, size_t)>& iteratee);
    
    /**
     * Dump information about free and allocated memory in the given chain.
     * Not thread safe!
     */
    static void dump(chainid_t chain);
    
    /**
     * Dump information about where all chain links fall in memory into the given stream.
     * Not thread safe!
     */
    static void dump_links(ostream& out = std::cerr);
    
    /**
     * Walk all allocated and free blocks in the heap in the chain and make
     * sure that they are actually in memory. If not, throws std::runtime_error.
     */
    static void check_heap_integrity(chainid_t chain);
    
    /**
     * Return the total number of chains that exist right now.
     */
    static size_t count_chains();
    
    /**
     * Return the total number of links that exist right now across all chains.
     */
    static size_t count_links();
    
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
        /// Size of the block in bytes, not counting this header. Used for free
        /// and allocated blocks.
        size_t size;
        
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
     *
     * Since the location of the first allocation is dependent on the size of
     * the first mapping in the chain, we also keep track of the address of the
     * first allocation. It's not allowed to be deallocated or Bad Things will
     * happen.
     */
    struct AllocatorHeader {
        /// Where is the first free block of memory?
        Pointer<AllocatorBlock> first_free;
        /// Where is the last free block of memory?
        Pointer<AllocatorBlock> last_free;
        /// Where is the first object allocated?
        Pointer<void> first_allocated;
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
     *
     * If link_data is set, the chain must not be file-backed, and link_data
     * it must point to a block of memory of length start_size already allocated
     * using malloc() and which can be freed using free(). The chain will take
     * ownership of the memory block.
     *
     * If the FD is not writable, this will be detected, and memory will be
     * mapped read-only.
     */
    static std::pair<chainid_t, bool> open_chain(int fd = 0, size_t start_size = BASE_SIZE, void* link_data = nullptr);
    
    /**
     * Extend the given chain to the given new total size.
     */
    static void extend_chain_to(chainid_t chain, size_t new_total_size);
    
    /**
     * Get the address of the given byte from the start of the chain, and the
     * number of contiguous bytes after it. The number of bytes is always 1 or
     * more if the address is not past the end of the chain.
     */
    static std::pair<void*, size_t> get_address_and_length_in_chain(chainid_t chain, size_t position);
    
    /**
     * Add a link into a chain. The caller must hold a write lock on the manager data structures.
     * The number of bytes must be nonzero.
     *
     * If link_data is set, the chain must not be filoe-backed, and link_data
     * it must point to a block of memory of length new_bytes already allocated
     * using malloc() and which can be freed using free(). The chain will take
     * ownership of the memory block.
     */
    static LinkRecord& add_link(LinkRecord& head, size_t new_bytes, void* link_data = nullptr);
    
    /**
     * Find the link overlapping the given address, or
     * address_space_index.end() if no link overlaps the given address. Must be
     * called while you hold a read lock on the chain data structures.
     */
    static std::map<intptr_t, LinkRecord>::iterator find_link(std::shared_lock<std::shared_timed_mutex>& lock, const void* address);
    
    /**
     * Create a new chain, using the given file if set, and copy data from the
     * given existing chain.
     */
    static chainid_t copy_chain(chainid_t chain, int fd = 0);
   
    /**
     * Set up the allocator data structures in the first link, assuming they
     * aren't present. Put them at the given offset, and carve them out of the
     * given amount of remaining space in the link. 
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
                                      
    /**
     * While a final free block exists, drop it from the free list.
     * Return the total number of bytes after the last allocated block.
     *
     * Note that you should not map anything after calling this function! You
     * should close the chain and trim down the backing file!
     */
    static size_t reclaim_tail(chainid_t chain);
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
    using difference_type = size_t;
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
     * A template we can use to re-instantiate this allocator for a different
     * type.
     */
    template<typename U>
    struct rebind {
        using other = Allocator<U>; 
    };
    
    /**
     * Return true if the two allocators can allocate and deallocate each
     * others' memory, and false otherwise.
     */
    bool operator==(const Allocator& other) const;
    
    /**
     * Return false if the two allocators can allocate and deallocate each
     * others' memory, and true otherwise.
     */
    bool operator!=(const Allocator& other) const;
    
    /**
     * Allocate the given number of items. Ought to be near the hint.
     */
    T* allocate(size_type n, const T* hint = nullptr);
    
    /**
     * Deallocate the given number of items. Must be the same number as were allocated.
     */
    void deallocate(T* p, size_type n);
    
    /**
     * Work out the maximum size that can be allocated.
     */
    size_t max_size() const;
    
private:
    
    /**
     * Determine the chain that an allocator will allocate from.
     */
    Manager::chainid_t get_chain() const;
    

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
    ~UniqueMappedPointer();
    
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

protected:

    /**
     * Make a new constructed T in memory, preceeded by the given
     * prefix. Forward other arguments to the constructor.
     */
    template <typename... Args>
    void construct_internal(const std::string& prefix, Args&&... constructor_args);

public:

    /**
     * Make a new default-constructed T in memory, with an empty preceeding
     * prefix.
     */
    void construct();

    /**
     * Make a new default-constructed T in memory, preceeded by the given
     * prefix.
     */
    void construct(const std::string& prefix);

    /**
     * Make a new constructed T in memory, preceeded by the given
     * prefix. Forward other arguments to the constructor.
     *
     * If you want to use arguments you must pass a prefix, although it may be
     * empty. Otherwise we get into trouble with templates replacing overloads.
     */
    template <typename... Args>
    void construct(const std::string& prefix, Args&&... constructor_args);
    
    /**
     * Point to the already-constructed T saved to the file at fd by a previous
     * save() call. The file must begin with the given prefix, or an error will
     * occur.
     */
    void load(int fd, const std::string& prefix);
    
    /**
     * Load into memory and point to the already-constructed T saved to the
     * given stream by a previous save() call. The stream must begin with the
     * given prefix, or an error will occur.
     */
    void load(std::istream& in, const std::string& prefix);
    
    /**
     * Load into memory and point to the already-constructed T emitted in
     * blocks by the given function. The data must begin with the given prefix,
     * or an error will occur.
     */
    void load(const std::function<std::string(void)>& iterator, const std::string& prefix);
    
    /** 
     * Load into memory and point to the already-constructed T saved to the
     * given stream by a previous save() call. The stream is expected to have
     * had the prefix read from it already, but the prefix must still be
     * provided.
     */
    void load_after_prefix(std::istream& in, const std::string& prefix);
    
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
     * Save the stored item to the given stream. The pointer must not be null.
     */
    void save(std::ostream& out) const;
    
    /**
     * Save the stored item as blocks of data shown to the given function. The
     * pointer must not be null.
     */
    void save(const std::function<void(const void*, size_t)>& iteratee) const;
    
    /**
     * Save the stored item to the given stream. The pointer must not be null.
     * The stream is expected to have already had the prefix written to it.
     */
    void save_after_prefix(std::ostream& out, const std::string& prefix) const;
    
    /**
     * Tell the memory management subsystem that the entire memory arena should
     * be loaded.
     *
     * If blocking is set to true, actually read from each page in the chain,
     * or make a syscall with similar effect, before returning.
     */
    void preload(bool blocking = false) const;
    
    /**
     * Free any associated memory and become empty.
     */
    void reset();
    
    /**
     * Get statistics about the pointer's associated memory chain.
     *
     * Returns the total bytes, the number of free bytes, and the number of
     * free bytes reclaimable when closed as a mapped file. 
     */
    std::tuple<size_t, size_t, size_t> get_usage();
    
    /**
     * Make sure that internal heap data structures are consistent with the
     * memory mapping. Raises std::runtime_error if not.
     */
    void check_heap_integrity();
    
private:
    /// The memory chain managed by this pointer.
    Manager::chainid_t chain = Manager::NO_CHAIN;
    /// The address of the pointed-to value, as mapped into memory.
    T* cached_value = nullptr;
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
    
    /// Allow copying within the same allocator
    CompatVector& operator=(const CompatVector& other);
    
    /// Allow copying across allocators.
    template<typename OtherAlloc>
    CompatVector& operator=(const CompatVector<T, OtherAlloc>& other);
    
    /// Alow moving within the same allocator.
    CompatVector& operator=(CompatVector&& other);
    
    size_t size() const;
    size_t capacity() const;
    void resize(size_t new_size);
    void reserve(size_t new_reserved_length);
    void shrink_to_fit();
    
    T& back();
    const T& back() const;
    
    void pop_back();
    
    template<typename... Args>
    void emplace_back(Args&&... args);
    
    /**
     * Empty out the vector and free any allocated memory.
     */
    void clear();
    T& at(size_t index);
    const T& at(size_t index) const;
    
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    
    using value_type = T;
    
    // We have iterators but they aren't safe to serialize.
    using iterator = T*;
    using const_iterator = const T*;
    
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    
    // Allow fast access to the data block
    
    /// Get the pointer to the first item in the contiguous array, or nullptr
    /// if no array is allocated.
    inline T* get_first() {
        return (T*)first;
    }
    /// Get the pointer to the first item in the contiguous array, or nullptr
    /// if no array is allocated.
    inline const T* get_first() const {
        return (const T*)first;
    }
    
    // Compatibility with SDSL-lite serialization
    
    /**
     * Serialize the data to the given stream.
     */
    void serialize(std::ostream& out) const;
    
    /**
     * Load the data from the given stream.
     */
    void load(std::istream& in);
    
protected:
    // We keep the allocator in ourselves, so if working in a chain we allocate
    // in the right chain.
    Alloc alloc;
    size_t length = 0;
    size_t reserved_length = 0;
    typename std::allocator_traits<Alloc>::pointer first = nullptr;
    
    static const int RESIZE_FACTOR = 2;
};

/**
 * Vector mapped in from a file.
 */
template<typename T>
using MappedVector = CompatVector<T, yomo::Allocator<T>>; 

/**
 * An int vector that is mostly API-compatible with SDSL's int vectors, but
 * which can exist in a memory mapping and uses the given allocator and its
 * pointers.
 *
 * Does *NOT* automatically adjust its width; you must call width() or repack()
 * to set the appropriate bit width for your values *before* you write them.
 * Consider a PagedVector if this seems hard, or if your values come in runs of
 * differing widths. Width defaults to 1 bit.
 */
template<typename Alloc = std::allocator<uint64_t>>
class CompatIntVector {
public:
    
    CompatIntVector() = default;
    
    /// Allow copy construction
    CompatIntVector(const CompatIntVector& other);
    /// Allow move construction
    CompatIntVector(CompatIntVector&& other);
    
    // To allow copy across backends, we need to be friends with other
    // instantiations of us.
    template<typename OtherAlloc> friend class CompatIntVector;
    
    /// Allow copy construction across allocators
    template<typename OtherAlloc>
    CompatIntVector(const CompatIntVector<OtherAlloc>& other);
    
    /// Allow copy assignment across allocators
    template<typename OtherAlloc>
    CompatIntVector& operator=(const CompatIntVector<OtherAlloc>& other);
    
    /// Allow copy construction from anything with the right methods
    template<typename OtherType>
    CompatIntVector(const OtherType& other);
    
    /// Allow copy assignment from anything with the right methods
    template<typename OtherType>
    CompatIntVector& operator=(const OtherType& other);
    
    /// Allow copy assignment
    CompatIntVector& operator=(const CompatIntVector& other);
    
    /// Allow move assignment
    CompatIntVector& operator=(CompatIntVector&& other);
    
    /**
     * Return the number of integers in the vector.
     */
    size_t size() const;
    
    /**
     * Resize the vector to hold the given number of elements.
     */
    void resize(size_t new_size);
    
    /**
     * Return the number of elements the vector can hold without reallocating.
     */
    size_t capacity() const;
    
    /**
     * Pre-allocate space to hold the given number of elements.
     */
    void reserve(size_t new_reserved_length);
    
    /**
     * Drop everything from the vector.
     */
    void clear();

    /**
     * Return the width in bits of the entries.
     */
    size_t width() const;
    /**
     * Set the width in bits of the entries, causing existing data to be
     * reinterpreted.
     *
     * Throws if any stored entry cannot be represented with the new width.
     */
    void width(size_t new_width);
    /**
     * Repack existing data, re-encoding it with the given width, and
     * simultaneously resize to the new given number of elements.
     *
     * Throws if any stored entry cannot be represented with the new width.
     */
    void repack(size_t new_width, size_t new_size);
    
    /**
     * Actual accessor method that sets the value at a position.
     * Does not check if value actually fits.
     * Uses the given width instead of the stored width to write the value.
     *
     * Throws if the value will not fit in the relevant number of bits.
     */
    void pack(size_t index, uint64_t value, size_t width);
    
    /**
     * Actual accessor method that gets the value at a position.
     * Uses the given width instead of the stored width to read the value.
     */
    uint64_t unpack(size_t index, size_t width) const;
    
    /**
     * Proxy that acts as a mutable reference to an entry in the vector.
     */
    class Proxy {
    protected:
        // What vector are we operating on?
        CompatIntVector& parent;
        // What location do we refer to?
        size_t index;
    public:
        // Must be movable via constructor for by-value return
        Proxy(Proxy&& other) = default;
        
        // But generally not movable or copyable
        Proxy(const Proxy& other) = delete;
        Proxy& operator=(const Proxy& other) = delete;
        Proxy& operator=(Proxy&& other) = delete;
        
        /// Make a proxy for the entry at the given position in the given int
        /// vector
        Proxy(CompatIntVector& parent, size_t index);
        
        /// Get the value the proxy refers to
        operator uint64_t () const;
        
        /// Set the value the proxy refers to
        Proxy& operator=(uint64_t new_value);
    };
    
    using reference = Proxy;
    
    /**
     * Proxy that acts as an immutable reference to an entry in the vector.
     */
    class ConstProxy {
    protected:
        // What vector are we operating on?
        const CompatIntVector& parent;
        // What location do we refer to?
        size_t index;
    public:
        // Must be movable via constructor for by-value return
        ConstProxy(ConstProxy&& other) = default;
        
        // But generally not movable or copyable
        ConstProxy(const ConstProxy& other) = delete;
        ConstProxy& operator=(const ConstProxy& other) = delete;
        ConstProxy& operator=(ConstProxy&& other) = delete;
        
        /// Make a proxy for the entry at the given position in the given int
        /// vector
        ConstProxy(const CompatIntVector& parent, size_t index);
        
        /// Get the value the proxy refers to
        operator uint64_t () const;
    };
    
    using const_reference = ConstProxy;
    
    /**
     * Get a proxy reference to read or write the given index.
     * Checks bounds, but not value width.
     */
    Proxy at(size_t index);
    /**
     * Get a proxy reference to read the given index.
     * Checks bounds.
     */
    ConstProxy at(size_t index) const;
    
    /**
     * Get a proxy reference to read or write the given index.
     */
    Proxy operator[](size_t index);
    /**
     * Get a proxy reference to read the given index.
     */
    ConstProxy operator[](size_t index) const;
    
    // Compatibility with SDSL-lite serialization
    
    /**
     * Serialize the data to the given stream.
     */
    void serialize(std::ostream& out) const;
    
    /**
     * Load the data from the given stream.
     */
    void load(std::istream& in);
    
protected:
    /// How many items are stored?
    size_t length = 0;
    /// How many bits are used to represent each item?
    size_t bit_width = 1;
    
    /// We store our actual data in this vector, which manages memory for us.
    CompatVector<uint64_t, Alloc> data;
    
};

/**
 * Int vector mapped in from a file.
 */
using MappedIntVector = CompatIntVector<yomo::Allocator<uint64_t>>; 

/**
 * Type enum value for selecting data structures that use memory-mappable data
 * structures but with the standard allocator.
 */
struct CompatBackend {
};

/**
 * Type enum value for selecting data structures that use YOMO memory mapping
 * and the YOMO allocator.
 *
 * They can safely exist outside of mapped memory, but are probably slower
 * there since YOMO's internal tables still need to be consulted when following
 * pointers.
 */
struct MappedBackend {
};

// Implementations for the data structures for MappedBackend

template<>
struct VectorFor<MappedBackend> {
    template<typename Item>
    using type = MappedVector<Item>;
};

template<>
struct IntVectorFor<MappedBackend> {
    using type = MappedIntVector;
};




// Implementations for the data structures for CompatBackend

template<>
struct VectorFor<CompatBackend> {
    template<typename Item>
    using type = CompatVector<Item>;
};

template<>
struct IntVectorFor<CompatBackend> {
    using type = CompatIntVector<>;
};

///////////////////////////////////////////////////////////////////////////////////


// Implementations

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
CompatVector<T, Alloc>::CompatVector(CompatVector&& other) {

#ifdef debug_compat_vector
    std::cerr << "Move-constructing a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif

    // We need to make sure that we and the other vector are in the same memory
    // arena (i.e. same chain or lack of chain), if applicable.
    
    if (alloc == other.alloc) {
        // We can accept whatever they have allocated.
        
        // Take everything out of other
        length = other.length;
        reserved_length = other.reserved_length;
        first = other.first;
    
        // And say they have no items or memory.
        other.length = 0;
        other.reserved_length = 0;
        other.first = nullptr;
    } else {
        // Fall back on copy assignment
        CompatVector& to_copy = other;
        *this = to_copy;
    }
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
}

template<typename T, typename Alloc>
CompatVector<T, Alloc>& CompatVector<T, Alloc>::operator=(const CompatVector& other) {
#ifdef debug_compat_vector
    std::cerr << "Copy-assigning a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    if ((void*)this != (void*)&other) {
        // Get rid of our memory
        clear();
        // Get some new memory
        reserve(other.size());
        
        for (size_t i = 0; i < other.size(); i++) {
            // Copy construct each thing.
            new (first + i) T(other[i]);
        }
        
        length = other.size();
    }
#ifdef debug_compat_vector
    std::cerr << "Result is of size " << size() << std::endl;
#endif
    return *this;
}

template<typename T, typename Alloc>
template<typename OtherAlloc>
CompatVector<T, Alloc>& CompatVector<T, Alloc>::operator=(const CompatVector<T, OtherAlloc>& other) {
#ifdef debug_compat_vector
    std::cerr << "Copy-assigning a vector of size " << other.size() << " from " << (intptr_t)&other << " to " << (intptr_t)this << std::endl;
#endif
    if ((void*)this != (void*)&other) {
        // Get rid of our memory
        clear();
        // Get some new memory
        reserve(other.size());
        
        for (size_t i = 0; i < other.size(); i++) {
            // Copy construct each thing.
            new (first + i) T(other[i]);
        }
        
        length = other.size();
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
    if ((void*)this != (void*)&other) {
        if (alloc == other.alloc) {
            // We can safely use their memory.
            
            // Get rid of our memory.
            clear();
        
            // Steal their memory
            length = other.length;
            reserved_length = other.reserved_length;
            first = other.first;
            
            // And say they have no items or memory.
            other.length = 0;
            other.reserved_length = 0;
            other.first = nullptr;
        } else {
            // Fall back on copy assignment
            CompatVector& to_copy = other;
            *this = to_copy;
        }
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
size_t CompatVector<T, Alloc>::capacity() const {
    return reserved_length;
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

        if (yomo::Manager::check_chains) {
            // make sure we got back memory in the right chain, and that all
            // our notions of the right chain agree.
            auto new_chain = yomo::Manager::get_chain(new_first);
            assert(new_chain == yomo::Manager::get_chain(&alloc));
            assert(new_chain == yomo::Manager::get_chain(this));
            assert(new_chain == yomo::Manager::get_chain(&first));
        }
        
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
void CompatVector<T, Alloc>::shrink_to_fit() {
    // Actually reallocate smaller.
    if (length == reserved_length) {
        // Nothing to do!
        return;
    }
    
    // TODO: unify some code with reserve()?
    
    // Find where the data is. This can't be null since we have a
    // reserved_length > length (since it isn't equal and can never be less).
    T* old_first = first;
    // And how much there is
    size_t old_reserved_length = reserved_length;
    
#ifdef debug_compat_vector
    std::cerr << "Shrinking vector at " << (intptr_t)this
        << " with " << old_reserved_length << " spaces at "
        << (intptr_t) old_first << " to have " << length  << " spaces" << std::endl;
#endif
    
    // Allocate space for the new data, and get the position in the context
    T* new_first  = alloc.allocate(length);
    
    // Record the new reserved length
    reserved_length = length;
    
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
    
    // Free old memory
    alloc.deallocate(old_first, old_reserved_length);
    
    first = new_first;
}

template<typename T, typename Alloc>
T& CompatVector<T, Alloc>::back() {
    return (*this)[size() - 1];
}

template<typename T, typename Alloc>
const T& CompatVector<T, Alloc>::back() const {
    return (*this)[size() - 1];
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::pop_back() {
    resize(size() - 1);
}

template<typename T, typename Alloc>
template<typename... Args>
void CompatVector<T, Alloc>::emplace_back(Args&&... args) {
    if (reserved_length <= length) {
        // Make sure we're big enough
        reserve(std::max((size_t) 1, reserved_length * RESIZE_FACTOR));
    }
    
    // Make the new item in place
    new (first + length) T(std::forward<Args>(args)...);
    // Record that we have it.
    length = length + 1;
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::clear() {
    // Deconstruct anything we have.
    resize(0);
    if (first) {
        // We have some memory allocated to us.
        // Get rid of it.
        alloc.deallocate(first, reserved_length);
        // And remember it is gone
        first = nullptr;
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
    
    return (*this)[index];
}

template<typename T, typename Alloc>
auto CompatVector<T, Alloc>::begin() -> iterator {
    return &(*this)[0];
}

template<typename T, typename Alloc>
auto CompatVector<T, Alloc>::begin() const -> const_iterator {
    return &(*this)[0];
}

template<typename T, typename Alloc>
auto CompatVector<T, Alloc>::end() -> iterator {
    return &(*this)[size()];
}

template<typename T, typename Alloc>
auto CompatVector<T, Alloc>::end() const -> const_iterator {
    return &(*this)[size()];
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::serialize(std::ostream& out) const {
    
    // Dump the length
    out.write((const char*)&length, sizeof(length));
    // And all the set items
    out.write((const char*)(T*)first, length * sizeof(T));
}

template<typename T, typename Alloc>
void CompatVector<T, Alloc>::load(std::istream& in) {
    // Start empty
    this->clear();

    // Determine the length
    decltype(length) new_len;
    in.read((char*)&new_len, sizeof(new_len));
    
    // Make sure we have that space
    resize(new_len);
    
    // Fill in the items
    in.read((char*)(T*)first, new_len * sizeof(T));
}


template<typename T, typename Alloc>
const T& CompatVector<T, Alloc>::at(size_t index) const {
    // Just run non-const at and constify result
    return const_cast<CompatVector<T, Alloc>*>(this)->at(index);
}


template<typename T, typename Alloc>
T& CompatVector<T, Alloc>::operator[](size_t index) {
    return *(first + index);
}
 

template<typename T, typename Alloc>
const T& CompatVector<T, Alloc>::operator[](size_t index) const {
    // Just run non-const at and constify result
    return (*const_cast<CompatVector<T, Alloc>*>(this))[index];
}



template<typename Alloc>
CompatIntVector<Alloc>::CompatIntVector(const CompatIntVector& other) :
    length(other.length), bit_width(other.bit_width), data(other.data) {
    // Nothing to do!
}

template<typename Alloc>
CompatIntVector<Alloc>::CompatIntVector(CompatIntVector&& other) :
    length(other.length), bit_width(other.bit_width), data(std::move(other.data)) {
    
    // Say they have no items or memory.
    other.clear();
}

template<typename Alloc>
template<typename OtherAlloc>
CompatIntVector<Alloc>::CompatIntVector(const CompatIntVector<OtherAlloc>& other) : length(other.length), bit_width(other.bit_width), data(other.data) {
    // Nothing to do!
}


template<typename Alloc>
template<typename OtherAlloc>
CompatIntVector<Alloc>& CompatIntVector<Alloc>::operator=(const CompatIntVector<OtherAlloc>& other) {
    // Copy their contents
    length = other.length;
    bit_width = other.bit_width;
    data = other.data;
    
    return *this;
}

template<typename Alloc>
template<typename OtherType>
CompatIntVector<Alloc>::CompatIntVector(const OtherType& other) {
    // Fall back on copy assignment
    *this = other;
}


template<typename Alloc>
template<typename OtherType>
CompatIntVector<Alloc>& CompatIntVector<Alloc>::operator=(const OtherType& other) {
    clear();
    width(other.width());
    resize(other.size());
    for (size_t i = 0; i < size(); i++) {
        // Copy all the entries, repacking each.
        // TODO: accelerate somehow if we can get direct access to bit-packed data.
        (*this)[i] = other[i];
    }
    return *this;
}

template<typename Alloc>
CompatIntVector<Alloc>& CompatIntVector<Alloc>::operator=(const CompatIntVector& other) {
    // Copy their contents
    length = other.length;
    bit_width = other.bit_width;
    data = other.data;
    
    return *this;
}

template<typename Alloc>
CompatIntVector<Alloc>& CompatIntVector<Alloc>::operator=(CompatIntVector&& other) {
    // Steal their contents
    length = other.length;
    bit_width = other.bit_width;
    data = std::move(other.data);
    
    // Say they have no items or memory.
    other.clear();
    
    return *this;
}

template<typename Alloc>
size_t CompatIntVector<Alloc>::size() const {
    return length;
}

template<typename Alloc>
void CompatIntVector<Alloc>::resize(size_t new_size) {
    // Work how many slots we need in the backing vector for this, rounding up.
    size_t item_slots = (new_size * width() + (std::numeric_limits<size_t>::digits - 1)) /
        std::numeric_limits<uint64_t>::digits;
    // Make sure we have that many slots
    data.resize(item_slots);
    // And say how big we are
    length = new_size;
}

template<typename Alloc>
size_t CompatIntVector<Alloc>::capacity() const {
    if (width() == 0) {
        // This is sort of nonsense.
        return 0;
    } else {
        // Work out how many items could be packed into the bits we have reserved.
        return data.capacity() * std::numeric_limits<uint64_t>::digits / width();
    }
}

template<typename Alloc>
void CompatIntVector<Alloc>::reserve(size_t new_reserved_length) {
    // Work how many slots we need in the backing vector for this, rounding up.
    size_t item_slots = (new_reserved_length * width() + (std::numeric_limits<uint64_t>::digits - 1)) /
        std::numeric_limits<uint64_t>::digits;
    // Reserve space in the backing vector
    data.reserve(item_slots);
}

template<typename Alloc>
void CompatIntVector<Alloc>::clear() {
    // Reset to our initial state
    length = 0;
    bit_width = 1;
    data.clear();
}

template<typename Alloc>
size_t CompatIntVector<Alloc>::width() const {
    return bit_width;
}

template<typename Alloc>
void CompatIntVector<Alloc>::width(size_t new_width) {
    // Save the new bit width, causing reinterpretation.
    bit_width = new_width;
    // TODO: make sure length won't take us out of bounds now?
    // TODO: prevent a width of 0?
}

template<typename Alloc>
void CompatIntVector<Alloc>::repack(size_t new_width, size_t new_size) {
    // TODO: Actually combine these operations to save a pass and an allocation
    resize(new_size);
    
    size_t old_width = bit_width;
    
    if (new_width == old_width) {
        // Nothing to do!
        return;
    }
    
    assert(new_width > 0);
    
    // Work out how many slots we need in backing storage.
    size_t new_entries = (length * new_width + (std::numeric_limits<uint64_t>::digits - 1)) /
        std::numeric_limits<uint64_t>::digits;
    
    if (new_width > old_width) {
        // We can expand in place
        data.resize(new_entries);
        
        // Starting at the end, to avoid overwriting
        size_t i = length;
        while (i > 0) {
            i--;
            // Copy all the data to the new size
            pack(i, unpack(i, old_width), new_width);
        }
    } else {
        // Must be shrinking
        
        // Starting at the beginning, to avoid overwriting
        for (size_t i = 0; i < length; i++) {
            // Copy all the data to the new size
            pack(i, unpack(i, old_width), new_width);
        }
        
        // Now shrink down
        data.resize(new_entries);
    }
    
    // And save the new width
    width(new_width);
}

template<typename Alloc>
void CompatIntVector<Alloc>::pack(size_t index, uint64_t value, size_t width) {
    if (width < std::numeric_limits<uint64_t>::digits && value) {
        // It is possible we have been given a number that we cannot represent.
        // Use the compiler's count leading zeroes function, which hopefully it has.
        size_t needed_bits = std::numeric_limits<uint64_t>::digits - __builtin_clzll(value);
        if (needed_bits > width) {
            // The value will not fit.
            throw std::invalid_argument("Need " + std::to_string(needed_bits) +
                                        " bits to represent value " + std::to_string(value) +
                                        " but only have " + std::to_string(width)); 
        }
    }
    
    // Find the bit index we start at
    size_t start_bit = index * width;
    // Use the last 6 bits (up to 64) for the offset in the 64-bit word, and
    // the others for the word number.
    sdsl::bits::write_int(data.get_first() + (start_bit >> 6), value, start_bit & 0x3F, width);
}

template<typename Alloc>
uint64_t CompatIntVector<Alloc>::unpack(size_t index, size_t width) const {
    // Find the bit index we start at
    size_t start_bit = index * width;
    // And then load.
    // Use the last 6 bits (up to 64) for the offset in the 64-bit word, and
    // the others for the word number.
    return sdsl::bits::read_int(data.get_first() + (start_bit >> 6), start_bit & 0x3F, width);
}

template<typename Alloc>
CompatIntVector<Alloc>::Proxy::Proxy(CompatIntVector& parent, size_t index) : parent(parent), index(index) {
    // Nothing to do!
}

template<typename Alloc>
CompatIntVector<Alloc>::Proxy::operator uint64_t () const {
    return parent.unpack(index, parent.bit_width);
}

template<typename Alloc>
auto CompatIntVector<Alloc>::Proxy::operator=(uint64_t new_value) -> Proxy& {
    parent.pack(index, new_value, parent.bit_width);
    return *this;
}

template<typename Alloc>
CompatIntVector<Alloc>::ConstProxy::ConstProxy(const CompatIntVector& parent, size_t index) : parent(parent), index(index) {
    // Nothing to do!
}

template<typename Alloc>
CompatIntVector<Alloc>::ConstProxy::operator uint64_t () const {
    return parent.unpack(index, parent.bit_width);
}

template<typename Alloc>
auto CompatIntVector<Alloc>::at(size_t index) -> Proxy {
    if (index > size()) {
        throw std::out_of_range("Accessing index " + std::to_string(index) +
            " in integer vector of length " + std::to_string(size()));
    }
    return Proxy(*this, index);
}

template<typename Alloc>
auto CompatIntVector<Alloc>::at(size_t index) const -> ConstProxy {
    if (index > size()) {
        throw std::out_of_range("Accessing index " + std::to_string(index) +
            " in integer vector of length " + std::to_string(size()));
    }
    return ConstProxy(*this, index);
}

template<typename Alloc>
auto CompatIntVector<Alloc>::operator[](size_t index) -> Proxy {
    return Proxy(*this, index);
}

template<typename Alloc>
auto CompatIntVector<Alloc>::operator[](size_t index) const -> ConstProxy {
    return ConstProxy(*this, index);
}

template<typename Alloc>
void CompatIntVector<Alloc>::serialize(std::ostream& out) const {
    // Dump the length
    out.write((const char*)&length, sizeof(length));
    // Dump the width
    out.write((const char*)&bit_width, sizeof(length));
    // Write the data
    data.serialize(out);
}

template<typename Alloc>
void CompatIntVector<Alloc>::load(std::istream& in) {
    // Read the length
    in.read((char*)&length, sizeof(length));
    // Read the width
    in.read((char*)&bit_width, sizeof(bit_width));
    // Read the data
    data.load(in);
}

namespace yomo {

// For some reason putting the default values in the class did not work for std::atomic.
template<typename T>
Pointer<T>::Pointer() : offset(std::numeric_limits<int64_t>::max()), local(false) {
    // Nothing to do!
}

template<typename T>
Pointer<T>::Pointer(T* destination) : Pointer() {
    *this = destination;
}

template<typename T>
Pointer<T>::Pointer(const Pointer& other) : Pointer(other.get()) {
    // Nothing to do!
}

template<typename T>
Pointer<T>::Pointer(Pointer&& other) : Pointer(other.get()) {
    // Nothing to do!
}

template<typename T>
Pointer<T>& Pointer<T>::operator=(const Pointer& other) {
    return *this = other.get();
}

template<typename T>
Pointer<T>& Pointer<T>::operator=(Pointer&& other) {
    return *this = other.get();
}

template<typename T>
Pointer<T>::operator bool () const {
    return offset != std::numeric_limits<int64_t>::max();
}

template<typename T>
std::add_lvalue_reference<T> Pointer<T>::operator*() const {
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
        offset = std::numeric_limits<int64_t>::max();
    } else {
        // Get the offset, requiring that it is in the same chain as us.
        auto result = Manager::get_offset_in_same_chain(this, addr);
        offset = result.first;
        local.store(result.second);
    }
    return *this;
}

template<typename T>
T* Pointer<T>::operator+(size_t items) const {
    return get() + items;
}

template<typename T>
T* Pointer<T>::get() const {
    if (offset == std::numeric_limits<int64_t>::max()) {
        return nullptr;
    } else if (local.load()) {
        // Just apply the offset directly
        return (T*) ((intptr_t) this + offset);
    } else {
        auto result = Manager::follow_offset_in_same_chain((const void*) this, offset);
        if (result.second) {
            // We're going to the same offset in memory as in the chain.
            // Might clobber part of a simultaneous write, but simultaneous
            // read and write is undefined bahavior anyway.
            local.store(true);
        }
        return (T*) result.first;
    }
}

template<typename T>
template<typename U>
Allocator<T>::Allocator(const Allocator<U>& alloc) {
    // Nothing to do!
}

template<typename T>
bool Allocator<T>::operator==(const Allocator& other) const {
    // TODO: Technically anybody can deallocate anybody else's memory since it
    // always deallocates from the chain it is in, not the one we are in. But
    // we say we're equal if we are using the same chain for allocations.
    return get_chain() == other.get_chain();
}

template<typename T>
bool Allocator<T>::operator!=(const Allocator& other) const {
    return !(*this == other);
}

template<typename T>
auto Allocator<T>::allocate(size_type n, const T* hint) -> T* {
    auto our_chain = get_chain();
    T* allocated = (T*) Manager::allocate_from(our_chain, n * sizeof(T));
    if (yomo::Manager::check_chains) {
        // Make sure we got the right chain for our allocated memory.
        assert(Manager::get_chain(allocated) == our_chain);
    }
    return allocated;
}

template<typename T>
void Allocator<T>::deallocate(T* p, size_type n) {
    Manager::deallocate((void*) p);
}

template<typename T>
size_t Allocator<T>::max_size() const {
    // TODO: this probably won't really fit in memory, but other than that
    // there's no reason we can't allocate something this big.
    return numeric_limits<size_t>::max();
}

template<typename T>
Manager::chainid_t Allocator<T>::get_chain() const {
    return Manager::get_chain((void*) this);
}

template<typename T>
UniqueMappedPointer<T>::~UniqueMappedPointer() {
    reset();
}

template<typename T>
UniqueMappedPointer<T>::operator bool () const {
    return (bool) cached_value;
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
    return cached_value;
}

template<typename T>
const T* UniqueMappedPointer<T>::get() const {
    return cached_value;
}

template<typename T>
template<typename... Args>
void UniqueMappedPointer<T>::construct_internal(const std::string& prefix, Args&&... constructor_args) {
    // Drop any existing chain.
    reset();
    
    // Make a new chain
    chain = Manager::create_chain(prefix);
    // Allocate space in the cahin for the item.
    // Can't use the Allocator because we don't have a place in the chain to
    // store one.
    T* item = (T*) Manager::allocate_from(chain, sizeof(T));
    
    // Save it
    cached_value = item;
    
    // Run the constructor.
    new (item) T(std::forward<Args>(constructor_args)...);
}

template<typename T>
void UniqueMappedPointer<T>::construct() {
    // Use an empty prefix.
    construct_internal("");
}

template<typename T>
void UniqueMappedPointer<T>::construct(const std::string& prefix) {
    // Use the provided prefix.
    construct_internal(prefix);
}

template<typename T>
template<typename... Args>
void UniqueMappedPointer<T>::construct(const std::string& prefix, Args&&... constructor_args) {
    // Pass along args and use the provided prefix.
    construct_internal(prefix, std::forward<Args>(constructor_args)...);
}

template<typename T>
void UniqueMappedPointer<T>::load(int fd, const std::string& prefix) {
    // Drop any existing chain.
    reset();
    
    // Just pass through to the Manager
    chain = Manager::create_chain(fd, prefix);
    // And find the item
    cached_value = (T*) Manager::find_first_allocation(chain, sizeof(T));
}

template<typename T>
void UniqueMappedPointer<T>::load(std::istream& in, const std::string& prefix) {
    if (!prefix.empty()) {
        // First read and check the prefix
        std::string prefix_buffer;
        prefix_buffer.resize(prefix.size());
        in.read(&prefix_buffer.at(0), prefix_buffer.size());
        if (!in || prefix_buffer != prefix) {
            // Hit EOF or got the wrong thing
            throw std::runtime_error("Expected prefix not found in input. Check file type.");
        }
    }
    // Then read everything after the prefix.
    load_after_prefix(in, prefix);
}

template<typename T>
void UniqueMappedPointer<T>::load(const std::function<std::string(void)>& iterator, const std::string& prefix) {
    // Drop any existing chain.
    reset();
    
    // Just pass through to the Manager
    chain = Manager::create_chain(iterator, prefix);
    // And find the item
    cached_value = (T*) Manager::find_first_allocation(chain, sizeof(T));
    
}

template<typename T>
void UniqueMappedPointer<T>::load_after_prefix(std::istream& in, const std::string& prefix) {
    // Drop any existing chain.
    reset();
    
    if (!in) {
        // Notice if something goes wrong
        throw std::runtime_error("Stream is in a bad state and cannot be used for input!");
    }
    
    const size_t MAX_CHUNK_SIZE = 4096;
    
    // Fill up this buffer with chunks of a certian size
    std::string buffer;
    
    // Make the chain through the Manager
    chain = Manager::create_chain([&]() {
        if (buffer.empty() && !prefix.empty()) {
            // Inject the prefix on the first call.
            // (Or on any call after hitting EOF and returning "", so don't keep calling us)
            buffer = prefix;
        } else if (in.eof()) {
            // Show an empty buffer for EOF
            buffer.clear();
        } else {
            // Other calls read data, until the last call shows an empty buffer
            // for EOF.
            buffer.resize(MAX_CHUNK_SIZE);
            // Grab a chunk
            if (!in) {
                // Notice if something goes wrong
                throw std::runtime_error("Error before reading chunk from stream!");
            }
            in.read(&buffer.at(0), MAX_CHUNK_SIZE);
            if (in.eof()) {
                // Didn't read all the characters, so shrink down (maybe to 0)
                buffer.resize(in.gcount());
            } else if (!in) {
                // Input error not co-occuirring with EOF
                throw std::runtime_error("Error reading chunk from stream!");
            }
        }
        // Copy the buffer over to the caller.
        // TODO: can we save a copy here?
        return buffer;
    }, prefix);
    // And find the item
    cached_value = (T*) Manager::find_first_allocation(chain, sizeof(T));
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
    // And find the item
    cached_value = (T*) Manager::find_first_allocation(chain, sizeof(T));
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
    // And find the item
    cached_value = (T*) Manager::find_first_allocation(chain, sizeof(T));
}

template<typename T>
void UniqueMappedPointer<T>::save(std::ostream& out) const {
    Manager::scan_chain(chain, [&](const void* start, size_t length) {
        // Go through all the data in the chain
        // And save it to the stream.
        out.write((const char*) start, length); 
    });
}

template<typename T>
void UniqueMappedPointer<T>::save(const std::function<void(const void*, size_t)>& iteratee) const {
    // Just pass through to the Manager.
    Manager::scan_chain(chain, iteratee);
}

template<typename T>
void UniqueMappedPointer<T>::save_after_prefix(std::ostream& out, const std::string& prefix) const {
    if (!out) {
        // Notice if something goes wrong
        throw std::runtime_error("Stream is in a bad state and cannot be used for output!");
    }
    
    // We need to drop as many characters from the chain as are in the prefix.
    size_t dropped = 0;
     
    Manager::scan_chain(chain, [&](const void* start, size_t length) {
        // Go through all the data in the chain
        const char* start_char = (const char*) start;
        
        // Adjust to skip any part of the prefix that's still here.
        size_t to_drop = std::min(length, prefix.size() - dropped);
        start_char += to_drop;
        length -= to_drop;
        dropped += to_drop;
        
        // And save it to the stream.
        out.write(start_char, length);
        if (!out) {
            // Notice if something goes wrong
            throw std::runtime_error("Error writing chunk to stream!");
        }
    });
}

template<typename T>
void UniqueMappedPointer<T>::preload(bool blocking) const {
    if (chain != Manager::NO_CHAIN) {
        Manager::preload_chain(chain, blocking);
    }
}

template<typename T>
void UniqueMappedPointer<T>::reset() {
    if (chain != Manager::NO_CHAIN) {
        Manager::destroy_chain(chain);
        chain = Manager::NO_CHAIN;
    }
    cached_value = nullptr;
}

template<typename T>
std::tuple<size_t, size_t, size_t> UniqueMappedPointer<T>::get_usage() {
    return Manager::get_usage(chain);
}

template<typename T>
void UniqueMappedPointer<T>::check_heap_integrity() {
    return Manager::check_heap_integrity(chain);
}

}


}

#endif
