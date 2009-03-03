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
using namespace curiepp;

#include <curie++/graph.h>

Graph::Graph()
{
  nodes = NULL;
  nodeCount = 0;
}

Graph::Graph(struct graph *g)
{
  nodes = new Node*[g->node_count];
  for(int i = 0; i < g->node_count; ++i) {
    struct graph_node *tmp = g->nodes[i];
    nodes[i] = new Node(new SExpr(tmp->label));
  }
}

Graph::Graph(SExpr *s) {
  //not yet implemented
}

void Graph::addNode(SExpr *label) {

  Node** new_nodes = new Node* [nodeCount + 1];
  for(int i = 0; i < nodeCount; i++) {
    nodes[i] = new_nodes[i];
  }
  new_nodes[nodeCount] = new Node(label);
  delete[] nodes;
  nodes = new_nodes;
  ++nodeCount;
}

Node * Graph::searchNode(SExpr *s) {
  int i = 0;
  while(falsep(equalp(nodes[i]->label, s))) i++;
  return nodes[i];
}

Node::Node(SExpr *l) {
  label = l;
  edgeCount = 0;
  edges = NULL;
}

Node::~Node() {
  delete[] edges; edges = NULL;
  delete label;
}

Node::addEdge(SExpr *l, Node *t) {
  Edge** new_edges = new Edge* [edgeCount + 1];
  for(int i = 0; i < edgeCount; i++) {
    edges[i] = new_edges[i];
  }
  new_edges[nodeCount] = new Edge(l, t);
  delete[] edges;
  edges = new_edges;
  ++edgeCount;
}

Edge * Node::searchEdge(SExpr *s) {
  int i = 0;
  while(falsep(equalp(edges[i]->label, s))) i++;
  return edges[i];
}

Edge::Edge(SExpr *l, Node *t) {
  label = l;
  target = t;
}

Edge::~Edge() {
  delete target;
  delete label;
}