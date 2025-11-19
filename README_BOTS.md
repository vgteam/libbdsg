# Useful Commands

## Building
To build the project (for most code changes):

```
(mkdir -p build && cd build && cmake .. -DRUN_DOXYGEN=OFF -DBUILD_PYTHON_BINDINGS=OFF && make -j8)
```

If something looks weirdly wrong with the CMake configuration, you can `rm -Rf build` and try the build again.

## Testing

To run the tests after building:

```
./bin/test_libbdsg
```

# Layout

All the code is in `bdsg/src` and `bdsg/include/bdsg`. Dependencies (like the HandleGraph headers) are in `bdsg/deps`.

The `*.classfragment` files in `bdsg/include/bdsg/internal` are used to assemble "proxy" objects that wrap and expose all the methods of a contained object which for technical reasons isn't actually allowed to inherit from a base class, in order to provide something that does actually inherit from that base class. Try not to fiddle with this system.

