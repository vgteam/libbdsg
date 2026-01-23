#ifndef BDSG_INDEXING_ITERATOR_HPP_INCLUDED 
#define BDSG_INDEXING_ITERATOR_HPP_INCLUDED

namespace bdsg {
/**
 * A forward iterator for anything vector-like (PackedVector, PagedVector,
 * etc.) that provides read-only access to elements and internally uses integer
 * indexes.
 *
 * This iterator is invalidated if the backing object changes size or moves.
 */
template<typename VectorLike>
class IndexingIterator {
public:
    // Iterator traits for standard library compatibility
    using iterator_category = std::random_access_iterator_tag;
    using value_type = uint64_t;
    using difference_type = typename std::make_signed<size_t>::type;
    using pointer = void;
    using reference = uint64_t;

    //Constructors
    IndexingIterator() = default; 
    IndexingIterator(const IndexingIterator& other) = default;
    IndexingIterator& operator=(const IndexingIterator& other) = default;
    ~IndexingIterator() = default;

    /// Pre-increment operator
    IndexingIterator& operator++();

    /// Post-increment operator
    IndexingIterator operator++(int);

    /// Dereference operator - returns value at current position
    uint64_t operator*() const;

    /// Equality comparison
    bool operator==(const IndexingIterator& other) const;

    /// Inequality comparison
    bool operator!=(const IndexingIterator& other) const;

    // Bidirectional methods

    /// Pre-decrement operator
    IndexingIterator& operator--();

    /// Post-decrement operator
    IndexingIterator operator--(int);

    // Random-access methods
    
    /// Addition of offset in place
    IndexingIterator& operator+=(difference_type offset);
     
    /// Addition of offset
    IndexingIterator operator+(difference_type offset) const;

    /// Subtraction of offset in place
    IndexingIterator& operator-=(difference_type offset);

    /// Subtraction of offset
    IndexingIterator operator-(difference_type offset) const;

    /// Subtraction of two iterators
    difference_type operator-(const IndexingIterator& other) const;

    /// Indexing into iterator. Even though we type this as reference, remember
    /// that we don't actually implement writing to our "references" and just
    /// use the value type.
    /// Result is undefined if itrators are to different collecitons.
    reference operator[](difference_type offset) const;

    // Comaprable iterator methods (TODO: Is there an STL concept name for this?)
    
    /// Determine if this iterator is strictly before another.
    /// Result is undefined if iterators are to different collecitons.
    bool operator<(const IndexingIterator& other) const;

    /// Determine if this iterator is before or at another.
    /// Result is undefined if iterators are to different collecitons.
    bool operator<=(const IndexingIterator& other) const;

    /// Determine if this iterator is strictly after another.
    /// Result is undefined if iterators are to different collecitons.
    bool operator>(const IndexingIterator& other) const;

    /// Determine if this iterator is at or after another.
    /// Result is undefined if itrators are to different collecitons.
    bool operator>=(const IndexingIterator& other) const;
    
private:
    // Private constructor - only associated class can create iterators
    IndexingIterator(const VectorLike* vec, size_t idx);

    const VectorLike* vec_ptr = nullptr;
    size_t index = 0;
    
    // We're not allowed to use "class" when befriending a template parameter.
    // See <https://stackoverflow.com/a/702701>
    friend VectorLike;
};

template<typename VectorLike>
IndexingIterator<VectorLike>::IndexingIterator(const VectorLike* vec, size_t idx)
    : vec_ptr(vec), index(idx) {
    // Constructor
}

template<typename VectorLike>
IndexingIterator<VectorLike>& IndexingIterator<VectorLike>::operator++() {
    ++index;
    return *this;
}

template<typename VectorLike>
IndexingIterator<VectorLike> IndexingIterator<VectorLike>::operator++(int) {
    IndexingIterator tmp = *this;
    ++index;
    return tmp;
}

template<typename VectorLike>
uint64_t IndexingIterator<VectorLike>::operator*() const {
    return vec_ptr->get(index);
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator==(const IndexingIterator& other) const {
    return vec_ptr == other.vec_ptr && index == other.index;
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator!=(const IndexingIterator& other) const {
    return !(*this == other);
} 

template<typename VectorLike>
IndexingIterator<VectorLike>& IndexingIterator<VectorLike>::operator--() {
    --index;
    return *this;
}

template<typename VectorLike>
IndexingIterator<VectorLike> IndexingIterator<VectorLike>::operator--(int) {
    IndexingIterator tmp = *this;
    --index;
    return tmp;
}

template<typename VectorLike>
IndexingIterator<VectorLike>& IndexingIterator<VectorLike>::operator+=(difference_type offset) {
    index += offset;
    return *this;
}

template<typename VectorLike>
IndexingIterator<VectorLike> IndexingIterator<VectorLike>::operator+(difference_type offset) const {
    return IndexingIterator(vec_ptr, index + offset);
}

template<typename VectorLike>
IndexingIterator<VectorLike>& IndexingIterator<VectorLike>::operator-=(difference_type offset) {
    index -= offset;
    return *this;
}

template<typename VectorLike>
IndexingIterator<VectorLike> IndexingIterator<VectorLike>::operator-(difference_type offset) const {
    return IndexingIterator(vec_ptr, index - offset);
} 

template<typename VectorLike>
typename IndexingIterator<VectorLike>::difference_type IndexingIterator<VectorLike>::operator-(const IndexingIterator& other) const {
    // TODO: I don't know a way to subtract two unsigned values and get the
    // signed difference in a single operation as long as that difference
    // itself fits the signed type. So we cast and hope.
    return (difference_type) index - (difference_type) other.index;
}

template<typename VectorLike>
typename IndexingIterator<VectorLike>::reference IndexingIterator<VectorLike>::operator[](difference_type offset) const {
    return *(*this + offset);
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator<(const IndexingIterator& other) const {
    return index < other.index;
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator<=(const IndexingIterator& other) const {
    return index <= other.index;
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator>(const IndexingIterator& other) const {
    return index > other.index;
}

template<typename VectorLike>
bool IndexingIterator<VectorLike>::operator>=(const IndexingIterator& other) const {
    return index >= other.index;
} 
}


#endif
