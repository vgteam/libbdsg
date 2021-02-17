//
//  mapped_structs.cpp
//

#include "bdsg/internal/mapped_structs.hpp"

#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include <mio/mmap.hpp>


namespace bdsg {

namespace yomo {

// we hide our LinkRecord in here because we can't forward-declare the MIO
// stuff it stores.

/**
 * This describes a link in a chain, which is a single contiguous memory
 * mapping. Mapping address is the key in address_space_index.
 */
struct Manager::LinkRecord {
    /// Offset of the start of the mapping in the chain (cumulative sum)
    size_t offset;
    /// Number of bytes in the mapping.
    size_t length;
    /// Mapping start address of the next link in the chain, or 0 if this is the last one.
    intptr_t next;
    /// Mapping start address of the first link in the chain.
    intptr_t first;
    
    /// MIO-managed memory mapping, if any.
    unique_ptr<mio::mmap_sink> mapping;
    
    // Then we have per-chain state only used in the first link.
    
    /// If this is the first link in the chain, stores the file descriptor
    /// associated with the chain, or 0 if no FD is associated.
    int fd;
    /// If this is the first link in the chain, stores the start of the
    /// last link in the chain, for fast append of new mappings.
    intptr_t last;
    /// If this is the first link in the chain, how many bytes in the chain are
    /// prefix, before the allocator?
    size_t prefix_size;
    /// If this is the first link in the chain, how many bytes in the chain exist overall?
    size_t total_size;
};

/**
 * This occurs inside the chains and represents the header of some free or
 * allocated memory.
 */
struct Manager::AllocatorBlock {
    /// Next block. Only used when block is free.
    Pointer<AllocatorBlock> prev;
    /// Previous block. Only used when block is free.
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
struct Manager::AllocatorHeader {
    /// Where is the first free block of memory?
    Pointer<AllocatorBlock> first_free;
    /// Where is the last free block of memory?
    Pointer<AllocatorBlock> last_free;
};

// Give the static members a compilation unit
std::unordered_map<Manager::chainid_t, std::map<size_t, intptr_t>> Manager::chain_space_index;
std::map<intptr_t, Manager::LinkRecord> Manager::address_space_index;
std::shared_timed_mutex Manager::mutex;

Manager::chainid_t Manager::create_chain(const std::string& prefix) {
    if (prefix.size() > MAX_PREFIX_SIZE) {
        // Prefix is too long and allocator might not fit.
        throw std::runtime_error("Prefix of " + std::to_string(prefix.size()) +
            " is longer than limit of " + std::to_string(MAX_PREFIX_SIZE));
    }
    
    // Make a no-file chain which can't possibly have data.
    chainid_t chain = open_chain().first;
    
    // Copy the prefix into place
    char* start = (char*)get_address_in_chain(chain, 0, prefix.size());
    std::copy(prefix.begin(), prefix.end(), start);
    
    
    // Set up the allocator data structures.
    set_up_allocator_at(chain, prefix.size());
    
    return chain;
}

Manager::chainid_t Manager::create_chain(int fd, const std::string& prefix) {
    if (prefix.size() > MAX_PREFIX_SIZE) {
        // Prefix is too long and allocator might not fit.
        throw std::runtime_error("Prefix of " + std::to_string(prefix.size()) +
            " is longer than limit of " + std::to_string(MAX_PREFIX_SIZE));
    }
    
    if (!fd) {
        throw std::runtime_error("File descriptor must be set for memory mapping a file.");
    }
    
    // Make a chain from a file, which may have data already.
    std::pair<chainid_t, bool> chain_info = open_chain(fd);
    auto& chain = chain_info.first;
    auto& had_data = chain_info.second;
    
    try {
        // Deal with the prefix? Where would it be/should it go?
        char* start = (char*)get_address_in_chain(chain, 0, prefix.size());
        
        if (had_data) {
            // Check the prefix
            bool found = std::equal(prefix.begin(), prefix.end(), start);
            if (!found) {
                // The magic number is wrong.
                throw std::runtime_error("Expected prefix not found in file. Check file type.");
            }
            
            // Assume the allocator data structures are ready.
            connect_allocator_at(chain, prefix.size());
        } else {
            // Copy the prefix into place
            std::copy(prefix.begin(), prefix.end(), start);
            
            // Set up the allocator data structures.
            set_up_allocator_at(chain, prefix.size());
        }
    } catch (std::exception& e) {
        // Clean up the chain because anyone who catches won't be able to.
        destroy_chain(chain);
        
        throw e;
    }
        
    return chain;
}

Manager::chainid_t Manager::get_dissociated_chain(chainid_t chain) {
    // Copy to a chain associated with no FD
    return copy_chain(chain, 0);
}

Manager::chainid_t Manager::get_associated_chain(chainid_t chain, int fd) {
    // Copy to a chain associated with the given FD
    return copy_chain(chain, fd);
}

void Manager::destroy_chain(chainid_t chain) {

    // Remember the FD of the chain for closing the file.
    int fd;
    
    // Remember any MIO mappings to unmap
    std::vector<std::unique_ptr<mio::mmap_sink>> mio_clean;
    // Remember any normal memory to clean up
    std::vector<char*> normal_clean;

    {
        // Get write access to manager data structures
        std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        auto head_entry = Manager::address_space_index.find((intptr_t) chain);
        
        if (head_entry == Manager::address_space_index.end()) {
            throw std::runtime_error("Trying to destroy nonexistent chain");
        }
        
        fd = head_entry->second.fd;
        
        auto link_entry = head_entry;
        
        while(link_entry != Manager::address_space_index.end()) {
            // Clean up each link
            if (link_entry->second.mapping) {
                // Clear up any MIO mapping
                // Note that we're allowed to modify the actual record with "read" access, just not the maps.
                mio_clean.emplace_back(std::move(link_entry->second.mapping));
            } else {
                // This is just a normal char array allocation.
                normal_clean.emplace_back((char*)link_entry->first);
            }
            
            // Work out where we are going to look next
            intptr_t next_link_addr = link_entry->second.next;
            
            // Delete where we are
            Manager::address_space_index.erase(link_entry);
            
            if (next_link_addr) {
                // Look for the next link in the chain.
                link_entry = Manager::address_space_index.find(next_link_addr);
            } else {
                // Stop
                link_entry = Manager::address_space_index.end();
            }
        }
        
        // Also clean up the chain position index
        Manager::chain_space_index.erase(chain);
    }
    
    // Now that we aren't holding locks, free the memory
    
    for (auto& mapping : mio_clean) {
        mapping.reset();
    }
    
    for (auto& mapping : normal_clean) {
        delete[] mapping;
    }
    
    if (fd) {
        // Close the backing file
        if (close(fd)) {
            throw std::runtime_error("Could not close file: " + std::string(strerror(errno)));
        }
    }
}

Manager::chainid_t Manager::get_chain(const void* address) {
    return get_chain_and_position(address).first;
}

void* Manager::get_address_in_chain(chainid_t chain, size_t position, size_t length) {
    // Get read access to manager data structures
    std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    // Find the index over this chain's space
    auto& chain_map = Manager::chain_space_index.at(chain);
    
    // Find the first chain link starting after our position 
    auto found = chain_map.upper_bound(position);
    
    if (found == chain_map.begin() || chain_map.empty()) {
        // There won't be a link covering the position
        // We really should never end up with no link over position 0, though,
        // so this should mostly mean empty.
        throw std::runtime_error("Attempted to find address for position that has no link.");
    }
    
    // Look left and find the link we are looking for
    --found;
    
    if (length) {
        // We need to do extra error checking, for which we need the LinkRecord.
        // TODO: should we waste some memory and save a lookup here?
        LinkRecord& link = Manager::address_space_index.at(found->second);
        
        if (link.offset + link.length < position + length) {
            // Whatever it is we're interested in would cross link boundaries.
            throw std::runtime_error("Attempted to find address for position range that does not fit completely within any link");
        }
    }
    
    // Convert to address by offsetting from address of containing block.
    return (void*)(found->second + (position - found->first));
   
}

std::pair<Manager::chainid_t, size_t> Manager::get_chain_and_position(const void* address, size_t length) {
    // Determine what we're looking for
    intptr_t sought = (intptr_t) address;
    
    // Fill these in with link info
    intptr_t link_base;
    size_t link_offset;
    size_t link_length;
    chainid_t chain;
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the earliest block starting after the address
        auto found = Manager::address_space_index.upper_bound((intptr_t)address);
        
        if (found == Manager::address_space_index.begin() || Manager::address_space_index.empty()) {
            // There won't be a link covering the address
            throw std::runtime_error("Attempted to place address that is before all chains.");
        }
        
        // Go left to the block that must include the address if any does.
        --found;
        
        // Copy out link info
        link_base = found->first;
        link_offset = found->second.offset;
        link_length = found->second.length;
        chain = (chainid_t) found->second.first;
    }
    
