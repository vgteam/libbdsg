//
//  packed_reference_path_overlay.hpp
//  
//  Contains an overlay that adds fast access to paths so they can be used as
//  references.
//

#ifndef BDSG_PACKED_REFERENCE_PATH_OVERLAY_HPP_INCLUDED
#define BDSG_PACKED_REFERENCE_PATH_OVERLAY_HPP_INCLUDED

#include "packed_path_position_overlay.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds fast access to paths in addition to allowing path
 * position queries on them.
 */
class PackedReferencePathOverlay : public PackedPositionOverlay {
        
public:
    
    
    using PackedPositionOverlay::PackedPositionOverlay;


    /// Make a PackedReferencePathOverlay. Do the indexing and compute the
    /// additional indexes that the base class doesn't have.
    PackedReferencePathOverlay(const PathHandleGraph* graph, size_t steps_per_index = 1000000);
    
    // We assume that tracing out a path is fast in the backing graph, but
    // finding visits on nodes is slow. We override the reverse lookups to go
    // fron graph nodes to paths.

protected:
    
    // PathHandleGraph interface
    
    /// Calls the given function for each step of the given handle on a path.
    /// We treat steps as "on" handles in either orientation. 
    virtual bool for_each_step_on_handle_impl(const handle_t& handle,
                                              const function<bool(const step_handle_t&)>& iteratee) const;


    // Construction hooks

    /// Get the length in steps of the given path. Also do any scanning necessary for the path to generate per-path user data.
    virtual size_t scan_path(const path_handle_t& path_handle, void*& user_data);

    /// Set the number of distinct indexes we will use.
    virtual void set_index_count(size_t count);

    /// Into index i, index the given range of paths, with the given total size in steps. Consumes and destroys any per-path user data.
    virtual void index_paths(size_t index_num, const std::vector<path_handle_t>::const_iterator& begin_path, const std::vector<path_handle_t>::const_iterator& end_path, size_t cumul_path_size, void** user_data_base);

    // Construction utilities
    
    // local BBHash style hash function for handles 
    struct HandleHash {
        uint64_t operator()(const handle_t& handle, uint64_t seed = 0xAAAAAAAA55555555ULL) const;
    };
    
    // Stored index data

    /// To facillitate parallel construction, we keep the index info for each
    /// path (or collection of tiny paths) in a separate object.
    struct PathVisitIndex {
        /// A perfect minimal hash function for the visited nodes on the path(s)
        /// We keep this in a vector so that we can be copyable, which the Python bindings want.
        /// TODO: replace with std::optional when we upgrade to C++17.
        std::vector<boomphf::mphf<nid_t, boomphf::SingleHashFunctor<nid_t>>> node_hash;
        
        /// Stores indexes in step_positions (i.e. ranks) at which visits to each handle
        /// occur, grouped contiguously by handle
        PackedVector<> visit_ranks;
    
        /// Stores first index in visit_ranks for each handle in the order provided by node_hash
        PackedVector<> visit_ranks_start;

        /// Stores number of indexes in visit_ranks for each handle in the same order as visit_ranks_start
        PackedVector<> visit_ranks_length;
    };
    
    /// This holds the indexes, each of which belongs to a path or collection
    /// of short paths. Order is the same as "indexes" in the base class.
    vector<PathVisitIndex> visit_indexes;
};

}

/*
 * A wrapper for constructing the perfect minimal hash function that sequentially
 * returns all unique keys in a key-value multi-container.
 */
template<typename Container>
struct UniqueKeyRange {
public:
    /// Set up for iteration over all paths in the given graph
    UniqueKeyRange(const Container& container) : container(container) {
        // Nothing to do!
    }
    UniqueKeyRange() = delete;
    ~UniqueKeyRange() = default;
    
    struct iterator {
    public:
        iterator(const iterator& other) = default;
        iterator() = delete;
        ~iterator() = default;
        iterator& operator=(const iterator& other) = default;
        iterator& operator++() {
            typename Container::key_type same_key = **this;
            do {
                // Scan past the current key
                ++wrapped;
            } while (wrapped != end && **this == same_key);
            return *this;
        }
        typename Container::key_type operator*() const {
            return wrapped->first;
        }
        bool operator==(const iterator& other) const {
            return wrapped == other.wrapped && end == other.end;
        }
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
    private:
        
        iterator(const typename Container::const_iterator& wrapped, const typename Container::const_iterator& end) : wrapped(wrapped), end(end) {
            // Nothing to do
        }
        
        typename Container::const_iterator wrapped;
        typename Container::const_iterator end;
        
        friend class UniqueKeyRange;
    };
    
    /// C++ style range begin
    iterator begin() const {
        return iterator(container.begin(), container.end());
    }
    
    /// C++ style range end
    iterator end() const {
        return iterator(container.end(), container.end());
    }
    
private:
    /// The container we will iterate on
    const Container& container;
};

#endif
