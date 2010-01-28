/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

/*! \file
 *  \brief Graphs
 *
 *  The functions in this header file implement fairly generic digraphs with
 *  sexpr payloads.
 */

#ifndef LIBCURIE_GRAPH_H
#define LIBCURIE_GRAPH_H

#include <curie/sexpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Type Identifier (Graphs)
 *
 *  S-Expression identifier for graphs. (The identifier is the unicode code of
 *  the greek lower case letter gamma.)
 */
#define graph_type_identifier 0x03b3

/*! \brief Check if the S-Expression is a Graph Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an expression of a graph, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define graphp(sx) sx_customp(sx,graph_type_identifier)

/*! \brief Digraph Root
 *
 *  This structure represents a digraph with an arbitrary number of nodes.
 */
struct graph {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all (pointer-style) sexprs.
     */
    unsigned int type;

    /*! \brief Callback on Graph Destruction
     *
     *  This callback is invoked whenever the graph's memory is about to be
     *  deallocated. Since graphs are pretty much the only mutable data type we
     *  have here, this does make sense since such a callback allows for
     *  caching the graph based on the data it contains.
     */
    void (*on_free) (struct graph *);

    /*! \brief Auxiliary Data
     *
     *  Useful for the on_free() callback.
     */
    void *aux;

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

/*! \brief Initialise the Graph Type
 *
 *  Use this function before using the graph functions. Actually you might even
 *  get away without using this function if you don't intend to read or write
 *  any of the graphs you use, but it really would be better to just call this.
 */
void graph_initialise ();

/*! \brief Create a new Graph
 *  \return The new graph.
 *
 *  This function initialises a new graph without any nodes.
 */
sexpr graph_create ( void );

/*! \brief Create Graph Node
 *  \param[in] g     The graph to add the node to.
 *  \param[in] label The label to give the new node.
 *  \return The new node.
 *
 *  This creates a new graph node, adds it to the given graph and then assigns
 *  the given label to the node.
 */
struct graph_node *graph_add_node (sexpr g, sexpr label);

/*! \brief Search for Graph Node
 *  \param[in] g     The graph to search in.
 *  \param[in] label The label to search for.
 *  \return The node, or (struct graph_node *)0 if there's no node with the 
 *          label in that graph.
 *
 *  This function searches for the given label in the graph, and returns a node
 *  with that label, if one is found.
 */
struct graph_node *graph_search_node (sexpr g, sexpr label);

/*! \brief Create Graph Edge
 *  \param[in] node   The originating node for the edge.
 *  \param[in] target The target node for the edge.
 *  \param[in] label  The label for the edge.
 *  \return The new edge.
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

#ifdef __cplusplus
}
#endif

#endif
