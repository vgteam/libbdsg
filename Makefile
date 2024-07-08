SRC_DIR:=bdsg/src
OBJ_DIR:=bdsg/obj
INC_DIR:=bdsg/include
DOC_DIR:=bdsg/docs
LIB_DIR:=lib
BIN_DIR:=bin

INSTALL_PREFIX?=/usr/local
INSTALL_LIB_DIR=$(INSTALL_PREFIX)/lib
INSTALL_INC_DIR=$(INSTALL_PREFIX)/include

LIB_FLAGS:=-lbdsg -lsdsl -lhandlegraph -ljansson

OBJS = $(OBJ_DIR)/eades_algorithm.o 
OBJS += $(OBJ_DIR)/hash_graph.o 
OBJS += $(OBJ_DIR)/is_single_stranded.o 
OBJS += $(OBJ_DIR)/mapped_structs.o 
OBJS += $(OBJ_DIR)/packed_graph.o 
OBJS += $(OBJ_DIR)/path_position_overlays.o 
OBJS += $(OBJ_DIR)/packed_path_position_overlay.o
OBJS += $(OBJ_DIR)/reference_path_overlay.o
OBJS += $(OBJ_DIR)/packed_reference_path_overlay.o
OBJS += $(OBJ_DIR)/path_subgraph_overlay.o
OBJS += $(OBJ_DIR)/subgraph_overlay.o
OBJS += $(OBJ_DIR)/vectorizable_overlays.o 
OBJS += $(OBJ_DIR)/packed_subgraph_overlay.o 
OBJS += $(OBJ_DIR)/snarl_distance_index.o
OBJS += $(OBJ_DIR)/strand_split_overlay.o 
OBJS += $(OBJ_DIR)/utility.o

CXXFLAGS :=-MMD -MP -O3 -Werror=return-type -std=c++14 -ggdb -g -I$(INC_DIR) $(CXXFLAGS)

ifeq ($(shell uname -s),Darwin)
	CXXFLAGS := $(CXXFLAGS) -Xpreprocessor -fopenmp
	LIB_FLAGS := $(LIB_FLAGS) -lomp
else
	CXXFLAGS := $(CXXFLAGS) -fopenmp
endif

.PHONY: .pre-build all clean install docs

all: $(LIB_DIR)/libbdsg.a

test: all $(BIN_DIR)/test_libbdsg
	./$(BIN_DIR)/test_libbdsg

docs:
	cd $(DOC_DIR) && $(MAKE) html

.pre-build:
	@if [ ! -d $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	@if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi

# run .pre-build before we make anything at all.
-include .pre-build

# Make sure to pull in dependency files
include $(wildcard $(OBJ_DIR)/*.d)

# Use a fake rule to build .d files, so we don't complain if they don't exist.
$(OBJ_DIR)/%.d: ;

# Don't delete them.
.PRECIOUS: $(OBJ_DIR)/%.d

# Build each object with this rule.
# Depend on the .d file so we rebuild if dependency info is missing/deleted
# Make sure to touch the .o file after the compiler finishes so it is always newer than the .d file
# Use static pattern rules so the dependency files will not be ignored if the output exists
# See <https://stackoverflow.com/a/34983297>
$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	@touch $@


$(LIB_DIR)/libbdsg.a: $(OBJS)
	rm -f $@
	ar rs $@ $(OBJS)

$(BIN_DIR)/test_libbdsg: $(LIB_DIR)/libbdsg.a $(SRC_DIR)/test_libbdsg.cpp
	mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -L $(LIB_DIR) $(SRC_DIR)/test_libbdsg.cpp -o $(BIN_DIR)/test_libbdsg $(LIB_FLAGS)
	chmod +x $(BIN_DIR)/test_libbdsg

install: $(LIB_DIR)/libbdsg.a
	mkdir -p $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INC_DIR)
	cp $(LIB_DIR)/libbdsg.a $(INSTALL_LIB_DIR)/
	cp -r $(INC_DIR)/bdsg $(INSTALL_INC_DIR)/

clean:
	[ ! -e $(OBJ_DIR) ] || rm -r $(OBJ_DIR)
	[ ! -e $(LIB_DIR) ] || rm -r $(LIB_DIR)
	[ ! -e $(BIN_DIR) ] || rm -r $(BIN_DIR)

