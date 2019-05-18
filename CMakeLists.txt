cmake_minimum_required(VERSION 3.9)
project(sglib VERSION 0.0.0 LANGUAGES CXX)

# Optimize by default, but also include debug info
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

# Add external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include(GNUInstallDirs)

# Define lib and include directories to install the external projects into
set(DEP_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(DEP_LIB ${DEP_PREFIX}/lib)
set(DEP_INCLUDE ${DEP_PREFIX}/include)

# TODO: We're using INSTALL_DIR very wrong. We *should* be actually installing
# the external projects into their prefixes and working with the installed
# files. Instead we're building but not installing them and trying to work with
# the non-installed build trees.
# 
# Hence the blanked out INSTALL_COMMANDs to suppress the install step.
#
# We need to NOT blank out UPDATE_COMMAND or we can never change the Git revision we point to.
# The cost of this is that we have to re-configure on every build if we do update.

# sdsl-lite (full build using its cmake config)
# Make sure to make it -fPIC so we can have a libsglib.so
ExternalProject_Add(sdsl-lite
    GIT_REPOSITORY "https://github.com/simongog/sdsl-lite.git"
    GIT_TAG "ddb0fbbc33bb183baa616f17eb48e261ac2a3672"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${DEP_PREFIX} -DCMAKE_CXX_FLAGS=-fPIC
    )

# libhandlegraph (full build using its cmake config)
ExternalProject_Add(handlegraph
    GIT_REPOSITORY "https://github.com/vgteam/libhandlegraph.git"
    GIT_TAG "b3b39eb936f8100c1ada83889eaf4b2873052eeb"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${DEP_PREFIX}
    )

# sparsepp
ExternalProject_Add(sparsepp
    GIT_REPOSITORY "https://github.com/greg7mdp/sparsepp.git"
    GIT_TAG "f9ad6d7f4246a78c7b5631e400f6936514cfceb3"
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND mkdir -p ${DEP_INCLUDE} && cp -R sparsepp/ ${DEP_INCLUDE}
    )
    
# Always include and link against the deps
include_directories(${DEP_INCLUDE})
link_directories(${DEP_LIB})


# Set where the LC_ID_DYLIB install name for Mac dylib files ought to point.
# It ought to point to where the dylibs will actually be installed
# Only takes effect after installation
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# Find all the CPP files
file(GLOB SOURCES "src/**.cpp")

# Build that into both shared and static libraies, with shared as the main one.
# Don't use an object library because we want the static library to be position-dependent code.
add_library(sglib SHARED ${SOURCES})
set_property(TARGET sglib PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(sglib_static STATIC ${SOURCES})
set_target_properties(sglib_static PROPERTIES OUTPUT_NAME sglib)
set_property(TARGET sglib_static PROPERTY POSITION_INDEPENDENT_CODE OFF)

add_dependencies(sglib handlegraph)
add_dependencies(sglib sdsl-lite)
add_dependencies(sglib sparsepp)
add_dependencies(sglib_static handlegraph)
add_dependencies(sglib_static sdsl-lite)
add_dependencies(sglib_static sparsepp)
target_include_directories(sglib
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${DEP_INCLUDE}
    )
target_link_libraries(sglib
    "-lsdsl"
    "-lhandlegraph"
    )
    
target_include_directories(sglib_static
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${DEP_INCLUDE}
    )
target_link_libraries(sglib_static
    "-lsdsl-lite"
    "-lhandlegraph"
    )

target_compile_features(sglib PUBLIC cxx_std_14)
target_compile_features(sglib_static PUBLIC cxx_std_14)

# Installation instructions

install(TARGETS sglib sglib_static
    EXPORT sglib-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
install(DIRECTORY src
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/sglib
    FILES_MATCHING PATTERN "*.hpp"
    )
# Drop include and lib(64) straight into the install prefix
install(DIRECTORY ${DEP_INCLUDE}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
install(DIRECTORY ${DEP_LIB}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    FILES_MATCHING PATTERN "*.a"
    )
install(DIRECTORY ${DEP_LIB}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    FILES_MATCHING PATTERN "*.so"
    )
install(DIRECTORY ${DEP_LIB}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    FILES_MATCHING PATTERN "*.dylib"
    )

