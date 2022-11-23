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

.. doxygenstruct:: handlegraph::net_handle_t

----------------
Graph Interfaces
----------------

The ``handlegraph`` namespace also defines a hierarchy of interfaces for graph implementations that provide different levels of features.

~~~~~~~~~~~
HandleGraph
~~~~~~~~~~~

The most basic is the :cpp:class:`handlegraph::HandleGraph`, a completely immutable, unannotated graph.

.. doxygenclass:: handlegraph::HandleGraph
   :members:
   
~~~~~~~~~~~~~~~
PathHandleGraph
~~~~~~~~~~~~~~~
   
On top of this, there is the :cpp:class:`handlegraph::PathHandleGraph`, which allows for embedded, named paths in the graph.

.. doxygenclass:: handlegraph::PathHandleGraph
   :members:
   
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mutable and Deletable Interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
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

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Position and Ordering Interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For paths, there is also the :cpp:class:`handlegraph::PathPositionHandleGraph` which provides efficient random access by or lookup of base offset along each embedded path. Additionally, there is :cpp:class:`handlegraph::VectorizableHandleGraph` which provides the same operations for a linearization of all of the graph's bases. There is also a :cpp:class:`handlegraph::RankedHandleGraph` interface, which provides an ordering, though not necessarily a base-level linearization, of nodes and edges.

.. doxygenclass:: handlegraph::PathPositionHandleGraph
   :members:

.. doxygenclass:: handlegraph::VectorizableHandleGraph
   :members:
   
.. doxygenclass:: handlegraph::RankedHandleGraph
   :members:

Algorithm implementers are encouraged to take the least capable graph type necessary for their algorithm to function.

~~~~~~~~~~~~~~~~~~~~~~~
SerializableHandleGraph
~~~~~~~~~~~~~~~~~~~~~~~

Orthogonal to the mutability and paths hierarchy, there is a :cpp:class:`handlegraph::SerializableHandleGraph` interface that is implemented by graphs that can be saved to and loaded from disk. The C++ API supports saving to and loading from C++ streams, but the Python API provides only the ability to save to or load from filenames.

.. doxygenclass:: handlegraph::SerializableHandleGraph
   :members:

~~~~~~~~~~~~~~~~~~~~~~~
SnarlDecomposition
~~~~~~~~~~~~~~~~~~~~~~~

A "snarl decomposition" describes the decomposition of the graph into nested substructures known as snarls and chains. The :cpp:class:`handlegraph::SnarlDecomposition` interface defines methods for traversing the snarl decomposition of a graph using :cpp:class:`handlegraph::net_handle_t`.

.. doxygenclass:: handlegraph::SnarlDecomposition
   :members:

====================================
libbdsg Handle Graph Implementations
====================================

The ``bdsg`` namespace provides useful implementations of the Handle Graph API.

--------------------------
Full Graph Implementations
--------------------------

There are two full graph implementations in the module: :cpp:class:`bdsg::PackedGraph`, :cpp:class:`bdsg::HashGraph`. Previously, a third implementation, ODGI, was provided, but that implementation is now part of its own `odgi project <https://github.com/pangenome/odgi#odgi>`_.

~~~~~~~~~~~
PackedGraph
~~~~~~~~~~~

.. doxygenclass:: bdsg::PackedGraph
   :members:

~~~~~~~~~
HashGraph
~~~~~~~~~

.. doxygenclass:: bdsg::HashGraph
   :members:
   
-----------------------------------
Snarl Decomposition Implementations
-----------------------------------

There is one implementation for a snarl decomposition

~~~~~~~~~~~~~~~~~~
SnarlDistanceIndex
~~~~~~~~~~~~~~~~~~

.. doxygenclass:: bdsg::SnarlDistanceIndex
   :members:
   
--------------
Graph Overlays
--------------
   
In addition to these basic implementations, there are several "overlays". These overlays are graphs that wrap other graphs, providing features not avialable in the backing graph, or otherwise transforming it. 

.. doxygenclass:: bdsg::PositionOverlay
   
.. doxygenclass:: bdsg::PackedPositionOverlay
   
.. doxygenclass:: bdsg::MutablePositionOverlay
   
.. doxygenclass:: bdsg::VectorizableOverlay
   
.. doxygenclass:: bdsg::PathVectorizableOverlay
   
.. doxygenclass:: bdsg::PathPositionVectorizableOverlay

Many of these are based on the :cpp:class:`handlegraph::ExpandingOverlayGraph` interface, which guarantees that the overlay does not remove any graph material, and allows handles form the backing graph and the overlay graph to be interconverted.

.. doxygenclass:: handlegraph::ExpandingOverlayGraph
   :members:
   :undoc-members:

~~~~~~~~~~~~~~~~~~~~~
Graph Overlay Helpers
~~~~~~~~~~~~~~~~~~~~~

From C++, some types are available to allow code to take an input of a more general type (say, a :cpp:class:`bdsg::HandleGraph`) and get a view of it as a more specific type (such as a :cpp:class:`bdsg::VectorizableHandleGraph`), using an overlay to bridge the gap if the backing graph implementation does not itself support the requested feature. For each pf these "overlay helpers", you instantiate the object (which allocates storage), use the ``apply()`` method to pass it a pointer to the backing graph and get a pointer to a graph of the requested type, and then use the ``get()`` method later if you need to get the requested-type graph pointer again. 

.. doxygentypedef:: bdsg::PathPositionOverlayHelper
   
.. doxygentypedef:: bdsg::RankedOverlayHelper
   
.. doxygentypedef:: bdsg::PathRankedOverlayHelper
   
.. doxygentypedef:: bdsg::VectorizableOverlayHelper
   
.. doxygentypedef:: bdsg::PathVectorizableOverlayHelper
   
.. doxygentypedef:: bdsg::PathPositionVectorizableOverlayHelper

All these overlay helpers are really instantiations of a couple of templates:

.. doxygenclass:: bdsg::OverlayHelper
   :members:
   :undoc-members:
   
.. doxygenclass:: bdsg::PairOverlayHelper
   :members:
   :undoc-members:


