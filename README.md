# sglib

Optimized sequence graph implementations for graph genomics

## About

The main purpose of `sglib` is to provide high performance implementations of sequence graphs for graph-based pangenomics applications. The repository contains three graph implementations with different performance tradeoffs:

- HashGraph: prioritizes speed
- ODGI: balances speed and low memory usage
- PackedGraph: prioritizes low memory usage

All of these graph objects implement a common interface, so they can be used interchangeably and swapped easily.

## Building and Installation

### Easy install

The easiest way to build `sglib` is to use the [easy install repository](https://github.com/vgteam/sglib-easy), which coordinates `sglib` and its dependencies using Git submodules.

### Dependencies

`sglib` has a few external dependencies:

- [`libhandlegraph`](https://github.com/vgteam/libhandlegraph)
- [`sdsl`](https://github.com/simongog/sdsl-lite)
- [`sparsepp`](https://github.com/greg7mdp/sparsepp)

The build process assumes that these libraries and their headers have been installed in a place on the system where the compiler can find them (e.g. in `CPLUS_INCLUDE_PATH`).

### Building

The following commands will create the `libsglib.a` library in the `lib` directory. 

```
git clone https://github.com/vgteam/sglib.git
cd sglib
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

