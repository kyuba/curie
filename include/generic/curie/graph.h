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
 *	Generic graphs are used by parts of the library code, and they 
 * 	are handy for a good deal of applications.
 * 	The functions in this header file implement a subset of useful graph operations.
 */

#ifndef LIBCURIE_GRAPH_H
#define LIBCURIE_GRAPH_H


#include <curie/int.h>

#ifdef __cplusplus
extern "C" {
#endif


struct graph {
    int_32 node_count;
    struct graph_node **nodes;
};

struct graph_node {
    struct sexpr * label;
    int_32 edge_count;
    struct graph_edge **edges;
};

struct graph_edge {
    struct graph_node *target;
    struct sexpr *label;
};

struct graph * graph_create();
void graph_destroy (struct graph *);

struct graph_node *graph_add_node (struct graph *, struct sexpr *);

struct graph_node *graph_search_node (struct graph *, struct sexpr *);

struct graph_edge *graph_node_add_edge(struct graph_node *, struct graph_node *, struct sexpr *);

struct graph_edge *graph_node_search_edge(struct graph_node *, struct sexpr *);

#ifdef __cplusplus
}
#endif

#endif
