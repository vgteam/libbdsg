#ifndef BSDG_BINDINGS_HPP
#define BSDG_BINDINGS_HPP

// Components needed to make pybind11/Binder work for the library.

#include <vector>
#include <string>

// This include is specifically banned from showing up in all_cmake_includes.hpp
#include <pybind11/stl_bind.h>

namespace bdsg {

template <typename Item, typename Allocator>
pybind11::class_<std::vector<Item, Allocator>, std::unique_ptr<std::vector<Item, Allocator>>> bind_vector(pybind11::handle& scope, const std::string& item_name, const std::string& allocator_name) {
    return pybind11::bind_vector<std::vector<Item, Allocator>, std::unique_ptr<std::vector<Item, Allocator>>>(scope, "Vector" + item_name);
}

}

#endif
