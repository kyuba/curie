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

#include "curie++/sexpr.h"
#include "curie/graph.h"
#include "curie++/regex.h"

namespace curiepp {

  class Node;

  class Edge {
    private:

    public:
      Edge(sexpr l, Node *t);
      ~Edge();
      sexpr label;
      Node *target;

  };


  class Node {
    private:
      int_32 edgeCount;
      Edge** edges;

    public:
      Node(sexpr l);
      ~Node();

      int_32 getEdgeCount();
      Edge *getEdge(int_32 i);
      sexpr label;
      void addEdge(Edge *e);
      Edge *searchEdge(sexpr s);
  };

  class Graph {
    private:
      int_32 nodeCount;
      Node** nodes;

    public:
      Graph();
      Graph(SExpr *);

      Graph(sexpr g);
      ~Graph();

      int_32 getNodeCount();
      Node* getNode(int_32 i);

      void addNode(sexpr label);
      Node* searchNode(sexpr s);


      //! converts a given graph to a S-expression
      SExpr* operator= (Graph *g);

   //   //! converts a graph to a regular expression
   //   RegEx* operator= (Graph *g);
  };

}

#endif
