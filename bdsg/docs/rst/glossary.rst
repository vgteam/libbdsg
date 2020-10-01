.. _glossary:

##########################
Sorted Glossary of Methods
##########################

This page divides the Handle Graph API methods by category. It is written in terms of the Python methods, but applies equally well to the C++ interface.

.. _mutator:

===============
Mutator Methods
===============

The following lists breaks out methods from the various handle graph interfaces by what types of objects they modify.

-------------
Node Mutators
-------------

:class:`bdsg.handlegraph.MutableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.MutableHandleGraph.create_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.divide_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.apply_orientation
   :noindex:
   
:class:`bdsg.handlegraph.DeletableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.DeletableHandleGraph.destroy_handle
   :noindex:
   
-------------
Edge Mutators
-------------
   
:class:`bdsg.handlegraph.MutableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.create_edge
   :noindex:
   
:class:`bdsg.handlegraph.DeletableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. autofunction:: bdsg.handlegraph.DeletableHandleGraph.destroy_edge
   :noindex:
   
-------------
Path Mutators
-------------
   
:class:`bdsg.handlegraph.MutablePathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.create_path_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.set_circularity
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.destroy_path
   :noindex:
   
------------------
Path Step Mutators
------------------

:class:`bdsg.handlegraph.MutablePathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.append_step
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.prepend_step
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.rewrite_segment
   :noindex:
   
--------------
Graph Mutators
--------------

:class:`bdsg.handlegraph.MutableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.MutableHandleGraph.optimize
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.apply_ordering
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.set_id_increment
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.increment_node_ids
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.reassign_node_ids
   :noindex:

:class:`bdsg.handlegraph.DeletableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.DeletableHandleGraph.clear
   :noindex:
   
:class:`bdsg.handlegraph.SerializableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. autofunction:: bdsg.handlegraph.SerializableHandleGraph.deserialize
   :noindex:
   
.. _accessor:

================
Accessor Methods
================

The following lists breaks out methods from the various handle graph interfaces by what types of objects they return information about.

--------------
Node Accessors
--------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.HandleGraph.get_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_id
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_is_reverse
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_length
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_sequence
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_subsequence
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_base
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_degree
   :noindex:
   
:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_step_count
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.steps_of_handle
   :noindex:
   
:class:`bdsg.handlegraph.VectorizableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.VectorizableHandleGraph.node_vector_offset
   :noindex:
   
.. autofunction:: bdsg.handlegraph.VectorizableHandleGraph.node_at_vector_offset
   :noindex:
   
---------------------
Node Handle Accessors
---------------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. autofunction:: bdsg.handlegraph.HandleGraph.flip
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.forward
   :noindex:
   
:class:`bdsg.handlegraph.ExpandingOverlayGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.ExpandingOverlayGraph.get_underlying_handle
   :noindex:
   
--------------
Edge Accessors
--------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.HandleGraph.edge_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.traverse_edge_handle
   :noindex:
   
:class:`bdsg.handlegraph.VectorizableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.VectorizableHandleGraph.edge_index
   :noindex:
   
--------------
Path Accessors
--------------

:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_path_handle
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_path_name
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_is_circular
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_step_count
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.is_empty
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.path_begin
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.path_end
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.path_back
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.path_front_end
   :noindex:

:class:`bdsg.handlegraph.PathPositionHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathPositionHandleGraph.get_path_length
   :noindex:

-------------------
Path Step Accessors
-------------------

:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_path_handle_of_step
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_handle_of_step
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.has_next_step
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_next_step
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.has_previous_step
   :noindex:

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_previous_step
   :noindex:
   
:class:`bdsg.handlegraph.PathPositionHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathPositionHandleGraph.get_position_of_step
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathPositionHandleGraph.get_step_at_position
   :noindex:
   
---------------
Graph Accessors
---------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.HandleGraph.has_node
   :noindex:

.. autofunction:: bdsg.handlegraph.HandleGraph.has_edge
   :noindex:

.. autofunction:: bdsg.handlegraph.HandleGraph.get_node_count
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_edge_count
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.get_total_length
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.min_node_id
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.max_node_id
   :noindex:
   
:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathHandleGraph.get_path_count
   :noindex:
   
:class:`bdsg.handlegraph.SerializableHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
.. autofunction:: bdsg.handlegraph.SerializableHandleGraph.serialize
   :noindex:
   
.. autofunction:: bdsg.handlegraph.SerializableHandleGraph.get_magic_number
   :noindex:

.. _iterator:
  
==================
Iteratator Methods
==================

The following lists breaks out methods from the various handle graph interfaces by what types of objects they iterate over. Note that iteration is **callback-based** and not via traditional Python iterator semantics. Iteratee functions should return ``False`` to stop iteration, and must return ``True`` to continue. Not returning anything (i.e. returning ``None``) will stop iteration early.

--------------
Node Iterators
--------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.HandleGraph.for_each_handle
   :noindex:

--------------
Edge Iterators
--------------

:class:`bdsg.handlegraph.HandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.HandleGraph.follow_edges
   :noindex:
   
.. autofunction:: bdsg.handlegraph.HandleGraph.for_each_edge
   :noindex:
   
--------------
Path Iterators
--------------

:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. TODO: This docstring doesn't come through, probably because there's several 
   places it could be looking (template declaration, template definition,
   template instantiation).

.. autofunction:: bdsg.handlegraph.PathHandleGraph.for_each_path_handle
   :noindex:

-------------------
Path Step Iterators
-------------------

:class:`bdsg.handlegraph.PathHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathHandleGraph.for_each_step_in_path
   :noindex:
   
.. autofunction:: bdsg.handlegraph.PathHandleGraph.for_each_step_on_handle
   :noindex:
   
:class:`bdsg.handlegraph.PathPositionHandleGraph`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: bdsg.handlegraph.PathPositionHandleGraph.for_each_step_position_on_handle
   :noindex:




