SRC_DIR:=src
OBJ_DIR:=obj
INC_DIR:=include
LIB_DIR:=lib

INSTALL_PREFIX?=/usr/local
INSTALL_LIB_DIR=$(INSTALL_PREFIX)/lib
INSTALL_INC_DIR=$(INSTALL_PREFIX)/include

OBJS:=$(OBJ_DIR)/eades_algorithm.o $(OBJ_DIR)/hash_graph.o $(OBJ_DIR)/is_single_stranded.o $(OBJ_DIR)/packed_graph.o $(OBJ_DIR)/packed_structs.o $(OBJ_DIR)/split_strand_graph.o $(OBJ_DIR)/utility.o

CXXFLAGS :=-O3 -Werror=return-type -std=c++14 -ggdb -g -msse4.2 -I$(INC_DIR) $(CXXFLAGS)

.PHONY: .pre-build all clean install

all:
	make $(LIB_DIR)/libbdsg.a

.pre-build:
	@if [ ! -d $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi

# run .pre-build before we make anything at all.
-include .pre-build

$(OBJ_DIR)/eades_algorithm.o: $(SRC_DIR)/eades_algorithm.cpp $(INC_DIR)/bdsg/eades_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/eades_algorithm.cpp -o $(OBJ_DIR)/eades_algorithm.o 

$(OBJ_DIR)/hash_graph.o: $(SRC_DIR)/hash_graph.cpp $(INC_DIR)/bdsg/hash_graph.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/hash_graph.cpp -o $(OBJ_DIR)/hash_graph.o 

$(OBJ_DIR)/is_single_stranded.o: $(SRC_DIR)/is_single_stranded.cpp $(INC_DIR)/bdsg/is_single_stranded.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/is_single_stranded.cpp -o $(OBJ_DIR)/is_single_stranded.o 

$(OBJ_DIR)/packed_graph.o: $(SRC_DIR)/packed_graph.cpp $(INC_DIR)/bdsg/packed_graph.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/packed_graph.cpp -o $(OBJ_DIR)/packed_graph.o 

$(OBJ_DIR)/packed_structs.o: $(SRC_DIR)/packed_structs.cpp $(INC_DIR)/bdsg/packed_structs.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/packed_structs.cpp -o $(OBJ_DIR)/packed_structs.o 

$(OBJ_DIR)/split_strand_graph.o: $(SRC_DIR)/split_strand_graph.cpp $(INC_DIR)/bdsg/split_strand_graph.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/split_strand_graph.cpp -o $(OBJ_DIR)/split_strand_graph.o 

$(OBJ_DIR)/utility.o: $(SRC_DIR)/utility.cpp $(INC_DIR)/bdsg/utility.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/utility.cpp -o $(OBJ_DIR)/utility.o 

$(LIB_DIR)/libbdsg.a: $(OBJS)
	rm -f $@
	ar rs $@ $(OBJ_DIR)/*.o

install: $(LIB_DIR)/libbdsg.a
	mkdir -p $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INC_DIR)
	cp $(LIB_DIR)/libbdsg.a $(INSTALL_LIB_DIR)/
	cp -r $(INC_DIR)/bdsg $(INSTALL_INC_DIR)/

clean:
	rm -r $(OBJ_DIR)
	rm -r $(LIB_DIR)