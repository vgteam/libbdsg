Setup
********

=============
Build libbdsg
=============

It is straightforward to build libbdsg on a Unix-based machine, such as Linux or macOS.

--------------------
Clone the Repository
--------------------

First, obtain a copy of the repository. Assuming you have Git installed already:

.. code-block:: bash 

   git clone --recursive https://github.com/vgteam/libbdsg.git
   cd libbdsg
   
--------------------
Install Dependencies
--------------------

Some dependencies of libbdsg need to be installed before building the library. A working compiler with C++14 and OpenMP support, CMake 3.10 or newer, Doxygen, and development headers for ``python3`` are required. How to install these varies depending on your operating system. Please follow the section for your OS below:

~~~~~
macOS
~~~~~
   
On Mac, you will need to make sure you have OpenMP installed, as it is not part of the Mac system by default. To install it and other dependencies with `Homebrew <https://brew.sh/>`_, you can do:

.. code-block:: bash

   brew install libomp doxygen
   
Preinstalled Mac versions of Python already come with their development headers.
   
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ubuntu, Mint, and Other Debian Derivatives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. code-block:: bash

   sudo apt update
   sudo apt install build-essential git cmake python3 python3-dev doxygen
   
~~~~~~~~~~
Arch Linux
~~~~~~~~~~
   
.. code-block:: bash

   sudo pacman -Sy base-devel git cmake doxygen python3
   
~~~~~~~~~~~~
Gentoo Linux
~~~~~~~~~~~~

Gentoo already ships Python 3 as part of the base system, but the libbdsg build process goes most smoothly when the latest installed version of Python is selected as the default. Run the following as root:
   
.. code-block:: bash

   emerge --sync
   emerge dev-vcs/git dev-util/cmake app-doc/doxygen dev-lang/python
   eselect python update --python3

-------------------
Configure and Build
-------------------

After installing dependencies, for all platforms, build through CMake. THe libbdsg build system expects an out-of-tree build, hence the creation of the ``build`` directory.

.. code-block:: bash

   mkdir build
   cd build
   cmake ..
   make
   
If you would like to run multiple build tasks in parallel, try ``make -j4`` or ``make -j8`` instead, for 4 or 8 prarllel tasks.
   
If you encounter error messages like ``No download info given for 'sdsl-lite' and its source directory``, then you neglected to clone with ``--recursive`` and don't have the submodule dependencies downloaded. To fix this, you can run:

.. code-block:: bash

   git submodule update --init --recursive
   
You might also encounter a message like:

.. code-block::

  Python version mismatch: CMake wants to build for Python 3.8.2 at
  /usr/bin/python3.8 but `python3` is Python 3.7.7 at /usr/bin/python3.  You
  will not be able to import the module in the current Python! To use the
  version CMake selected, run the build in a virtualenv with that Python
  version activated.  To use the version on your PATH, restart the build with
  -DPYTHON_EXECUTABLE=/usr/bin/python3 on the command line.
  
This happens when you have installed a newer version of Python, but it is not set as the default ``python3``. The easiest thing to do is to tell libbdsg to build against your current default ``python3`` instead of the newest installed one:

.. code-block:: bash

    cmake -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
    make

---------
Run Tests
---------

To make sure that your built version of ``libbdsg`` works, yoiu can run the included tests.

If you were in ``build``, make sure to run ``cd ..`` to go back to the root of the repository. Then run:

.. code-block:: bash

   ./bin/test_libbdsg
   
-------------------
Build Documentation
-------------------

To make a local copy of this documentation, first make sure you are in the root of the repository (not in ``build``), and then run:

.. code-block:: bash

   # Install Sphinx
   virtualenv --python python3 venv
   . venv/bin/activate
   pip3 install -r docs/requirements.txt
   
   # Build the documentation
   make docs

Then open ``docs/_build/html/index.html`` in your web browser.

Note that for documentation updates in the source code to propagate to the HTML output, you first need to regenerate the Python bindings (to update the docstrings in the Python module source) and rerun the CMake-based build (to build the module, and to generate the C++ Docygen XML).

=======================
Use libbdsg From Python
=======================

To import the ``bdsg`` module in python, make sure that the compiled ``lib/bdsg.cpython*.so`` file is on your Python import path. There are three ways to do this:

1. Add ``lib`` to your ``PYTHONPATH`` environment variable.
2. Added ``lib`` your ``sys.path`` from within Python.
3. Just be in the ``lib`` directory.

Once the module is on your Python import path, you can run ``import bdsg``.

For example, assuming that your current working directory is the root of the libbdsg project:

.. code-block:: python

   import sys
   sys.path.append("./lib")
   import bdsg

