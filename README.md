# sglib
Optimized sequence graph implementations for graph genomics

## Building and Installation

```
git clone https://github.com/vgteam/sglib.git
cd sglib
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j8 install
```

We automatically download and build our dependencies: libhandlegraph, sdsl-lite, and sparsepp.

We build sdsl-lite with `-fPIC` to enable us to build `libsglib.so`.

After building, dependency libraries and includes are under `lib/` and `include/` in our build directory.

Installing will install them and the built libsglib and its headers to the prefix passed with `-DCMAKE_INSTALL_PREFIX`.

## TODO List:

- [ ] Don't re-cmake all the external projects and rebuild ourselves on every make
- [ ] Make the installed version of the library able to find the libhandlegraph.so that it installs alongside itself, because it has been moved from where it was linked against.
- [ ] Use correct permissions on installed libhandlegraph.so

