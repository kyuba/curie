/*
 *  graph.h
 *  libcurie
 * 
 *  Created by Nadja Klein on 29/09/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *  Copyright 2008 Nadja Klein. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*! \file
 *  \brief Graphs
 *
 *  The functions in this header file implement fairly generic digraphs with
 *  sexpr payloads.
 */

#ifndef LIBCURIE_GRAPH_H
#define LIBCURIE_GRAPH_H


#include <curie/int.h>
#include <curie/sexpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Digraph Root
 *
 *  This structure represents a digraph with an arbitrary number of nodes.
 */
struct graph {
    /*! \brief Node Count
     *
     *  The number of nodes currently in the graph.
     */
    int_32 node_count;

    /*! \brief Nodes
     *
     *  An array of nodes that the graph consists of.
     */
    struct graph_node **nodes;
};

/*! \brief Graph Node
 *
 *  A single node in the graph.
 */
struct graph_node {
    /*! \brief Node Label
     *
     *  This should be all the payload that is needed, since sexprs are able to
     *  represent just about anything.
     */
    sexpr label;

    /*! \brief Edge Count
     *
     *  The number of edges originating from this node. Remember, this is a
     *  digraph.
     */
    int_32 edge_count;

    /*! \brief Edges
     *
     *  An array of edges that originate from this node.
     */
    struct graph_edge **edges;
};

/*! \brief Graph Edge
 *
 *  An edge in the digraph. Kind of like a pointer with a label, really.
 */
struct graph_edge {
    /*! \brief Edge Label
     *
     *  The edge's label. Used in search operations, when a specific edge is
     *  needed.
     */
    sexpr label;

    /*! \brief Target Node
     *
     *  This is the node that the edge connects to.
     */
    struct graph_node *target;
};

/*! \brief Create a new Graph
 *  \return The new graph, or (struct graph *)0 when there's no memory
 *          available.
 *
 *  This function initialises a new graph without any nodes.
 */
struct graph * graph_create();

/*! \brief Destroy a Graph
 *  \param[in] g The graph to destroy.
 *
 *  Deallocates any memory associated with the graph. After calling this
 *  function, any references to the given graph or any of its nodes, edges or
 *  labels may provoke a segmentation violation.
 */
void graph_destroy (struct graph *g);

/*! \brief Create Graph Node
 *  \param[in] g     The graph to add the node to.
 *  \param[in] label The label to give the new node.
 *  \return The new node, or (struct graph_node *)0 when no memory is available.
 *
 *  This creates a new graph node, adds it to the given graph and then assigns
 *  the given label to the node.
 */
struct graph_node *graph_add_node (struct graph *g, sexpr label);

/*! \brief Search for Graph Node
 *  \param[in] g     The graph to search in.
 *  \param[in] label The label to search for.
 *  \return The node, or (struct graph_node *)0 if there's no node with the 
 *          label in that graph.
 *
 *  This function searches for the given label in the graph, and returns a node
 *  with that label, if one is found.
 */
struct graph_node *graph_search_node (struct graph *g, sexpr label);

/*! \brief Create Graph Edge
 *  \param[in] node   The originating node for the edge.
 *  \param[in] target The target node for the edge.
 *  \param[in] label  The label for the edge.
 *  \return The new edge, or (struct graph_edge *)0 when no memory is available.
 *
 *  Analoguous to graph_add_node, but for edges: Creates a new graph edge from
 *  node to target with the given label.
 */
struct graph_edge *graph_node_add_edge
        (struct graph_node *node, struct graph_node *target, sexpr label);

/*! \brief Search for Graph Edge
 *  \param[in] node  The originating node to search from.
 *  \param[in] label The label to search for.
 *  \return The edge, or (struct graph_node *)0 if there's no node with the
 *          label in that graph.
 *
 *  Analoguous to graph_search_node, but for edges: Search the given node for
 *  an edge with a particular label, and return it if found.
 */
struct graph_edge *graph_node_search_edge
        (struct graph_node *node, sexpr label);

/*! \brief Turn Graph into an S-Expression
 *  \param[in] graph The graph to convert.
 *  \return S-expression representation of the graph.
 *
 *  This function turns a graph into an s-expression suitable for storage and
 *  later reconstruction. Should be useful for caching, exports and the like.
 */
sexpr graph_to_sexpr (struct graph *graph);

/*! \brief Reconstruct a Graph from its S-Expression Representation
 *  \param[in] graph The graph to convert.
 *  \return The reconstructed graph.
 *
 *  This is the inverse of graph_to_sexpr().
 */
struct graph *sexpr_to_graph (sexpr graph);

#ifdef __cplusplus
}
#endif

#endif
