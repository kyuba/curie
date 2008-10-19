/*
 *  tree.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 01/06/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
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

#include <curie/graph.h>
#include <curie/memory.h>
#include <curie/immutable.h>
#include <curie/sexpr.h>

struct graph * graph_create() {
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph));

    struct graph *gr = (struct graph *) get_pool_mem(&pool);
    gr->nodes = (struct graph_node **)0;
    gr->node_count = 0;
    return gr;
}

struct graph_node * graph_add_node(struct graph * gr, sexpr label) {
    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_node));
    struct graph_node *node = (struct graph_node *) get_pool_mem(&pool);

    if (gr->node_count > 0)
    {
      gr->nodes = (struct graph_node **) arealloc(sizeof(struct graph_node *) * gr->node_count, gr->nodes, sizeof(struct graph_node *) * (gr->node_count + 1));
    }
    else
    {
      gr->nodes = (struct graph_node **) aalloc(sizeof(struct graph_node *));
    }
    
    node->edge_count = 0;
    node->edges = (struct graph_edge **)0;
    node->label = label;
    
    gr->nodes[gr->node_count] = node;
    gr->node_count++;
    return node;
}


void graph_destroy (struct graph * gr) {
    afree(sizeof(struct graph_node *) * gr->node_count, gr->nodes);   
}


struct graph_node * graph_search_node(struct graph *gr, sexpr label) {
    for(int i = 0; i < gr->node_count; i++) {
        if(truep(equalp(gr->nodes[i]->label, label)))
           return gr->nodes[i];
    }

    return (struct graph_node *)0;
}

struct graph_edge *graph_node_add_edge(struct graph_node *node, struct graph_node *target, sexpr label) {

    static struct memory_pool pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_edge));
    struct graph_edge *edge = (struct graph_edge *) get_pool_mem(&pool);

    if(node->edge_count > 0) {
         node->edges = (struct graph_edge **) arealloc(sizeof(struct graph_edge *) * node->edge_count, node->edges, sizeof(struct graph_edge *) * (node->edge_count + 1));   
    }
    else {
        node->edges = (struct graph_edge **)aalloc(sizeof(struct graph_edge *));
    }
    
    edge->target = target;
    edge->label = label;   
    
    node->edges[node->edge_count] = edge;
    node->edge_count++;
    return edge;
}

struct graph_edge * graph_node_search_edge(struct graph_node *node, sexpr label) {
    for(int i = 0; i < node->edge_count; i++) {
        if(truep(equalp(node->edges[i]->label, label)))
           return node->edges[i];
    }
    return (struct graph_edge *)0;
}
