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
#include <curie++/graph.h>

#define NULL ((void *)0)

using namespace curiepp;
Graph::Graph() {
  nodes = (Node**) NULL;
  nodeCount = 0;
}

Graph::Graph(struct graph *g) {
  nodes = new Node*[g->node_count];
  for(int i = 0; i < g->node_count; ++i) {
    struct graph_node *tmp = g->nodes[i];
    nodes[i] = new Node(tmp->label);
  }
}

Graph::Graph(SExpr *s) {
  //not yet implemented
}

int_32 Graph::getNodeCount() {
  return nodeCount;
}

Node* Graph::getNode(int_32 i) {

  if(i >= 0 && i < nodeCount) {
    return nodes[i];
  }
  else {
    throw make_symbol("Node index out of bounds");
  }
}

void Graph::addNode(sexpr label) {

  Node** new_nodes = new Node* [nodeCount + 1];
  for(int i = 0; i < nodeCount; i++) {
    new_nodes[i] = nodes[i];
  }
  new_nodes[nodeCount] = new Node(label);
  delete[] nodes;
  nodes = new_nodes;
  ++nodeCount;
}

Node * Graph::searchNode(sexpr s) {

  for(int i = 0; i < nodeCount; ++i) {
    Node *n = nodes[i];
    if(n != NULL && truep(equalp(n->label, s))) return nodes[i];
  }
  return (Node *) NULL;
}

Node::Node(sexpr l) {
  label = l;
  edgeCount = 0;
  edges = 0;
}

Node::~Node() {
  delete[] edges;
  edges = (Edge**) NULL;
//   delete label;
}

int_32 Node::getEdgeCount() {
  return edgeCount;
}

Edge* Node::getEdge(int_32 i) {
  if(i >= 0 && i < edgeCount) {
    return edges[i];
  }
  else {
    throw make_symbol("Edge index out of bounds");
  }
}

void Node::addEdge(Edge *e) {

  Edge** new_edges = new Edge* [edgeCount + 1];
  for(int i = 0; i < edgeCount; i++) {
    new_edges[i] = edges[i];
  }
  new_edges[edgeCount] = e;
  delete[] edges;
  edges = new_edges;
  ++edgeCount;
}

Edge * Node::searchEdge(sexpr s) {
  int i = 0;
  Edge *rv = 0;
  while(i < edgeCount && falsep(equalp(edges[i]->label, s))) i++;
  if(i < edgeCount) rv = edges[i];
  return rv;
}

Edge::Edge(sexpr l, Node *t) {
  label = l;
  target = t;
}

Edge::~Edge() {
  delete target;
//   delete label;
}