    if (link_base + link_length < sought + length) {
        throw std::runtime_error("Attempted to place address range that does not fit completely within any link");
    }
    
    // Translate first link's address to chain ID, and address to link local offset to chain position.
    return std::make_pair(chain, link_offset + (sought - link_base));
}

void* Manager::allocate_from(chainid_t chain, size_t bytes) {
}

void Manager::deallocate(void* address) {
}

void* Manager::find_first_allocation(chainid_t chain, size_t bytes) {
}

std::pair<Manager::chainid_t, bool> Manager::open_chain(int fd, size_t start_size) {

    // Set up our return value
    std::pair<chainid_t, bool> to_return;
    chainid_t& chain_id = to_return.first;
    bool& had_data = to_return.second;
    
    // Have a place to write down where we put the link's memory.
    intptr_t mapping_address;

    // Make a record for the link
    LinkRecord record;
    
    // Fill in the shared fields.
    record.offset = 0;
    record.next = 0;
    
    if (fd) {
        // Use file-mapped memory
        
        // Duplicate the FD so we can own our own and close it later.
        int our_fd = dup(fd);
        if (!our_fd) {
            throw std::runtime_error("Could not duplicate file descriptor: " + std::string(strerror(errno)));
        }
    
        // We can only map a nonempty file.
        struct stat fileinfo;
        if (fstat(our_fd, &fileinfo)) {
            throw std::runtime_error("Could not stat file: " + std::string(strerror(errno)));
        }
        size_t file_size = fileinfo.st_size;
        // TODO: check st_blksize and try to use a multiple of that for allocating.
        if (file_size == 0) {
            // The file is currently empty and we need to expand it to be able to write to it.
            if (ftruncate(our_fd, start_size)) {
                throw std::runtime_error("Could not grow file to be mapped: " + std::string(strerror(errno)));
            }
        }
        
        // Make the MIO mapping of the whole file, or throw.
        record.mapping = std::make_unique<mio::mmap_sink>(our_fd);
    
        // Remember where the memory starts
        mapping_address = (intptr_t)&((*record.mapping)[0]);
        
        // Fill in the record for a MIO mapping
        record.length = record.mapping->size();
        record.fd = our_fd;
        
        // We may have had data
        had_data  = (file_size != 0);
    } else {
        // Use boring allocated memory
        
        // TODO: when MIO gets anonymous mapping support, use that.
        
        char* link = new char[start_size];
        if (!link) {
            throw std::runtime_error("Could not allocate " + std::to_string(start_size) + " bytes");
        }
        
        // Remember where the memory starts
        mapping_address = (intptr_t) link;
        
        // Fill in the record for normal memory
        record.length = start_size;
        record.fd = 0;
        
        // We didn't have data
        had_data = false;
    }
    
    // Fill in chain stats
    record.first = mapping_address;
    record.last = mapping_address;
    record.total_size = record.length;
    
    // Give the chain an id
    chain_id = (chainid_t) mapping_address;
    
    {
        // Get write access to manager data structures
        std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Save the record that says this link is here in this chain
        Manager::address_space_index[mapping_address] = std::move(record);
        
        // Save the chain space record that says the chain starts at this link
        chain_space_index[chain_id][0] = mapping_address;
    }
    
    return to_return;
}

void Manager::extend_chain_to(chainid_t chain, size_t new_total_size) {
    // Get write access to manager data structures
    std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    LinkRecord& head = Manager::address_space_index.at((intptr_t)chain);
    
    if (head.total_size <= new_total_size) {
        // Already done.
        return;
    }
    
    size_t new_bytes = new_total_size - head.total_size;
    
    add_link(head, new_bytes);
}

Manager::LinkRecord& Manager::add_link(LinkRecord& head, size_t new_bytes) {
    // Assume we're already locked.
    
    // What used to be the last link?
    LinkRecord& old_tail = Manager::address_space_index.at(head.last);
    
    // Where in the chain does it end?
    size_t old_end = old_tail.offset + old_tail.length;
    
    // what's our new total size?
    size_t new_total = head.total_size + new_bytes;
    
    // Make a new mapping link
    LinkRecord new_tail;
    intptr_t mapping_address;
    
    if (head.fd) {
        // Grow the file
        if (ftruncate(head.fd, new_total)) {
            throw std::runtime_error("Could not grow mapped file: " + std::string(strerror(errno)));
        }
    
        // Map the new tail with MIO
        new_tail.mapping = std::make_unique<mio::mmap_sink>(head.fd, old_end, new_bytes);
        
        // Find its address
        mapping_address = (intptr_t)&((*new_tail.mapping)[0]);
    } else {
        char* link = new char[new_bytes];
        if (!link) {
            throw std::runtime_error("Could not allocate an additional " + std::to_string(new_bytes) + " bytes");
        }
        
        // Remember where the memory starts
        mapping_address = (intptr_t) link;
    }
    
    // Fill in the link record
    new_tail.offset = old_end;
    new_tail.length = new_bytes;
    new_tail.next = 0;
    new_tail.first = head.first;
    
    // Save the new link
    auto& where = Manager::address_space_index[mapping_address];
    where = std::move(new_tail);
    chain_space_index[(chainid_t) head.first][old_end] = mapping_address;
    
    // And hook it up.
    old_tail.next = mapping_address;
    head.last = mapping_address;
    
    return where;
}

Manager::chainid_t Manager::copy_chain(chainid_t chain, int fd) {
    // First we need to grab the prefix length, so we know where to site the new allocator.
    size_t prefix_size;
    // And the total size of the data in the source chain
    size_t total_size;
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the record for this chain
        auto& record = Manager::address_space_index.at(chain);
        
        // Steal its stats
        prefix_size = record.prefix_size;
        total_size = record.total_size;
    }
    
    // Make the new chain with the appropriate size hint.
    chainid_t new_chain = open_chain(fd, total_size).first;
    
    // Extend it to the required total size if it isn't long enough already
    extend_chain_to(new_chain, total_size);
    
    // Copy all the data
    
    // We already know the addresses of the first links
    intptr_t from_link_adddr = (intptr_t) chain;
    intptr_t to_link_addr = (intptr_t) new_chain;
    // We can walk pointers to the link records along as we copy. The owning
    // map is guaranteed not to move them.
    LinkRecord* from_link = nullptr;
    LinkRecord* to_link = nullptr;
    
    // Track how many bytes we have copied
    size_t cursor = 0;
    
    while (cursor < total_size) {
        // Until we are done, we need to copy an overlapping range between the two chains' links, as a block.
        // The block will reach to the end of a link in one or both chains.
        
        // Make sure we have mapping addresses of an dpointers to records for
        // the current links.
        {
            // Get read access to manager data structures
            std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
            if (from_link && (cursor - from_link->offset) == from_link->length) {
                // Advance out of old from link
                from_link_adddr = from_link->next;
                from_link = nullptr;
            }
            if (to_link && (cursor - to_link->offset) == to_link->length) {
                // Advance out of old to link
                to_link_addr = to_link->next;
                to_link = nullptr;
            }
            
            if (!from_link) {
                // We aren't using the same from link as last time
                from_link = &Manager::address_space_index.at(from_link_adddr);
            }
            if (!to_link) {
                // We aren't using the same to link as last time
                to_link = &Manager::address_space_index.at(to_link_addr);
            }
        }
        
        // Pull out block ranges
        size_t from_link_cursor = cursor - from_link->offset;
        size_t to_link_cursor = cursor - to_link->offset;
        size_t from_link_available = from_link->length - from_link_cursor;
        size_t to_link_available = to_link->length - to_link_cursor;
        
        // Do the copy of the minimum overlap
        size_t to_copy = std::min(from_link_available, to_link_available);
        memcpy((void*)(to_link_addr + to_link_cursor), (void*)(from_link_adddr + from_link_cursor), to_copy);
        
        // Record the copy
        cursor += to_copy;
    }
    
