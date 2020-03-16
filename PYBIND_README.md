# An Overview on Python Bindings in libbdsg

## Python Usage

Using libbdsg in python is straightforward.  After building libbdsg, make sure that the `bdsg.cpython*.so` file is on your `PYTHONPATH` or added through `sys.path`, and run `import libbdsg`.

## Development Usage

Python bindings for libbdsg are generated automatically using [Binder](https://github.com/RosettaCommons/binder) and [PyBind11](https://github.com/pybind/pybind11).

### PyBind Overview

PyBind exposes C++ code from a given set of *python bindings*.  These bindings are a set of .cpp files that specify information about the objects and methods to be exposed to python.  At compile time, cmake instructs PyBind to use these bindings to create a `*.so` file that can be imported into python.

#### When to Update Bindings
 
Changes to the backend of libbdsg do not require modification to the python bindings, as they will be incorporated into the linked library on compile-time automatically. It is only necessary to update binding files if new methods or objects are created that need to be exposed to python.

#### How to Update Bindings

The included script `make_and_run_binder.py` will automatically download and build Binder (if this has not already been done) and run binder on libbdsg, placing the output .cpp files in cmake_bindings.  *Note: Binder will require up to ~2.6 Gb of free disk space, and takes some time to compile.*

Before running this script, it is required to run cmake and make to populate the dependency source files.  Additionally, besides `git`, this script depends on `ninja`, which is needed by binder's build system. This can be installed on Ubuntu with:

```
sudo apt-get install ninja-build
```

Note that binder requires that includes be of the format `#include <*>` and not `#include "*"`.  This script will automatically change includes in the source files before running binder and will revert them when completed.  If a fatal error occurs while binding, these changes may not be reverted properly.  It is advised to ensure that all changes have been committed so that it is possible to revert with `git checkout -- src include`.

Specific functions/classes/enums can be manually included or excluded from binding by modifying the included `config.cfg`, as specified in the [binder documentation](https://cppbinder.readthedocs.io/en/latest/config.html#config-file-options).

