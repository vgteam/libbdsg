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
    std::unique_ptr<mio::mmap_sink> mapping;
    
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
    /// If this is the first link in the chain, use this mutex to synchromize
    /// access to the allocator data structures across threads.
    /// MUST NEVER be acquired if the thread is holding a lock on the chain
    /// info data structures; always acquire this mutex *BEFORE* LOCKING CHAIN
    /// INFO, if you are going to hold both simultaneously.
    std::unique_ptr<std::mutex> allocator_mutex;
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
    chainid_t chain = open_chain(0, BASE_SIZE).first;
    
    // Copy the prefix into place
    char* start = (char*)get_address_in_chain(chain, 0, prefix.size());
    std::copy(prefix.begin(), prefix.end(), start);
    
    
    // Set up the allocator data structures.
    set_up_allocator_at(chain, prefix.size(), BASE_SIZE - prefix.size());
    
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
    std::pair<chainid_t, bool> chain_info = open_chain(fd, BASE_SIZE);
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
            set_up_allocator_at(chain, prefix.size(), BASE_SIZE - prefix.size());
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

void* Manager::get_address_in_same_chain(const void* here, size_t position) {
    // TODO: accelerate with some kind of alignment and mod scheme
    chainid_t chain = get_chain(here);
    return get_address_in_chain(chain, position);
}
    
size_t Manager::get_position_in_same_chain(const void* here, const void* address) {
    // TODO: accelerate with some kind of alignment and mod scheme

    std::pair<chainid_t, size_t> chain_and_pos = get_chain_and_position(address);
    
    // TODO: skip the check for speed?
    if (chain_and_pos.first != get_chain(here)) {
        throw std::runtime_error("Attempted to refer across chains!");
    }
    
    return chain_and_pos.second;
}

void* Manager::allocate_from(chainid_t chain, size_t bytes) {
    cerr << "Allocate " << bytes << "bytes from chain " << chain << endl;
    
    // How much space do we need with block overhead, if we need a new block?
    size_t block_bytes = bytes + sizeof(AllocatorBlock);
    
    AllocatorBlock* found;
    
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // With exclusive use of the free list
    
        // This will hold a ref to the free block we found or made that is big enough to hold this item.
        // Starts null if there is no first_free.
        found = header->first_free;
        while (found && found->size < bytes) {
            // Won't fit here. Try the next place.
            found = found->next;
        }
       
        if (!found) {
            // We have no free memory big enough.
            // We will make a new link.
            LinkRecord* new_link;
            
            {
                // Get write access to chain data structures.
                std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
                
                // Find the first link in the chain
                LinkRecord& first = address_space_index.at((intptr_t) chain);
                
                // Find the last link in the chain
                LinkRecord& last = address_space_index.at(first.last);
                
                // We need our factor as much memory as last time, or enough for
                // the thing we want to allocate
                size_t new_link_size = std::max(last.length * SCALE_FACTOR, block_bytes);
                
                // Go get the new link
                new_link = &add_link(first, new_link_size); 
            }
            
            // Work out where new free memory will start
            found = (AllocatorBlock*) get_address_in_chain(chain, new_link->offset, block_bytes);
            
            // Construct the block
            new (found) AllocatorBlock();
            
            // Set up its size.
            found->size = block_bytes - sizeof(AllocatorBlock);
            
            // Put it in the linked list
            found->next = nullptr;
            if (header->last_free) {
                header->last_free->next = found;
                found->prev = header->last_free;
            } else {
                found->prev = nullptr;
            }
            header->last_free = found;
            if (!header->first_free) {
                header->first_free = found;
            }
        }
        
        // Now we can allocate (part of) this block.
        
        if (found->size > block_bytes) {
            // We could break the user data off of this block and have some space left over.
            // TODO: use a min block size here instead.
            
            // So split the block.
            AllocatorBlock* second = found->split(bytes);
            if (header->last_free == found) {
                // And fix up the end of the linked list
                header->last_free = second;
            }
        }
        
        // Now we have a free block of the right size. Make it not free.
        auto connected = found->detach();
        if (header->first_free == found) {
            // This was the first free block
            header->first_free = connected.first;
        }
        if (header->last_free == found) {
            // This was the last free block 
            header->last_free = connected.second;
        }
    });
    
    cerr << "Allocated at " << found->get_user_data() << endl;
    
    // Give out the address of its data
    return found->get_user_data();
}

