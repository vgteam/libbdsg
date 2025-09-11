#include <handlegraph/types.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <stl_binders.hpp>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <bdsg/internal/binder_hook_compile.hpp>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_std_bdsg_internal_binder_hook_bind(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:37
	binder::vector_binder<unsigned long,std::allocator<unsigned long>>(M("std"), "unsigned_long", "std_allocator_unsigned_long_t");

	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:38
	binder::vector_binder<handlegraph::handle_t,std::allocator<handlegraph::handle_t>>(M("std"), "handlegraph_handle_t", "std_allocator_handlegraph_handle_t_t");

	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:39
	binder::vector_binder<handlegraph::path_handle_t,std::allocator<handlegraph::path_handle_t>>(M("std"), "handlegraph_path_handle_t", "std_allocator_handlegraph_path_handle_t_t");

	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:40
	binder::vector_binder<handlegraph::step_handle_t,std::allocator<handlegraph::step_handle_t>>(M("std"), "handlegraph_step_handle_t", "std_allocator_handlegraph_step_handle_t_t");

}
