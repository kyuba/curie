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
#include <curie++/tree.h>
#include <curie/memory.h>
using namespace curiepp;


TreeNode::TreeNode(int_pointer key_, void *value_, TreeNode *right_, TreeNode *left_) {
  key = key_;
  value = value_;
  right = right_;
  left = left_;
}

TreeNode::~TreeNode() {
  delete left;
  delete right;

  if(value) free_pool_mem(value);
}


Tree::Tree() {
  root = (TreeNode *) NULL;
}

Tree::~Tree() {
  delete root;
}

void Tree::addNode(TreeNode *n) {
  TreeNode *cur = root, *last = (TreeNode *) NULL;

  if(root == NULL) {
    root = n;
    return;
  }
  for(;cur != NULL;) {
    last = cur;
    if(n->key < cur->key) {
      cur = cur->left;
    }
    else {
      cur = cur->right;
    }

    if(n->key < last->key) {
      last->left = n;
    }
    else {
      last->right = n;
    }

  }

}

TreeNode* Tree::getNode(int_pointer key)
{
  Tree *t = this;
  this->root = t->root;

  for(;;) {
    if(t->root == NULL) return (TreeNode*) NULL;

    else if(t->root->key == key) return root;

    if(root->key < key)
      t->root = this->root->right;

    if(root->key > key)
      t->root = this->root->left;
  }

}
