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

#ifndef LIBCURIEPP_TREE_H
#define LIBCURIEPP_TREE_H

#include <curie++/int.h>
#include <curie/int.h>

#define NULL ((void *) 0)
namespace curiepp
{
  class TreeNode;

   class Tree {

 #define TREE_INITIALIZER ((TreeNode*) 0)
 #define removeNode(t, k) t.removeNodeSpecific (k, ((TreeNode *) 0))
 #define removeNodeString(t, k) t.removeNodeStringSpecific (k, ((TreeNode *) 0))
 #define getValue (n) ((TreeNode *) n)->value
  private:

  public:
    Tree();
    ~Tree();
    TreeNode *root;

    void addNode(TreeNode *);
    TreeNode* getNode(int_pointer key);
    void removeNodeSpecific(int_pointer key, TreeNode *node);

    void addNodeString(char* key);
    void addNodeStringValue(char* key, void* aux);
    void getNodeString(char* key);
    void removeNodeStringSpecific(char* key, TreeNode *node);

    void map(void(*f)(TreeNode *, void *), void *aux);

  };

   class TreeNode {
   public:
     TreeNode(int_pointer key, void *value, TreeNode *right, TreeNode *left);
    ~TreeNode();

    int_pointer key;
    void* value;
    TreeNode *right;
    TreeNode *left;
  };

 }

#endif
