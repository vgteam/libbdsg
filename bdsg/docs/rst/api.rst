.. _api:

##########
Python API
##########

================
Handle Graph API
================

The :mod:`bdsg.handlegraph` module defines the handle graph interface.

.. automodule:: bdsg.handlegraph
   
-------
Handles
-------
   
The module contains definitions for different types of handles. THese are references to graph elements. A basic :class:`bdsg.handelgraph.handle_t` is a reference to a strand or orientation of a node in the graph.

.. autoclass:: bdsg.handlegraph.handle_t
   :show-inheritance:
   :members:
   :inherited-members:
   
.. autoclass:: bdsg.handlegraph.path_handle_t
   :show-inheritance:
   :members:
   :inherited-members:
   
.. autoclass:: bdsg.handlegraph.step_handle_t
   :show-inheritance:
   :members:
   :inherited-members:
   
.. autoclass:: bdsg.handlegraph.net_handle_t
   :show-inheritance:
   :members:
   :inherited-members:

----------------
Graph Interfaces
----------------

The :mod:`bdsg.handlegraph` module also defines a hierarchy of interfaces for graph implementations that provide different levels of features.

~~~~~~~~~~~
HandleGraph
~~~~~~~~~~~

The most basic is the :class:`bdsg.handlegraph.HandleGraph`, a completely immutable, unannotated graph.

.. autoclass:: bdsg.handlegraph.HandleGraph
   :show-inheritance:
   :members:

~~~~~~~~~~~~~~~
PathHandleGraph
~~~~~~~~~~~~~~~

On top of this, there is the :class:`bdsg.handlegraph.PathHandleGraph`, which allows for embedded, named paths in the graph.

.. autoclass:: bdsg.handlegraph.PathHandleGraph
   :show-inheritance:
   :members:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mutable and Deletable Interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Then for each there are versions where the underlying graph is "mutable" (meaning that material can be added to it and nodes can be split) and "deletable" (meaning that nodes and edges can actually be removed from the graph), and for :class:`bdsg.handlegraph.PathHandleGraph` there are versions where the paths can be altered.

.. autoclass:: bdsg.handlegraph.MutableHandleGraph
   :show-inheritance:
   :members:
   
.. autoclass:: bdsg.handlegraph.DeletableHandleGraph
   :show-inheritance:
   :members:
   
.. autoclass:: bdsg.handlegraph.MutablePathHandleGraph
   :show-inheritance:
   :members:
   
.. autoclass:: bdsg.handlegraph.MutablePathMutableHandleGraph
   :show-inheritance:
   :members:
   
.. autoclass:: bdsg.handlegraph.MutablePathDeletableHandleGraph
   :show-inheritance:
   :members:
   
Note that there is no :class:`bdsg.handlegraph.PathMutableHandleGraph` or :class:`bdsg.handlegraph.PathDeletableHandleGraph`; it does not make sense for the paths to be static while the graph can be modified.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Position and Ordering Interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For paths, there is also the :class:`bdsg.handlegraph.PathPositionHandleGraph` which provides efficient random access by or lookup of base offset along each embedded path. Additionally, there is :class:`bdsg.handlegraph.VectorizableHandleGraph` which provides the same operations for a linearization of all of the graph's bases. There is also a :class:`bdsg.handlegraph.RankedHandleGraph` interface, which provides an ordering, though not necessarily a base-level linearization, of nodes and edges.

.. autoclass:: bdsg.handlegraph.PathPositionHandleGraph
   :show-inheritance:
   :members:

.. autoclass:: bdsg.handlegraph.VectorizableHandleGraph
   :show-inheritance:
   :members:
   
.. autoclass:: bdsg.handlegraph.RankedHandleGraph
   :show-inheritance:
   :members:

Algorithm implementers are encouraged to take the least capable graph type necessary for their algorithm to function.

~~~~~~~~~~~~~~~~~~~~~~~
SerializableHandleGraph
~~~~~~~~~~~~~~~~~~~~~~~

Orthogonal to the mutability and paths hierarchy, there is a :class:`bdsg.handlegraph.SerializableHandleGraph` interface that is implemented by graphs that can be saved to and loaded from disk. The C++ API supports saving to and loading from C++ streams, but the Python API provides only the ability to save to or load from filenames.

