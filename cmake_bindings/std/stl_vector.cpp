#include <BooPHF.h>
#include <bdsg/hash_graph.hpp>
#include <bdsg/node.hpp>
#include <bdsg/odgi.hpp>
#include <bdsg/packed_graph.hpp>
#include <bdsg/packed_structs.hpp>
#include <binder/source/stl_binders.hpp>
#include <chrono>
#include <functional>
#include <handlegraph/types.hpp>
#include <internal/gap_bitvector.hpp>
#include <internal/packed_vector.hpp>
#include <internal/spsi.hpp>
#include <istream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <ratio>
#include <sstream> // __str__
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>
#include <bdsg/bindings.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

void bind_std_stl_vector(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// std::vector file:bits/stl_vector.h line:216
	binder::vector_binder<unsigned long,std::allocator<unsigned long>>(M("std"), "unsigned_long", "std_allocator_unsigned_long_t");

	// std::vector file:bits/stl_vector.h line:216
	binder::vector_binder<handlegraph::handle_t,std::allocator<handlegraph::handle_t>>(M("std"), "handlegraph_handle_t", "std_allocator_handlegraph_handle_t_t");

	// std::vector file:bits/stl_vector.h line:216
	binder::vector_binder<handlegraph::path_handle_t,std::allocator<handlegraph::path_handle_t>>(M("std"), "handlegraph_path_handle_t", "std_allocator_handlegraph_path_handle_t_t");

}