    // Set up the allocator data structures.
    connect_allocator_at(new_chain, prefix_size);
    
    return new_chain;
}

void Manager::set_up_allocator_at(chainid_t chain, size_t offset) {
}

void Manager::connect_allocator_at(chainid_t chain, size_t offset) {
}

}

ArenaAllocatorBlockRef ArenaAllocatorBlockRef::get_prev() const {
    auto& body = *reinterpret_cast<const body_t*>(this->get_body());
    return body.prev.get(this->context);
}

void ArenaAllocatorBlockRef::set_prev(const ArenaAllocatorBlockRef& prev) {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    body.prev = prev;
}

ArenaAllocatorBlockRef ArenaAllocatorBlockRef::get_next() const {
    auto& body = *reinterpret_cast<const body_t*>(this->get_body());
    return body.next.get(this->context);
}

void ArenaAllocatorBlockRef::set_next(const ArenaAllocatorBlockRef& next) {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    body.next = next;
}

const big_endian<size_t>& ArenaAllocatorBlockRef::size() const {
    auto& body = *reinterpret_cast<const body_t*>(this->get_body());
    return body.size;
}

big_endian<size_t>& ArenaAllocatorBlockRef::size() {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    return body.size;
}

size_t ArenaAllocatorBlockRef::get_user_data() const {
    return position + sizeof(body_t);
}

