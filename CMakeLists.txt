# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)
# We can't use CMake 3.11 until we no longer have to run on Ubuntu 18.04.

include(FindPkgConfig)

# Make MACOSX_RPATH on explicitly, so non-installed libraries use @rpath in install name
set(CMAKE_MACOSX_RPATH ON)

# Project's name
project(bdsg)

option(RUN_DOXYGEN "Build Doxygen files required for Breathe-based docs" ON)
option(BUILD_PYTHON_BINDINGS "Compile the bdsg Python module" ON)
option(OPTIMIZE "Build with optimization" ON)
OPTION(USE_INSTALLED_LIBHANDLEGRAPH "Use the version of libhandlegraph installed on the system, if it exists, instead of the bundled version [default: off]" OFF)

# TODO: We can only do out-of-source builds!
# TODO: How do we error out meaningfully on in-source builds?

# We build using c++14
set(CMAKE_CXX_STANDARD 14)
# We need library paths to be relative in the build directories so we can let
# the libraries in our Python module find each other when we package them into
# a wheel. This only works on CMake 3.14+; older CMake we have to bully with
# linker options.
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

# And GNU ideas of install directories
include(GNUInstallDirs)

if (OPTIMIZE)
# Use all standard-compliant optimizations
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()
# Always add debug info
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Find OMP system depenency and configure for OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  # assumes clang build
  # we can't reliably detect when we're using clang, so for the time being we assume
  # TODO: can't we though?
  
  # adapted from https://stackoverflow.com/questions/46414660/macos-cmake-and-openmp
  # find_package(OpenMP) does not work reliably on macOS, so we do its work ourselves
  set (HOMEBREW_PREFIX "$ENV{HOMEBREW_PREFIX}")
  if ("${HOMEBREW_PREFIX}" STREQUAL "")
    # We sometimes (like on Github Actions) have Homebrew but no HOMEBREW_PREFIX environment variable.
    # Ask Homebrew if it exists and where it is.
    # Returns an empty string if this command can't run.
    execute_process(COMMAND brew --prefix OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE HOMEBREW_PREFIX)
  endif()
  if ("${HOMEBREW_PREFIX}" STREQUAL "")
    # Default to somewhere.
    set (HOMEBREW_PREFIX "/opt/homebrew")
  else()
    message("Homebrew appears to be installed in ${HOMEBREW_PREFIX}")
  endif()
  
  find_path(OMP_INCLUDE_PATH omp.h
            PATHS "${HOMEBREW_PREFIX}/opt/libomp/include" "${HOMEBREW_PREFIX}/include" /opt/local/include/libomp /usr/local/include
            REQUIRED)
  if ("${OMP_INCLUDE_PATH}" STREQUAL "")
    # REQUIRED is a new CMake feature
    message(FATAL_ERROR "Could not find omp.h")
  else()
    message("Found libomp headers in ${OMP_INCLUDE_PATH}")
  endif()
            
  find_path(OMP_LIB_PATH libomp.dylib
            PATHS "${HOMEBREW_PREFIX}/opt/libomp/lib" "${HOMEBREW_PREFIX}/lib" /opt/local/lib /usr/local/lib
            REQUIRED)
  if ("${OMP_LIB_PATH}" STREQUAL "")
    # REQUIRED is a new CMake feature
    message(FATAL_ERROR "Could not find libomp.dylib")
  else()
    message("Found libomp libraries in ${OMP_LIB_PATH}")
  endif()
  
  set (OMP_INCLUDE_FLAGS "-I${OMP_INCLUDE_PATH}")
  set (OMP_LIB_FLAGS "-L${OMP_LIB_PATH}")
  
  # We also might need manual libdl for some reason, so find it properly
  find_path(DL_LIB_PATH libdl.dylib
            PATHS "${HOMEBREW_PREFIX}/lib" /opt/local/lib /usr/local/lib /usr/lib /lib)
  if ("${DL_LIB_PATH}" STREQUAL "DL_LIB_PATH-NOTFOUND")
    # On new (10.14+?) MacOS, there is no longer a libdl that is libSystem wearing a hat.
    # See <https://github.com/meetecho/janus-gateway/issues/1486#issuecomment-451542851>
    # But since libSystem is just the standard library, the functions are all already there.
    # So don't try to link anything.
    message("Could not find libdl.dylib; hoping it is not needed")
    set (DL_LIB_FLAGS "")
  else()
    # On old MacOS, we have needed to link against libdl. So if we found it, use it.
    message("Found libdl libraries in ${DL_LIB_PATH}")
    set (DL_LIB_FLAGS "-L${DL_LIB_PATH}" -ldl)
  endif()
  
  set (OpenMP_C "${CMAKE_C_COMPILER}")
  set (OpenMP_C_FLAGS " -Xpreprocessor -fopenmp ${OMP_INCLUDE_FLAGS}")
  set (OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set (OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  set (OpenMP_CXX_FLAGS " -Xpreprocessor -fopenmp ${OMP_INCLUDE_FLAGS}")
  set (OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set (OpenMP_EXE_LINKER_FLAGS "${OMP_LIB_FLAGS}")
  set (OpenMP_SHARED_LINKER_FLAGS "${OMP_LIB_FLAGS}")
  set (OpenMP_libomp_LIBRARY "omp")
  set (OpenMP_libgomp_LIBRARY "gomp")
  set (OpenMP_libiomp5_LIBRARY "iomp5")
  
  # and now add the OpenMP parameters to the compile flags
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lomp")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS} -lomp")
  
  # Mac needs libomp when linking the library and a hint on where to look for them
  # Mac doesn't have $ORIGIN for RPaths. We might not be able to produce a working Mac wheel for Python bindings.
  set(PLATFORM_EXTRA_LIB_FLAGS -lomp ${DL_LIB_FLAGS} ${OMP_LIB_FLAGS})
  
  # Mac needs install names set after installation
  SET(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  find_package(OpenMP REQUIRED)
  
  # add the flags it detects to the compile flags
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -fopenmp")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  
  # Linux only needs libdl when linking the library
  # Also make sure to manually force a $ORIGIN in the RPath for when CMAKE_BUILD_RPATH_USE_ORIGIN doesn't work.
  set(PLATFORM_EXTRA_LIB_FLAGS -ldl -Wl,-rpath='$ORIGIN')
  
endif()

# Find other system dependencies
pkg_check_modules(Jansson REQUIRED IMPORTED_TARGET jansson)

# Find our bdsg package directory where input sources and dependencies are
set(bdsg_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bdsg")

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# Add external projects
include(ExternalProject)

# sdsl-lite (gives an "sdsl" target)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build sdsl-lite shared libraries")
add_subdirectory("${bdsg_DIR}/deps/sdsl-lite")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # It produces divsufsort and divsufsort64 targets that don't know they need OMP on Mac.
    set_target_properties(divsufsort PROPERTIES LINK_FLAGS "-lomp")
    set_target_properties(divsufsort64 PROPERTIES LINK_FLAGS "-lomp")
endif()

# hopscotch_map (required by DYNAMIC, gives a "tsl::hopscotch_map" target)
add_subdirectory("${bdsg_DIR}/deps/hopscotch-map")

# DYNAMIC (header only)
# Does not ship its own install step or define a target, so we make our own target
add_library(dynamic INTERFACE)
target_include_directories(dynamic INTERFACE "${bdsg_DIR}/deps/DYNAMIC/include")
target_link_libraries(dynamic INTERFACE tsl::hopscotch_map)

# libhandlegraph (full build using its cmake config)
# Produces handlegraph_shared and handlegraph_static targets (as well as handlegraph_objs)
if (USE_INSTALLED_LIBHANDLEGRAPH)
    find_package(libhandlegraph)
endif()
if (${libhandlegraph_FOUND})
    message("Using installed libhandlegraph")
elseif (TARGET handlegraph_objs)
    message("Using libhandlegraph built by another CMake")
else ()
    message("Using bundled libhandlegraph")
    add_subdirectory("${bdsg_DIR}/deps/libhandlegraph")
endif()


# BBHash perfect hasher (header only)
# Does not ship its own install step or define a target, so we make our own target
add_library(bbhash INTERFACE)
target_include_directories(bbhash INTERFACE "${bdsg_DIR}/deps/BBHash/")
# We would be able to use public_header for the one header, but that's not allowed with IMPORTED libraries.

# sparsepp (header only)
# Does not ship its own install step or define a target, so we make our own target
add_library(sparsepp INTERFACE)
target_include_directories(sparsepp INTERFACE "${bdsg_DIR}/deps/sparsepp/")

# mio (header only)
add_subdirectory("${bdsg_DIR}/deps/mio")

if (BUILD_PYTHON_BINDINGS)

    # Binder (because some generated bindings depend on headers packaged with Binder)
    # See also: Binder commit defined in make_and_run_binder.py which actually generates bindings.
    set(BINDER_COMMIT 46ec0e88137d368eeedafecfa123004f8ad028d1)
    ExternalProject_Add(binder
      GIT_REPOSITORY "https://github.com/RosettaCommons/binder.git"
      GIT_TAG "${BINDER_COMMIT}"
      # we don't actually build or install Binder via its CMake because we just need its headers
      #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND bash -c "mkdir -p <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR} && cp -r <SOURCE_DIR>/source/*.hpp <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR}/")
    ExternalProject_Get_property(binder INSTALL_DIR)
    set(binder_INCLUDE "${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")

    # pybind11
    # See also: pybind11 commit defined in make_and_run_binder.py.
    set(PYBIND11_COMMIT 5b0a6fc2017fcc176545afe3e09c9f9885283242)
    if (CMAKE_MAJOR_VERSION EQUAL "3" AND CMAKE_MINOR_VERSION EQUAL "10")
        # We need pybind11 installed in ./pybind11 *before* CMake can finish processing this file.
        # On CMake 3.11+ we can do that with FetchContent
        # But on CMake 3.10, available on Ubuntu 18.04, we have to just call git ourselves.
        if (NOT EXISTS "${PROJECT_SOURCE_DIR}/pybind11")
            message(WARNING "Running on CMake without FetchContent_Declare; attempting to download pybind11 manually")
            execute_process(COMMAND git clone https://github.com/RosettaCommons/pybind11.git "${PROJECT_SOURCE_DIR}/pybind11")
            execute_process(COMMAND git checkout "${PYBIND11_COMMIT}"
                            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/pybind11")
        endif()
        
        # Whether we cloned it this time or not, include it.
        add_subdirectory(pybind11)
    else()
        # This FetchContent_Declare logic works only on CMake 3.11+.
        # We can't use it on Ubuntu 18.04
        
        # set up FetchContent so we can incorporate pybind11
        include(FetchContent)
        
        FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/RosettaCommons/pybind11.git
            GIT_TAG "${PYBIND11_COMMIT}"
        )
        FetchContent_GetProperties(pybind11)
        if (NOT pybind11_POPULATED)
            FetchContent_Populate(pybind11)
        endif()
        
        add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
    endif()


    # Make sure that pybind11 is looking at the same Python our shell is looking at.
    # If this isn't true, on RtD, our build can go very wrong because we can't import the module.
    # And on a user's machine, it's almost certainly what they expect to happen.
    # PYTHON_EXECUTABLE and other PYTHON_ variables are set by pybind11's CMakeLists.txt
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" --version
                    OUTPUT_VARIABLE PYTHON_EXECUTABLE_VERSION
                    ERROR_VARIABLE PYTHON_EXECUTABLE_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND which python3
                    OUTPUT_VARIABLE PYTHON_WHICH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${PYTHON_WHICH}" --version
                    OUTPUT_VARIABLE PYTHON_WHICH_VERSION
                    ERROR_VARIABLE PYTHON_WHICH_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT PYTHON_EXECUTABLE_VERSION STREQUAL PYTHON_WHICH_VERSION)
        message(FATAL_ERROR "Python version mismatch: CMake wants to build for ${PYTHON_EXECUTABLE_VERSION} at ${PYTHON_EXECUTABLE} "
                "but `python3` is ${PYTHON_WHICH_VERSION} at ${PYTHON_WHICH}. You will not be able to import the module in the "
                "current Python! To use the version CMake selected, run the build in a virtualenv with that Python version activated. "
                "To use the version on your PATH, restart the build with -DPYTHON_EXECUTABLE=${PYTHON_WHICH} on the command line.")
    endif()
    
    # Work out where Python binary modules should be installed for this Python,
    # under the prefix. TODO: is there a variable for this? Should we reach out
    # of the prefix and try to install in whatever directory is appropriate for
    # the current Python, assuming we are root?
    set(CMAKE_INSTALL_PYTHON_LIBDIR "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages")
endif()


#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

# set up our target executable and specify its dependencies and includes
add_library(bdsg_objs OBJECT
  ${bdsg_DIR}/src/eades_algorithm.cpp
  ${bdsg_DIR}/src/hash_graph.cpp
  ${bdsg_DIR}/src/is_single_stranded.cpp
  ${bdsg_DIR}/src/mapped_structs.cpp
  ${bdsg_DIR}/src/packed_graph.cpp
  ${bdsg_DIR}/src/packed_path_position_overlay.cpp
  ${bdsg_DIR}/src/packed_reference_path_overlay.cpp
  ${bdsg_DIR}/src/reference_path_overlay.cpp
  ${bdsg_DIR}/src/packed_subgraph_overlay.cpp
  ${bdsg_DIR}/src/path_position_overlays.cpp
  ${bdsg_DIR}/src/path_subgraph_overlay.cpp
  ${bdsg_DIR}/src/subgraph_overlay.cpp
  ${bdsg_DIR}/src/strand_split_overlay.cpp
  ${bdsg_DIR}/src/utility.cpp
  ${bdsg_DIR}/src/vectorizable_overlays.cpp
  ${bdsg_DIR}/src/snarl_distance_index.cpp
  )

# Add includes for ourselves
set(bdsg_INCLUDES
  "${bdsg_DIR}/include"
)
# Note that binder_INCLUDE will be empty if Binder isn't in use.

# Targets we depend on that are real library targets that we can install.
set(bdsg_TARGET_DEPS
  sdsl
  tsl::hopscotch_map
  dynamic
  handlegraph_shared
  bbhash
  sparsepp
  mio::mio
  PkgConfig::Jansson)

set(bdsg_LIBS
  ${bdsg_TARGET_DEPS}
  ${PLATFORM_EXTRA_LIB_FLAGS})

if (BUILD_PYTHON_BINDINGS)
    add_dependencies(bdsg_objs binder)
endif()

target_include_directories(bdsg_objs PUBLIC ${bdsg_INCLUDES})
set_target_properties(bdsg_objs PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

if (CMAKE_MAJOR_VERSION EQUAL "3" AND (CMAKE_MINOR_VERSION EQUAL "10" OR CMAKE_MINOR_VERSION EQUAL "11"))
    # Before CMake 3.12 we can't ise target_link_libraries on an object library to convey the need to use depencies' include directories
    get_target_property(sdsl_INCLUDE sdsl INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${sdsl_INCLUDE})
    get_target_property(hopscotch_map_INCLUDE tsl::hopscotch_map INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${hopscotch_map_INCLUDE})
    get_target_property(dynamic_INCLUDE dynamic INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${dynamic_INCLUDE})
    get_target_property(handlegraph_INCLUDE handlegraph_shared INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${handlegraph_INCLUDE})
    get_target_property(bbhash_INCLUDE bbhash INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${bbhash_INCLUDE})
    get_target_property(sparsepp_INCLUDE sparsepp INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${sparsepp_INCLUDE})
else()
    # Set all the libs as public dependencies of the object library
    target_link_libraries(bdsg_objs PUBLIC ${bdsg_LIBS})
endif()

add_library(libbdsg SHARED $<TARGET_OBJECTS:bdsg_objs>)
set_target_properties(libbdsg PROPERTIES OUTPUT_NAME "bdsg")
set_target_properties(libbdsg PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Make sure we can find our dependency libraries after installation, if we aren't installing into the system search path
set_target_properties(libbdsg PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
# Since we have a directory structure of headers, we can't just associate the
# individual header files as PUBLIC_HEADERS and PRIVATE_HEADERS on the target.

# On older CMake, libraries weren't added as dependencies of the object library
# (which is not yet possible) so we need to add them as dependencies of the
# actual library. On Mac, we always need to do this, because you can't link a
# library there without the libraries it depends on at runtime.
target_link_libraries(libbdsg PUBLIC ${bdsg_LIBS})


# Remember to bring along the includes
target_include_directories(libbdsg PUBLIC ${bdsg_INCLUDES})

# This is what we want to actually install for users.
# Dependencies from add_subdirectory install themselves, and the interface
# targets we make are header-only and get their heraders copied later.
install(TARGETS libbdsg 
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Install headers separately. See <https://stackoverflow.com/a/54285948>
install(DIRECTORY ${bdsg_DIR}/include/bdsg
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Install headers from all dependencies. Make sure to use a trailing slash to
# dump all contents rather than the directory. Don't install Binder's includes
# since we don't need headers for the Python module.
install(DIRECTORY "${bdsg_DIR}/deps/DYNAMIC/include/"
                  "${bdsg_DIR}/deps/BBHash/"
                  "${bdsg_DIR}/deps/sparsepp/sparsepp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h")

add_executable(test_libbdsg
  ${bdsg_DIR}/src/test_libbdsg.cpp)
target_link_libraries(test_libbdsg libbdsg)
set_target_properties(test_libbdsg PROPERTIES OUTPUT_NAME "test_libbdsg")
set_target_properties(test_libbdsg PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

if (BUILD_PYTHON_BINDINGS)
    # Build the Pythoin bindings
    file(GLOB_RECURSE pybind11_API "${bdsg_DIR}/cmake_bindings/*.cpp")
    list(FILTER pybind11_API EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp$")
    pybind11_add_module(bdsg_pybind11 ${pybind11_API})
    add_dependencies(bdsg_pybind11 ${bdsg_DEPS} libbdsg)
    target_include_directories(bdsg_pybind11 PUBLIC ${bdsg_INCLUDES} "${binder_INCLUDE}")
    target_link_libraries(bdsg_pybind11 PRIVATE libbdsg "${bdsg_LIBS}")
    set_target_properties(bdsg_pybind11 PROPERTIES OUTPUT_NAME "bdsg")
    set_target_properties(bdsg_pybind11 PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    
    # And install them
    install(TARGETS bdsg_pybind11
            LIBRARY DESTINATION "${CMAKE_INSTALL_PYTHON_LIBDIR}")
endif()

if (APPLE)
elseif (TRUE)
  if (BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
  endif()
endif()

# Configure the Doxygen pass that we need for Breathe and Sphinx to document
# our C++ code. See <https://vicrucann.github.io/tutorials/quick-cmake-doxygen/>
if (RUN_DOXYGEN)
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # We need to substitute in paths in the Doxyfile depending on where CMake is having us build.
    set(DOXYGEN_IN ${bdsg_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(run_doxygen ALL VERBATIM
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  else()
    message(FATAL_ERROR "No Doxygen found; cannot build with RUN_DOXYGEN ON")
  endif()
endif()


