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
  node->key = key_;
  node->right = right_->node;
  node->left = left_->node;
  value = value_;
}

TreeNode::TreeNode(struct tree_node_pointer *node_) {
  node->key = node_->key;
  node->right = node_->right;
  node->left = node_->left;
  value = node_->value;
}

TreeNode::TreeNode(struct tree_node *node_) {
  node = node_;
  value = ((struct tree_node_pointer *) node_)->value;
}

TreeNode::~TreeNode() {

  if(value) free_pool_mem(value);
}

int_pointer TreeNode::getKey() {
  return this->node->key;
}

TreeNode* TreeNode::getLeft() {
  TreeNode* ret = (TreeNode *) 0;
  if(this->node->left != (struct tree_node *) 0) {
    ret =  new TreeNode(this->node->left);
  }
  return ret;
}

TreeNode* TreeNode::getRight() {
  TreeNode* ret = (TreeNode *) 0;
  if(this->node->right != (struct tree_node *) 0) {
    ret =  new TreeNode(this->node->right);
  }
  return ret;
}

struct tree_node * TreeNode::getStruct() {
  return this->node;
}


Tree::Tree() {
  tree = tree_create();
}

Tree::Tree(struct tree *tree_) {
  tree = tree_;
}

Tree::~Tree() {
//   delete root;
}



void Tree::addNode(int_pointer key) {
  tree_add_node(tree, key);
}

void Tree::addNodeValue(int_pointer key, void *aux) {
  tree_add_node_value(tree, key, aux);
}

TreeNode* Tree::getNode(int_pointer key)
{
  struct tree_node *tn = tree_get_node(this->tree, key);
  TreeNode *ret = (TreeNode *) 0;
  if(tn != (struct tree_node *) 0) {
    ret = new TreeNode(tn);
  }
  return ret;
}

TreeNode* Tree::getRoot() {
  struct tree_node *tn = tree->root;
  TreeNode *ret = (TreeNode *)0;
  if(tn != (struct tree_node *) 0) {
    if(((struct tree_node_pointer *) tn)->value != (void*)0) {
      ret = new TreeNode(((struct tree_node_pointer *) tn));
    }
    else {
      ret = new TreeNode(tn);
    }
  }
  return ret;
}

void Tree::removeNodeSpecific(int_pointer key, TreeNode *node) {
    tree_remove_node_specific(tree, key, node->getStruct());
}

void Tree::addNodeString(char * key) {
    tree_add_node_string(tree, key);
}

void Tree::addNodeStringValue(char * key, void * aux) {
    tree_add_node_string_value(tree, key, aux);
}

TreeNode* Tree::getNodeString(char* key) {
  struct tree_node *tn = tree_get_node_string(this->tree, key);
  TreeNode *ret = (TreeNode *) 0;
  if(tn != (struct tree_node *) 0) {
    ret = new TreeNode(tn);
  }
  return ret;
}

void Tree::removeNodeStringSpecific(char* key, TreeNode *node) {
  tree_remove_node_string_specific(tree, key, node->getStruct());
}
