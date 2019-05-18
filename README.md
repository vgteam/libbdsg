# sglib
Optimized sequence graph implementations for graph genomics

## Building

```
git clone https://github.com/vgteam/sglib.git
cd sglib
mkdir build
cd build
cmake ..
make -j8
```

We automatically download and build our dependencies: libhandlegraph, sdsl-lite, and sparsepp.

We build sdsl-lite with `-fPIC` to enable us to build `libsglib.so`.

After building, dependency libraries and includes are under `lib/` and `include/` in our build directory.

Installation support and adjusting build layout to be usable as a submodule is still WIP.
