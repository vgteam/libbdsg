//
//  mapped_structs.cpp
//

#include "bdsg/internal/mapped_structs.hpp"

#include <mutex>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include <mio/mmap.hpp>

//#define debug_manager
//#define debug_pointers

namespace bdsg {

namespace yomo {

// Leave extra chain debugging checks off by default.
bool Manager::check_chains = false;

// This constant needs a compilation unit.
const Manager::chainid_t Manager::NO_CHAIN;

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
   
protected:
    /// MIO-managed read-write memory mapping, if any.
    std::unique_ptr<mio::mmap_sink> rw_mapping;
    /// If none, we may have an MIO-managed read-only memory mapping.
    std::unique_ptr<mio::mmap_source> ro_mapping;
    
public:
    // We have some accessors to abstract over the different kinds of mappings.
    
    /// Set up rw_mapping or ro_mapping to map the given range of the given file. Throws on failure.
    inline void map_file_range(int fd, size_t start, size_t length) {
        try {
            rw_mapping = std::make_unique<mio::mmap_sink>(fd, start, length);
        } catch (std::system_error& e) {
            // Probably permission denied. Try read-only.
            ro_mapping = std::make_unique<mio::mmap_source>(fd, start, length);
        }
    }
    
    /// Set up rw_mapping or ro_mapping to map the entirety of the given file. Throws on failure.
    inline void map_file(int fd) {
        try {
            rw_mapping = std::make_unique<mio::mmap_sink>(fd);
        } catch (std::system_error& e) {
            // Probably permission denied. Try read-only.
            ro_mapping = std::make_unique<mio::mmap_source>(fd);
        }
    }
    
    /// Return the address at which the link is mapped.
    inline intptr_t get_mapped_address() const {
        if (rw_mapping) {
            return (intptr_t)&((*rw_mapping)[0]);
        } else if (ro_mapping) {
            return (intptr_t)&((*ro_mapping)[0]);
        } else {
            throw std::runtime_error("Attempted to get address of unmapped link");
        }
    }
    
    /// Return the length of the stored mapping.
    inline size_t get_mapped_length() const {
        if (rw_mapping) {
            return rw_mapping->size();
        } else if (ro_mapping) {
            return ro_mapping->size();
        } else {
            throw std::runtime_error("Attempted to get mapped length of unmapped link");
        }
    }
    
    /// Return true if a mapping exists.
    inline bool is_mapped() const {
        return rw_mapping || ro_mapping;
    }
    
    /// Return true if the link can be written (rw mapping or no mapping, which
    /// indicates non-memory-mapped memory)
    inline bool is_writable() const {
        return !ro_mapping;
    }
    
    /// Release any mapping into one of the two returned unique_ptr objects.
    inline std::pair<std::unique_ptr<mio::mmap_sink>, std::unique_ptr<mio::mmap_source>> release() {
        return std::make_pair(std::move(rw_mapping), std::move(ro_mapping));
    }
    
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
    
#ifdef debug_manager
    dump(chain);
#endif
    
    return chain;
}

Manager::chainid_t Manager::create_chain(const std::function<std::string(void)>& iterator, const std::string& prefix) {
    if (prefix.size() > MAX_PREFIX_SIZE) {
        // Prefix is too long and allocator might not fit.
        throw std::runtime_error("Prefix of " + std::to_string(prefix.size()) +
            " is longer than limit of " + std::to_string(MAX_PREFIX_SIZE));
    }
    
    // We need to get all the data into one contiguous memory block, but we
    // don't know how long it is yet. So we'll manage a buffer ourselves, keep
    // realloc()ing it bigger until we've fit everything, then realloc() it
    // smaller to the size we needed, and then give it to the chain as an
    // externally-allocated link.
    //
    // As long as the virtual memory system doesn't actually give us the pages
    // we don't touch at the end of the last allocation, we shouldn't have too
    // much of a memory usage problem by doubling the allocation sizes, and if
    // realloc() is smart it can probably extend in place at least sometimes.
    //
    // If it was *really* smart it would move everything around with page table
    // magic, but it's probably not.
    
    // Start with a buffer size that is probably about a whole page in hopes
    // that we'll get it.
    size_t buffer_size = 4096;
    void* buffer = malloc(buffer_size);
    
    if (!buffer) {
        throw std::runtime_error("Could not allocate buffer!");
    }
    
    // We'll set this when we've verified the prefix
    bool prefix_checked = false;
    
    // Start a cursor at the start of the buffer, pointing to where the next
    // block will go.
    size_t cursor = 0;
    
    // Go and get some data
    std::string block = iterator();
    
#ifdef debug_manager
    std::cerr << "Received block of size " << block.size() << endl;
#endif
    
    while (!block.empty()) {
        if (cursor + block.size() > buffer_size) {
            // Embiggen the buffer
            buffer_size *= 2;
            void* new_buffer = realloc(buffer, buffer_size);
            if (!new_buffer) {
                // Make sure to free the old buffer (and not have clobbered it)
                free(buffer);
                throw std::runtime_error("Could not expand buffer to " +
                    std::to_string(buffer_size) + " bytes");
            }
            buffer = new_buffer;
        }
        
        // Put the new data in the buffer
        memcpy((char*)buffer + cursor, block.c_str(), block.size());
        // And move the cursor
        cursor += block.size();
        
        if (!prefix_checked && cursor >= prefix.size()) {
            // We've read in enough to check the prefix.
            
            // Go find where the prefix should be
            char* start = (char*)buffer;
            if (!std::equal(prefix.begin(), prefix.end(), start)) {
                // It's not the right prefix so clean up and bail out.
                free(buffer);
                throw std::runtime_error("Expected prefix not found in input. Check file type.");
            }
            
            // If we get here we got the right prefix.
            prefix_checked = true;
        }
        
        // We copied the whole block so go get another block.
        block = iterator();
        
#ifdef debug_manager
        std::cerr << "Received block of size " << block.size() << endl;
#endif
        
    }
    
    if (cursor < prefix.size()) {
        // We should have copied the whole prefix
        free(buffer);
        throw std::runtime_error("Input ended before expected prefix could be read");
    }
    
    // Shrink the buffer to jsut what we filled
    void* new_buffer = realloc(buffer, cursor);
    if (!new_buffer) {
        free(buffer);
        throw std::runtime_error("Could not shrink buffer to " +
            std::to_string(cursor) + " bytes");
    }
    buffer = new_buffer;
    
#ifdef debug_manager
    std::cerr << "Create chain with preallocated link of size " << cursor << endl;
#endif
    
    // Just hand the whole block over
    chainid_t chain = open_chain(0, cursor, buffer).first;
    
    // Assume the allocator data structures are ready.
    connect_allocator_at(chain, prefix.size());
    
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

    // Reclaim any bytes from the end of the file that we can.
    size_t bytes_to_drop = reclaim_tail(chain);
    // We'll need to get the total chain size out of the first link.
    size_t total_size;

    // Remember the FD of the chain for closing the file.
    int fd;
    
    // Remember any MIO mappings to unmap
    std::vector<std::pair<std::unique_ptr<mio::mmap_sink>, std::unique_ptr<mio::mmap_source>>> mio_clean;
    // Remember any normal memory to clean up
    std::vector<void*> normal_clean;

    {
        // Get write access to manager data structures
        std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        auto head_entry = Manager::address_space_index.find((intptr_t) chain);
        
        if (head_entry == Manager::address_space_index.end()) {
            throw std::runtime_error("Trying to destroy nonexistent chain");
        }
        
        // Store the info we need to tear down the backing file.
        fd = head_entry->second.fd;
        total_size = head_entry->second.total_size;
        
        auto link_entry = head_entry;
        
        while(link_entry != Manager::address_space_index.end()) {
            // Clean up each link
            if (link_entry->second.is_mapped()) {
                // Clear up any MIO mapping
                // Note that we're allowed to modify the actual record with "read" access, just not the maps.
                mio_clean.emplace_back(std::move(link_entry->second.release()));
            } else {
                // This is just a normal char array allocation.
                normal_clean.emplace_back((void*)link_entry->first);
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
    
    for (auto& mappings : mio_clean) {
        mappings.first.reset();
        mappings.second.reset();
    }
    
    for (auto& mapping : normal_clean) {
        free(mapping);
    }
    
    if (fd) {
        // We have a backing file.
        // Truncate off any bytes we reclaimed as trailing free space.
        if (bytes_to_drop > 0 && ftruncate(fd, total_size - bytes_to_drop)) {
            throw std::runtime_error("Could not truncate " + std::to_string(bytes_to_drop) +
                " bytes off of file: " + std::string(strerror(errno)));
        }
        
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
    if (chain == NO_CHAIN) {
        // When using no chain we fall back to normal C addressing.
        return (void*) position; 
    }
    
    // Get read access to manager data structures
    std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    // Find the index over this chain's space
    auto& chain_map = Manager::chain_space_index.at(chain);
    
    // Find the first chain link starting after our position 
    auto found = chain_map.upper_bound(position);
    
    if (found == chain_map.begin() || chain_map.empty()) {
        // There won't be a link covering the position
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

std::map<intptr_t, Manager::LinkRecord>::iterator Manager::find_link(std::shared_lock<std::shared_timed_mutex>& lock, const void* address) {

    // Determine what we're looking for
    intptr_t sought = (intptr_t) address;
    
    // Find the earliest link starting after the address
    auto found = Manager::address_space_index.upper_bound((intptr_t)address);
        
    if (found == Manager::address_space_index.begin() || Manager::address_space_index.empty()) {
        // There won't be a link covering the address.
        // Say this is an address not in any chain.
        
#ifdef debug_pointers
        std::cerr << "Address " << sought << " has no links starting after it, or no link before such a link, so is not in any chain" << std::endl;
#endif
        
        return Manager::address_space_index.end();
    }
        
#ifdef debug_pointers
    std::cerr << "Address " << sought << " has a link starting after it at " << found->first << std::endl;
#endif
        
    // Go left to the link that must include the address if any does.
    --found;
    
    if (found->first + found->second.length > sought) {
        // The address is inside the link
        return found;
    } else {
        // The link ends before the address
        return Manager::address_space_index.end();
    }
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
        
        // Find the earliest link starting after the address
        auto found = find_link(lock, address);
        
        if (found == Manager::address_space_index.end()) {
            // There won't be a link covering the address.
            return std::make_pair(NO_CHAIN, (size_t) address); 
        }
        
        // Copy out link info
        link_base = found->first;
        link_offset = found->second.offset;
        link_length = found->second.length;
        chain = (chainid_t) found->second.first;
    }
    
#ifdef debug_pointers
    std::cerr << "The link before is at " << link_base << " and runs for " << link_length << std::endl;
#endif
    
    if (link_base + link_length >= sought + length) {
        // End of sought range (if different) is also in the link (exclusive end coordinate nearer or coinciding)
        
#ifdef debug_pointers
        std::cerr << "This fully covers " << sought << " so it is in chain " << chain << std::endl;
#endif

        // Emit link's chain ID, and translate address to link local offset to chain position.
        return std::make_pair(chain, link_offset + (sought - link_base));
                
    } else {
        // End of sought range is not in the link
        // The link we found covers the start but not the end of our range. This is a problem.
        throw std::runtime_error("Attempted to place address range that crosses a link boundary");
    }
}

void* Manager::get_address_in_same_chain(const void* here, size_t position) {
    // TODO: accelerate with some kind of alignment and mod scheme
    chainid_t chain = get_chain(here);
    void* result = get_address_in_chain(chain, position);
#ifdef debug_pointers
    std::cerr << "In chain of " << (intptr_t)here << " which is " << chain
        << " we find position " << position << " at " << (intptr_t) result << std::endl;
#endif
    return result;
}
    
size_t Manager::get_position_in_same_chain(const void* here, const void* address) {
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        auto here_link = find_link(lock, here);
        auto there_link = find_link(lock, address);
        
        if (here_link == Manager::address_space_index.end()) {
            if (there_link == Manager::address_space_index.end()) {
                // We're not actually in a chain. Use a raw position
                return (size_t) address;
            } else {
                throw std::runtime_error("Attempted to refer into or out of a chain!");
            }
        } else {
            if (here_link->second.first != there_link->second.first) {
                // These are links of different chains
                throw std::runtime_error("Attempted to refer across chains!");
            } else {
                // These are the same chain.
                // Get how far the address is into its link, plus the start
                // offset of the link in the chain.
                return ((intptr_t) address - (intptr_t) there_link->first) + there_link->second.offset; 
            }
        }
    }
}

std::pair<int64_t, bool> Manager::get_offset_in_same_chain(const void* here, const void* address) {
    
    // Compute tha address offset
    int64_t in_memory_offset = (intptr_t) address - (intptr_t) here;

    // Get read access to manager data structures
    std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    // Find where the pointers fall in the chain.
    auto here_link = find_link(lock, here);
    auto there_link = find_link(lock, address);
    
    if (here_link == there_link) {
        // Same link (possibly no link).
        // Just do a straight offset.
        return std::make_pair(in_memory_offset, true);
    } else if (here_link == Manager::address_space_index.end() ||
        there_link == Manager::address_space_index.end()) {
        
        // One of them is not in a chain 
        throw std::runtime_error("Attempted to refer into or out of a chain!");
    } else if (here_link->second.first != there_link->second.first) {
        // These are links of different chains
        throw std::runtime_error("Attempted to refer across chains!");
    } else {
        // These are the same chain, but different links. Compute how the
        // distance between the links in the chain differs from the distance
        // between them in memory. If the links are on the same diagonal in
        // memory, this is 0. If the link we are going to is further along in
        // memory than we expect, this is positive.
        // So we take the distance in memory and subtract the distance in the chain.
        int64_t correction = (there_link->first - here_link->first) -
            ((int64_t) there_link->second.offset - (int64_t) here_link->second.offset);
        // Then we take the distance in memory, and subtract the correction
        // (which is positive if the distance in memory is too big) to get the
        // distance in the chain.
        // We can still accelerate future accesses if the links happen to line
        // up so the distances match and the correction is 0.
        return std::make_pair(in_memory_offset - correction, correction == 0); 
    }
}

std::pair<void*, bool> Manager::follow_offset_in_same_chain(const void* here, int64_t offset) {
    // Determine where we would be if we just applied the offset directly to the address
    void* applied_local = (void*)((intptr_t) here + offset);
    
    // Get read access to manager data structures
    std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    // Find the link we are starting in.
    auto link = find_link(lock, here);
    
    if (link == Manager::address_space_index.end() ||
        (link->first <= (intptr_t) here + offset &&
        link->second.length > (intptr_t) here - link->first + offset)) {
        // We are actually in the same link (possibly no link)
        // Just need to move in memory.
        // If the link is nonexistent or writable, set the writable-direct-offset flag.
        return std::make_pair(applied_local, link == Manager::address_space_index.end() || link->second.is_writable());
    } else {
        // Need to move in this chain to a different link
        chainid_t chain = (chainid_t) link->second.first;
        
        // These are all the links along the chain
        auto& chain_map = Manager::chain_space_index.at(chain);
    
        // This is where we're going along the chain
        size_t position = link->second.offset + ((intptr_t) here - link->first) + offset;
        
        // Find the first chain link starting after our position 
        auto found = chain_map.upper_bound(position);
        if (found == chain_map.begin() || chain_map.empty()) {
            // There won't be a link covering the position
            throw std::runtime_error("Attempted to find address for position that has no link.");
        }
        // Look left and find the link we are looking for that covers the position
        --found;
        
        // Work out where we are going: that offset in the found link along the chain.
        void* applied_chain = (void*)(found->second + (position - found->first));
        
        // Return the result, and check if it's actually the same as the local
        // offset and where *we* are is writable, so we can just do that in the future.
        return std::make_pair(applied_chain, applied_chain == applied_local && link->second.is_writable());
    }
}

void Manager::dump(chainid_t chain) {
    
    if (chain == NO_CHAIN) {
        // No need to dump a non-chain
        return;
    }
    
    auto& chain_space = chain_space_index.at(chain);
    
    std::cerr << "Chain " << chain << std::endl;
    
    for (auto it = chain_space.begin(); it != chain_space.end(); ++it) {
        size_t chain_offset = it->first;
        intptr_t link_id = it->second;
        
        LinkRecord* link = &address_space_index.at(link_id);
        assert(link->offset == chain_offset);
        
        std::cerr << "\tLink " << link_id << " offset " << link->offset
            << " length " << link->length << std::endl;
        
        AllocatorHeader* header;
        
        size_t link_cursor = 0;
        if (it == chain_space.begin()) {
            // In the first link we have a prefix and an allocator header
            std::cerr << "\t\t" << link_cursor + link->offset << "\t"
                << "Prefix: " << link->prefix_size << " bytes" << std::endl;
            link_cursor += link->prefix_size;
            std::cerr << "\t\t" << link_cursor + link->offset << "\t"
                << "AllocatorHeader: " << sizeof(AllocatorHeader) << " bytes" << std::endl;
            header = (AllocatorHeader*)get_address_in_chain(chain, link_cursor + link->offset, sizeof(AllocatorHeader));
            std::cerr << "\t\t\t\tFirst free: " << (header->first_free ? get_chain_and_position(header->first_free).second : 0) << std::endl;
            std::cerr << "\t\t\t\tLast free: " << (header->last_free ? get_chain_and_position(header->last_free).second : 0) << std::endl;
            link_cursor += sizeof(AllocatorHeader);
        }
        // Now we know the cursor is at an allocator block.
        while (link_cursor < link->length) {
            AllocatorBlock* block = (AllocatorBlock*)get_address_in_chain(chain, link_cursor + link->offset, sizeof(AllocatorBlock));
            
            bool is_free = (block->prev || block->next || (header->first_free == block && header->last_free == block));
            
            std::cerr << "\t\t" << link_cursor + link->offset << "\t"
                << "AllocatorBlock" << (is_free ? " (FREE)" : "") << ": "
                    << sizeof(AllocatorBlock) << " bytes (" << (intptr_t)block << ")" << std::endl;
            std::cerr << "\t\t\t\tPrev free: " << (block->prev ? get_chain_and_position(block->prev).second : 0) << std::endl;
            std::cerr << "\t\t\t\tNext free: " << (block->next ? get_chain_and_position(block->next).second : 0) << std::endl;
            link_cursor += sizeof(AllocatorBlock);
            std::cerr << "\t\t" << link_cursor + link->offset << "\t"
                << (is_free ? "Free Space" : "Payload") << ": " << block->size << " bytes (" << (intptr_t)block->get_user_data() << ")" << std::endl;
                
            if (!is_free) {
                // Dump the block
                std::stringstream ss;
                for (size_t i = 0; i < block->size; i++) {
                    if (i % 8 == 0) {
                        if (i != 0) {
                            ss << std::endl;
                        }
                        ss << "\t\t\t\t";
                    } else {
                        ss << " ";
                    }
                    ss << std::hex << std::setw(2) << std::setfill('0') << (int)*((uint8_t*)block->get_user_data() + i);
                }
                ss << std::endl;
                std::cerr << ss.str();
            }
                
            link_cursor += block->size;
        }
    }
}

void Manager::dump_links(ostream& out) {
    out << "All chain links: " << std::endl;
    for (auto& link : address_space_index) {
        out << "\t" << (void*)link.first 
            << "-" << (void*)(link.first + link.second.length) 
            << " in chain " << link.second.first 
            << " with next link at " << link.second.next 
            << " is " << (link.second.is_mapped() ? " not " : "" ) << "mapped" 
            << std::endl; 
    }
}
    

void* Manager::allocate_from(chainid_t chain, size_t bytes) {
#ifdef debug_manager
    cerr << "Allocate " << bytes << " bytes from chain " << chain << endl;
    dump(chain);
#endif
    
    if (chain == NO_CHAIN) {
#ifdef debug_manager
        cerr << "Fall back to malloc" << endl;
#endif
        // Just allocate off the heap
        void* allocated = malloc(bytes);
        if (allocated == nullptr) {
            throw std::bad_alloc();
        }
        
#ifdef debug_manager
        cerr << "Allocated from heap at " << (intptr_t) allocated << endl;
#endif
        
        if (check_chains) {
            // Make sure it didn't come from a chain, because it should have come from the heap.
            auto source_chain = get_chain(allocated);
            if (source_chain != NO_CHAIN) {
                // Report error with some debugging details.
                std::cerr << "Error: tried to allocate non-chain memory but got memory at " << (intptr_t)allocated << " that appears to be in chain " << source_chain << std::endl;
                dump_links();
            }
            assert(source_chain == NO_CHAIN);
        }
        
        return allocated;
    }
    
    // How much space do we need with block overhead, if we need a new block?
    size_t block_bytes = bytes + sizeof(AllocatorBlock);
    
    AllocatorBlock* found;
    
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // With exclusive use of the free list
    
        // This will hold a ref to the free block we found or made that is big enough to hold this item.
        // Starts null if there is no first_free.
        found = header->first_free;
#ifdef debug_manager
        std::cerr << "Start at " << (intptr_t) found << std::endl;
#endif
        while (found && found->size < bytes) {
            // Won't fit here. Try the next place.
#ifdef debug_manager
            std::cerr << "Skip block of " << found->size << " bytes at " << (intptr_t) found << std::endl;
#endif
            AllocatorBlock* old = found;
            assert(found != found->next);
            found = found->next;
            if (found && found->prev != old) {
                throw std::runtime_error("Free block " + std::to_string((intptr_t) found) +
                    " must point back to " + std::to_string((intptr_t) old) + " but instead points to " +
                    std::to_string((intptr_t) found->prev));
            }
        }
       
        if (!found) {
            // We have no free memory big enough.
            // We will make a new link.
            LinkRecord* new_link;
            // How big will it be? At least as big as the block we need from it.
            size_t new_link_size = block_bytes;
            
            {
                // Get write access to chain data structures.
                std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
                
                // Find the first link in the chain
                LinkRecord& first = address_space_index.at((intptr_t) chain);
                
                // Find the last link in the chain
                LinkRecord& last = address_space_index.at(first.last);
                
                // We need our factor as much memory as last time, or enough for
                // the thing we want to allocate
                new_link_size = std::max(last.length * SCALE_FACTOR, new_link_size);
                
#ifdef debug_manager
                std::cerr << "Create new link of size " << new_link_size << " bytes" << std::endl;
#endif
                
                // Go get the new link
                new_link = &add_link(first, new_link_size);
            }
            
            // Work out where new free memory will start
            found = (AllocatorBlock*) get_address_in_chain(chain, new_link->offset, block_bytes);
            
#ifdef debug_manager
            std::cerr << "New link starts at " << (intptr_t)found << std::endl;
#endif
            
            // Construct the block
            new (found) AllocatorBlock();
            
            // Set up its size (all of the new link except the block header)
            found->size = new_link_size - sizeof(AllocatorBlock);
            
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
            
#ifdef debug_manager
            std::cerr << "Split block of " << found->size << " bytes at " << (intptr_t)found << std::endl;
#endif
            
            // So split the block.
            AllocatorBlock* second = found->split(bytes);
            
#ifdef debug_manager
            std::cerr << "Created block of " << second->size << " bytes at " << (intptr_t)second << std::endl;
#endif
            
            if (header->last_free == found) {
                // And fix up the end of the linked list
                header->last_free = second;
            }
        }
        
        // Now we have a free block of the right size. Make it not free.
#ifdef debug_manager
        std::cerr << "Detach block of " << found->size << " bytes at " << (intptr_t)found << std::endl;
#endif
        auto connected = found->detach();
        if (header->first_free == found) {
            // This was the first free block.
            // The first free block is now the right neighbor, if any.
            header->first_free = connected.second;
#ifdef debug_manager
            std::cerr << "\tWas first free block; now that's " << (intptr_t)header->first_free.get() << std::endl;
#endif
        }
        if (header->last_free == found) {
            // This was the last free block
            // The last free block is now the left neighbor, if any.
            header->last_free = connected.first;
#ifdef debug_manager
            std::cerr << "\tWas last free block; now that's " << (intptr_t)header->last_free.get() << std::endl;
#endif
        }
        
        if (!header->first_allocated) {
            // This is the first thing we are allocating from the chain, so we
            // need to remember where it is so we can re-find it when someone
            // remaps the chain later.
#ifdef debug_manager
            std::cerr << "Recording first allocation" << std::endl;
#endif
            header->first_allocated = found->get_user_data();
        }
    });
    
#ifdef debug_manager
    std::cerr << "Allocated at " << found->get_user_data() << std::endl;
    dump(chain);
#endif
    
    // Give out the address of its data
    void* allocated = found->get_user_data();
    
    if (check_chains) {
        assert(get_chain(allocated) == chain);
    }
    
    return allocated;
}

void* Manager::allocate_from_same_chain(void* here, size_t bytes) {
    // TODO: accelerate by coalescing locks?
    return allocate_from(get_chain(here), bytes);
}

void Manager::preload_chain(chainid_t chain, bool blocking) {
    // madvise calls need to be page-aligned, so get the page size
    intptr_t page_size = (intptr_t) getpagesize();
    
    // We may have a way to use madvise to populate a mapping. If we think so, this will be nonzero.
    int populate_read_advice = 0;
#ifndef __APPLE__
#ifdef MADV_POPULATE_READ
    // Linux has an MADV_POPULATE_READ in kernel 5.14+ that will wait
    // for everything to be loaded from disk once.
    // The new advice value for a blocking fake read is in the C library.
    populate_read_advice = MADV_POPULATE_READ;
#else
    // Try guessing the number and hoping it is not anything else.
    // See https://patchwork.kernel.org/project/linux-mm/patch/20210701015228.QXA77Jpli%25akpm@linux-foundation.org/
    populate_read_advice = 22;
#endif
#endif

    scan_chain(chain, [&](const void* link_start, size_t link_length) {
        // For each link in the chain

#ifdef debug
        std::cerr << "Preloading link: " << link_start << "-" << (void*)((intptr_t)link_start + link_length) << std::endl;
#endif
        
        // Start address for load has to be page-aligned, but length just has to be nonnegative.
        void* advice_start = (void*) link_start;
        size_t advice_length = link_length;
        
        // How much of the first page isn't included?
        intptr_t before_start_bytes = (intptr_t)advice_start % page_size;
        
        // Budge the start left.
        advice_start = (void*)((intptr_t)advice_start - before_start_bytes);
        advice_length += before_start_bytes;
        if (advice_length % page_size != 0) {
            // And finish out the page
            advice_length += (page_size - advice_length % page_size);
        }
        
#ifdef debug
        std::cerr << "Preloading addresses " << advice_start << "-" << (void*)((intptr_t)advice_start + advice_length) << std::endl;
#endif
        
        if (blocking) {
            
            if (populate_read_advice) {
                // Make the call
                int result = madvise(advice_start, advice_length, populate_read_advice);
            
                if (result == 0) {
                    // It worked!
                    return;
                }
            
                // Otherwise the call failed
                auto madvise_error = errno;
                
                switch (madvise_error) {
                case EINVAL:
                    // It is possible the advice we used doesn't exist on the
                    // runtime kernel, which may not be the build kernel or
                    // batch the build glibc.
                    // Also possible something weird about the memory range,
                    // like it being secret to the process, is preventing us
                    // from using madvise() here even if every byte in the
                    // range is readable.
                    // TODO: Figure out why this seems to mostly fail. Until
                    // then, don't usually warn.
#ifdef debug
                    std::cerr << "warning[yomo::Manager::preload_chain] Cannot MADV_POPULATE_READ memory range " << advice_start << "-" << (void*)((intptr_t)advice_start + advice_length) << "; falling back to reading each page: " << strerror(madvise_error) << std::endl;
#endif
                    break;
                default:
                    // Something else weird happened. This is a problem.
                    throw  std::runtime_error(std::string("Could not prefault memory: ") + std::string(strerror(madvise_error)));
                    break;
                }
            }
            
            for (size_t page = 0; page < (advice_length / page_size); page++) {
                volatile const unsigned char* page_start = (volatile const unsigned char*) ((intptr_t)advice_start + page * page_size);
                // Read first byte of the page
                (void) *page_start;
#ifdef debug
                // Dump the page structure
                std::cerr << "Page at " << (void*)page_start << std::endl;
                for (size_t i = 0; i < page_size; i++) {
                    if (*(page_start + i) == 0) {
                        std::cerr << " ";
                    } else {
                        std::cerr << ".";
                    }
                    if ((i + 1) % 128 == 0) {
                        std::cerr << std::endl;
                    }
                }
                // See if this page in particular doesn't want to madvise in.
                std::cerr << "Re-advise page: " <<  madvise((void*)page_start, page_size, populate_read_advice) << std::endl;
#endif
            }
        } else {
            // Just tell the memory management subsystem we will want this
            int result = madvise(advice_start, advice_length, MADV_WILLNEED);
            
            if (result == 0) {
                // It worked!
                return;
            }
            
            // Otherwise the call failed
            auto madvise_error = errno;
            throw std::runtime_error(std::string("Could not mark memory needed: ") + std::string(strerror(madvise_error)));
        }
    });
}

void Manager::deallocate(void* address) {
#ifdef debug_manager
    std::cerr << "Deallocate at " << address << std::endl;
#endif
    
    // Find the chain
    chainid_t chain = get_chain(address);
    
    if (chain == NO_CHAIN) {
        // This isn't really ours.
        
        #ifdef debug_manager
            std::cerr << "Deallocate from heap" << std::endl;
        #endif
        
        // Just free it off the heap.
        free(address);
        return;
    }
    
    // Otherwise this really is in a chain.
    
    // Find the block
    AllocatorBlock* found = AllocatorBlock::get_from_data(address);
    
#ifdef debug_manager
    std::cerr << "Freeing block at " << (intptr_t)found << std::endl;
#endif
    
    
#ifdef debug_manager
    dump(chain);
#endif

    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // With exclusive use of the free list
        
        bool is_free = (found->prev || found->next || (header->first_free == found && header->last_free == found));
        if (is_free) {
            // This is free already!
            throw std::runtime_error("Detected double-free!");
        }
       
        // Find the block in the free list after it, if any
        AllocatorBlock* right = header->first_free;
        
        while(right && Manager::get_chain_and_position(right).second < Manager::get_chain_and_position(found).second) {
            // We have a free block, but it occurs before the block being freed in the chain.
            // TODO: can we save chain lookups here somehow?
            // Go to the next free block, or off the end if that was the last one.
#ifdef debug_manager
            std::cerr << "\tComes after block " << (intptr_t)right << " in chain space" << std::endl;
#endif
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
#ifdef debug_manager
            std::cerr << "\tIs new last free block, replacing " << (intptr_t)header->last_free.get() << std::endl;
#endif
            header->last_free = found;
        }
        if (header->first_free == right) {
#ifdef debug_manager
            std::cerr << "\tIs new first free block, replacing " << (intptr_t)header->first_free.get() << std::endl;
#endif
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
    
#ifdef debug_manager
    std::cerr << "Deallocated." << std::endl;
    dump(chain);
#endif
}

void* Manager::find_first_allocation(chainid_t chain, size_t bytes) {

    assert(chain != NO_CHAIN);

    void* first_allocated;
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // Go look in the header for where we stashed this.
        first_allocated = header->first_allocated;
    });
    return first_allocated;
}

size_t Manager::get_chain_size(chainid_t chain) {
    if (chain == NO_CHAIN) {
        return 0;
    }

    LinkRecord* first;
    
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the first link
        first = &address_space_index.at((intptr_t) chain);
    }
    
    // The size is recorded here
    return first->total_size;

}

bool Manager::is_chain_writable(chainid_t chain) {
    if (chain == NO_CHAIN) {
        return false;
    }

    LinkRecord* first;
    
    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        // Find the first link
        first = &address_space_index.at((intptr_t) chain);
    }
    
