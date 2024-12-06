#include <handlegraph/types.hpp>
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
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_std_bdsg_internal_binder_hook_bind(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:29
	binder::vector_binder<unsigned long,std::allocator<unsigned long>>(M("std"), "unsigned_long", "std_allocator_unsigned_long_t");

	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:30
	binder::vector_binder<handlegraph::handle_t,std::allocator<handlegraph::handle_t>>(M("std"), "handlegraph_handle_t", "std_allocator_handlegraph_handle_t_t");

	// std::vector file:bdsg/internal/binder_hook_bind.hpp line:32
	binder::vector_binder<handlegraph::step_handle_t,std::allocator<handlegraph::step_handle_t>>(M("std"), "handlegraph_step_handle_t", "std_allocator_handlegraph_step_handle_t_t");

}