ArenaAllocatorBlockRef ArenaAllocatorBlockRef::get_from_data(MappingContext* context, size_t user_data) {
    return ArenaAllocatorBlockRef(context, user_data - sizeof(body_t));
}

ArenaAllocatorBlockRef ArenaAllocatorBlockRef::split(size_t first_bytes) {
    size_t available_bytes = size();
    // Resize down 
    size() = first_bytes;
    
    // Define the new block
    // TODO: we skip the body constructor.
    ArenaAllocatorBlockRef new_next(context, position + first_bytes);
    new_next.size() = available_bytes - first_bytes - sizeof(body_t);
    
    // Wire it in
    new_next.set_prev(*this);
    new_next.set_next(get_next());
    set_next(new_next);
    
    // Return it
    return new_next;
}

pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> ArenaAllocatorBlockRef::detach() {
    // Grab out initial neighbors
    pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> old_neighbors = make_pair(get_prev(), get_next());
    
    if (get_prev()) {
        // Attach the thing before us to whatever is after us instead of us.
        get_prev().set_next(old_neighbors.second);
        set_prev(ArenaAllocatorBlockRef());
    }
    
    if (get_next()) {
        // Attach the thing after us to whatever was before us instead of us
        get_next().set_prev(old_neighbors.first);
        set_next(ArenaAllocatorBlockRef());
    }
    
    return old_neighbors;
}

void ArenaAllocatorBlockRef::attach(ArenaAllocatorBlockRef& left, ArenaAllocatorBlockRef& right) {
    set_prev(left);
    if (left) {
        left.set_next(*this);
    }
    set_next(right);
    if (right) {
        right.set_prev(*this);
    }
}

pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> ArenaAllocatorBlockRef::coalesce() {
    // Start here
    pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> contiguous = make_pair(*this, *this);
    while (contiguous.first.get_prev() && contiguous.first.get_prev().immediately_before(contiguous.first)) {
        // Walk left until neighbor is null or not abutting
        contiguous.first = contiguous.first.get_prev();
    }
    while (contiguous.second.get_next() && contiguous.second.immediately_before(contiguous.second.get_next())) {
        // Walk right until neighbor is null or not abutting
        contiguous.second = contiguous.second.get_next();
    }
    
    // Compute size
    // TODO: could simplify and cancel two body sizes.
    size_t total_bytes = contiguous.second.get_user_data() + contiguous.second.size() - contiguous.first.position - sizeof(body_t);
    
    // Expand leftmost thing to cover theough end of rightmost thing
    contiguous.first.size() = total_bytes;
    
    // Connect to right neighbor of rightmost thing
    contiguous.first.set_next(contiguous.second.get_next());
    if (contiguous.first.get_next()) {
        contiguous.first.get_next().set_prev(contiguous.first);
    }
    
    // Clear out rightmost adjacencies in case we wander off the rails somehow.
    contiguous.second.set_next(ArenaAllocatorBlockRef());
    contiguous.second.set_prev(ArenaAllocatorBlockRef());
    
    // Return leftmost and rightmost things so caller can update list ends.
    return contiguous;
}

bool ArenaAllocatorBlockRef::immediately_before(const ArenaAllocatorBlockRef& other) const {
    return position + size() == other.position;
}
    

}

