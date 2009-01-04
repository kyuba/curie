/*
 *  tree.h
 *  libcurie++
 *
 *  Created by Magnus Deininger on 01/01/2009.
 *  Copyright 2008/2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008/2009, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 #ifndef LIBCURIEPP_TREE_H
 #define LIBCURIEPP_TREE_H
 
 #include <curie++/int.h>
 #include <curie/int.h>
 
 namespace curiepp 
 {
   class Tree {
  // use the "named constructor" idiom here?
 
 #define TREE_INITIALIZER ((TreeNode*) 0)
 #define removeNode(t, k) t.removeNodeSpecific (k, ((TreeNode *) 0))
 #define removeNodeString(t, k) t.removeNodeStringSpecific (k, ((TreeNode *) 0))
 #define getValue (n) ((TreeNode *) n)->value
  private:
    Tree();
  
  public:
    TreeNode *root;
 
    Tree* create();

    void destroy();
    
    void addNode(TreeNode *);
    void addNodeValue(int_pointer key, void *aux);
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
   
    int_pointer key;
    void* value;
    TreeNode *right;
    TreeNode *left;
  };
 
 }
 
 #endif