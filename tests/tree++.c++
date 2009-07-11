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

#include <curie/main.h>
#include <curie++/tree.h>

using namespace curiepp;

int cxxmain() {
  Tree *t = new Tree();

  t->addNode(10);


  if(t == NULL) return -2;
  if(t->getRoot() == (TreeNode *) NULL) return -1;

  if(t->getNode(10) == (TreeNode *)NULL) return 1;

  t->addNode(1);

  if(t->getRoot()->getLeft() == (TreeNode *)NULL) return 2;
  if(t->getRoot()->getRight() != (TreeNode *)NULL) return 3;

  t->addNode(23);


  if(t->getRoot()->getRight() == (TreeNode *)NULL) return 4;
  if(t->getRoot()->getRight()->getKey() != 23) return 5;
//
  TreeNode *tn = t->getNode(23);

  if(tn == (TreeNode*) NULL) return 6;

  char *x;
  x = "nyuu";

  t->addNodeValue(20, (void *)x);
  TreeNode *tn2 = t->getNode(20);

  if(tn2 == (TreeNode*) 0) return 7;

  if(tn2->value == (void*)0) return 8;

  if((char*) tn2->value == x) return 0;
  else return 9;

  delete t;
  return 0;
}
