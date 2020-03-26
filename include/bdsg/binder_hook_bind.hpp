#ifndef BSDG_BINDER_HOOK_BIND_HPP
#define BSDG_BINDER_HOOK_BIND_HPP

// Components needed at binding generation time to make pybind11/Binder work for the library.
// Forced to be used as a source for things to bind, even though nothing includes it.

#include <vector>

// We need all the handle graph types that ever get used in vectors.
#include <handlegraph/types.hpp>

// We need to make sure that some object/member exists with each vector type
// that we need to use as an argument or return type in the library. Having a
// function use the type isn't enough to get bindings generated for the type.
// TODO: keep in sync with the PYBIND11_MAKE_OPAQUE invocations in the compile
// hook.
namespace bdsg {

/**
 * Class that holds members that need to be declared for Binder to bind their
 * types. Excluded from binding generation itself.
 */
class BinderHook {
private:

    // It can't actually be made.
    BinderHook() = delete;
    BinderHook(const BinderHook& other) = delete;

    // It would have these members, one for each type of vector that needs binding.
    std::vector<long unsigned int> v1;
    std::vector<handlegraph::handle_t> v2;
    std::vector<handlegraph::path_handle_t> v3;
    std::vector<handlegraph::step_handle_t> v4;
};

}

#endif
