.. _glossary:

##########################
Sorted Glossary of Methods
##########################

.. _mutator:

===============
Mutator Methods
===============

The following lists sorts methods in :class:odgi.graph by what types of objects they modify.

:class:`bdsg.handlegraph.MutableHandleGraph`
--------------------------------------------

.. autofunction:: bdsg.handlegraph.MutableHandleGraph.create_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutableHandleGraph.create_edge
   :noindex:
   
   
:class:`bdsg.handlegraph.DeletableHandleGraph`
----------------------------------------------

.. autofunction:: bdsg.handlegraph.DeletableHandleGraph.destroy_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.DeletableHandleGraph.destroy_edge
   :noindex:
   
:class:`bdsg.handlegraph.MutablePathHandleGraph`
------------------------------------------------

.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.create_path_handle
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.append_step
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.prepend_step
   :noindex:
   
..
   TODO: This isn't being bound because of the vector argument.
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.rewrite_segment
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.set_circularity
   :noindex:
   
.. autofunction:: bdsg.handlegraph.MutablePathHandleGraph.destroy_path
   :noindex:
   
.. _accessor:

================
Accessor Methods
================

The following list sorts methods in :class:odgi.graph by what object they return information about.

.. _iterator:
  
==================
Iteratator Methods
==================

The following list sorts methods in :class:odgi.graph by what kind of iteratee they operate on. 