    // Check if the first link is writable.
    return first->is_writable();

}

std::tuple<size_t, size_t, size_t> Manager::get_usage(chainid_t chain) {
    if (chain == NO_CHAIN) {
        return std::make_tuple<size_t, size_t, size_t>(0, 0, 0);
    }
    
    // We need the total chain length so we can tell if the last
    // chain-contiguous run of free blocks abuts the end of the chain.
    size_t total_length = get_chain_size(chain);
    
    // How many bytes of free payload (and associated headers) have we seen?
    size_t free_bytes = 0;
    
    // How many free bytes are at the end?
    size_t reclaimable = 0;
    
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        AllocatorBlock* free_block = header->last_free;
        while (free_block) {
            // The block is free, so count it and its header as free
            free_bytes += free_block->size + sizeof(AllocatorBlock);
            
            // Where in the chain does it end?
            size_t block_end = get_chain_and_position(free_block).second +
                free_block->size + sizeof(AllocatorBlock);
            
            if (block_end == total_length - reclaimable) {
                // This abuts the existing trailing reclaimable space, so
                // expand it over this block.
                reclaimable += free_block->size + sizeof(AllocatorBlock);
            }
            
            // Go left to the previous free block in the chain, if any.
            free_block = free_block->prev;
        }
    });

#ifdef debug_manager
    std::cerr << "Memory usage: " << total_length << " total, "
        << free_bytes << " free, "
        << reclaimable << " reclaimable" << endl;
