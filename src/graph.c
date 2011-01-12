/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

#include <curie/graph.h>
#include <curie/memory.h>
#include <curie/gc.h>

/* TODO: the sexpr graph type needs to get turned into a proper, immutable
 *       type, so that creating the same graph multiple times  has the
 *       usual benefits w.r.t. storage requirements. */

static void  graph_destroy  (sexpr);
static sexpr graph_to_sexpr (sexpr);
static sexpr sexpr_to_graph (sexpr);
static char initialised = 0;

#define get_chunked_node_size(n)\
    ((((n) & ~(GRAPH_NODE_CHUNK_SIZE-1)) + 1)\
        * GRAPH_NODE_CHUNK_SIZE * sizeof(struct graph_node *))

#define get_chunked_edge_size(n)\
    ((((n) & ~(GRAPH_EDGE_CHUNK_SIZE-1)) + 1)\
        * GRAPH_EDGE_CHUNK_SIZE * sizeof(struct graph_edge *))

void graph_initialise ()
{
    if (!initialised)
    {
        sx_register_type
                (graph_type_identifier, graph_to_sexpr, sexpr_to_graph,
                 (void *)0, graph_destroy, (void *)0, (void *)0);

        initialised = 1;
    }
}

sexpr graph_create()
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph));
    struct graph *gr;

    if (!initialised)
    {
        graph_initialise ();
    }

    gr = (struct graph *) get_pool_mem(&pool);
    gr->type = graph_type_identifier;
    gr->node_count = 0;
    gr->nodes = (struct graph_node **)aalloc (get_chunked_node_size(0));
    gr->on_free = (void*)0;

    gc_base_items++;

    return (sexpr)gr;
}

struct graph_node *graph_add_node(sexpr sx, sexpr label)
{
    struct graph *gr = (struct graph *)sx_pointer(sx);
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_node));
    struct graph_node *node = (struct graph_node *) get_pool_mem(&pool);
    unsigned int size_before = get_chunked_node_size (gr->node_count),
                 size_after  = get_chunked_node_size (gr->node_count + 1);


    if (size_before != size_after)
    {
        gr->nodes = (struct graph_node **) arealloc
                (size_before, gr->nodes, size_after);
    }

    node->edge_count = 0;
    node->edges = (struct graph_edge **)aalloc (get_chunked_edge_size(0));
    node->label = label;

    gr->nodes[gr->node_count] = node;
    gr->node_count++;
    return node;
}

static void graph_destroy (sexpr sx)
{
    struct graph *gr = (struct graph *)sx_pointer(sx);

    if (gr->on_free != (void *)0)
    {
        gr->on_free (gr);
    }

    if (gr->node_count != 0)
    {
        unsigned int i;

        for (i = 0; i < gr->node_count; i++)
        {
            struct graph_node *n = gr->nodes[i];

            if (n->edge_count != 0)
            {
                unsigned int j;

                for (j = 0; j < n->edge_count; j++)
                {
                    struct graph_edge *e = n->edges[j];

                    free_pool_mem ((void *)e);
                }
            }

            afree (get_chunked_edge_size(n->edge_count), n->edges);

            free_pool_mem ((void *)n);
        }
    }

    afree (get_chunked_node_size(gr->node_count), gr->nodes);

    free_pool_mem ((void *)gr);

    gc_base_items--;
}

struct graph_node *graph_search_node (sexpr sx, sexpr label)
{
    struct graph *gr = (struct graph *)sx_pointer(sx);
    int i;

    for(i = 0; i < gr->node_count; i++) {
        if(truep(equalp(gr->nodes[i]->label, label)))
           return gr->nodes[i];
    }

    return (struct graph_node *)0;
}

struct graph_edge *graph_node_add_edge (struct graph_node *node, struct graph_node *target, sexpr label)
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_edge));
    struct graph_edge *edge = (struct graph_edge *) get_pool_mem(&pool);
    unsigned int size_before = get_chunked_edge_size (node->edge_count),
                 size_after  = get_chunked_edge_size (node->edge_count + 1);

    if (size_before != size_after)
    {
        node->edges = (struct graph_edge **) arealloc
                (size_before, node->edges, size_after);
    }

    edge->target = target;
    edge->label = label;

    node->edges[node->edge_count] = edge;
    node->edge_count++;
    return edge;
}

struct graph_edge *graph_node_search_edge (struct graph_node *node, sexpr label)
{
    int i;

    for(i = 0; i < node->edge_count; i++) {
        if(truep(equalp(node->edges[i]->label, label)))
           return node->edges[i];
    }

    return (struct graph_edge *)0;
}

static sexpr graph_to_sexpr (sexpr gsx)
{
    struct graph *g = (struct graph *)sx_pointer(gsx);
    sexpr sx = sx_false;

    if (g != (struct graph *)0)
    {
        sexpr nodes = sx_end_of_list;
        sexpr edges = sx_end_of_list;
        unsigned int i;

        for (i = 0; i < g->node_count; i++)
        {
            struct graph_node *n = g->nodes[i];
            sexpr sxx = n->label;
            sexpr sxn = make_integer(i);
            unsigned int j;

            nodes = cons (cons (sxn, sxx), nodes);

            for (j = 0; j < n->edge_count; j++)
            {
                struct graph_edge *e = n->edges[j];
                unsigned int k;

                for (k = 0; k < g->node_count; k++)
                {
                    if (g->nodes[k] == e->target)
                    {
                        edges = cons (cons (sxn,
                                            cons (make_integer(k),
                                                  e->label)),
                                      edges);
                    }
                }
            }
        }

        sx = cons (nodes, edges);
    }

    return sx;
}

static sexpr sexpr_to_graph (sexpr sx)
{
    sexpr g = graph_create ();
    sexpr c = car(sx);

    while (consp(c))
    {
        sexpr cx    = car (c);
        sexpr cxcar = car (cx);

        graph_add_node (g, cxcar);

        c = cdr (c);
    }

    c = cdr(sx);

    while (consp(c))
    {
        sexpr cx     = car (c);
        sexpr cxcar  = car (cx);
        sexpr cxcdr  = cdr (cx);
        sexpr cxcadr = car (cxcdr);
        sexpr cxcddr = cdr (cxcdr);

        struct graph_node *ns = graph_search_node (g, cxcar);
        struct graph_node *nt = graph_search_node (g, cxcadr);

        if ((ns != (struct graph_node *)0) && (nt != (struct graph_node *)0))
        {
            graph_node_add_edge (ns, nt, cxcddr);
        }

        c = cdr (c);
    }

    c = car(sx);

    while (consp(c))
    {
        sexpr cx    = car (c);
        sexpr cxcar = car (cx);
        sexpr cxcdr = cdr (cx);

        struct graph_node *n = graph_search_node (g, cxcar);

        if (n != (struct graph_node *)0)
        {
            n->label = cxcdr;
        }

        c = cdr (c);
    }

    return g;
}
