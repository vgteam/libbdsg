.. _cppapi:

#######
C++ API
#######

Being written in C++, ``libbdsg`` and ``libhandlegraph`` offer a C++ API.

================
Handle Graph API
================

The ``handlegraph`` namespace defines the handle graph interface.
   
-------
Handles
-------
   
The namespace contains definitions for different types of handles. THese are references to graph elements. A basic :cpp:class:`handlegraph::handle_t` is a reference to a strand or orientation of a node in the graph.

.. doxygenstruct:: handlegraph::handle_t

.. doxygenstruct:: handlegraph::path_handle_t

.. doxygenstruct:: handlegraph::step_handle_t

----------------
Graph Interfaces
----------------

The ``handlegraph`` namespace also defines a hierarchy of interfaces for graph implementations that provide different levels of features.

The most basic is the :cpp:class:`handlegraph::HandleGraph`, a completely immutable, unannotated graph.

.. doxygenclass:: handlegraph::HandleGraph
   :members:
   
On top of this, there is the :cpp:class:`handlegraph::PathHandleGraph`, which allows for embedded, named paths in the graph.

.. doxygenclass:: handlegraph::PathHandleGraph
   :members:
   
Then for each there are versions where the underlying graph is "mutable" (meaning that material can be added to it and nodes can be split) and "deletable" (meaning that nodes and edges can actually be removed from the graph), and for :cpp:class:`handlegraph::PathHandleGraph` there are versions where the paths can be altered.

.. doxygenclass:: handlegraph::MutableHandleGraph
   :members:
   
.. doxygenclass:: handlegraph::DeletableHandleGraph
   :members:
   
.. doxygenclass:: handlegraph::MutablePathHandleGraph
   :members:
   
.. doxygenclass:: handlegraph::MutablePathMutableHandleGraph
   :members:
   
.. doxygenclass:: handlegraph::MutablePathDeletableHandleGraph
   :members:
   
Note that there is no :cpp:class:`handlegraph::PathMutableHandleGraph` or :cpp:class:`handlegraph::PathDeletableHandleGraph`; it does not make sense for the paths to be static while the graph can be modified.

For paths, there is also the :cpp:class:`handlegraph::PathPositionHandleGraph` which provides efficient random access by or lookup of base offset along each embedded path. Additionally, there is :cpp:class:`handlegraph::VectorizableHandleGraph` which provides the same operations for a linearization of all of the graph's bases.

.. doxygenclass:: handlegraph::PathPositionHandleGraph
   :members:

.. doxygenclass:: handlegraph::VectorizableHandleGraph
   :members:

Algorithm implementers are encouraged to take the least capable graph type necessary for their algorithm to function.

Orthogonal to the mutability and paths hierarchy, there is a :cpp:class:`handlegraph::SerializableHandleGraph` interface that is implemented by graphs that can be saved to and loaded from disk. The C++ API supports saving to and loading from C++ streams, but the Python API provides only the ability to save to or load from filenames.

.. doxygenclass:: handlegraph::SerializableHandleGraph
   :members:

====================================
libbdsg Handle Graph Implementations
====================================

The ``bdsg`` namespace provides useful implementations of the Handle Graph API.

--------------------------
Full Graph Implementations
--------------------------

There are three full graph implementations in the module: :cpp:class:`bdsg::PackedGraph`, :cpp:class:`bdsg::HashGraph`, and :cpp:class:`bdsg::ODGI`.

.. doxygenclass:: bdsg::PackedGraph
   :members:

.. doxygenclass:: bdsg::HashGraph
   :members:
   
.. doxygenclass:: bdsg::ODGI
   :members:
   
--------------
Graph Overlays
--------------
   
In addition to these basic implementations, there are several "overlays". These overlays are graphs that wrap other graphs, providing features not avialable in the backing graph, or otherwise transforming it. 

.. doxygenclass:: handlegraph::ExpandingOverlayGraph
   :members:
   
.. doxygenclass:: bdsg::PositionOverlay
   
.. doxygenclass:: bdsg::PackedPositionOverlay
   
.. doxygenclass:: bdsg::MutablePositionOverlay
   
.. doxygenclass:: bdsg::VectorizableOverlay
   
.. doxygenclass:: bdsg::PathVectorizableOverlay
   
.. doxygenclass:: bdsg::PathPositionVectorizableOverlay





