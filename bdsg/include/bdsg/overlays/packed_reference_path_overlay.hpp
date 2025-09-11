//
//  packed_reference_path_overlay.hpp
//  
//  Contains an overlay that adds fast access to paths so they can be used as
//  references.
//

#ifndef BDSG_PACKED_REFERENCE_PATH_OVERLAY_HPP_INCLUDED
#define BDSG_PACKED_REFERENCE_PATH_OVERLAY_HPP_INCLUDED

#include <bdsg/overlays/packed_path_position_overlay.hpp>

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds fast access to paths in addition to allowing path
 * position queries on them.
 *
 * TODO: Won't work properly with paths hidden from for_each_path_handle on the
 * backing graph and don't appear in extra_path_names, since they won't be
 * indexed.
 *
 * We also won't pass any kind of queries through to the backing graph for
 * queries we expect to be able to fulfil from the index. Unkike in
 * PackedPositionOverlay, we now expect the index to have some path data in it,
 * not just offset tables that we wouldn't expect to use for hidden (i,e,
 * haplotype) paths. We should make the overlay transparent so hidden paths
 * work properly, or remove hidden paths.
 */
class PackedReferencePathOverlay : public PackedPositionOverlay {

protected:
    PackedReferencePathOverlay() = default;

public:
    
    /// Make a PackedReferencePathOverlay. Do the indexing and compute the
    /// additional indexes that the base class doesn't have.
    PackedReferencePathOverlay(const PathHandleGraph* graph, const std::unordered_set<std::string>& extra_path_names = {}, size_t steps_per_index = 20000000);
    
    // We assume that tracing out a path is fast in the backing graph, but
    // finding visits on nodes is slow. We override the reverse lookups to go
    // from graph nodes to paths.

    /// overload this to use the cache 
    virtual path_handle_t get_path_handle_of_step(const step_handle_t& step_handle) const;

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

        /// Hash steps for the step to path cache
        std::vector<boomphf::mphf<step_handle_t, StepHash>> step_hash;
        
        /// Step to path cache to speed up get_path_handle_of_step()
        PackedVector<> step_to_path;

        /// Since we're binning the hashes, we need this to verify collisions in bbhash.
        PackedVector<> step_to_step1;
        PackedVector<> step_to_step2;

    };

    /// This holds the indexes, each of which belongs to a path or collection
    /// of short paths. Order is the same as "indexes" in the base class.
    vector<PathVisitIndex> visit_indexes;

    /// Last visit_index accessed, one per thread
    /// (works around some simple worst-case scenarios like repeatedly scanning to the end)
    mutable std::vector<size_t> last_step_to_path_idx;
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
