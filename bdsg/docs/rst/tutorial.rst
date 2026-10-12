#########
Tutorial
#########

.. testsetup::

   # Need to be in the directory with the test data
   import os
   os.chdir('exdata')

*************
Preliminaries
*************
See the `vg wiki <https://github.com/vgteam/vg/wiki#understanding-our-data-formats>`_ for an explanation of our variation graph data model.

Graph Objects
=============
`libbdsg` implements the `handlegraph <https://github.com/vgteam/libhandlegraph>`_ interface in which all objects in the graph are accessed using `handles <https://github.com/vgteam/libbdsg/blob/master/bdsg/docs/handlegraph.md>`_. 
*Edges* and *nodes* are accessed through an implementation of the :class:`bdsg.handlegraph.HandleGraph` interface. Individual nodes in the graph are pointed at by :class:`bdsg.handlegraph.handle_t` objects.

Paths exist in graphs that implement the :class:`bdsg.handlegraph.PathHandleGraph`. Paths are accessed through :class:`bdsg.handlegraph.path_handle_t`, which is a series of :class:`bdsg.handlegraph.step_handle_t` linked together. Each :class:`bdsg.handlegraph.step_handle_t` points to the node in that step, and also contains directional information regarding the nodes preceeding and following it.

Handles are pointers to specific pieces of the graph, and it is not possible to operate on them directly, aside from comparing whether the objects are equal. To get information regarding the object that each handle is pointing to, it is necessary to use the corresponding `get` accessor method on the graph that issued the handle.

Reference materials for these methods can be found at the :ref:`api`, as well as the :ref:`glossary`, which contains lists sorted by object type for :ref:`accessor`, :ref:`mutator`, and :ref:`iterator`.


****************
Creating Graphs
****************
Let's say that you wanted to create the following graph:

.. image:: /img/exampleGraph.png

This graph is a combination of nodes (labelled as `n1`, `n2`, ..., `n10`) and bidirected edges.

Making a Graph
===============
First, we must create the graph, then make each node and keep track of their handles. We're going to be using the **HashGraph**, :class:`bdsg.bdsg.HashGraph`, which is a good all-around graph that implements :class:`bdsg.handlegraph.MutablePathDeletableHandleGraph`.

.. testcode::

    from bdsg.bdsg import HashGraph
    gr = HashGraph()
    seq = ["CGA", "TTGG", "CCGT", "C", "GT", "GATAA", "CGG", "ACA", "GCCG", "ATATAAC"]
    n = ["node0"] #Add a placeholder so that the indices are the same as the node ids
    for s in seq:
        n.append(gr.create_handle(s))

Now we link together these nodes using their handles. Note that each of these handles is directional, and we create each edge from the first handle to the second. In order to create both of the edges between `n7` and `n9` we use ``create_edge`` twice, once going from `n7` to `n9`, and once going from `n9` to `n7`.
Since the handles are all traversing nodes in the forward orientation by default, this creates the edge from the right side of `n7` to the left side of `n9`, then from the right side of `n9` to the left side of `n7`.

.. testcode::

    gr.create_edge(n[1], n[2])
    gr.create_edge(n[2], n[3])
    gr.create_edge(n[3], n[4])
    gr.create_edge(n[3], n[5])
    gr.create_edge(n[4], n[6])
    gr.create_edge(n[6], n[7])
    # Connect the end of n7 to the start of n9
    gr.create_edge(n[7], n[9])
    gr.create_edge(n[7], n[8])
    gr.create_edge(n[7], n[9])
    gr.create_edge(n[8], n[10])
    gr.create_edge(n[9], n[10])
    # Connect the end of n9 back around to the start of n7
    gr.create_edge(n[9], n[7])

Traversing Edges
================
If we wanted to traverse these edges, we could do it using the iterator method :func:`bdsg.handlegraph.HandleGraph.follow_edges`.

.. testcode::

    def next_node_list(handle):
        lis = []
        gr.follow_edges(handle, False, lambda y: lis.append(y))
        return lis

    print(f'n1: {gr.get_sequence(n[1])}')
    next_node = next_node_list(n[1])[0]
    print(f'n2: {gr.get_sequence(next_node)}')
    next_node = next_node_list(next_node)[0]
    print(f'n2: {gr.get_sequence(next_node)}')

Which will output the following:

.. testoutput::
        
    n1: CGA
    n2: TTGG
    nk: CCGT

Creating a Path
===============

Generating a linear sequence from this graph could be done in infinitely many ways, due to the internal loop between `n7`, and `n9`. If we wanted to define a single consensus sequence, we would do this by defining a path.

.. image:: /img/exampleGraphPath.png

To create the highlighted path, we would need to create a :class:`bdsg.handlegraph.path_handle_t` in the graph, and then append each :class:`bdsg.handlegraph.handle_t` to the end of the path.

.. testcode::

    path = gr.create_path_handle("path")
    gr.append_step(path, n[1])
    gr.append_step(path, n[2])
    gr.append_step(path, n[3])
    gr.append_step(path, n[5])
    gr.append_step(path, n[6])
    gr.append_step(path, n[7])
    gr.append_step(path, n[8])
    gr.append_step(path, n[10])

.. warning::

    :func:`bdsg.handlegraph.MutablePathHandleGraph.append_step` will not stop you from appending nodes that are not connected to the preceeding node.

.. testcode::
        
    # the following code runs without error
    badpath = gr.create_path_handle("badpath")
    gr.append_step(badpath, n[1])
    gr.append_step(badpath, n[4])

Traversing a path
=================

To traverse a path, we need to fetch a series of :class:`bdsg.handlegraph.step_handle_t` from the graph. Note that although we are effectively asking the path for these items in it, all accessor methods are a part of the :class:`bdsg.handlegraph.PathHandleGraph` object.

.. testcode::

    step = gr.path_begin(path)
    while(gr.has_next_step(step)):
        # get the node handle from the step handle
        current_node_handle = gr.get_handle_of_step(step)
        # ask the node handle for the sequence
        print(gr.get_sequence(current_node_handle))
        # progress to the next step
        step = gr.get_next_step(step)
    current_node_handle = gr.get_handle_of_step(step)
    print(gr.get_sequence(current_node_handle))

Which will output the following:

.. testoutput::
        
    CGA
    TTGG
    CCGT
    GT
    GATAA
    CGG
    ACA
    ATATAAC

********************
Using Graph Overlays
********************

The libhandlegraph library defines generic graph interfaces that describe different functionalities of a graph.
Overlays are wrappers on top of a graph that add these functionalities.
For example, the :class:`bdsg.handlegraph.PathPositionHandleGraph` defines an interface for accessing the sequence positions along paths in a graph.
The :class:`bdsg.bdsg.PositionOverlay` and :class:`bdsg.bdsg.ReferencePathOverlay` are two different implementations of overlays that can be applied to a graph to add this functionality.
In C++, :class:`bdsg.bdsg.OverlayHelpers` are used to add specific combinations of overlays to graphs.

bdsg.bdsg.PositionOverlay
=========================

A :class:`bdsg.bdsg.PositionOverlay` adds the :class:`bdsg.handlegraph.PathPositionHandleGraph` functionality to a graph, allowing it to find the sequence positions along paths.
For example, using the same example graph as above, say we want to find the offset along the path of node `n5`. 
We do this by creating a `bdsg.bdsg.PositionOverlay`, then getting a `bdsg.handlegraph.step_handle_t` that points to the node `n5` on the path `path`.
The `bdsg.bdsg.PositionOverlay` can be queried using :func:`bdsg.handlegraph.get_position_of_step` to find the position of the given step.

