//
//  packed_structs.hpp
//

#include "sglib/packed_structs.hpp"


namespace sglib {
    
const double PackedVector::factor = 1.25;
const double PackedDeque::factor = 1.25;

PackedVector::PackedVector() {
    vec.width(1); // by default we start as a bitvector
}
    
PackedVector::PackedVector(istream& in) {
    deserialize(in);
}
    
PackedVector::~PackedVector() {
    
}
    
void PackedVector::deserialize(istream& in) {
    sdsl::read_member(filled, in);
    vec.load(in);
}
    
void PackedVector::serialize(ostream& out) const {
    sdsl::write_member(filled, out);
    vec.serialize(out);
}
    
size_t PackedVector::memory_usage() const {
    // sdsl vectors return the number of bits, but we want bytes
    return sizeof(filled) + sizeof(vec) + vec.capacity() / 8;
}
    
PagedVector::PagedVector(size_t page_size) : page_size(page_size) {
    
}

PagedVector::PagedVector(istream& in) {
    deserialize(in);
}

PagedVector::~PagedVector() {
    
}
    
void PagedVector::deserialize(istream& in) {
    sdsl::read_member(filled, in);
    sdsl::read_member(page_size, in);
    anchors.deserialize(in);
    for (size_t i = 0; i < anchors.size(); i++) {
        pages.emplace_back(in);
    }
}

void PagedVector::serialize(ostream& out) const  {
    sdsl::write_member(filled, out);
    sdsl::write_member(page_size, out);
    anchors.serialize(out);
    for (size_t i = 0; i < pages.size(); i++) {
        pages[i].serialize(out);
    }
}

size_t PagedVector::memory_usage() const {
    size_t total = sizeof(page_size) + sizeof(filled) + sizeof(pages);
    total += anchors.memory_usage();
    // add the memory of pages that are filled in the vector
    for (const auto& page : pages) {
        total += page.memory_usage();
    }
    // add the memory of excess capacity
    total += (pages.capacity() - pages.size()) * sizeof(decltype(pages)::value_type);
    return total;
}

PackedDeque::PackedDeque() {
    
}
    
PackedDeque::PackedDeque(istream& in) {
    deserialize(in);
}

PackedDeque::~PackedDeque() {
    
}
    
void PackedDeque::deserialize(istream& in) {
    sdsl::read_member(begin_idx, in);
    sdsl::read_member(filled, in);
    vec.deserialize(in);
}

void PackedDeque::serialize(ostream& out) const  {
    sdsl::write_member(begin_idx, out);
    sdsl::write_member(filled, out);
    vec.serialize(out);
}

size_t PackedDeque::memory_usage() const {
    return sizeof(begin_idx) + sizeof(filled) + vec.memory_usage();
}

RobustPagedVector::RobustPagedVector(size_t page_size) : latter_pages(page_size) {
    // Nothing to do
}

RobustPagedVector::RobustPagedVector() : latter_pages(1) {
    // Nothing to do
}

RobustPagedVector::RobustPagedVector(istream& in) {
    deserialize(in);
}

RobustPagedVector::~RobustPagedVector() {
    
}

void RobustPagedVector::deserialize(istream& in) {
    first_page.deserialize(in);
    latter_pages.deserialize(in);
}

void RobustPagedVector::serialize(ostream& out) const {
    first_page.serialize(out);
    latter_pages.serialize(out);
}

size_t RobustPagedVector::memory_usage() const {
    return first_page.memory_usage() + latter_pages.memory_usage();
}
}
