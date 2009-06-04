/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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
#include <curie/immutable.h>

struct graph *graph_create()
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph));

    struct graph *gr = (struct graph *) get_pool_mem(&pool);
    gr->nodes = (struct graph_node **)0;
    gr->node_count = 0;
    return gr;
}

struct graph_node *graph_add_node(struct graph *gr, sexpr label)
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_node));
    struct graph_node *node = (struct graph_node *) get_pool_mem(&pool);

    if (gr->node_count == 0) {
        gr->nodes = (struct graph_node **)aalloc(sizeof(struct graph_node *));
    } else {
        gr->nodes = (struct graph_node **)arealloc(sizeof(struct graph_node *) * gr->node_count, gr->nodes, sizeof(struct graph_node *) * (gr->node_count + 1));
    }

    node->edge_count = 0;
    node->edges = (struct graph_edge **)0;
    node->label = label;

    gr->nodes[gr->node_count] = node;
    gr->node_count++;
    return node;
}

void graph_destroy (struct graph *gr)
{
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

                afree (sizeof(struct graph_node *) * n->edge_count, n->edges);
            }

            free_pool_mem ((void *)n);
        }

        afree (sizeof(struct graph_node *) * gr->node_count, gr->nodes);
    }

    free_pool_mem ((void *)gr);
}

struct graph_node *graph_search_node(struct graph *gr, sexpr label)
{
    int i;

    for(i = 0; i < gr->node_count; i++) {
        if(truep(equalp(gr->nodes[i]->label, label)))
           return gr->nodes[i];
    }

    return (struct graph_node *)0;
}

struct graph_edge *graph_node_add_edge(struct graph_node *node, struct graph_node *target, sexpr label)
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_edge));
    struct graph_edge *edge = (struct graph_edge *) get_pool_mem(&pool);

    if (node->edge_count == 0) {
        node->edges = (struct graph_edge **)aalloc(sizeof(struct graph_edge *));
    } else {
        node->edges = (struct graph_edge **)arealloc(sizeof(struct graph_edge *) * node->edge_count, node->edges, sizeof(struct graph_edge *) * (node->edge_count + 1));
    }

    edge->target = target;
    edge->label = label;

    node->edges[node->edge_count] = edge;
    node->edge_count++;
    return edge;
}

struct graph_edge *graph_node_search_edge(struct graph_node *node, sexpr label)
{
    int i;

    for(i = 0; i < node->edge_count; i++) {
        if(truep(equalp(node->edges[i]->label, label)))
           return node->edges[i];
    }

    return (struct graph_edge *)0;
}

sexpr graph_to_sexpr (struct graph *g)
{
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

            sx_xref (sxx);

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

struct graph *sexpr_to_graph (sexpr sx)
{
    struct graph *g = graph_create ();
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
            sx_xref(cxcddr);
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
            sx_xref(cxcdr);
            n->label = cxcdr;
        }

        c = cdr (c);
    }

    return g;
}
