Setup
********

=================
Build directions
=================

It is straightforward to build libbdsg on a unix-based machine.
First, obtain a copy of the repository:

.. code-block:: bash 

   git clone https://github.com/vgteam/libbdsg.git
   cd libbdsg
   
On Mac, you will need to make sure you have OpenMP installed, as it is not part of the Mac system by default. To install it with Homebrew using libbdsg's Brewfile, you can do:

.. code-block:: bash

   brew bundle

Then, for all platforms, build through cmake:

.. code-block:: bash

   mkdir build
   cd build
   cmake ..
   make

To make a local copy of the documentation:

.. code-block:: bash

   cd docs
   pip3 install -r requirements.txt
   make html

================
Python Usage
================

To import the `bdsg` module in python, make sure that the compiled ``lib/bdsg.cpython*.so`` file is on your Python import path. There are three ways to do this:

1. Add `lib` to your `PYTHONPATH` environment variable.
2. Added `lib` your `sys.path` from within Python.
3. Just be in the `lib` directory.

Once the module is on your Python import path, you can run ``import bdsg``.

For example, assuming that your current working directory is the root of the libbdsg project:

.. code-block:: python

   import sys
   sys.path.append("./lib")
   import bdsg

