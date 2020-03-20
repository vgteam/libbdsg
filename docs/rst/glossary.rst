
.. py:function:: create_handle(*args, **kwargs)
   :module: bdsg.handlegraph.MutableHandleGraph

Overloaded function.

1. create_handle(self: bdsg.handlegraph.MutableHandleGraph, sequence: str) -> bdsg.handlegraph.handle_t

Create a new node with the given sequence and return the handle.

C++: handlegraph::MutableHandleGraph::create_handle(const std::string &) --> struct handlegraph::handle_t

2. create_handle(self: bdsg.handlegraph.MutableHandleGraph, sequence: str, id: int) -> bdsg.handlegraph.handle_t

Create a new node with the given id and sequence, then return the handle.

C++: handlegraph::MutableHandleGraph::create_handle(const std::string &, const long long &) --> struct handlegraph::handle_t

