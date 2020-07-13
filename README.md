# libbdsg

Optimized **b**i**d**irected **s**equence **g**raph implementations for graph genomics

[![Documentation Status](https://readthedocs.org/projects/bdsg/badge/?version=master)](https://bdsg.readthedocs.io/en/master/?badge=master)

## About

The main purpose of `libbdsg` is to provide high performance implementations of sequence graphs for graph-based pangenomics applications. The repository contains three graph implementations with different performance tradeoffs:

- HashGraph: prioritizes speed
- ODGI: balances speed and low memory usage
- PackedGraph: prioritizes low memory usage

All of these graph objects implement a common interface defined by [`libhandlegraph`](https://github.com/vgteam/libhandlegraph), so they can be used interchangeably and swapped easily.

Additionally, `libbdsg` provides a few "overlays", which are applied to the graph implementations in order to expand their functionality. The expanded functionality is also described generically using `libhandlegraph` interfaces.

#### Programming languages

`libbdsg` is written in C++. Using the instructions below, it is also possible to generate Python bindings to the underlying C++ library. The Python API is documented [here](https://bdsg.readthedocs.io/). The documentation also includes a tutorial that serves as a useful introduction to `libhandlegraph` and `libbdsg` concepts.

## Citation

A preprint that discusses the implementation and functionality of `libbdsg` is available under the following citation:

Eizenga, JM, Novak, AM, Kobayashi, E, Villani, F, Cisar, C, Heumos, S, Hickey, G, Colonna, V, Paten, B, Garrison, E. (2020) Succinct dynamic variation graphs. _bioRxiv_. doi:[10.1101/2020.04.23.056317](https://doi.org/10.1101/2020.04.23.056317).

The peer-reviewed article is forthcoming in _Bioinformatics_ under the title "Efficient dynamic variation graphs". It was drafted in [this GitHub respository](https://github.com/vgteam/handlegraph-paper).

## Building and Installation

When obtaining the source repo, make sure to clone with `--recursive` to get all the submodules:

```
git clone --recursive https://github.com/vgteam/libbdsg.git
```

### With `cmake` (library and Python bindings)

Full CMake-based installation instructions, including tips on dependency installation, are available in [the documentation](https://bdsg.readthedocs.io/en/master/rst/install.html). A basic guide is provided here.

With CMake, we are able to build Python bindings that use `pybind11`. However, we only support out-of-source builds from a directory named `build`, and we still put the built artifacts in `lib` in the main project directory.

To run a CMake-based build:
```
mkdir build
cd build
cmake ..
make -j 8
```

If the build fails, the Python bindings may be out of date with respect to the source files. See [PYBIND_README.md](PYBIND_README.md) for instructions on updating them. You may also need to install Doxygen. If you cannot install Doxygen, you can bypass the Doxygen portion of the build with `cmake .. -DRUN_DOXYGEN=OFF`.


#### Building Documentation

The documentation for `libbdsg` is built using Sphinx, and will invoke the CMake-based build process if not already run. To build it, from the main project directory:

```
# Install Sphinx
virtualenv --python python3 venv
. venv/bin/activate
pip3 install -r docs/requirements.txt

# Build the documentation
make docs
```

The documentation can then be found at `docs/_build/html/index.html`.

### With `make` (library only)

#### Dependencies

`libbdsg` has a few external dependencies:

- [`libhandlegraph`](https://github.com/vgteam/libhandlegraph)
- [`sdsl`](https://github.com/simongog/sdsl-lite)
- [`sparsepp`](https://github.com/greg7mdp/sparsepp)
- [`DYNAMIC`](https://github.com/xxsds/DYNAMIC)
- [`BBHash/alltypes`](https://github.com/rizkg/BBHash/tree/alltypes) 

The build process with `make` assumes that these libraries and their headers have been installed in a place on the system where the compiler can find them (e.g. in `CPLUS_INCLUDE_PATH`).

#### Building

The following commands will create the `libbdsg.a` library in the `lib` directory. 

```
git clone https://github.com/vgteam/libbdsg.git
cd libbdsg
make -j 8
```

To install system-wide (in `/usr/local/`):

```
make install
```

Or to install in an alternate location:

```
INSTALL_PREFIX=/other/path/ make install
```

