//
//  mapped_structs.cpp
//

#include "bdsg/internal/mapped_structs.hpp"


namespace bdsg {

offset_to<ArenaAllocatorBlockRef>& ArenaAllocatorBlockRef::prev() {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    return body.prev;
}

offset_to<ArenaAllocatorBlockRef>& ArenaAllocatorBlockRef::next() {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    return body.next;
}

big_endian<size_t>& ArenaAllocatorBlockRef::size() {
    auto& body = *reinterpret_cast<body_t*>(this->get_body());
    return body.size;
}

size_t ArenaAllocatorBlockRef::get_user_data() const {
    return offset + sizeof(body_t);
}

static ArenaAllocatorBlockRef ArenaAllocatorBlockRef::get_from_data(MappingContext* context, size_t user_data) {
    return ArenaAllocatorBlockRef(context, user_data - sizeof(body_t));
}

ArenaAllocatorBlockRef ArenaAllocatorBlockRef::split(size_t first_bytes) {
}

pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> ArenaAllocatorBlockRef::detach() {
}

void ArenaAllocatorBlockRef::attach(ArenaAllocatorBlockRef& left, ArenaAllocatorBlockRef& right) {
}

pair<ArenaAllocatorBlockRef, ArenaAllocatorBlockRef> ArenaAllocatorBlockRef::coalesce() {
}
    

}