#endif
    
    return std::make_tuple(total_length, free_bytes, reclaimable);
}

void Manager::scan_chain(chainid_t chain, const std::function<void(const void*, size_t)>& iteratee) {
    // Start a cursor in the chain
    size_t chain_offset = 0;
    
    std::pair<void*, size_t> block = get_address_and_length_in_chain(chain, chain_offset);
    
    while (block.second > 0) {
        // Show the block to the iteratee
        iteratee(block.first, block.second);
        // And advance
        chain_offset += block.second;
        block = get_address_and_length_in_chain(chain, chain_offset);
    }
}

Manager::AllocatorHeader* Manager::find_allocator_header(chainid_t chain) {
    
    assert(chain != NO_CHAIN);

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
    
    assert(chain != NO_CHAIN);
    
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

size_t Manager::reclaim_tail(chainid_t chain) {
    if (chain == NO_CHAIN) {
        // Nothing to free here.
        return 0;
    }
    
    if (!is_chain_writable(chain)) {
        // Can't free anything.
        return 0;
    }
    
    // Get the past-end position in the chain, and use that as our cursor to walk backward.
    size_t first_unused_byte = get_chain_size(chain);
    
    // Track how many bytes we removed
    size_t reclaimed_bytes = 0;
    
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        while (header->last_free) {
            // For each free block, end to start
            AllocatorBlock* last_free = header->last_free;
            size_t total_block_size = last_free->size + sizeof(AllocatorBlock);
            
            // Get its past-end position in the chain
            size_t past_block_end = get_chain_and_position(last_free).second + total_block_size;
                
            if (first_unused_byte == past_block_end) {
                // We can pop off this block
                
#ifdef debug_manager
                std::cerr << "Reclaiming trailing free block " << last_free << std::endl;
#endif
                
                // Record reclaiming these bytes
                reclaimed_bytes += total_block_size;
                // Bring the cursor back to before the block we reclaimed.
                first_unused_byte -= total_block_size;
                
                // Remove the block from the free list. We have to update the
                // header pointers ourselves.
                auto connected = last_free->detach();
                header->last_free = connected.first;
                if (header->first_free == last_free) {
                    // This was the first free block.
                    // The first free block is now the right neighbor, if any.
                    header->first_free = connected.second;
                }
                
                // Now last_free has moved, so check that block, if it exists.
            } else {
                // We've reached a block we can't reclaim. We have to stop the scan.
                
#ifdef debug_manager
                std::cerr << "Last free block ends at " << past_block_end
                    << " and not " << first_unused_byte << std::endl;
#endif
                
                break;
            }
        }
    });
    
