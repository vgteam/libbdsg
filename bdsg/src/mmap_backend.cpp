#include "bdsg/internal/mmap_backend.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cassert>
#include <sstream>


/** \file mmap_backend.cpp.cpp
 * Implement MmapBackend base class for mmap-able things.
 */
 
//#define debug
//#define debug_ref

namespace bdsg {

// Because Mac doesn't have mremap, we need a remapping function that takes the FD.

/**
 * mremap-like implementation that always copies data into the new mapping if
 * it is not a shared mapping to a file.
 * Useful on systems where mremap() is not available.
 *
 * Resizes the mapping at old_address of old_size to be a mapping of new_size,
 * probably at a new address.
 *
 * If fd is -1, the new mapping is anonymous, and is copied into.
 *
 * If fd is not -1, the new mapping is a shared mapping from that file.
 *
 * Returns MAP_FAILED if the mapping cannot be created or anything else goes wrong.
 */
static void* copying_mremap(int fd, void* old_address, size_t old_size, size_t new_size) {

    if (old_size == new_size) {
        // Handle no-ops
        return old_address;
    }
    
    if (fd == -1) {
        // We are working with an anonymous mapping, or at least want to have
        // an anonymous mapping next.
        // We can't actually grow it, because if we call mmap() asking for it
        // to be bigger we might clobber something after it.
        // We can't have allocated a big sparse arena mapping to let it grow
        // into because we also probably don't have MAP_NORESERVE.
        // We can't shrink it because the unmap we would need to do to shrink
        // it isn't page-aligned.
        
        // TODO: map an arena that gets bigger in powers of 2 that we can grow
        // into, so we can just map more memory in there when we need it
        // without forcing a copy. We want to worry about the costs of 2x-ing
        // instead of the user code.
        
        // Map a new version
        void* new_address = ::mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (new_address == MAP_FAILED) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not create anonymous mapping: " << ::strerror(problem);
            std::cerr << ss.str() << std::endl;
            return MAP_FAILED;
        }
        
        // Copy the part of the data that should be preserved
        ::memcpy(new_address, old_address, std::min(old_size, new_size));
        
        // Unmap the old memory
        if (::munmap(old_address, old_size) != 0) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not remove old mapping: " << ::strerror(problem);
            // Try to unmap the new mapping so we don't leave it behind
            // after the exception is caught.
            ::munmap(new_address, new_size);
            std::cerr << ss.str() << std::endl;
            return MAP_FAILED;
        }
        
        // Return the new address
        return new_address;
    } else {
        // We are working with a file-backed mapping.
        
        // TODO: We should be smart and discard the tail end in place if we're
        // shrinking. Right now we don't even try to not move.
        
        // Map first, then unmap, so that if map fails we don't leave the
        // mapping gone and the object in a bad state for the destructor.
        
        // Always make a shared mapping.
        // Private and anonymous mappings both go around with no associated
        // open FD to call this function with.
        void* new_address = ::mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (new_address == MAP_FAILED) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not map memory: " << ::strerror(problem);
            std::cerr << ss.str() << std::endl;
            return MAP_FAILED;
        }
        
        if (::munmap(old_address, old_size) != 0) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not remove old mapping: " << ::strerror(problem);
            
            // Clean up the new mapping, if possible
            ::munmap(new_address, new_size);
            
            std::cerr << ss.str() << std::endl;
            return MAP_FAILED;
        }
        
        return new_address;
    }

}

/**
 * Remap the given region of the given file descriptor (or -1) to be the given
 * size.
 *
 * Return the new mapped address, which may have moved even if MREMAP_MAYMOVE
 * is not provided as a flag. If we absolutely can't get a new mapping returns
 * MAP_FAILED.
 *
 * If we just can't grow or shrink the mapping, may destroy and recreate the
 * mapping, or even copy contents of an anonymous mapping that can't grow.
 */
static void* portable_mremap(int fd, void* old_address, size_t old_size, size_t new_size, int mremap_flags) {
    
#ifdef MREMAP_MAYMOVE
    // Assume if we have the flag macros for it, we actually have mremap.
    return ::mremap(old_address, old_size, new_size, mremap_flags);
#else
// Give the flag a value for user code later
#define MREMAP_MAYMOVE (1)

    // Call the copying-if-anonymous-or-private version.
    // We can't respect the moving flags.
    return copying_mremap(fd, old_address, old_size, new_size);
    
#endif
}



