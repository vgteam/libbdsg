#include <bdsg/internal/base_packed_graph.hpp>
#include <bdsg/internal/mapped_structs.hpp>
#include <functional>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <tuple>
#include <typeinfo>
#include <utility>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>
#include <bdsg/internal/binder_hook_compile.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_bdsg_internal_mapped_structs_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::yomo::Manager file:bdsg/internal/mapped_structs.hpp line:173
		pybind11::class_<bdsg::yomo::Manager, std::shared_ptr<bdsg::yomo::Manager>> cl(M("bdsg::yomo"), "Manager", "Global manager of mapped memory segments. Talked to by pointers in memory\n segments to figure out where they actually point to.\n\n The manager manages one or more \"chains\", each potentially corresponding to\n a file. The chains are made up of mapped memory \"segments\", and each segment\n can be mapped at a different base address.\n\n When a file is initially mapped, it is mapped as a single segment.\n Additional segments may be mapped later to fulfill allocations from\n yomo::Allocator<T> instances stored in the chain.");
		cl.def( pybind11::init( [](){ return new bdsg::yomo::Manager(); } ) );
		cl.def_static("create_chain", []() -> long { return bdsg::yomo::Manager::create_chain(); }, "");
		cl.def_static("create_chain", (long (*)(const std::string &)) &bdsg::yomo::Manager::create_chain, "Create a chain not backed by any file. The given prefix data will be\n placed before the chain allocator data structures.\n\nC++: bdsg::yomo::Manager::create_chain(const std::string &) --> long", pybind11::arg("prefix"));
		cl.def_static("create_chain", [](int const & a0) -> long { return bdsg::yomo::Manager::create_chain(a0); }, "", pybind11::arg("fd"));
		cl.def_static("create_chain", (long (*)(int, const std::string &)) &bdsg::yomo::Manager::create_chain, "Create a chain by mapping all of the given open file. The file must\n begin with the given prefix, if specified, or an error will occur.\n\n Modifications to the chain will affect the file, and it will grow as\n necessary.\n\n The Manager will not take ownership of the file descriptor.\n\n If the file is nonempty, data after the length of the passed prefix must\n contain the chain allocator data structures. If it is empty, the prefix\n and the chain allocator data structures will be written to it.\n\n The entire file will be mapped in one contiguous link.\n\nC++: bdsg::yomo::Manager::create_chain(int, const std::string &) --> long", pybind11::arg("fd"), pybind11::arg("prefix"));
		cl.def_static("create_chain", [](const class std::function<std::string (void)> & a0) -> long { return bdsg::yomo::Manager::create_chain(a0); }, "", pybind11::arg("iterator"));
		cl.def_static("create_chain", (long (*)(const class std::function<std::string (void)> &, const std::string &)) &bdsg::yomo::Manager::create_chain, "Create a chain by calling the given function until it returns an empty\n string, and concatenating all the results.\n\n The result must begin with the given prefix, if specified, or an error\n will occur.\n\n All content from the iterator will be stored in one contiguous link,\n despite the length not being known ahead of time.\n\nC++: bdsg::yomo::Manager::create_chain(const class std::function<std::string (void)> &, const std::string &) --> long", pybind11::arg("iterator"), pybind11::arg("prefix"));
		cl.def_static("get_dissociated_chain", (long (*)(long)) &bdsg::yomo::Manager::get_dissociated_chain, "Return a chain which has the same stored data as the given chain, but\n for which modification of the chain will not modify any backing file on\n disk. The chain returned may be the same chain as the given chain.\n\n Not thread safe with concurrent modificatons to the source chain.\n\nC++: bdsg::yomo::Manager::get_dissociated_chain(long) --> long", pybind11::arg("chain"));
		cl.def_static("get_associated_chain", (long (*)(long, int)) &bdsg::yomo::Manager::get_associated_chain, "Return a chain which has the same stored data as the given chain, but\n for which modification of the chain will modify the open file with the\n given file descriptor. The chain returned may be the same chain as the\n given chain.\n\n The Manager will not take ownership of the file descriptor.\n\n Not thread safe with concurrent modificatons to the source chain.\n\nC++: bdsg::yomo::Manager::get_associated_chain(long, int) --> long", pybind11::arg("chain"), pybind11::arg("fd"));
		cl.def_static("destroy_chain", (void (*)(long)) &bdsg::yomo::Manager::destroy_chain, "Destroy the given chain and unmap all of its memory, and close any\n associated file.\n\n Also drops trailing free blocks and truncates them out of the backing\n file.\n\nC++: bdsg::yomo::Manager::destroy_chain(long) --> void", pybind11::arg("chain"));
		cl.def_static("get_chain", (long (*)(const void *)) &bdsg::yomo::Manager::get_chain, "Get the chain that contains the given address, or NO_CHAIN if the\n address is outside all current chains.\n\nC++: bdsg::yomo::Manager::get_chain(const void *) --> long", pybind11::arg("address"));
		cl.def_static("get_address_in_chain", [](long const & a0, unsigned long const & a1) -> void * { return bdsg::yomo::Manager::get_address_in_chain(a0, a1); }, "", pybind11::return_value_policy::automatic, pybind11::arg("chain"), pybind11::arg("position"));
		cl.def_static("get_address_in_chain", (void * (*)(long, unsigned long, unsigned long)) &bdsg::yomo::Manager::get_address_in_chain, "Get the address of the given byte from the start of the chain.\n If a length is provided, throws if the given length of bytes from\n position are not contiguous in memory.\n For NO_CHAIN just uses an offset from address 0 in memory.\n\nC++: bdsg::yomo::Manager::get_address_in_chain(long, unsigned long, unsigned long) --> void *", pybind11::return_value_policy::automatic, pybind11::arg("chain"), pybind11::arg("position"), pybind11::arg("length"));
		cl.def_static("get_chain_and_position", [](const void * a0) -> std::pair<long, unsigned long> { return bdsg::yomo::Manager::get_chain_and_position(a0); }, "", pybind11::arg("address"));
		cl.def_static("get_chain_and_position", (struct std::pair<long, unsigned long> (*)(const void *, unsigned long)) &bdsg::yomo::Manager::get_chain_and_position, "Get the position of the given byte in the chain it is in, along with the\n identifier for that chain.\n If a length is provided, throws if the given length of bytes from\n position are not contiguous in memory.\n\nC++: bdsg::yomo::Manager::get_chain_and_position(const void *, unsigned long) --> struct std::pair<long, unsigned long>", pybind11::arg("address"), pybind11::arg("length"));
		cl.def_static("get_address_in_same_chain", (void * (*)(const void *, unsigned long)) &bdsg::yomo::Manager::get_address_in_same_chain, "Find the address of the given position in the chain that the given address exists in.\n For NO_CHAIN just uses an offset from address 0 in memory.\n\nC++: bdsg::yomo::Manager::get_address_in_same_chain(const void *, unsigned long) --> void *", pybind11::return_value_policy::automatic, pybind11::arg("here"), pybind11::arg("position"));
		cl.def_static("get_position_in_same_chain", (unsigned long (*)(const void *, const void *)) &bdsg::yomo::Manager::get_position_in_same_chain, "Find the position of the given address in the chain that here exists in.\n For NO_CHAIN just uses an offset from address 0 in memory.\n\nC++: bdsg::yomo::Manager::get_position_in_same_chain(const void *, const void *) --> unsigned long", pybind11::arg("here"), pybind11::arg("address"));
		cl.def_static("get_offset_in_same_chain", (struct std::pair<long, bool> (*)(const void *, const void *)) &bdsg::yomo::Manager::get_offset_in_same_chain, "Find the offset from the given here to the given address, constraining\n them to come from the same chain (or NO_CHAIN). Also returns a flag that\n is true if the offset in the chain equals the offset in memory, and\n false otherwise.\n\nC++: bdsg::yomo::Manager::get_offset_in_same_chain(const void *, const void *) --> struct std::pair<long, bool>", pybind11::arg("here"), pybind11::arg("address"));
		cl.def_static("follow_offset_in_same_chain", (struct std::pair<void *, bool> (*)(const void *, long)) &bdsg::yomo::Manager::follow_offset_in_same_chain, "Apply an offset from the given here to get an address, within the chain\n that here is in (or NO_CHAIN). Also returns a flag that is true if the\n offset in the chain equals the offset in memory, and false otherwise.\n\nC++: bdsg::yomo::Manager::follow_offset_in_same_chain(const void *, long) --> struct std::pair<void *, bool>", pybind11::arg("here"), pybind11::arg("offset"));
		cl.def_static("allocate_from", (void * (*)(long, unsigned long)) &bdsg::yomo::Manager::allocate_from, "Allocate the given number of bytes from the given chain.\n For NO_CHAIN just allocates with malloc().\n\nC++: bdsg::yomo::Manager::allocate_from(long, unsigned long) --> void *", pybind11::return_value_policy::automatic, pybind11::arg("chain"), pybind11::arg("bytes"));
		cl.def_static("allocate_from_same_chain", (void * (*)(void *, unsigned long)) &bdsg::yomo::Manager::allocate_from_same_chain, "Allocate the given number of bytes from the chain containing the given\n address.\n For NO_CHAIN just allocates with malloc().\n\nC++: bdsg::yomo::Manager::allocate_from_same_chain(void *, unsigned long) --> void *", pybind11::return_value_policy::automatic, pybind11::arg("here"), pybind11::arg("bytes"));
		cl.def_static("deallocate", (void (*)(void *)) &bdsg::yomo::Manager::deallocate, "Free the given allocated block in the chain to which it belongs.\n For NO_CHAIN just frees with free().\n\nC++: bdsg::yomo::Manager::deallocate(void *) --> void", pybind11::arg("address"));
		cl.def_static("find_first_allocation", (void * (*)(long, unsigned long)) &bdsg::yomo::Manager::find_first_allocation, "Find the mapped address of the first thing allocated in the chain, given\n that it was allocated with the given size.\n\n That first allocated thing must exist and not be deallocated.\n\n Must not be called for NO_CHAIN.\n\nC++: bdsg::yomo::Manager::find_first_allocation(long, unsigned long) --> void *", pybind11::return_value_policy::automatic, pybind11::arg("chain"), pybind11::arg("bytes"));
		cl.def_static("get_chain_size", (unsigned long (*)(long)) &bdsg::yomo::Manager::get_chain_size, "Return the total number of bytes in the given chain.\n\nC++: bdsg::yomo::Manager::get_chain_size(long) --> unsigned long", pybind11::arg("chain"));
		cl.def_static("get_usage", (class std::tuple<unsigned long, unsigned long, unsigned long> (*)(long)) &bdsg::yomo::Manager::get_usage, "Get statistics about the memory in a chain. Returns all 0s if not a managed chain.\n\n Returns the total bytes in the chain, the number of free bytes,\n and the number of free bytes reclaimable when the chain is closed. \n\nC++: bdsg::yomo::Manager::get_usage(long) --> class std::tuple<unsigned long, unsigned long, unsigned long>", pybind11::arg("chain"));
		cl.def_static("scan_chain", (void (*)(long, const class std::function<void (const void *, unsigned long)> &)) &bdsg::yomo::Manager::scan_chain, "Scan all memory regions in the given chain. Calls the iteratee with each\n region's start address and length, in order.\n\nC++: bdsg::yomo::Manager::scan_chain(long, const class std::function<void (const void *, unsigned long)> &) --> void", pybind11::arg("chain"), pybind11::arg("iteratee"));
		cl.def_static("dump", (void (*)(long)) &bdsg::yomo::Manager::dump, "Dump information about free and allocated memory in the given chain.\n Not thread safe!\n\nC++: bdsg::yomo::Manager::dump(long) --> void", pybind11::arg("chain"));
		cl.def_static("check_heap_integrity", (void (*)(long)) &bdsg::yomo::Manager::check_heap_integrity, "Walk all allocated and free blocks in the heap in the chain and make\n sure that they are actually in memory. If not, throws std::runtime_error.\n\nC++: bdsg::yomo::Manager::check_heap_integrity(long) --> void", pybind11::arg("chain"));
		cl.def_static("count_chains", (unsigned long (*)()) &bdsg::yomo::Manager::count_chains, "Return the total number of chains that exist right now.\n\nC++: bdsg::yomo::Manager::count_chains() --> unsigned long");
		cl.def_static("count_links", (unsigned long (*)()) &bdsg::yomo::Manager::count_links, "Return the total number of links that exist right now across all chains.\n\nC++: bdsg::yomo::Manager::count_links() --> unsigned long");
	}
}
