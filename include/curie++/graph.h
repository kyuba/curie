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

#ifndef LIBCURIEPP_GRAPH_H
#define LIBCURIEPP_GRAPH_H

#include "sexpr.h"

namespace curiepp {

  class Graph {
    private:
      int_32 nodeCount;
      Node **nodes;

    public:
      Graph();
      Graph(SExpr *);
      ~Graph();
      void addNode();
      Node *searchNode(SExpr *s);

      // converts a given graph to a S-expression
      SExpr* operator= ();

  };

  class Node {
    private:
      int_32 edgeCount;
      Edge **edges;

    public:
      Node(SExpr *l);
      ~Node();
      SExpr *label;
      void addEdge();
      Edge *searchEdge(SExpr *s);
  };

  class Edge {
    private:

    public:
      Edge(SExpr *l);
      ~Edge();
      SExpr *label;
      Node *target;

  };


}

#endif