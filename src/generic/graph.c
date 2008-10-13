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

static struct memory_pool graph_pool = MEMORY_POOL_INITIALISER(sizeof (struct graph));
static struct memory_pool graph_node_pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_node));
static struct memory_pool graph_edge_pool = MEMORY_POOL_INITIALISER(sizeof (struct graph_edge));

struct graph * graph_create() {
    struct graph *gr = (struct graph *) get_pool_mem(&graph_pool);
    gr->nodes = aalloc(sizeof(struct graph_node *));
    return gr;
}

int get_size(struct graph * gr) {
    return (sizeof(gr->nodes)/sizeof(gr->nodes[0]));
}

int graph_add_node(struct graph * gr, struct graph_node * node) {
    int i = 0;
    if(gr->nodes[0]) {
        i = get_size(gr) + 1;
    }
    gr->nodes = (struct graph_nodes **) arealloc(sizeof(gr->nodes), gr->nodes, sizeof(gr->nodes) + sizeof(node));
    gr->nodes[i] = node;
    
    return i;
}

void graph_node_destroy(struct graph * gr, int node_index) {
    
    afree(sizeof (gr->nodes[node_index]), gr->nodes[node_index]);
    gr->nodes = (struct graph_nodes **) arealloc(sizeof(gr->nodes), gr->nodes, (sizeof(gr->nodes)/sizeof(gr->nodes[0]) - sizeof(gr->nodes[node_index]));
}

void graph_destroy (struct graph * gr) {
    for(int i = 0; i < get_size(gr); i++) {
//         if(gr->nodes[i])
        graph_node_destroy(gr, i);
    }
    afree(sizeof(struct graph_node), gr->nodes);
    
}

// struct graph_node * make_node(struct sexpr * label, struct graph_edge ** edges) {
//     struct graph_node * node = (struct graph_node *) get_pool_mem(&graph_node_pool);
//     node->label = label;
//     node->edges = edges;
// }

signed int graph_search_node(struct graph * gr, struct sexpr * label) {
    int x = -1;

    for(int i = 0; i < sizeof(gr->nodes)/sizeof(gr->nodes[0]); i++) {
        if(truep(equalp(gr->nodes[i], label)))
           return i;
    }
    return x;
}