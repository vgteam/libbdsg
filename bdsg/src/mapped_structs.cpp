//
//  mapped_structs.cpp
//

#include "bdsg/internal/mapped_structs.hpp"

#include <mio/mmap.hpp>


namespace bdsg {

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

