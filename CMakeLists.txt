# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

# Project's name
project(bdsg)
# We build using c++14
set(CMAKE_CXX_STANDARD 14)

# Use all standard-compliant optimizations
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  # assumes clang build
  # we can't reliably detect when we're using clang, so for the time being we assume
  # TODO: can't we though?
  
  # adapted from https://stackoverflow.com/questions/46414660/macos-cmake-and-openmp
  # find_package(OpenMP) does not work reliably on macOS, so we do its work ourselves
  set (OpenMP_C "${CMAKE_C_COMPILER}")
  set (OpenMP_C_FLAGS " -Xpreprocessor -fopenmp -I/opt/local/include/libomp -I/usr/local/include -L/opt/local/lib/libomp -L/usr/local/lib")
  set (OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set (OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  set (OpenMP_CXX_FLAGS " -Xpreprocessor -fopenmp -I/opt/local/include/libomp -I/usr/local/include -L/opt/local/lib/libomp -L/usr/local/lib")
  set (OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set (OpenMP_libomp_LIBRARY "omp")
  set (OpenMP_libgomp_LIBRARY "gomp")
  set (OpenMP_libiomp5_LIBRARY "iomp5")
  
  # and now add the OpenMP parameters to the compile flags
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lomp")
  
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  find_package(OpenMP REQUIRED)
  
  # add the flags it detects to the compile flags
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -fopenmp")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  
endif()

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# Add external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

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
ExternalProject_Add(sdsl-lite
  GIT_REPOSITORY "https://github.com/simongog/sdsl-lite.git"
  GIT_TAG "ddb0fbbc33bb183baa616f17eb48e261ac2a3672"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} # TODO ADD static build flag
  UPDATE_COMMAND ""
  INSTALL_COMMAND "")
ExternalProject_Get_property(sdsl-lite INSTALL_DIR)
set(sdsl-lite_INCLUDE "${INSTALL_DIR}/src/sdsl-lite-build/include")
set(sdsl-lite-divsufsort_INCLUDE "${INSTALL_DIR}/src/sdsl-lite-build/external/libdivsufsort/include")
set(sdsl-lite_LIB "${INSTALL_DIR}/src/sdsl-lite-build/lib")
set(sdsl-lite-divsufsort_LIB "${INSTALL_DIR}/src/sdsl-lite-build/external/libdivsufsort/lib")

# DYNAMIC (full build using its cmake config)
ExternalProject_Add(dynamic
  GIT_REPOSITORY "https://github.com/vgteam/DYNAMIC.git"
  GIT_TAG "0da557fbd0613377bbde26dd60ded655bea58d3b"
  # we don't actually install dynamic... it's header only
  #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  CONFIGURE_COMMAND "")
ExternalProject_Get_property(dynamic INSTALL_DIR)
set(dynamic_INCLUDE "${INSTALL_DIR}/src/dynamic/include")

# hopscotch_map (required by DYNAMIC)
ExternalProject_Add(hopscotch_map
  GIT_REPOSITORY "https://github.com/Tessil/hopscotch-map.git"
  # we don't actually install hopscotch_map... it's header only
  #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  CONFIGURE_COMMAND "")
ExternalProject_Get_property(hopscotch_map INSTALL_DIR)
set(hopscotch_map_INCLUDE "${INSTALL_DIR}/src/hopscotch_map/include")

# libhandlegraph (full build using its cmake config)
ExternalProject_Add(handlegraph
  GIT_REPOSITORY "https://github.com/vgteam/libhandlegraph.git"
  GIT_TAG "f261c36d0343ca1643bbf4d3ca5ef66a381708ef"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
  UPDATE_COMMAND ""
  INSTALL_COMMAND "")
ExternalProject_Get_property(handlegraph INSTALL_DIR)
set(handlegraph_INCLUDE "${INSTALL_DIR}/src/handlegraph/src/include")
set(handlegraph_LIB "${INSTALL_DIR}/src/handlegraph-build")

# BBHash perfect hasher
ExternalProject_Add(bbhash
  GIT_REPOSITORY "https://github.com/vgteam/BBHash.git"
  GIT_TAG "ccc22805eae48aca2a01a7ff5ef8e002857020d3"
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  CONFIGURE_COMMAND "")
ExternalProject_Get_property(bbhash SOURCE_DIR)
set(bbhash_INCLUDE "${SOURCE_DIR}")

# sparsepp
ExternalProject_Add(sparsepp
  GIT_REPOSITORY "https://github.com/edawson/sparsepp.git"
  GIT_TAG "1c5a285e87b2244383efe0398f9992acd61234e9"
  BUILD_IN_SOURCE TRUE
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} # TODO ADD static build flag
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  CONFIGURE_COMMAND "")
ExternalProject_Get_property(sparsepp INSTALL_DIR)
set(sparsepp_INCLUDE "${INSTALL_DIR}/src/sparsepp/")
set(sparsepp_LIB "${INSTALL_DIR}/src/sparsepp/")

# pybind11
#ExternalProject_Add(pybind11
#    GIT_REPOSITORY https://github.com/pybind/pybind11.git 
#    GIT_TAG v2.2.4
#    CONFIGURE_COMMAND ""
#    BUILD_COMMAND ""
#    INSTALL_COMMAND "")
#ExternalProject_Get_Property(pybind11 SOURCE_DIR)
#set(pybind11_INCLUDE_DIRS ${SOURCE_DIR}/include)
#set(pybind11_DIR ${SOURCE_DIR})
#add_subdirectory(${pybind11_DIR})

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

# set up our target executable and specify its dependencies and includes
add_library(bdsg_objs OBJECT
  ${CMAKE_SOURCE_DIR}/src/eades_algorithm.cpp
  ${CMAKE_SOURCE_DIR}/src/hash_graph.cpp
  ${CMAKE_SOURCE_DIR}/src/is_single_stranded.cpp
  ${CMAKE_SOURCE_DIR}/src/node.cpp
  ${CMAKE_SOURCE_DIR}/src/odgi.cpp
  ${CMAKE_SOURCE_DIR}/src/packed_graph.cpp
  ${CMAKE_SOURCE_DIR}/src/packed_path_position_overlays.cpp
  ${CMAKE_SOURCE_DIR}/src/packed_structs.cpp
  ${CMAKE_SOURCE_DIR}/src/path_position_overlays.cpp
  ${CMAKE_SOURCE_DIR}/src/split_strand_graph.cpp
  ${CMAKE_SOURCE_DIR}/src/utility.cpp
  ${CMAKE_SOURCE_DIR}/src/vectorizable_overlays.cpp
  )

set(bdsg_DEPS
    sdsl-lite
    dynamic
    hopscotch_map
    handlegraph
    bbhash
    sparsepp)
add_dependencies(bdsg_objs ${bdsg_DEPS})

set(bdsg_INCLUDES
  "${CMAKE_SOURCE_DIR}/include"
  "${sdsl-lite_INCLUDE}"
  "${sdsl-lite-divsufsort_INCLUDE}"
  "${dynamic_INCLUDE}"
  "${hopscotch_map_INCLUDE}"
  "${handlegraph_INCLUDE}"
  "${sparsepp_INCLUDE}"
  "${bbhash_INCLUDE}")

set(bdsg_LIBS
  "${CMAKE_SOURCE_DIR}/lib/libbdsg.a"
  "${sdsl-lite_LIB}/libsdsl.a"
  "${sdsl-lite-divsufsort_LIB}/libdivsufsort.a"
  "${sdsl-lite-divsufsort_LIB}/libdivsufsort64.a"
  "${handlegraph_LIB}/libhandlegraph.a"
  "-ldl")

target_include_directories(bdsg_objs PUBLIC ${bdsg_INCLUDES})
set_target_properties(bdsg_objs PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_library(libbdsg STATIC $<TARGET_OBJECTS:bdsg_objs>)
set_target_properties(libbdsg PROPERTIES OUTPUT_NAME "bdsg")
set_target_properties(libbdsg PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_executable(test_libbdsg
  ${CMAKE_SOURCE_DIR}/src/test_libbdsg.cpp)
add_dependencies(test_libbdsg libbdsg)
target_link_libraries(test_libbdsg ${bdsg_LIBS})
#  "${sdsl-lite_LIB}/libsdsl.a")
target_include_directories(test_libbdsg PUBLIC ${bdsg_INCLUDES})
set_target_properties(test_libbdsg PROPERTIES OUTPUT_NAME "test_libbdsg")

# todo, for python bindings

#if (NOT EXISTS ${CMAKE_SOURCE_DIR}/deps/pybind11/CMakeLists.txt)
#  execute_process(COMMAND git submodule update --init --recursive)
#endif()

#add_subdirectory(deps/pybind11)
#pybind11_add_module(bdsg_pybind11 "${CMAKE_SOURCE_DIR}/src/pythonmodule.cpp")
#add_dependencies(bdsg_pybind11 ${bdsg_DEPS} libbdsg)
#target_include_directories(bdsg_pybind11 PUBLIC ${bdsg_INCLUDES})
#target_link_libraries(bdsg_pybind11 PRIVATE "${CMAKE_SOURCE_DIR}/lib/libbdsg.a" "${bdsg_LIBS}")
#set_target_properties(bdsg_pybind11 PROPERTIES OUTPUT_NAME "bdsg")

if (APPLE)
elseif (TRUE)
  if (BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
  endif()
endif()