MmapBackend::~MmapBackend() {
    // Don't check errors here!
    if (serializedData != nullptr) {
        // Length must be correct
        ::munmap(serializedData, serializedLength);
        serializedData = nullptr;
        serializedLength = 0;
    }
    if (serializedFD != -1) {
        ::close(serializedFD);
        serializedFD = -1;
    }
}

void MmapBackend::dissociate() {
    if (serializedFD != -1) {
        // If we have a file we are writing back to
        
        // We really should have a mapping.
        assert(serializedData != nullptr);
        
#ifdef debug
        std::cerr << "Cutting file write-back association." << std::endl;
#endif
        
        // Remap as private (just over the existing mapping, without unmapping)
        void* new_mapping = ::mmap(serializedData, serializedLength, PROT_READ | PROT_WRITE, MAP_PRIVATE, serializedFD, 0);
        
        if (new_mapping == MAP_FAILED) {
            // Mapping failed!
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not remap private: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        
        if (new_mapping != serializedData) {
            // The data had to move
            
            // So we need to get rid of the old one
            if (::munmap(serializedData, serializedLength) != 0) {
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not remove old mapping: " << ::strerror(problem);
                // Try to unmap the new mapping so we don't leave it behind
                // after the exception is caught.
                ::munmap(new_mapping, serializedLength);
                throw std::runtime_error(ss.str());
            }
            
            serializedData = new_mapping;
        }
        
        // And close the file
        close_fd(serializedFD);
        serializedFD = -1;
    } else {
#ifdef debug
        std::cerr << "No file write-back association to cut." << std::endl;
#endif
    }
}

void MmapBackend::serialized_data_resize(size_t bytes) {
    
    // Determine new total size including magic number
    size_t new_serialized_length = MAGIC_SIZE + bytes;
    
#ifdef debug
    std::cerr << "Resizing to " << new_serialized_length << " = " << MAGIC_SIZE << " magic + " << bytes << " user..." << std::endl;
#endif
    
    if (serializedData == nullptr) {
        // We need to initially allocate, and populate the magic number.
        
#ifdef debug
        std::cerr << "\tCreate new anonymous mapping" << std::endl;
#endif
        
        // No file ought to be associated
        assert(serializedFD == -1);
        assert(mappingFileSize == std::numeric_limits<size_t>::max());
        
        serializedData = ::mmap(nullptr, new_serialized_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (serializedData == MAP_FAILED) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not create anonymous mapping: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        
#ifdef debug
        std::cerr << "\tAnonymous mapping at " << (void*)serializedData << std::endl;
#endif
        
        // Fill in the magic number
        *((uint32_t*)serializedData) = htonl(get_magic_number());
        
        // Save the length
        serializedLength = new_serialized_length;
        
    } else if (new_serialized_length == serializedLength) {
        // Some data is already allocated, and it is already the right size.
        // Do nothing.
        
#ifdef debug
        std::cerr << "\tLength already correct" << std::endl;
#endif
        
        return;
    } else {
        // Some data is already allocated, and it is the wrong size.
        
#ifdef debug
        std::cerr << "\tReallocating..." << std::endl;
#endif
        
        // See if we have a file
        if (serializedFD != -1) {
            // If so, we need to lengthen or shorten it.
            // Supposedly you can get undefined behavior with ftruncate and
            // partial mapped pages, but the man page at
            // <https://linux.die.net/man/3/ftruncate> doesn't back that up.
        
#ifdef debug
            std::cerr << "\t\tResizing file mapping..." << std::endl;
#endif
            
            if (new_serialized_length > serializedLength) {
                // Lengthening, so truncate and then remap, allowing to move
                if (::ftruncate(serializedFD, new_serialized_length) != 0) {
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not grow mapped file: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                void* new_mapping = portable_mremap(serializedFD, serializedData,
                                                    serializedLength, new_serialized_length,
                                                    MREMAP_MAYMOVE);
                if (new_mapping == MAP_FAILED) {
                    // This one returns a sentinel and not nullptr on error.
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not grow mapping: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                
                // Commit to object
                serializedData = new_mapping;
                serializedLength = new_serialized_length;
                mappingFileSize = new_serialized_length;
            } else {
                // Shortening, so remap in place and then truncate.
                void* new_mapping = portable_mremap(serializedFD, serializedData,
                                                    serializedLength, new_serialized_length,
                                                    0);
                if (new_mapping == MAP_FAILED) {
                    // This one returns a sentinel and not nullptr on error.
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not shrink mapping: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                
                // We can't guarantee that it didn't move, even if we told it
                // not to, because sometimes we don't have a real mremap(). So
                // if it managed to move, record that.
                serializedData = new_mapping;
                
                // Save the length change now so we can unmap on exception
                serializedLength = new_serialized_length;
                
                if (::ftruncate(serializedFD, new_serialized_length) != 0) {
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not shrink mapped file: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                
                // Remember the new size of the file.
                mappingFileSize = new_serialized_length;
            }
        } else {
            // Otherwise, we have no open file. serializedFD == -1.
            
#ifdef debug
            std::cerr << "\t\tResizing mapping of " << serializedLength << " to " << new_serialized_length << "..." << std::endl;
#endif

            if (new_serialized_length > mappingFileSize) {
                // The mapping may still be a private mapping to a now-closed
                // file, instead of an anonymous mapping. If we mremap it
                // bigger than how big the file is, that may be undefined
                // behavior. TODO: See if the interaction is documented, or the
                // unspecified behavior past the end of the file is only for
                // mmap.
                
                // To deal with this, we do a faked remap that copies to a new anonymous mapping.
                void* new_mapping = copying_mremap(-1, serializedData, serializedLength, new_serialized_length);
                if (new_mapping == MAP_FAILED) {
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not replace with anonymous mapping: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                
                // Commit to object
                serializedData = new_mapping;
                serializedLength = new_serialized_length;
                // No more limiting file
                mappingFileSize = std::numeric_limits<size_t>::max();
                
#ifdef debug
                std::cerr << "\t\tResized to anonymous mapping at " << (void*)serializedData << std::endl;
#endif
                
            } else {
                // We aren't exceeding a file size on a private mapping.
                
                // Just mremap the anonymous mapping, allowing to move.
                // We pass the FD along so we can use the mremap-faking wrapper.
                void* new_mapping = portable_mremap(serializedFD, serializedData, serializedLength, new_serialized_length, MREMAP_MAYMOVE);
                if (new_mapping == MAP_FAILED) {
                    auto problem = errno;
                    std::stringstream ss;
                    ss << "Could not resize mapping: " << ::strerror(problem);
                    throw std::runtime_error(ss.str());
                }
                
                // Commit to object
                serializedData = new_mapping;
                serializedLength = new_serialized_length;
                
#ifdef debug
                std::cerr << "\t\tResized mapping at " << (void*)serializedData << std::endl;
#endif
            }
        }
    }
}

size_t MmapBackend::serialized_data_size() const {
    if (serializedData != nullptr) {
#ifdef debug_ref
            std::cerr << "Current length of " << serializedLength << " includes " << MAGIC_SIZE << " magic bytes and " << (serializedLength - MAGIC_SIZE) << " user bytes" << std::endl;
#endif
        assert(serializedLength >= MAGIC_SIZE);
        return serializedLength - MAGIC_SIZE;
    }
    return 0;
}

char* MmapBackend::serialized_data() {
    if (serializedData != nullptr) {
    
#ifdef debug_ref
        std::cerr << "Report base address of " << (void*)(((char*)serializedData) + MAGIC_SIZE) << " valid until " << (void*)(((char*)serializedData) + serializedLength) << std::endl;
#endif
    
        return ((char*)serializedData) + MAGIC_SIZE;
    }
    
#ifdef debug_ref
    std::cerr << "Report base address of 0" << std::endl;
#endif
    return nullptr;
}

const char* MmapBackend::serialized_data() const {
    if (serializedData != nullptr) {
        return ((const char*)serializedData) + MAGIC_SIZE;
    }
    return nullptr;
}

void MmapBackend::serialize_members(std::ostream& out) const {
    // Serializable handles the magic number for us
    
#ifdef debug
    std::cerr << "Serializing to stream." << std::endl;
#endif
    
    out.clear();
    out.write(serialized_data(), serialized_data_size());
    if (out.fail()) {
        throw std::runtime_error("Could not write " + std::to_string(serialized_data_size()) + " bytes to stream");
    }
}

void MmapBackend::deserialize_members(std::istream& in) {
    // Serializable handles the magic number for us
    
#ifdef debug
    std::cerr << "Deserializing from stream..." << std::endl;
#endif
    
    // We might have an implementation with a constructor that allocates some
    // memory already. That's fine; we'll clobber it.
    // We shouldn't be called when we've already read or written, but just to be safe...
    dissociate();

    auto data_start = in.tellg();
    if (data_start != -1 && !in.fail()) {
        // Stream seems seekable
        in.seekg(0, in.end);
        auto data_end = in.tellg();
        in.seekg(data_start);
        
        if (data_end != -1 && !in.fail()) {
            // All this seeking worked and we have the file length.
            // Resize and do one big read.
            serialized_data_resize(data_end - data_start);
            in.clear();
            in.read(serialized_data(), serialized_data_size());
            if (in.fail() || in.bad() || in.eof()) {
                throw std::runtime_error("Could not read " + std::to_string(serialized_data_size()) + " bytes from stream");
            }
            return;
        }
    }
    
    // Otherwise the stream wasn't seekable. Hopefully we're at the start.
    in.seekg(data_start);
    in.clear();
    
    // This tracks how many bytes of our data are actually used.
    size_t read_bytes = 0;
    
    // Start with a relatively large size, because big IO calls are fast and
    // untouched pages here are free.
    serialized_data_resize(1024 * 1024);
    
    while (!in.eof()) {
        if (read_bytes == serialized_data_size()) {
            // Make the buffer bigger because we have filled it.
            // Because untouched pages don't take any memory, we don't need to
            // worry about this getting too big really.
            serialized_data_resize(serialized_data_size() * 2);
        }
    
        // Try and read enough to fill the buffer.
        in.read(serialized_data() + read_bytes, serialized_data_size() - read_bytes);
        if (in.bad()) {
            throw std::runtime_error("Could not read from stream");
        }
        // We'll eventually EOF, probably in the middle of one of these reads.
        // Record how much we actually got this time.
        read_bytes += in.gcount();
    }
    
    // Now we have all the data. Shrink back to the size we actually got.
    serialized_data_resize(read_bytes);
    
    // Now we're done!
    
#ifdef debug
    std::cerr << "Deserialized from stream." << std::endl;
#endif
    
}

void* MmapBackend::serialize_and_get_mapping(int fd) const {
    // We need to serialize and return the new maping (which may be the same as the current one, which may be null).
    // Then non-const FD serialize can adopt it.
    
    if (serializedFD != -1) {
        // We have a file already. Maybe this is the same one.
        // If so, serializing would be a no-op.
        
        // Reserve buffers for file metadata
        struct stat our_stat;
        struct stat fd_stat;
        
        // Get info on the file we have open
        if (::fstat(serializedFD, &our_stat) != 0) {
            // It didn't work
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not interrogate mapped file: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        
        // Get info on the file we want to use
        if (::fstat(fd, &fd_stat) != 0) {
            // It didn't work
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not interrogate destination file: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        
        if (our_stat.st_dev == fd_stat.st_dev && our_stat.st_ino == fd_stat.st_ino) {
            // These two file descriptors do indeed refer to the same file.
            // We don't have to do anything.
            return serializedData;
        }
    }
    
    // Otherwise, we need to write ourselves out to the new file.
    // And we need to handle the magic number ourselves.
    
    // Work out how many bytes we need to write, given that we might not have any real data yet.
    size_t output_length;
    uint32_t magic_buffer;
    const char* data_to_write;
    
    if (serializedData) {
        // We have data
        data_to_write = (const char*) serializedData;
        output_length = serializedLength;
    } else {
        // Just buffer the magic number on the stack.
        magic_buffer = htonl(get_magic_number());
        data_to_write = (const char*) &magic_buffer;
        output_length = MAGIC_SIZE;
    }
    
    // We first try to truncate it to the right length
    if (::ftruncate(fd, output_length) != 0) {
        // One reason we might not be able to truncate is that this is a stream
        // and not a normal file.
        // Try dumping to it. We assume that if it's not a normal file we'll
        // still fail.
        
        // Track how much of our data we have written so far.
        size_t written_bytes = 0;
        
        while (written_bytes < output_length) {
            // Until all the bytes are written
            
            // Try writing all the remaining bytes.
            auto written_now = ::write(fd, (const void*)(data_to_write + written_bytes), output_length - written_bytes);
            
            if (written_now == -1) {
                // We encountered an error writing.
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not write to destination file: " << ::strerror(problem);
                throw std::runtime_error(ss.str());
            }
            
            // Record how many we wrote
            written_bytes += written_now;
        }
        
        // Now we're done!
        // Keep whatever (maybe null) mapping we had.
        return serializedData;
    }
    
    // We managed to truncate to the right length, so this must be a seekable
    // normal file.
    
    // TODO: We would like the new mapped memory to come into existence knowing it ought to match the old mapped memory.
    // TODO: work out a way to reflink here instead so the file can really be CoW.
    // Right now we will just do a big memcpy between disk-backed regions.
    
    // Make the mapping
    void* new_mapping = ::mmap(nullptr, output_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (new_mapping == MAP_FAILED) {
        auto problem = errno;
        std::stringstream ss;
        ss << "Could not map memory: " << ::strerror(problem);
        throw std::runtime_error(ss.str());
    }
    
    // Fill it in from either our current mapping or the magic number on stack.
    ::memcpy(new_mapping, data_to_write, output_length);
    
    // Return it
    return new_mapping;
}

void MmapBackend::serialize(int fd) const {

#ifdef debug
    std::cerr << "Serializing to FD as const..." << std::endl;
#endif
    
    void* new_mapping = serialize_and_get_mapping(fd);
    
    // Now turn around and unmap the new mapping
    // We know it's either the size of our data, or the size of the magic number if we have no data.
    if (::munmap(new_mapping, serializedData ? serializedLength : MAGIC_SIZE) != 0) {
        // We encountered an error unmapping.
        auto problem = errno;
        std::stringstream ss;
        ss << "Could not unmap memory: " << ::strerror(problem);
        throw std::runtime_error(ss.str());
    }
    
#ifdef debug
    std::cerr << "Serialized to FD as const." << std::endl;
#endif

}

void MmapBackend::serialize(int fd) {

#ifdef debug
    std::cerr << "Serializing to FD..." << std::endl;
#endif

    void* new_mapping = serialize_and_get_mapping(fd);
    
    if (new_mapping != serializedData) {
        // We have a (distinct!) new mapping to adopt.
    
#ifdef debug
        std::cerr << "\tSerializing produced new mapping at " << new_mapping << std::endl;
#endif
        
        if (serializedData) {
        
#ifdef debug
            std::cerr << "\t\tReplacing old mapping at " << serializedData << std::endl;
#endif
        
            // Unmap the old one (which we know is distinct)
            if (::munmap(serializedData, serializedLength) != 0) {
                // We encountered an error unmapping.
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not unmap memory: " << ::strerror(problem);
                throw std::runtime_error(ss.str());
            }
            serializedData = nullptr;
        }
        
        // Save the new mapping.
        serializedData = new_mapping;
        
        // We only want to replace our old FD if we changed the mapping.
        // Otherwise we might grab a pipe FD when serializing to a pipe and hold the pipe open.
        if (serializedFD != -1) {
            // Close the old FD
            close_fd(serializedFD);
            serializedFD = -1;
        }
        // Adopt the new one via dup
        serializedFD = dup(fd);
        if (serializedFD == -1) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not keep descriptor to mapped file: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
    }
    
#ifdef debug
    std::cerr << "Serialized to FD." << std::endl;
#endif
    
}

void MmapBackend::deserialize(int fd) {

#ifdef debug
    std::cerr << "Deserializing from FD..." << std::endl;
#endif
    
    if (serializedData != nullptr) {
        // We have data already. Unmap it.
        if (::munmap(serializedData, serializedLength) != 0) {
            // We encountered an error unmapping.
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not unmap memory: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        serializedData = nullptr;
    }
    if (serializedFD != -1) {
        // Close any old FD
        close_fd(serializedFD);
        serializedFD = -1;
    }

    auto file_length = ::lseek(fd, 0, SEEK_END);
    if (file_length != -1) {
        // We can probably seek in this probably normal file.
        
#ifdef debug
        std::cerr << "\tSought to end of non-stream file at " << file_length << std::endl;
#endif
        
        // Go back to the start.
        if (::lseek(fd, 0, SEEK_SET) != 0) {
            // But we couldn't seek back. We messed it up!
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not seek back to start of file: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        
        if (file_length == 0) {
            // We can't mmap a 0-length file.
            throw std::runtime_error("Cannot read empty file; no magic number");
        }
        
        // Now mmap
        
        // Usually we want to establish a write-back connection with the original file
        bool write_back = true;
        
        serializedLength = file_length;
        serializedData = ::mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (serializedData == MAP_FAILED) {
            // Try mapping private if read-write and shared doesn't work.
            // Needs to be writeable or we will segfault if someone tries to modify us.
            // But we may be loading a read-only file.
            serializedData = ::mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
            
            // Since we are a read-only file, we don't want a write-back connection.
            write_back = false;
        }
        if (serializedData == MAP_FAILED) {
            auto problem = errno;
            std::stringstream ss;
            ss << "Could not map memory: " << ::strerror(problem);
            throw std::runtime_error(ss.str());
        }
        // Remember the size of the file we mapped in.
        mappingFileSize = file_length;
        
        if (write_back) {
            // We want to keep writing to this file.
            // Adopt the new FD via dup
            serializedFD = dup(fd);
            if (serializedFD == -1) {
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not keep descriptor to mapped file: " << ::strerror(problem);
                throw std::runtime_error(ss.str());
            }
        } else {
            // We don't want a write-back connection to this file or anywhere
            if (serializedFD != -1) {
                close_fd(serializedFD);
                serializedFD = -1;
            }
        }
        
        // Validate magic number length and value.
        if (serializedLength < MAGIC_SIZE) {
            throw std::runtime_error("EOF in magic number");
        }
        // Just grab it from the start of our mapping.
        uint32_t observed_magic = ntohl(*(uint32_t*)serializedData);
        if (observed_magic != get_magic_number()) {
            throw std::runtime_error("Incorrect magic number " + std::to_string(observed_magic) + " should be " + std::to_string(get_magic_number()));
        }
        
        // Now we're done!
    } else {
    
        // If we get here, we have to handle the streaming case.
        // TODO: deduplicate with deserialize_members?
        
#ifdef debug
        std::cerr << "\tDeserializing from FD as a stream!" << std::endl;
#endif
        
        // This tracks the number of bytes read in the last read operation.
        ssize_t current_bytes;
        
        // Read magic number
        uint32_t observed_magic;
        size_t magic_cursor = 0;
        do {
        
#ifdef debug
            std::cerr << "\t\tAttempt magic read of " << (MAGIC_SIZE - magic_cursor) << " bytes" << std::endl;
#endif        

            // Get bytes
            current_bytes = ::read(fd, (void*)(((char*)&observed_magic) + magic_cursor), MAGIC_SIZE - magic_cursor);
            
#ifdef debug
            std::cerr << "\t\t\tMagic read() result: " << current_bytes << std::endl;
#endif
            
            if (current_bytes < 0) {
                // An error occurred
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not read from FD stream: " << ::strerror(problem);
                throw std::runtime_error(ss.str());
            }
            magic_cursor += current_bytes;
            
#ifdef debug
            std::cerr << "\t\t\tMagic cursor at: " << magic_cursor << std::endl;
#endif
            
        } while (current_bytes > 0 && magic_cursor < MAGIC_SIZE);
        
        if (current_bytes == 0) {
            // We got an EOF in the magic number.
            throw std::runtime_error("EOF in magic number");
        }
        
        // Validate magic number
        observed_magic = ntohl(observed_magic);
        if (observed_magic != get_magic_number()) {
            // This sort of mismatch is now the programmer's problem.
            throw std::runtime_error("Incorrect magic number " + std::to_string(observed_magic) + " should be " + std::to_string(get_magic_number()));
        }
        
        // From here on we can just use the normal size and data functions we
        // present to users, but populating from an FD to a stream.
        
        // This tracks how many bytes of our data are actually used.
        size_t read_bytes = 0;
        
        // Start with a relatively large size, because big IO calls are fast and
        // untouched pages here are free.
        serialized_data_resize(1024 * 1024);
        
        do {
            if (read_bytes == serialized_data_size()) {
                // Make the buffer bigger because we have filled it.
                // Because untouched pages don't take any memory, we don't need to
                // worry about this getting too big really.
                serialized_data_resize(serialized_data_size() * 2);
            }
            
#ifdef debug
            std::cerr << "\t\tAttempt data read of " << (serialized_data_size() - read_bytes) << " bytes" << std::endl;
#endif
            
            // Get bytes
            current_bytes = ::read(fd, (void*)(serialized_data() + read_bytes), serialized_data_size() - read_bytes);
            
#ifdef debug
            std::cerr << "\t\t\tData read() result: " << current_bytes << std::endl;
#endif
            
            if (current_bytes < 0) {
                // An error occurred
                auto problem = errno;
                std::stringstream ss;
                ss << "Could not read from FD stream: " << ::strerror(problem);
                throw std::runtime_error(ss.str());
            }
            read_bytes += current_bytes;
            
#ifdef debug
            std::cerr << "\t\t\tData bytes read overall: " << read_bytes << std::endl;
#endif
            
        } while (current_bytes > 0);
        
        // Now we have all the data. Shrink back to the size we actually got.
        serialized_data_resize(read_bytes);
    }
#ifdef debug
    std::cerr << "Deserialized from FD." << std::endl;
#endif
    
}

// TODO overall:
// EINTR handling

}


