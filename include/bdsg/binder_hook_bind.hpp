#ifndef BSDG_BINDER_HOOK_BIND_HPP
#define BSDG_BINDER_HOOK_BIND_HPP

// Components needed at binding generation time to make pybind11/Binder work for the library.
// Forced to be used as a source for things to bind, even though nothing includes it.

#include <vector>

// We need all the handle graph types that ever get used in vectors.
#include <handlegraph/types.hpp>

// Binder can only bind functions that deal in templates if those template
// types are actually instantiated elsewhere. See:
// https://github.com/RosettaCommons/binder/issues/71#issuecomment-486799361
//
// So we instantiate all the templates we use as argument and return types
// here. (Except pairs which pybind11 magically interconverts with Python
// tuples.) TODO: keep in sync with the PYBIND11_MAKE_OPAQUE invocations in the
// compile hook.

template class std::vector<long unsigned int>;
template class std::vector<handlegraph::handle_t>;
template class std::vector<handlegraph::path_handle_t>;
template class std::vector<handlegraph::step_handle_t>;

#endif
