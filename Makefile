SRC_DIR:=bdsg/src
OBJ_DIR:=bdsg/obj
INC_DIR:=bdsg/include
LIB_DIR:=lib
BIN_DIR:=bin

INSTALL_PREFIX?=/usr/local
INSTALL_LIB_DIR=$(INSTALL_PREFIX)/lib
INSTALL_INC_DIR=$(INSTALL_PREFIX)/include

LIB_FLAGS:=-lbdsg -lsdsl -lhandlegraph -lomp

OBJS:=$(OBJ_DIR)/eades_algorithm.o $(OBJ_DIR)/hash_graph.o $(OBJ_DIR)/is_single_stranded.o $(OBJ_DIR)/node.o $(OBJ_DIR)/odgi.o $(OBJ_DIR)/packed_graph.o $(OBJ_DIR)/packed_structs.o $(OBJ_DIR)/path_position_overlays.o $(OBJ_DIR)/packed_path_position_overlay.o $(OBJ_DIR)/vectorizable_overlays.o $(OBJ_DIR)/strand_split_overlay.o $(OBJ_DIR)/utility.o

CXXFLAGS :=-O3 -Werror=return-type -std=c++14 -ggdb -g -I$(INC_DIR) $(CXXFLAGS)

ifeq ($(shell uname -s),Darwin)
	CXXFLAGS := $(CXXFLAGS) -Xpreprocessor -fopenmp
else
	CXXFLAGS := $(CXXFLAGS) -fopenmp
endif

.PHONY: .pre-build all clean install docs

all: $(LIB_DIR)/libbdsg.a

test: all $(BIN_DIR)/test_libbdsg
	./$(BIN_DIR)/test_libbdsg

docs:
	cd docs && $(MAKE) html

.pre-build:
	@if [ ! -d $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	@if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi

# run .pre-build before we make anything at all.
-include .pre-build

$(OBJ_DIR)/eades_algorithm.o: $(SRC_DIR)/eades_algorithm.cpp $(INC_DIR)/bdsg/internal/eades_algorithm.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/eades_algorithm.cpp -o $(OBJ_DIR)/eades_algorithm.o 

$(OBJ_DIR)/hash_graph.o: $(SRC_DIR)/hash_graph.cpp $(INC_DIR)/bdsg/hash_graph.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/hash_graph.cpp -o $(OBJ_DIR)/hash_graph.o 

$(OBJ_DIR)/is_single_stranded.o: $(SRC_DIR)/is_single_stranded.cpp $(INC_DIR)/bdsg/internal/is_single_stranded.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/is_single_stranded.cpp -o $(OBJ_DIR)/is_single_stranded.o 

$(OBJ_DIR)/node.o: $(SRC_DIR)/node.cpp $(INC_DIR)/bdsg/internal/node.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/node.cpp -o $(OBJ_DIR)/node.o 

$(OBJ_DIR)/odgi.o: $(SRC_DIR)/odgi.cpp $(INC_DIR)/bdsg/odgi.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/odgi.cpp -o $(OBJ_DIR)/odgi.o 

$(OBJ_DIR)/packed_graph.o: $(SRC_DIR)/packed_graph.cpp $(INC_DIR)/bdsg/packed_graph.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/packed_graph.cpp -o $(OBJ_DIR)/packed_graph.o 

$(OBJ_DIR)/packed_structs.o: $(SRC_DIR)/packed_structs.cpp $(INC_DIR)/bdsg/internal/packed_structs.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/packed_structs.cpp -o $(OBJ_DIR)/packed_structs.o 

$(OBJ_DIR)/path_position_overlays.o: $(SRC_DIR)/path_position_overlays.cpp $(INC_DIR)/bdsg/overlays/path_position_overlays.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/path_position_overlays.cpp -o $(OBJ_DIR)/path_position_overlays.o

$(OBJ_DIR)/packed_path_position_overlay.o: $(SRC_DIR)/packed_path_position_overlay.cpp $(INC_DIR)/bdsg/overlays/packed_path_position_overlay.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/packed_path_position_overlay.cpp -o $(OBJ_DIR)/packed_path_position_overlay.o

$(OBJ_DIR)/vectorizable_overlays.o: $(SRC_DIR)/vectorizable_overlays.cpp $(INC_DIR)/bdsg/overlays/vectorizable_overlays.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/vectorizable_overlays.cpp -o $(OBJ_DIR)/vectorizable_overlays.o

$(OBJ_DIR)/strand_split_overlay.o: $(SRC_DIR)/strand_split_overlay.cpp $(INC_DIR)/bdsg/overlays/strand_split_overlay.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/strand_split_overlay.cpp -o $(OBJ_DIR)/strand_split_overlay.o


$(OBJ_DIR)/utility.o: $(SRC_DIR)/utility.cpp $(INC_DIR)/bdsg/internal/utility.hpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/utility.cpp -o $(OBJ_DIR)/utility.o 

$(LIB_DIR)/libbdsg.a: $(OBJS)
	rm -f $@
	ar rs $@ $(OBJS)

$(BIN_DIR)/test_libbdsg: $(LIB_DIR)/libbdsg.a $(SRC_DIR)/test_libbdsg.cpp 
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -L $(LIB_DIR) $(SRC_DIR)/test_libbdsg.cpp -o $(BIN_DIR)/test_libbdsg $(LIB_FLAGS)
	chmod +x $(BIN_DIR)/test_libbdsg

install: $(LIB_DIR)/libbdsg.a
	mkdir -p $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INC_DIR)
	cp $(LIB_DIR)/libbdsg.a $(INSTALL_LIB_DIR)/
	cp -r $(INC_DIR)/bdsg $(INSTALL_INC_DIR)/

clean:
	rm -r $(OBJ_DIR)
	rm -r $(LIB_DIR)
	rm -r $(BIN_DIR)