#ifdef debug_manager
    std::cerr << "Reclaimed " << reclaimed_bytes << " bytes at end of chain " << chain << std::endl;
#endif
    
    // We reclaimed all those blocks and their headers from the allocator's management.
    // Now they're totally unused at the end of the chain.
    return reclaimed_bytes;
}

void Manager::check_heap_integrity(chainid_t chain) {
     if (chain == NO_CHAIN) {
        // Nothing to scan.
        return;
    }
    
    // All blocks, allocated or free, form a linked list connected by block length.
    
    with_allocator_header(chain, [&](AllocatorHeader* header) {
        // Get the past-end position in the chain, where we expect out scan to end.
        size_t first_unused_byte = get_chain_size(chain);
        
        // Get the position that the next block (allocated or free) should be at
        size_t block_cursor = get_chain_and_position(header).second + sizeof(AllocatorHeader);
        
        while (block_cursor < first_unused_byte) {
            // Find this block
            AllocatorBlock* block = (AllocatorBlock*) get_address_in_chain(chain, block_cursor, sizeof(AllocatorBlock));
            
            // Make sure its size isn't some enormous garbage that will make us overflow
            if (block->size > first_unused_byte) {
                throw std::runtime_error("An allocator block at offset " + std::to_string(block_cursor) +
                    " in chain " + std::to_string(chain) +
                    " has a size of " + std::to_string(block->size) +
                    " which is larger then the whole chain's length of " +
                    std::to_string(first_unused_byte) + " bytes. This indicated data corruption.");
            }
            
            // Advance to the next block
            block_cursor += sizeof(AllocatorBlock) + block->size;
        }
        
        if (block_cursor != first_unused_byte) {
            // We've blown past the end of the chain.
            throw std::runtime_error("An allocator block in chain " + std::to_string(chain) +
                " runs to byte " + std::to_string(block_cursor) +
                " but the chain only has " + std::to_string(first_unused_byte) +
                " bytes in it. Is the backing file truncated?");
        }
    });
}

