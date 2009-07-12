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

#include "curie/io.h"
#include "curie/graph.h"
#include "curie/sexpr.h"

define_string(str_hello_world, "hello world!");

int cmain(void) {
    struct io *out = io_open_write ("temporary-sexpr-custom"), *in = io_open_null;
    struct sexpr_io *io = sx_open_io (in, out);
    sexpr forest = graph_create();
    sexpr s = make_integer(1);
    sexpr s2 = make_integer(2);
    struct graph_node *node1 = graph_add_node (forest, s);
    struct graph_node *node2 = graph_add_node (forest, s2);
    struct graph_edge *edge1;

    if (graph_search_node(forest, s) != node1) {
        return 1;
    }

    edge1 = graph_node_add_edge(node1, node2, s2);

    if(graph_node_search_edge(node1, s2) != edge1) {
        return 2;
    }

    sx_write(io, forest);

    sx_close_io (io);
    sx_destroy (forest);

    in = io_open_read ("temporary-sexpr-custom");
    out = io_open_null;
    io = sx_open_io (in, out);

    while (!eofp(forest = sx_read (io)))
    {
        if (consp(forest))
        {
            return 4;
        }
        else if (graphp(forest))
        {
            if ((node1 = graph_search_node(forest, s))
                 == (struct graph_node *)0)
            {
                return 5;
            }

            if ((edge1 = graph_node_search_edge(node1, s2))
                 == (struct graph_edge *)0)
            {
                return 6;
            }

            sx_destroy (forest);

            return 0;
        }
        else if (nexp(forest)) {}
        else
        {
            return 5;
        }
    }

    return 3;
}