.. testcode::
    from bdsg.bdsg import PositionOverlay

    # Get the overlay
    position_graph = PositionOverlay(gr)

    # Get the step_handle_t to node n5 on path. Since there is only one path in the graph there will only be one step
    n5_steps = [] 
    gr.for_each_step_on_handle(nodes[5], lambda y : n5_steps.append(y) or True)

    # Get the path offset of the step
    print(position_graph.get_position_of_step(n5_steps[0]))

.. testoutput::

    11

**************************
Using a SnarlDistanceIndex
**************************

A :class:`bdsg.bdsg.SnarlDistanceIndex` implements the :class:`bdsg.handlegraph.SnarlDecomposition` interface and additionally adds minimum distance finding functionality.
It can be used to explore the snarl decomposition of a graph.
Similar to a :class:`bdsg.handlegraph.handle_t` used to point to a node in a graph, a :class:`bdsg.handlegraph.net_handle_t` is used to point to a node in the snarl tree, which represents a snarl, chain, or node in the graph. 
The snarl decomposition is described in more detail in the `vg wiki <https://github.com/vgteam/vg/wiki/Snarls-and-chains>`_. 

libbdsg does not support building a distance index, it must be build using the `vg toolkit <https://github.com/vgteam/vg>`_.

.. code-block:: bash

    vg index -j example_graph.dist example_graph.hg



Traversing the Distance Index
=============================
We will use the following graph and corresponding distance index for this example.

.. image:: /assests/simple_nested_chain.png

Start by loading the distance index.
.. testcode::
    from bdsg.bdsg import SnarlDistanceIndex

    distance_index = SnarlDistanceIndex()
    distance_index.deserialize("/assets/simple_nested_chain.dist")

    graph = bdsg.bdsg.HashGraph()
    graph.deserialize("/assets/simple_nested_chain.hg")

Starting from a node in the graph, its ancestors in the snarl decomposition can be found using :func:`bdsg.handlegraph.SnarlDecomposition.get_parent`.
Lets look for the ancestors of node `n6` in the snarl decomposition.

.. testcode::

    net = distance_index.get_node_net_handle(6)
    while not distance_index.is_root(net):
        print(distance_index.net_handle_as_string(net))
        net = distance_index.get_parent(net) 

In the snarl decomposition, a node is always the child of a chain, which may contain only that node.
Node `n6` therefore has a parent that is a chain containing only node `n6`.

.. testoutput::
    node 6fd
    node 6fd pretending to be a chain in a simple snarl
    simple snarl 5fd->7fdtraversing start->end
    chain 5fd->7fdtraversing start->end
    snarl 4fd->8fdtraversing start->end
    chain 1fd->10fdtraversing start->end

Now lets go in the other direction, finding the descendants of a net, starting from the root.

.. testcode::
    root = distance_index.get_root()

    children = []
    def get_and_print_child(net):
        nonlocal children
        print(distance_index.net_handle_as_string(net))
        children.append(net)
        return True
    distance_index.for_each_child(root, get_and_print_child)

    chain = children[0]
    distance_index.for_each_child(chain, get_and_print_child)

The root contains only one child, the top-level chain.
The top-level chain contains node 1, snarl 1-4, node 4, etc.

.. testoutput::
  chain 1fd->10fdtraversing start->end
  True
  node 1fd
  simple snarl 1fd->4fdtraversing start->end
  node 4fd
  snarl 4fd->8fdtraversing start->end
  node 8fd
  simple snarl 8fd->10fdtraversing start->end
  node 10fd
  True





*************************
Saving and Loading Graphs
*************************

Graphs that implement :class:`bdsg.handlegraph.SerializableHandleGraph`, such as :class:`bdsg.bdsg.HashGraph`, can be saved and loaded through the :func:`bdsg.handlegraph.SerializableHandleGraph.serialize` and :func:`bdsg.handlegraph.SerializableHandleGraph.deserialize` methods. 

Graph File Example
==================

If you wish to save the graph from the above session, that can be done with:

