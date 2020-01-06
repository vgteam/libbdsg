project(bdsg)
set(CMAKE_CXX_STANDARD 14)
include_directories(/home/ctcisar/git/binder/source)
include_directories(/home/ctcisar/git/libbdsg-easy/deps/libbdsg/src)
include_directories(/home/ctcisar/git/libbdsg-easy/deps/libbdsg/include)
include_directories(/home/ctcisar/git/binder/build/pybind11/include)
include_directories(/usr/include/python3.6m)
set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)
add_definitions(-DNDEBUG)
add_library(bdsg SHARED
	bdsg.cpp
	std/postypes.cpp
	handlegraph/types.cpp
	std/locale_classes.cpp
	std/ostream_tcc.cpp
	handlegraph/handle_graph.cpp
	bdsg/is_single_stranded.cpp
	handlegraph/path_handle_graph.cpp
	handlegraph/deletable_handle_graph.cpp
	bdsg/wang_hash.cpp
	bdsg/packed_structs.cpp
	bdsg/utility.cpp
	bdsg/varint.cpp
	bdsg/varint_1.cpp
	bdsg/node.cpp
	handlegraph/expanding_overlay_graph.cpp
	bdsg/split_strand_graph.cpp
	handlegraph/path_position_handle_graph.cpp
	bdsg/packed_path_position_overlays.cpp
	bdsg/path_position_overlays.cpp
	bdsg/vectorizable_overlays.cpp
	bdsg/vectorizable_overlays_1.cpp
)
set_target_properties(bdsg PROPERTIES PREFIX "")
set_target_properties(bdsg PROPERTIES SUFFIX ".so")