void* Manager::allocate_from_same_chain(void* here, size_t bytes) {
    // TODO: accelerate by coalescing locks?
    return allocate_from(get_chain(here), bytes);
}

void Manager::deallocate(void* address) {
    cerr << "Deallocate at " << address << endl;
    
    // Find the block
    AllocatorBlock* found = AllocatorBlock::get_from_data(address);
    
    // Find the chain
    chainid_t chain = get_chain(address);
   
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // With exclusive use of the free list
       
        // Find the block in the free list after it, if any
        // TODO: leave a link to it in the block so we don't need to scan for it.
        // We know it can't move, although it might allocate.
        AllocatorBlock* right = found->next;
        while(right && !right->prev) {
            // The block exists, but it isn't free (linked into the free list)
            right = right->next;
        }
        AllocatorBlock* left;
        if (!right) {
            // The new block should be the last block in the list.
            // So it comes after the existing last block, if any.
            left = header->last_free;
        } else {
            // The new block comes between right and its free predecessor, if any
            left = right->prev;
        }
        
        // Wire in the block
        found->attach(left, right);
        
        // Update haed and tail
        if (header->last_free == left) {
            header->last_free = found;
        }
        if (header->first_free == right) {
            header->first_free = found;
        }
        
        // Defragment.
        auto bounds = found->coalesce();
        // We can't need to update the first free when defragmenting, but we may
        // need to update the last free.
        if (header->last_free == bounds.second) {
            header->last_free = bounds.first;
        }
    });
}

void* Manager::find_first_allocation(chainid_t chain, size_t bytes) {

    if (bytes > (BASE_SIZE - MAX_PREFIX_SIZE - sizeof(AllocatorBlock) - sizeof(AllocatorHeader))) {
        // Too big to fit in the first block. Can't find it.
        throw std::runtime_error("Could not find first allocation of " + std::to_string(bytes) +
            " because it is too big for the first block");
        // TODO: can we predict the behavior of the allocator to find bigger things?
        // TODO: what if the thing or allocator changes?
    }
    
    // The first allocated item is just in the first block, after the prefix and allocator stuff.
    // Doesn't actually use the allocator header so doesn't need to synchronize.
    return (void*)(((char*)find_allocator_header(chain)) + sizeof(AllocatorHeader) + sizeof(AllocatorBlock));
}

Manager::AllocatorHeader* Manager::find_allocator_header(chainid_t chain) {
    // We know the header is always in the first link.

    LinkRecord* first;
    
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the first link
        first = &address_space_index.at((intptr_t) chain);
    }
    
    // The header lives after the prefix.
    return (AllocatorHeader*)(((char*) chain) + first->prefix_size);
}

void Manager::with_allocator_header(chainid_t chain,
    const std::function<void(AllocatorHeader*)>& callback) {
    
    LinkRecord* first;
    
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the first link
        first = &address_space_index.at((intptr_t) chain);
    }
    
    // Find the header
    AllocatorHeader* header = (AllocatorHeader*)(((char*) chain) + first->prefix_size);
    
    {
        // Get exclusive access to the allocator
        std::unique_lock<std::mutex> lock(*(first->allocator_mutex));
        
        // Run the callback with lock protection
        callback(header);
        
    }
}

std::pair<Manager::chainid_t, bool> Manager::open_chain(int fd, size_t start_size) {

    // Set up our return value
    std::pair<chainid_t, bool> to_return;
    chainid_t& chain_id = to_return.first;
    bool& had_data = to_return.second;
    
    // Have a place to write down where we put the link's memory.
    intptr_t mapping_address;

    // Make a record for the first link
    LinkRecord record;
    
    // Fill in the shared fields.
    record.offset = 0;
    record.next = 0;
    record.allocator_mutex = std::make_unique<std::mutex>();
    
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

void Manager::set_up_allocator_at(chainid_t chain, size_t offset, size_t space) {

    // Find where the header and block should go
    AllocatorHeader* header = (AllocatorHeader*) get_address_in_chain(chain, offset, sizeof(AllocatorHeader));
    AllocatorBlock* block = (AllocatorBlock*) get_address_in_chain(chain, offset + sizeof(AllocatorHeader), sizeof(AllocatorBlock));
    
    // Construct the header
    new (header) AllocatorHeader();
    // And point it at the block
    header->first_free = block;
    header->last_free = block;
    
    // Construct the block
    new (block) AllocatorBlock();
    block->prev = nullptr;
    block->next = nullptr;
    block->size = space - sizeof(AllocatorHeader) - sizeof(AllocatorBlock);
    
    // Now that it's there, connect to it
    connect_allocator_at(chain, offset);
}

void Manager::connect_allocator_at(chainid_t chain, size_t offset) {
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        LinkRecord& head = Manager::address_space_index.at((intptr_t) chain);
        // Save the allocator position
        head.prefix_size = offset;
    }
}