.. testcode::

    gr.serialize("example_graph.hg")

This can be loaded into a new python session by using:

.. testcode::
        
    from bdsg.bdsg import HashGraph
    gr = HashGraph()
    gr.deserialize("example_graph.hg")

Loading in Pre-Existing Data
============================

Each graph implementation knows how to read files in its respective file format.

For example, provided that data has been serialized in PackedGraph format, it is possible to read it directly from a file with :class:`bdsg.bdsg.PackedGraph`. Download :download:`this graph <../exdata/cactus-brca2.pg>` and load it into python with:

.. testcode::
        
    from bdsg.bdsg import PackedGraph
    brca2 = PackedGraph()
    brca2.deserialize("cactus-brca2.pg")

We can poke around this data and get the sequence of the path with:

.. testcode::

    path_handle = [] 
    handles = []
    brca2.for_each_path_handle(lambda y: path_handle.append(y) or True)
    brca2.for_each_step_in_path(path_handle[0], 
        lambda y: handles.append(brca2.get_handle_of_step(y)) or True)
    sequence = ""
    for handle in handles:
        sequence += brca2.get_sequence(handle)
    print(sequence[0:10])
    print(len(sequence))
    
.. testoutput::
    :hide:
    
    TGTGGCGCGA
    84159
        
Note how we are using ``or True`` in the iteratee callback lambda functions to make sure they return ``True``. If a callback returns ``False`` or ``None`` (which is what is returned when you don't return anything), iteration will stop early and the ``for_each`` call will return ``False``.

Reading in a Graph from vg
==========================

Graph assembies can be created with `vg <https://github.com/vgteam/vg>`_. Many ``.vg`` files that vg 1.28.0 or newer produces will be in HashGraph format, directly loadable by :func:`bdsg.bdsg.HashGraph.deserialize`.
To check a file, you can use ``vg stats --format``, like so:

.. code-block:: bash

    vg stats --format graph.vg
    
If you see one of ``format: HashGraph`` or ``format: PackedGraph``, you can probably (but not always; see the note on encapsulation_) load the graph with :class:`bdsg.bdsg.HashGraph` or :class:`bdsg.bdsg.PackedGraph`, respectively.

However, in some circumstances, you will need to convert the graph to one of those formats. Some graphs (most notably, graphs from ``vg construct``) will report ``format: VG-Protobuf``, and ``.xg`` files will report ``format: XG``. These graphs will need to be converted to HashGraph or PackedGraph format, and then loaded with the appropriate class. For example, you can do this:

.. code-block:: bash

    vg convert --packed-out graph.vg > graph.pg
    
The resulting PackedGraph file can be loaded with :func:`bdsg.bdsg.PackedGraph.deserialize`.

.. testcode::
        
    from bdsg.bdsg import PackedGraph
    graph = PackedGraph()
    graph.deserialize("graph.pg")

To use :class:`bdsg.bdsg.HashGraph` instead, substitute ``--hash-out`` for ``--packed-out``.

.. _encapsulation:

Older vg Graphs with Encapsulation
==================================

Versions of vg before 1.28.0 would encapsulate HashGraph and PackedGraph graphs in a file format that vg can read but libbdsg cannot. Consequently, some older graph files will be reported as ``format: HashGraph`` or ``format: PackedGraph`` by ``vg stats --format``, but will still not be readable using libbdsg.

If you encounter one of these files, you can use ``vg view --extract-tag`` to remove the encapsulation and pull out the internal file which libbdsg can understand. For example, for a file that reports ``format: PackedGraph`` but is not loadable by libbdsg, you can do:

.. code-block:: bash

    vg view graph.vg --extract-tag PackedGraph > graph.pg

This also works for ``HashGraph`` files, by replacing ``PackedGraph`` with ``HashGraph``.

Running the file through ``vg convert`` with vg 1.28.0 or newer will also solve the problem, but could take longer.
