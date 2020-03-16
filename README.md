# libbdsg

Optimized **b**i**d**irected **s**equence **g**raph implementations for graph genomics

## About

The main purpose of `libbdsg` is to provide high performance implementations of sequence graphs for graph-based pangenomics applications. The repository contains three graph implementations with different performance tradeoffs:

- HashGraph: prioritizes speed
- ODGI: balances speed and low memory usage
- PackedGraph: prioritizes low memory usage

All of these graph objects implement a common interface, so they can be used interchangeably and swapped easily.

## Building and Installation

### Easy install

The easiest way to build `libbdsg` is to use the [easy install repository](https://github.com/vgteam/libbdsg-easy), which coordinates `libbdsg` and its dependencies using Git submodules.

### Dependencies

`libbdsg` has a few external dependencies:

- [`libhandlegraph`](https://github.com/vgteam/libhandlegraph)
- [`sdsl`](https://github.com/simongog/sdsl-lite)
- [`sparsepp`](https://github.com/greg7mdp/sparsepp)
- [`DYNAMIC`](https://github.com/xxsds/DYNAMIC)
- [`BBHash/alltypes`](https://github.com/rizkg/BBHash/tree/alltypes) 

The build process assumes that these libraries and their headers have been installed in a place on the system where the compiler can find them (e.g. in `CPLUS_INCLUDE_PATH`).

### Building

#### With `make` (library only)

The following commands will create the `libbdsg.a` library in the `lib` directory. 

```
git clone https://github.com/vgteam/libbdsg.git
cd libbdsg
make -j8
```

To install system-wide (in `/usr/local/`):

```
make install
```

Or to install in an alternate location:

```
INSTALL_PREFIX=/other/path/ make install
```

#### With `cmake` (library and Python binding)

**If running on Ubuntu 18.04**, or other platforms with **CMake 3.10**, CMake cannot automatically install pybind11 and use it as part of the build process. You will have to install it manually. From the project root:

```
git clone https://github.com/RosettaCommons/pybind11.git
cd pybind11
git checkout 35045eeef8969b7b446c64b192502ac1cbf7c451
cd ..
```

Once you have done that, or if you have **CMake 3.11 or newer**, run:

```
mkdir build
cd build
cmake ..
make
```