void* Manager::AllocatorBlock::get_user_data() const {
    return (void*)(((char*)this) + sizeof(AllocatorBlock));
}

Manager::AllocatorBlock* Manager::AllocatorBlock::get_from_data(void* user_data) {
    return (AllocatorBlock*)(((char*)user_data) - sizeof(AllocatorBlock));
}

Manager::AllocatorBlock* Manager::AllocatorBlock::split(size_t first_bytes) {
    size_t available_bytes = size;
    
    if (available_bytes - first_bytes < sizeof(AllocatorBlock)) {
        throw std::runtime_error("Insufficient free space for block header");
    }
    
    // Resize down 
    size = first_bytes;
    
    // Construct and define the new block
    AllocatorBlock* new_next = (AllocatorBlock*)(((char*)get_user_data()) + first_bytes);
    new (new_next) AllocatorBlock();
    new_next->size = available_bytes - first_bytes - sizeof(AllocatorBlock);
    
    // Wire it in
    new_next->prev = this;
    new_next->next = next;
    next = new_next;
    
    // Return it
    return new_next;
}

std::pair<Manager::AllocatorBlock*, Manager::AllocatorBlock*> Manager::AllocatorBlock::detach() {
    // Grab out initial neighbors
    pair<AllocatorBlock*, AllocatorBlock*> old_neighbors = make_pair(prev.get(), next.get());
    
    if (prev) {
        // Attach the thing before us to whatever is after us instead of us.
        prev->next = old_neighbors.second;
        // Null out our prev; if we aren't the first block this means we're
        // allocated.
        prev = nullptr;
    }
    
    if (next) {
        // Attach the thing after us to whatever was before us instead of us
        next->prev = old_neighbors.first;
        // Leave our next set for finding a free successor.
    }
    
    return old_neighbors;
}

void Manager::AllocatorBlock::attach(AllocatorBlock* left, AllocatorBlock* right) {
    prev = left;
    if (left) {
        left->next = this;
    }
    next = right;
    if (right) {
        right->prev = this;
    }
}

std::pair<Manager::AllocatorBlock*, Manager::AllocatorBlock*> Manager::AllocatorBlock::coalesce() {
    // We need to make sure we don't try and coalesce across link boundaries.
    // We rely on the adjacency check to operate in memory space.
    
    // Start here
    pair<AllocatorBlock*, AllocatorBlock*> contiguous = make_pair(this, this);
    while (contiguous.first->prev &&
           contiguous.first->prev->immediately_before(contiguous.first)) {
        // Walk left until neighbor is null or not abutting
        contiguous.first = contiguous.first->prev;
    }
    while (contiguous.second->next &&
           contiguous.second->immediately_before(contiguous.second->next)) {
        // Walk right until neighbor is null or not abutting
        contiguous.second = contiguous.second->next;
    }
    
    if (contiguous.first != contiguous.second) {
        // Something to coalesce
    
        // Compute size if first eats through end of second
        size_t total_bytes = ((char*)contiguous.second->get_user_data() -
                              (char*)contiguous.first->get_user_data()) +
                             contiguous.second->size;
        
        // Expand leftmost thing to cover theough end of rightmost thing
        contiguous.first->size = total_bytes;
        
        // Connect to right neighbor of rightmost thing
        contiguous.first->next = contiguous.second->next;
        if (contiguous.first->next) {
            contiguous.first->next->prev = contiguous.first;
        }
        
        // Clear out rightmost adjacencies in case we wander off the rails somehow.
        contiguous.second->next = nullptr;
        contiguous.second->prev = nullptr;
        
        // Destruct header
        contiguous.second->~AllocatorBlock();
    }
    
    // Return leftmost and rightmost things so caller can update list ends.
    return contiguous;
    
}

bool Manager::AllocatorBlock::immediately_before(const AllocatorBlock* other) const {
    // We are adjacent if we abut in memory, regardless of chain positioning.
    return ((char*)get_user_data()) + size == (char*)other;
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

