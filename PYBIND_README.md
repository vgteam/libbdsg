# An Overview on Python Bindings in libbdsg

## Python Usage

Using libbdsg in python is straightforward.  After building libbdsg, make sure that the `bdsg.cpython*.so` file is on your `PYTHONPATH` or added through `sys.path`, and run `import libbdsg`.

## Development Usage

Python bindings for libbdsg are generated automatically using [Binder](https://github.com/RosettaCommons/binder) and [PyBind11](https://github.com/pybind/pybind11).

### PyBind Overview

PyBind exposes C++ code from a given set of *python bindings*.  These bindings are a set of .cpp files that specify information about the objects and methods to be exposed to python.  At compile time, cmake instructs PyBind to use these bindings to create a `*.so` file that can be imported into python.

#### When to Update Bindings
 
If the only changes were to the backend of libbdsg, there is no need to modify the python bindings because they will automatically be incorporated into the linked library on compile-time. It is only necessary to update the binding .cpp files if new methods or objects need to be exposed to python.

#### How to Update Bindings

The included script `make_and_run_binder.py` will automatically download and build Binder (if this has not already been done) and run binder on libbdsg, placing the output .cpp files in cmake_bindings.  *Note: Binder will require up to ~2.6 Gb of free disk space, and takes some time to compile.*

Before running this script, it is required to have run cmake and make to populate the dependency source files.  Additionally, this script has two dependencies: `gitpython` is needed to clone the repo and `ninja` is needed to build binder.  These can be installed with:

```
pip3 install gitpython
sudo apt-get install ninja-build
```

Note that binder requires that includes be of the format `#include <*>` and not `#include "*"`.  This script will automatically change the includes in the source files before running them into binder and will revert them when completed, but if an error is thrown from binder they will not be reverted.  It is advised to ensure that you can revert any changes through `git checkout -- src`.

Specific functions/classes/enums can be manually included or excluded from binding by modifying the included `config.cfg`, as specified in the [binder documentation](https://cppbinder.readthedocs.io/en/latest/config.html#config-file-options).

