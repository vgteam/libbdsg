//
//  packed_path_position_overlay.hpp
//  
//  Contains a memory efficient, generic overlays for PathHandleGraph's that add
//  the PathPositionHandleGraph interface methods for querying steps by base-pair
//  position.
//

#ifndef BDSG_PACKED_PATH_POSITION_OVERLAY_HPP_INCLUDED
#define BDSG_PACKED_PATH_POSITION_OVERLAY_HPP_INCLUDED

#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#include <handlegraph/path_position_handle_graph.hpp>
#include <handlegraph/expanding_overlay_graph.hpp>
#include <handlegraph/util.hpp>
#include <BooPHF.h>

#include "bdsg/graph_proxy.hpp"
#include "bdsg/internal/hash_map.hpp"
#include "bdsg/internal/packed_structs.hpp"

namespace bdsg {
    
using namespace std;
using namespace handlegraph;

/*
 * An overlay that adds the PathPositionHandleGraph interface to a static PathHandleGraph
 * by augmenting it with compressed index data structures
 */
class PackedPositionOverlay : public PathPositionHandleGraph, public ExpandingOverlayGraph, public PathHandleGraphProxy<PathHandleGraph> {
        
public:
    
    /// Make a new PackedPositionOverlay, on the given graph. Glom short paths
    /// together to make internal indexes each over at least the given number
    /// of steps.
    PackedPositionOverlay(const PathHandleGraph* graph, size_t steps_per_index = 1000000);
    PackedPositionOverlay() = default;
    PackedPositionOverlay(const PackedPositionOverlay& other) = default;
    PackedPositionOverlay(PackedPositionOverlay&& other) = default;
    ~PackedPositionOverlay() = default;
    PackedPositionOverlay& operator=(const PackedPositionOverlay& other) = default;
    PackedPositionOverlay& operator=(PackedPositionOverlay&& other) = default;
    
    // Most path handle graph stuff is provided by the proxy.

private:
    
    /// Get the PathHandleGraph to which most methods should be delegated.
    inline const PathHandleGraph* get() const {
        return graph;
    }
    
    // But we override some methods, because we need to hide paths we don't end up indexing.

public:
    /// Loop through all the paths matching the given query. Query elements
    /// which are null match everything. Returns false and stops if the
    /// iteratee returns false.
    bool for_each_path_matching_impl(const std::unordered_set<PathSense>* senses,
                                     const std::unordered_set<std::string>* samples,
                                     const std::unordered_set<std::string>* loci,
                                     const std::function<bool(const path_handle_t&)>& iteratee) const;
    
    /// Loop through all steps on the given handle for paths with the given
    /// sense. Returns false and stops if the iteratee returns false.
    bool for_each_step_of_sense_impl(const handle_t& visited,
                                     const PathSense& sense,
                                     const std::function<bool(const step_handle_t&)>& iteratee) const;
                                     
    /// Determine if a path name exists and is legal to get a path handle for.
    bool has_path(const std::string& path_name) const;
    
    // We don't need to intercept get_path_handle because it's not allowed to
    // be called if has_path is false.
    
public:

    ////////////////////////////////////////////////////////////////////////////
    // Path position interface
    ////////////////////////////////////////////////////////////////////////////
    
    /// Returns the length of a path measured in bases of sequence.
    size_t get_path_length(const path_handle_t& path_handle) const;
    
    /// Returns the position along the path of the beginning of this step measured in
    /// bases of sequence. In a circular path, positions start at the step returned by
    /// path_begin().
    size_t get_position_of_step(const step_handle_t& step) const;
    
    /// Returns the step at this position, measured in bases of sequence starting at
    /// the step returned by path_begin(). If the position is past the end of the
    /// path, returns path_end().
    step_handle_t get_step_at_position(const path_handle_t& path,
                                       const size_t& position) const;
    
