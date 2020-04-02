# An Overview on Python Bindings in libbdsg

## Python Usage

Using libbdsg in python is straightforward. After building libbdsg, make sure that the `bdsg.cpython*.so` file is on your `PYTHONPATH` or added through `sys.path`, and run `import bdsg`.

## Development Usage

Python bindings for libbdsg are generated automatically using [Binder](https://github.com/RosettaCommons/binder) and [PyBind11](https://github.com/pybind/pybind11).

### PyBind Overview

PyBind exposes C++ code from a given set of *python bindings*. These bindings are a set of .cpp files that specify information about the objects and methods to be exposed to python. At compile time, cmake instructs PyBind to use these bindings to create a `*.so` file that can be imported into python.

#### When to Update Bindings
 
Changes to the backend of libbdsg do not require modification to the python bindings, as they will be incorporated into the linked library on compile-time automatically. It is only necessary to update binding files if new methods or objects are created that need to be exposed to python.

#### How to Update Bindings

The included script `make_and_run_binder.py` will automatically download and build Binder (if this has not already been done) and run binder on libbdsg, placing the output .cpp files in cmake_bindings. *Note: Binder will require up to ~2.6 Gb of free disk space, and takes some time to compile.*

1. Make sure the dependencies for the script are installed. Besides `git`, the script depends on `ninja`, which is needed by binder's build system. On Ubuntu, you can install both with:
```
sudo apt-get install git ninja-build
```

2. The binding generator needs to see the source code for libbdsg's dependencies. If you haven't already done so, you need to run CMake in a directory named `build` under the project root. **It does not need to succeed, and in fact will not succeed if the bindings are out of date.** It just needs to start running, in order to download dependencies. To do this:
```
mkdir build
cd build
cmake .. || true
cd ..
```

3. Now you can run the actual binding-generating script. **Note that the first time you run it, it downloads and builds an entire C++ compiler**, so it may take a long time. Also note that it can't yet detect the core limit of a container; it tries to use all the cores of the host machine for its compiler build. To run it:
```
./make_and_run_binder.py
```

4. **Make sure to add new files** that Binder may have created to source control:
```
git add cmake_bindings
```

Note that binder requires that includes be of the format `#include <*>` and not `#include "*"`. This script will automatically change includes in the source files before running binder and will revert them when completed. If a fatal error occurs while binding, these changes may not be reverted properly. It is advised to ensure that all changes have been committed so that it is possible to revert with `git checkout -- src include`.

Specific functions/classes/enums can be manually included or excluded from binding by modifying the included `config.cfg`, as specified in the [binder documentation](https://cppbinder.readthedocs.io/en/latest/config.html#config-file-options).

