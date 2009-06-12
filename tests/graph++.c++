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
#include <curie/main.h>

using namespace curiepp;

#define NULL ((void *)0)

int cxxmain() {
  struct io *out = io_open_write ("temporary-graph"), *in = io_open (0);
  struct sexpr_io *io = sx_open_io (in, out);
//
  Graph *forest = new Graph();

  sexpr s = make_integer(1);
  sexpr s2 = make_integer(42);

  if(forest->getNodeCount() > 0) {
    return 1;
  }
  forest->addNode(s);

  try {
    forest->getNode(3);
  }
  catch(sexpr e) {

    sx_write (io, e);
  }
  try {
    forest->getNode(-3);
  }
  catch(sexpr e) {

    sx_write (io, e);
  }

  forest->addNode(s2);

//  if(forest->searchNode(s2) == 0) {
  //  return 3;
 // }


  if(forest->searchNode(s2) == NULL) return 6;


  forest->getNode(0)->addEdge(new Edge(make_integer(3), forest->getNode(1)));


  if(forest->getNode(0)->searchEdge(make_integer(3)) == 0) {
    return 4;
  }

  try {
    forest->getNode(0)->getEdge(2);

  }
  catch(sexpr e) {

    sx_write (io, e);
  }
  try {
    forest->getNode(0)->getEdge(-2);

  }
  catch(sexpr e) {

    sx_write (io, e);
  }

  sexpr s3 = make_string("nyu");

  if(forest->searchNode(s3) != NULL) return 7;
  forest->addNode(s3);
  if(forest->searchNode(s3) == NULL) return 8;

  return 0;

}