    ////////////////////////////////////////////////////////////////////////////
    // Expanding overlay interface
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Returns the handle in the underlying graph that corresponds to a handle in the
     * overlay
     */
    handle_t get_underlying_handle(const handle_t& handle) const;
    
protected:
    
    
    // local BBHash style hash function for step handles
    struct StepHash {
        uint64_t operator()(const step_handle_t& step, uint64_t seed = 0xAAAAAAAA55555555ULL) const;
    };
    
    /// Construct the index over path positions
    void index_path_positions();
    
    /// The graph we're overlaying
    const PathHandleGraph* graph = nullptr;
    
    /// The number of steps we target when coalescing small paths into larger indexes.
    const size_t steps_per_index;
    
    /// To facillitate parallel construction, we keep the index info for each
    /// path (or collection of tiny paths) in a separate object.
    struct PathIndex {
        /// The first half of the steps
        PagedVector<> steps_0;
        
        /// The second half of the steps
        PagedVector<> steps_1;
        
        /// The positions of the steps
        PagedVector<> positions;
        
        /// A perfect minimal hash function for the step handles on the path(s)
        /// We keep this in a vector so that we can be copyable, which the Python bindings want.
        /// TODO: replace with std::optional when we upgrade to C++17.
        std::vector<boomphf::mphf<step_handle_t, StepHash>> step_hash;
        
        /// The position of the step that hashes to a given index
        PackedVector<> step_positions;
    };
    
    /// This holds the indexes, each of which belongs to a path or collection
    /// of short paths.
    vector<PathIndex> indexes;
    
    /// And this represents a reference to an offset range in a PathIndex, where a path can be found.
    struct PathRange {
        size_t index_number;
        size_t start;
        size_t end;
    };
    
    /// Map from path_handle to the index and range of positions that contain
    /// its records in the steps and positions vectors. Note that access to
    /// existing entries is not thread-safe with the addition of new entries!
    /// Adding a new entry may deallocate existing entries, and will invalidate
    /// pointers and references to them! This is different from how the STL
    /// unordered_map behaves!
    hash_map<int64_t, PathRange> path_range;
};

/*
 * A wrapper for constructing the perfect minimal hash function that sequentially
 * returns all steps of a PathHandleGraph from an iterator struct
 */
struct BBHashHelper {
public:
    /// Set up for iteration over all paths in the given graph
    BBHashHelper(const PathHandleGraph* graph);
    /// Set up for iteration over the given paths (as iterators over path_handle_ts)
    template<typename InputIt>
    BBHashHelper(const PathHandleGraph* graph, InputIt first_path, InputIt last_path);
    BBHashHelper() = delete;
    ~BBHashHelper() = default;
    
    struct iterator {
    public:
        iterator(const iterator& other) = default;
        iterator() = delete;
        ~iterator() = default;
        iterator& operator=(const iterator& other) = default;
        iterator& operator++();
        step_handle_t operator*() const;
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        
    private:
        
        iterator(const BBHashHelper* iteratee, size_t path_handle_idx);
        
        size_t path_handle_idx = 0;
        step_handle_t step;
        const BBHashHelper* iteratee;
        
        friend class BBHashHelper;
    };
    
    /// C++ style range begin over steps
    iterator begin() const;
    
    /// C++ style range end over steps
    iterator end() const;
    
private:
    /// The graph whose steps this iterates over
    const PathHandleGraph* graph;
    /// An ordering of the path handles so we can refer to them by index
    vector<path_handle_t> path_handles;
    
    friend class iterator;
};

template<typename InputIt>
BBHashHelper::BBHashHelper(const PathHandleGraph* graph, InputIt first_path, InputIt last_path) : graph(graph) {
    path_handles.reserve(last_path - first_path);
    for (InputIt it = first_path; it != last_path; ++it) {
        if (!graph->is_empty(*it)) {
            // this path contains steps, we want to iterate over it
            path_handles.push_back(*it);
        }
    };
}
    
}

#endif
