.. _api:

##########
Python API
##########

================
Handle Graph API
================

The `bdsg.handlegraph` module defines the handle graph interface.

.. automodule:: bdsg.handlegraph
   :show-inheritance:
   :members: handle_t, path_handle_t, step_handle_t, HandleGraph, MutableHandleGraph, PathHandleGraph, MutablePathHandleGraph, MutablePathMutableHandleGraph, DeletableHandleGraph, MutablePathDeletableHandleGraph
   
====================================
libbdsg Handle Graph Implementations
====================================

The `bdsg.bdsg` module provides useful implementations of the Handle Graph API: :class:`bdsg.bsdg.PackedGraph`, :class:`bdsg.bsdg.HashGraph`, and :class:`bdsg.bsdg.ODGI`.

..
   TODO: Why can't we see bdsg.bdsg classes with automodule?
   We don't want all the global functions from the module anyway so just do each class.

.. autoclass:: bdsg.bdsg.PackedGraph
   :show-inheritance:
   :members:
   :inherited-members:

.. autoclass:: bdsg.bdsg.HashGraph
   :show-inheritance:
   :members:
   :inherited-members:
   
.. autoclass:: bdsg.bdsg.ODGI
   :show-inheritance:
   :members:
   :inherited-members:
   
..
   TODO: Document overlays
   