size_t Manager::count_chains() {
    // Get write access to manager data structures
    std::unique_lock<std::shared_timed_mutex> lock(mutex);
    
    // Count the chains
    return chain_space_index.size();
}

size_t Manager::count_links() {
    // Get write access to manager data structures
    std::unique_lock<std::shared_timed_mutex> lock(mutex);
    
    // Count the links
    return address_space_index.size();
}

std::pair<Manager::chainid_t, bool> Manager::open_chain(int fd, size_t start_size, void* link_data) {

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
    
    // TODO: deduplicate initial link and add_link?
    
    if (fd) {
        // Use file-mapped memory
        
        if (link_data) {
            // If there's a file, we have to make all the memory.
            throw std::logic_error("Cannot use preallocated block of memory with a backing file!");
        }
        
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
        if (file_size < start_size) {
            // The file is currently too small and we need to expand it to be able to write to it.
            if (ftruncate(our_fd, start_size)) {
                throw std::runtime_error("Could not grow file to be mapped: " + std::string(strerror(errno)));
            }
        }
        
        // Make the MIO mapping of the whole file, or throw.
        record.map_file(our_fd);
    
        // Remember where the memory starts
        mapping_address = record.get_mapped_address();
        
        // Fill in the record for a MIO mapping
        record.length = record.get_mapped_length();
        record.fd = our_fd;
        
        // We may have had data
        had_data = (file_size != 0);
    } else {
        // Use boring allocated memory
        
        // TODO: when MIO gets anonymous mapping support, use that.
        
        if (!link_data) {
            // Allocate our own link
            link_data = malloc(start_size);
        }
        if (!link_data) {
            throw std::runtime_error("Could not allocate initial " + std::to_string(start_size) + " bytes");
        }
        
        // Remember where the memory starts
        mapping_address = (intptr_t) link_data;
        
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
    
    assert(chain != NO_CHAIN);

    // Get write access to manager data structures
    std::unique_lock<std::shared_timed_mutex> lock(Manager::mutex);
    
    LinkRecord& head = Manager::address_space_index.at((intptr_t)chain);
    
    if (head.total_size >= new_total_size) {
        // Already done.
        return;
    }
    
    size_t new_bytes = new_total_size - head.total_size;
    
    add_link(head, new_bytes);
}

std::pair<void*, size_t> Manager::get_address_and_length_in_chain(chainid_t chain, size_t position) {
    if (chain == NO_CHAIN) {
        // This doesn't make any sense for no chain
        throw std::runtime_error("Cannot enumerate blocks in non-chain");
    }
    
    // TODO: Deduplicate code with get_address_in_chain? This version always
    // needs to have the length.
    
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
    
    // Go get the LinkRecord for the link
    LinkRecord& link = Manager::address_space_index.at(found->second);
    
    if (position - link.offset > link.length) {
        // Actually we went off the end. Say there's nothing here.
        return std::make_pair(nullptr, 0);
    }
    
    // Convert to address by offsetting from address of containing block, and include length
    return std::make_pair((void*)(found->second + (position - found->first)), link.length - (position - link.offset));
}

Manager::LinkRecord& Manager::add_link(LinkRecord& head, size_t new_bytes, void* link_data) {
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
        if (link_data) {
            // If there's a file, we have to make all the memory.
            throw std::logic_error("Cannot use preallocated block of memory with a backing file!");
        }
    
        // Grow the file
        if (ftruncate(head.fd, new_total)) {
            throw std::runtime_error("Could not grow mapped file: " + std::string(strerror(errno)));
        }
    
        // Map the new tail with MIO
        new_tail.map_file_range(head.fd, old_end, new_bytes);
        
        // Find its address
        mapping_address = new_tail.get_mapped_address();
    } else {
        if (!link_data) {
            // Allocate our own link
            link_data = malloc(new_bytes);
        }
        if (!link_data) {
            throw std::runtime_error("Could not allocate an additional " + std::to_string(new_bytes) + " bytes");
        }
        
        // Remember where the memory starts
        mapping_address = (intptr_t) link_data;
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
    head.total_size += new_bytes;
    
    return where;
}

Manager::chainid_t Manager::copy_chain(chainid_t chain, int fd) {

    assert(chain != NO_CHAIN);

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
    
#ifdef debug_manager
    std::cerr << "Duplicating chain of total size " << total_size << " bytes" << std::endl;
#endif
    
    if (fd) {
        // Make sure to clear out the file we are writing to in case we are trying
        // to overwrite a file.
        if (ftruncate(fd, 0)) {
            throw std::runtime_error("Could not truncate destination file: " + std::string(strerror(errno))); 
        }
    }
    
    // Make the new chain with the appropriate size hint.
    std::pair<chainid_t, bool> chain_info = open_chain(fd, total_size);
    auto& new_chain = chain_info.first;
    auto& had_data = chain_info.second;
    
    if (had_data) {
        // If there's something there after we truncate, someone else is
        // fighting us over the file.
        throw std::runtime_error("Data was added to file at FD " + std::to_string(fd) + " while we were clearing it");
    }
    
    // If we made a fresh new chain we know the first block will be total_size.
    
    // Copy all the data
    
    // We already know the addresses of the first links
    intptr_t from_link_addr = (intptr_t) chain;
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
        
        // TODO: We really should only ever use this to copy from more divided
        // to more consolidated blocks; if a new boundary occurs in the new
        // chain it could break up data that needs to be contiguous. Luckily,
        // we're in charge of creating our destination chain.
        
        // Make sure we have mapping addresses of and pointers to records for
        // the current links.
        {
            // Get read access to manager data structures
            std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
            if (from_link && (cursor - from_link->offset) == from_link->length) {
                // Advance out of old from link
                from_link_addr = from_link->next;
                from_link = nullptr;
            }
            if (to_link && (cursor - to_link->offset) == to_link->length) {
                // Advance out of old to link
                to_link_addr = to_link->next;
                to_link = nullptr;
            }
            
            if (!from_link) {
                // We aren't using the same from link as last time
                from_link = &Manager::address_space_index.at(from_link_addr);
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

#ifdef debug_manager
        std::cerr << "Copying " << to_copy << " bytes at offset " << from_link_cursor
            << " in link " << from_link_addr << " of length " << from_link->length
            << " to offset " << to_link_cursor
            << " in link " << to_link_addr << " of length " << to_link->length << std::endl;
#endif
        
        memcpy((void*)(to_link_addr + to_link_cursor), (void*)(from_link_addr + from_link_cursor), to_copy);
        
        // Record the copy
        cursor += to_copy;
    }
    
    // Set up the allocator data structures.
    connect_allocator_at(new_chain, prefix_size);
    
    return new_chain;
}

void Manager::set_up_allocator_at(chainid_t chain, size_t offset, size_t space) {

    assert(chain != NO_CHAIN);

    // Find where the header and block should go
    AllocatorHeader* header = (AllocatorHeader*) get_address_in_chain(chain, offset, sizeof(AllocatorHeader));
    AllocatorBlock* block = (AllocatorBlock*) get_address_in_chain(chain, offset + sizeof(AllocatorHeader), sizeof(AllocatorBlock));
    
    // Construct the header
    new (header) AllocatorHeader();
    // And point it at the block
    header->first_free = block;
    header->last_free = block;
    // Let the constructor handle the other fields.
    
    // Construct the block
    new (block) AllocatorBlock();
    block->prev = nullptr;
    block->next = nullptr;
    block->size = space - sizeof(AllocatorHeader) - sizeof(AllocatorBlock);
    
    // Now that it's there, connect to it
    connect_allocator_at(chain, offset);
}

void Manager::connect_allocator_at(chainid_t chain, size_t offset) {

    assert(chain != NO_CHAIN);

    {
        // Get read access to manager data structures
        std::shared_lock<std::shared_timed_mutex> lock(Manager::mutex);
        
        LinkRecord& head = Manager::address_space_index.at((intptr_t) chain);
        // Save the allocator position
        head.prefix_size = offset;
    }
    
    if (check_chains) {
        // Make sure that we never allow an allocator to come up broken.
        check_heap_integrity(chain);
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
    if (new_next->next) {
        // We had an old successor that needs to point to our new successor.
        new_next->next->prev = new_next;
    }
    
    // Return the new block
    return new_next;
}

std::pair<Manager::AllocatorBlock*, Manager::AllocatorBlock*> Manager::AllocatorBlock::detach() {
    // Grab out initial neighbors
    pair<AllocatorBlock*, AllocatorBlock*> old_neighbors = make_pair(prev.get(), next.get());
    
#ifdef debug_manager
    std::cerr << "\tOriginal neighbors: " << (intptr_t)old_neighbors.first << " and " << (intptr_t)old_neighbors.second << std::endl;
#endif
    
    if (prev) {
#ifdef debug_manager
        std::cerr << "\tDetach from prev at " << (intptr_t)prev.get() << std::endl;
#endif
        // Attach the thing before us to whatever is after us instead of us.
        prev->next = old_neighbors.second;
        // Null out our prev; it can't be relied on to point anywhere safe.
        prev = nullptr;
    }
    
    if (next) {
#ifdef debug_manager
        std::cerr << "\tDetach from next at " << (intptr_t)next.get() << std::endl;
#endif
        // Attach the thing after us to whatever was before us instead of us
        next->prev = old_neighbors.first;
        // Null out our next; it can't be relied on to point anywhere safe.
        next = nullptr;
    }
    
    return old_neighbors;
}

void Manager::AllocatorBlock::attach(AllocatorBlock* left, AllocatorBlock* right) {
#ifdef debug_manager
    std::cerr << "\tAttach block at " << (intptr_t)this << " between "
        << (intptr_t)left << " and " << (intptr_t)right << std::endl;
#endif
    
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

}

