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
};

// Give the static members a compilation unit
std::unordered_map<Manager::chainid_t, std::map<size_t, intptr_t>> Manager::chain_space_index;
std::map<intptr_t, Manager::LinkRecord> Manager::address_space_index;
std::shared_timed_mutex Manager::mutex;

Manager::chainid_t Manager::create_chain(const std::string& prefix) {
    // Allocate some non-file-backed memory
    char* link = new char[BASE_SIZE];
    if (!link) {
        throw std::runtime_error("Could not allocate " + std::to_string(BASE_SIZE) + " bytes");
    }
    
    // Make a record for it
    LinkRecord record;
    
    // Fill it in
    record.offset = 0;
    record.length = BASE_SIZE;
    record.next = 0;
    record.first = (intptr_t)link;
    record.fd = 0;
    record.last = (intptr_t)link;
    
    // Give it an id
    chainid_t chain_id = (intptr_t)link;
    
    {
        // Get write access to manager data structures
        std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Save the record that says this link is here in this chain
        Manager::address_space_index[(intptr_t)link] = std::move(record);
        
        // Save the chain space record that says the chain starts at this link
        chain_space_index[chain_id][0] = (intptr_t)link;
    }
    
    // Set up the allocator data structures.
    set_up_allocator(chain_id);
    
    return chain_id;
}

Manager::chainid_t Manager::create_chain(int fd, const std::string& prefix) {

    // Make a record for it
    LinkRecord record;
    
    // We can only map a nonempty file.
    struct stat fileinfo;
    if (fstat(fd, &fileinfo)) {
        throw std::runtime_error("Could not stat file: " + std::string(strerror(errno)));
    }
    size_t file_size = fileinfo.st_size;
    // TODO: check st_blksize and try to use a multiple of that for allocating.
    if (file_size == 0) {
        // The file is currently empty and we need to expand it to be able to write to it.
        if (ftruncate(fd, BASE_SIZE)) {
            throw std::runtime_error("Could not grow file to be mapped: " + std::string(strerror(errno)));
        }
    }
    
    // Make the MIO mapping of the whole file, or throw.
    record.mapping = std::make_unique<mio::mmap_sink>(fd);
    
    // Find the first mapped byte
    intptr_t mapping_address = (intptr_t)&((*record.mapping)[0]);
    
    // Fill it in
    record.offset = 0;
    record.length = record.mapping->size();
    record.next = 0;
    record.first = mapping_address;
    record.fd = fd;
    record.last = mapping_address;
    
    // Give it an id
    chainid_t chain_id = mapping_address;
    
    {
        // Get write access to manager data structures
        std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Save the record that says this link is here in this chain
        Manager::address_space_index[mapping_address] = std::move(record);
        
        // Save the chain space record that says the chain starts at this link
        chain_space_index[chain_id][0] = mapping_address;
    }
    
    // Set up the allocator data structures.
    set_up_allocator(chain_id);
    
    return chain_id;
    
}

size_t Manager::get_dissociated_chain(chainid_t chain) {
}

size_t Manager::get_associated_chain(chainid_t chain, int fd) {
}

void Manager::destroy_chain(chainid_t chain) {
}

Manager::chainid_t Manager::get_chain(const void* address) {
}

void* Manager::get_address_in_chain(chainid_t chain, size_t position) {
}

size_t Manager::get_position_in_chain(chainid_t chain, size_t position) {
}

void* Manager::allocate_from(chainid_t chain, size_t bytes) {
}

void Manager::deallocate(void* address) {
}

void* Manager::find_first_allocation(chainid_t chain, size_t bytes) {
}

void Manager::set_up_allocator(chainid_t chain) {
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

