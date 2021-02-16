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

size_t Manager::get_dissociated_chain(chainid_t chain) {
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
    
    // Then grab the prefix string
    std::string prefix(prefix_size, 0);
    char* start = (char*)get_address_in_chain(chain, 0, prefix_size);
    std::copy(start, start + prefix_size, prefix.begin());
    
    // Make the new chain with the appropriate size hint.
    chainid_t new_chain = open_chain(0, total_size).first;
    
    // Extend it to the required total size if it isn't long enough already
    extend_chain_to(new_chain, total_size);
    
    // Copy all the data
    // TODO
}

size_t Manager::get_associated_chain(chainid_t chain, int fd) {
}

void Manager::destroy_chain(chainid_t chain) {
}

Manager::chainid_t Manager::get_chain(const void* address) {
}

void* Manager::get_address_in_chain(chainid_t chain, size_t position, size_t length) {
}

std::pair<Manager::chainid_t, size_t> Manager::get_chain_and_position(const void* address) {
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
    
    // And hook it up.
    old_tail.next = mapping_address;
    head.last = mapping_address;
    
    return where;
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