.. autoclass:: bdsg.handlegraph.SerializableHandleGraph
   :show-inheritance:
   :members:
   :inherited-members:
 
~~~~~~~~~~~~~~~~~~
SnarlDecomposition
~~~~~~~~~~~~~~~~~~

A "snarl decomposition" describes the decomposition of the graph into nested substructures known as snarls and chains. The :class:`bdsg.handlegraph.SnarlDecomposition` interface defines methods for traversing the snarl decomposition of a graph using :class:`bdsg.handelgraph.net_handle_t`.

.. autoclass:: bdsg.handlegraph.SnarlDecomposition
   :show-inheritance:
   :members:
   :inherited-members:
     
====================================
libbdsg Handle Graph Implementations
====================================

The :mod:`bdsg.bdsg` module provides useful implementations of the Handle Graph API.

.. automodule:: bdsg.bdsg

..
   TODO: Why can't we see bdsg.bdsg classes with automodule :members:?
   We don't want all the global functions from the module anyway so just do each class.
   
--------------------------
Full Graph Implementations
--------------------------

There are two full graph implementations in the module: :class:`bdsg.bdsg.PackedGraph` and :class:`bdsg.bdsg.HashGraph`. Previously, a third implementation, ODGI, was provided, but that implementation is now part of its own `odgi project <https://github.com/pangenome/odgi#odgi>`_.

~~~~~~~~~~~
PackedGraph
~~~~~~~~~~~

.. autoclass:: bdsg.bdsg.PackedGraph
   :show-inheritance:
   :members:
   
~~~~~~~~~
HashGraph
~~~~~~~~~

.. autoclass:: bdsg.bdsg.HashGraph
   :show-inheritance:
   :members:
   
----------------------------------
SnarlDecomposition Implementations
----------------------------------

There is one implementation for a snarl decomposition

~~~~~~~~~~~~~~~~~~
SnarlDistanceIndex
~~~~~~~~~~~~~~~~~~

.. autoclass:: bdsg.bdsg.SnarlDistanceIndex
   :show-inheritance:
   :members:

--------------
Graph Overlays
--------------
   
In addition to these basic implementations, there are several "overlays". These overlays are graphs that wrap other graphs, providing features not avialable in the backing graph, or otherwise transforming it. 

.. autoclass:: bdsg.bdsg.PositionOverlay
   :show-inheritance:
   
.. autoclass:: bdsg.bdsg.PackedPositionOverlay
   :show-inheritance:
   
.. autoclass:: bdsg.bdsg.MutablePositionOverlay
   :show-inheritance:
   
.. autoclass:: bdsg.bdsg.VectorizableOverlay
   :show-inheritance:
   
.. autoclass:: bdsg.bdsg.PathVectorizableOverlay
   :show-inheritance:
   
.. autoclass:: bdsg.bdsg.PathPositionVectorizableOverlay
   :show-inheritance:
   
Many of these are based on the :class:`bdsg.handlegraph.ExpandingOverlayGraph` interface, which guarantees that the overlay does not remove any graph material, and allows handles form the backing graph and the overlay graph to be interconverted.

.. autoclass:: bdsg.handlegraph.ExpandingOverlayGraph
   :show-inheritance:
   :members:

=================
Typed Collections
=================

Some methods, such as :func:`bdsg.handlegraph.MutableHandleGraph.divide_handle`, produce or consume collections of typed objects: C++ STL vectors with Python bindings. The typed collection classes are available in :mod:`bdsg.std`. They are convertible from and to Python lists via their constructors and the list constructor, respectively.

Here is an example of how to use these typed collections:

.. code-block:: python

    import bdsg
    g = bdsg.bdsg.HashGraph()
    h = g.create_handle("GATTACA")
    v = bdsg.std.vector_unsigned_long([1, 3])
    parts = g.divide_handle(h, v)
    # parts is a bdsg.std.vector_handlegraph_handle_t
    print(list(parts))

.. automodule:: bdsg.std

.. autoclass:: vector_handlegraph_handle_t
    :members:

.. autoclass:: vector_handlegraph_step_handle_t
    :members:
    
.. autoclass:: vector_unsigned_long
    :members:
