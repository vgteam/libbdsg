#ifndef BSDG_BINDER_HOOK_COMPILE_HPP
#define BSDG_BINDER_HOOK_COMPILE_HPP

// Components needed at compile time to make pybind11/Binder work for the library.
// Forced to be included in every binding compilation unit.

#include <vector>

// We need all the handle graph types that ever get used in vectors.
#include <handlegraph/types.hpp>

// We need to declare the vectors we bind with binder's binder::vector_binder
// from its stl_binders.hpp to be opaque. Otherwise pybind11 gets upset and
// throws out "static assertion failed: Holder classes are only supported for
// custom types" errors, because the Binder bindings use pybind11's "holder"
// system to wrap the vectors in smart pointers, and pybind11 worries that we
// can't guarantee that that wrapping is always applied to vectors. See:
// <https://github.com/RosettaCommons/pybind11/commit/501135fa769ca5ad73ca81315da509bffa5adeb9>,
// <https://github.com/RosettaCommons/binder/issues/100>,
// <https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html#>
// These need to be in every compilation unit that uses the vectors in
// bindings, and if we make any more vector types we need to add them here.
// TODO: keep in sync with the member declarations in the bind hook.
PYBIND11_MAKE_OPAQUE(std::vector<long unsigned int>);
PYBIND11_MAKE_OPAQUE(std::vector<handlegraph::handle_t>);
PYBIND11_MAKE_OPAQUE(std::vector<handlegraph::path_handle_t>);
PYBIND11_MAKE_OPAQUE(std::vector<handlegraph::step_handle_t>);

#endif
