# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)
# We can't use CMake 3.11 until we no longer have to run on Ubuntu 18.04.

# Project's name
project(bdsg)

option(RUN_DOXYGEN "Build Doxygen files required for Breathe-based docs" ON)
option(BUILD_PYTHON_BINDINGS "Compile the bdsg Python module" ON)

# TODO: We can only do out-of-source builds!
# TODO: How do we error out meaningfully on in-source builds?

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
  
  # Mac needs libdl and libomp when linking the library
  set(PLATFORM_EXTRA_LIB_FLAGS -ldl -lomp)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  find_package(OpenMP REQUIRED)
  
  # add the flags it detects to the compile flags
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -fopenmp")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  
  # Linux only needs libdl when linking the library
  set(PLATFORM_EXTRA_LIB_FLAGS -ldl)
  
endif()

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# Add external projects
include(ExternalProject)
# And GNU ideas of install directories
include(GNUInstallDirs)

# sdsl-lite (gives an "sdsl" target)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build sdsl-lite shared libraries")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/sdsl-lite")

# hopscotch_map (required by DYNAMIC, gives a "tsl::hopscotch_map" target)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/hopscotch-map")

# DYNAMIC (header only)
# Does not ship its own install step or define a target, so we make our own target
add_library(dynamic INTERFACE)
target_include_directories(dynamic INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/deps/DYNAMIC/include")
target_link_libraries(dynamic INTERFACE tsl::hopscotch_map)

# libhandlegraph (full build using its cmake config)
ExternalProject_Add(handlegraph
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/libhandlegraph"
  CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")
ExternalProject_Get_property(handlegraph INSTALL_DIR)
set(handlegraph_INCLUDE "${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
set(handlegraph_LIB "${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}")

# BBHash perfect hasher
# Does not ship its own install step so we have to copy stuff ourselves
ExternalProject_Add(bbhash
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/BBHash"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND bash -c "mkdir -p <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR} && cp -r <SOURCE_DIR>/*.h <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR}/")
ExternalProject_Get_property(bbhash INSTALL_DIR)
set(bbhash_INCLUDE "${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")

# sparsepp
# Does not ship its own install step so we have to copy stuff ourselves
ExternalProject_Add(sparsepp
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/sparsepp"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND bash -c "mkdir -p <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR} && cp -r <SOURCE_DIR>/sparsepp <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR}/")
ExternalProject_Get_property(sparsepp INSTALL_DIR)
set(sparsepp_INCLUDE "${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")

if (BUILD_PYTHON_BINDINGS)
    # Binder (because some generated bindings depend on headers packaged with Binder)
    ExternalProject_Add(binder
      GIT_REPOSITORY "https://github.com/RosettaCommons/binder.git"
      GIT_TAG "788ab422f9e919478944d79d5890441a964dd1db"
      # we don't actually build or install Binder via its CMake because we just need its headers
      #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND bash -c "mkdir -p <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR} && cp -r <SOURCE_DIR>/source/*.hpp <INSTALL_DIR>/${CMAKE_INSTALL_INCLUDEDIR}/")
    ExternalProject_Get_property(binder INSTALL_DIR)
    set(binder_INCLUDE "${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")

    # pybind11
    if (CMAKE_MAJOR_VERSION EQUAL "3" AND CMAKE_MINOR_VERSION EQUAL "10")
        # We need pybind11 installed in ./pybind11 *before* CMake can finish processing this file.
        # On CMake 3.11+ we can do that with FetchContent
        # But on CMake 3.10, available on Ubuntu 18.04, we have to just call git ourselves.
        if (NOT EXISTS "${PROJECT_SOURCE_DIR}/pybind11")
            message(WARNING "Running on CMake without FetchContent_Declare; attempting to download pybind11 manually")
            execute_process(COMMAND git clone https://github.com/RosettaCommons/pybind11.git "${PROJECT_SOURCE_DIR}/pybind11")
            execute_process(COMMAND git checkout 4f72ef846fe8453596230ac285eeaa0ce3278bb4
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
            GIT_TAG 4f72ef846fe8453596230ac285eeaa0ce3278bb4
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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eades_algorithm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hash_graph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/is_single_stranded.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/odgi.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/packed_graph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/packed_path_position_overlay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/packed_structs.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/packed_subgraph_overlay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/path_position_overlays.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/strand_split_overlay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utility.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vectorizable_overlays.cpp
  )

set(bdsg_INCLUDES
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${handlegraph_INCLUDE}"
  "${sparsepp_INCLUDE}"
  "${bbhash_INCLUDE}"
  "${binder_INCLUDE}"
)
# Note that binder_INCLUDE will be empty if Binder isn't in use.

# Targets we depend on that are real library targets that we can install.
set(bdsg_TARGET_DEPS
  sdsl
  tsl::hopscotch_map
  dynamic)

set(bdsg_LIBS
  ${bdsg_TARGET_DEPS}
  "${handlegraph_LIB}/libhandlegraph${CMAKE_SHARED_LIBRARY_SUFFIX}"
  ${PLATFORM_EXTRA_LIB_FLAGS})

# Targets we depend on that are just external projects
set(bdsg_DEPS
    handlegraph
    bbhash
    sparsepp
)
set(bdsg_PYTHON_DEPS
    binder
)
add_dependencies(bdsg_objs ${bdsg_DEPS})
if (BUILD_PYTHON_BINDINGS)
    add_dependencies(bdsg_objs ${bdsg_PYTHON_DEPS})
endif()

target_include_directories(bdsg_objs PUBLIC ${bdsg_INCLUDES})
set_target_properties(bdsg_objs PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

if (CMAKE_MAJOR_VERSION EQUAL "3" AND (CMAKE_MINOR_VERSION EQUAL "10" OR CMAKE_MINOR_VERSION EQUAL "11"))
    # Before CMake 3.12 we can't ise target_link_libraries on an object library to convey the need to use depencies' include directories
    get_target_property(sdsl_INCLUDE sdsl INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${sdsl_INCLUDE})
    get_target_property(hopscotch_map_INCLUDE tsl::hopscotch_map INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${hopscotch_map_INCLUDE})
    get_target_property(dynamic_INCLUDE dynamic INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(bdsg_objs PUBLIC ${dynamic_INCLUDE})
else()
    # Set all the libs as public dependencies of the object library
    target_link_libraries(bdsg_objs PUBLIC ${bdsg_LIBS})
endif()

# Make sure when we install our library we set its rpath so it can find its
# dependencies we also install.
# Set this up before library target is created.
# If using this library as an ExternalProject, this needs to stay the default
# or non-installed consumers of the library won't be able to find its
# dependencies.
# We put this in the cache if it isn't set, and preserve the cached value (from
# "-D" on the command line if specified) if it is set.
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Library search path upon installation")

add_library(libbdsg SHARED $<TARGET_OBJECTS:bdsg_objs>)
set_target_properties(libbdsg PROPERTIES OUTPUT_NAME "bdsg")
set_target_properties(libbdsg PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Since we have a directory structure of headers, we can't just associate the
# individual header files as PUBLIC_HEADERS and PRIVATE_HEADERS on the target.

if (CMAKE_MAJOR_VERSION EQUAL "3" AND (CMAKE_MINOR_VERSION EQUAL "10" OR CMAKE_MINOR_VERSION EQUAL "11"))
    # Libraries weren't added as dependencies of the object library (which is
    # not yet possible) so we need to add them as dependencies of the actual
    # library
    target_link_libraries(libbdsg PUBLIC ${bdsg_LIBS})
endif()


# Remember to bring along the includes
target_include_directories(libbdsg PUBLIC ${bdsg_INCLUDES})

# This is what we want to actually install for users
install(TARGETS libbdsg
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Install headers separately. See <https://stackoverflow.com/a/54285948>
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/bdsg
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Install headers from all dependencies. Make sure to use a trailing slash so
# we merge all the include directory contents. Don't install Binder's includes
# since we don't need headers for the Python module.
install(DIRECTORY "${handlegraph_INCLUDE}/"
                  "${sparsepp_INCLUDE}/"
                  "${bbhash_INCLUDE}/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Install libraries for out non-header-only library dependencies.
# Don't bring along pkg-config files; they have wrong paths
install(DIRECTORY "${sdsl-lite_LIB}/"
                  "${handlegraph_LIB}/"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PATTERN "pkgconfig" EXCLUDE)

add_executable(test_libbdsg
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test_libbdsg.cpp)
# add_dependencies(test_libbdsg libbdsg)
target_link_libraries(test_libbdsg libbdsg)
target_include_directories(test_libbdsg PUBLIC ${bdsg_INCLUDES})
set_target_properties(test_libbdsg PROPERTIES OUTPUT_NAME "test_libbdsg")

if (BUILD_PYTHON_BINDINGS)
    # Build the Pythoin bindings
    file(GLOB_RECURSE pybind11_API "${CMAKE_CURRENT_SOURCE_DIR}/cmake_bindings/*.cpp")
    list(FILTER pybind11_API EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp$")
    pybind11_add_module(bdsg_pybind11 ${pybind11_API})
    add_dependencies(bdsg_pybind11 ${bdsg_DEPS} libbdsg)
    target_include_directories(bdsg_pybind11 PUBLIC ${bdsg_INCLUDES})
    target_link_libraries(bdsg_pybind11 PRIVATE libbdsg "${bdsg_LIBS}")
    set_target_properties(bdsg_pybind11 PROPERTIES OUTPUT_NAME "bdsg")
    
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
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(run_doxygen ALL VERBATIM
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  else()
    message(FATAL_ERROR "No Doxygen found; cannot build with RUN_DOXYGEN ON")
  endif()
endif()


