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

/*! \file
 *  \brief Basic BSTs
 *
 *  Binary search trees are used by parts of the library code itself, and they
 *  are generally needed by a good deal of applications. The functions from this
 *  header file implement a small subset of the useful functions on BSTs.
 */

#ifndef LIBCURIE_TREE_H
#define LIBCURIE_TREE_H

#include <curie/int.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief BST Root
 *
 *  This is the root of a binary seach tree.
 */
struct tree {
    /*! \brief Root Node
     *
     *  Root node for a BST.
     */
    /*@null@*/ /*@only@*/ struct tree_node * root;
};

/*! \brief BST Node
 *
 *  A single node in a binary search tree.
 */
struct tree_node {
    /*! \brief Left Branch
     *
     *  This points to the next node to the left of the current node.
     */
    /*@null@*/ /*@only@*/ struct tree_node * left;

    /*! \brief Right Branch
     *
     *  This points to the next node to the right of the current node.
     */
    /*@null@*/ /*@only@*/ struct tree_node * right;

    /*! \brief Node Key
     *
     *  This is the node's key, or its value.
     */
    int_pointer key;
};

/*! \brief BST Node with Value
 *  \internal
 *
 *  A single node in a binary search tree, with an extra value attached to it.
 */
struct tree_node_pointer {
    /*! \brief Left Branch
     *
     *  This points to the next node to the left of the current node.
     */
    /*@null@*/ /*@only@*/ struct tree_node * left;

    /*! \brief Right Branch
     *
     *  This points to the next node to the right of the current node.
     */
    /*@null@*/ /*@only@*/ struct tree_node * right;

    /*! \brief Node Key
     *
     *  This is the node's key, for lookups.
     */
    int_pointer key;

    /*! \brief Node Value
     *
     *  The node's value, or payload.
     */
    /*@null@*/ /*@dependent@*/ void *value;
};

/*! \brief Static Tree Initialiser
 *
 *  This macro can be used to initialise a tree statically, that is to
 *  initialise a tree in a variable definition in the file scope, or a static
 *  variable somewhere else.
 */
#define TREE_INITIALISER { (struct tree_node *)0 }

/*! \brief Create a new Tree
 *  \return New tree root, or (struct tree *)0 when no memory is available.
 *
 *  Create and initialise a new tree without any nodes.
 */
/*@null@*/ /*@only@*/ struct tree * tree_create ();

/*! \brief Destroy a Tree
 *  \param[in] tree The tree to destroy.
 *
 *  Destroy a tree and deallocate any memory associated with it. Using either
 *  tree's root pointer, or any of the nodes is a good way to get your app
 *  killed.
 */
void tree_destroy
        (/*@notnull@*/ /*@only@*/ struct tree *tree);

/*! \brief Create Tree Node
 *  \param[in] tree The tree to add the node to.
 *  \param[in] key  The lookup key to use.
 *
 *  This will initialise a new tree node with the give key, and add it to the
 *  given tree.
 */
void tree_add_node
        (/*@notnull@*/ struct tree *tree, int_pointer key);

/*! \brief Create Tree Node with Value
 *  \param[in] tree The tree to add the node to.
 *  \param[in] key  The lookup key to use.
 *  \param[in] aux  The payload of the new node.
 *
 *  Like tree_add_node(), but the node will contain the aux pointer as its
 *  value.
 */
void tree_add_node_value
        (/*@notnull@*/ struct tree * tree,
         int_pointer key,
         /*@null@*/ /*@dependent@*/ void *aux);

/*! \brief Search for a Tree Node
 *  \param[in] tree The tree to search in.
 *  \param[in] key  The key to look for.
 *  \return The node with that key, or (struct tree_node *)0 if the node is not
 *          found.
 *
 *  Searches the given tree for a node with the given key. 
 */
/*@null@*/ /*@shared@*/ struct tree_node * tree_get_node
        (/*@notnull@*/ struct tree *tree,
         int_pointer key);

/*! \brief Remove a (specific) Tree Node
 *  \param[in] tree The tree to remove the node from.
 *  \param[in] key  The key of the node to remove.
 *  \param[in] node The node to remove.
 *
 *  The node argument is optional. This searches the tree for the given key, and
 *  if the key is found, then the node associated with it is removed. If the
 *  node argument is specified, then only that particular node will be removed;
 *  this would be of significance if a tree had the same key more than once.
 */
void tree_remove_node_specific
        (/*@notnull@*/ struct tree *tree,
         int_pointer key,
         /*@null@*/ struct tree_node *node);

/*! \brief Remove a Tree Node
 *  \param[in] t The tree to remove the node from.
 *  \param[in] k The key of the node to remove.
 *
 *  A simple wrapper for tree_remove_node_specific(), to use whenever you don't
 *  quite care for the particular node that should be removed.
 */
#define tree_remove_node(t,k) tree_remove_node_specific(t, k, (struct tree_node *)0)

/*! \brief Retrieve a Node's Value
 *  \param[in] node The node whose value to get.
 *  \return The value of that node.
 *
 *  Use this macro to extract a node's value if you've added the ndoe with 
 *  tree_add_node_value().
 *
 *  \note Don't use this with nodes you've added with tree_add_node(). It may
 *        get your app killed.
 */
#define node_get_value(node) ((struct tree_node_pointer *)node)->value

/*! \brief Map over a Tree
 *  \param[in] tree The tree to use.
 *  \param[in] f    The function to call.
 *  \param[in] aux  Data to pass to f().
 *
 *  A map operation is when you apply a function to each node of some structure.
 *  It's usually called map for lists, but the same concept applies to trees as
 *  well. The exact order in which the elements are mapped is undefined.
 */
void tree_map
        (/*@notnull@*/ struct tree *tree,
         /*@notnull@*/ void (*f)(struct tree_node *, void *),
         /*@null@*/ void *aux);

/*! \brief Create Tree Node with a String Key
 *  \param[in] tree The tree to add the node to.
 *  \param[in] key  The lookup key to use.
 *
 *  Analoguous to tree_add_node(), except that the key is a string. The string
 *  is hashed, so that two strings with different addresses will still match up
 *  in the tree.
 */
void tree_add_node_string
        (/*@notnull@*/ struct tree *tree,
         /*@observer@*/ /*@notnull@*/ char *key);

/*! \brief Create Tree Node with a String Key and Value
 *  \param[in] tree The tree to add the node to.
 *  \param[in] key  The lookup key to use.
 *  \param[in] aux  The payload of the new node.
 *
 *  Analoguous to tree_add_node_value(), except that the key is a string. The
 *  string is hashed, so that two strings with different addresses will still
 *  match up in the tree.
 */
void tree_add_node_string_value
        (/*@notnull@*/ struct tree *tree,
         /*@observer@*/ /*@notnull@*/ char *key,
         /*@null@*/ /*@dependent@*/ void *aux);

/*! \brief Search for a Tree Node with a String Key
 *  \param[in] tree The tree to search in.
 *  \param[in] key  The key to look for.
 *  \return The node with that key, or (struct tree_node *)0 if the node is not
 *          found.
 *
 *  Analoguous to tree_get_node(), except that the key is a string. The string
 *  is hashed, so that two strings with different addresses will still match up
 *  in the tree.
 */
/*@null@*/ /*@shared@*/ struct tree_node * tree_get_node_string
        (/*@notnull@*/ struct tree *tree,
         /*@observer@*/ /*@notnull@*/ char *key);

/*! \brief Remove a (specific) Tree Node with a String Key
 *  \param[in] tree The tree to remove the node from.
 *  \param[in] key  The key of the node to remove.
 *  \param[in] node The node to remove.
 *
 *  Analoguous to tree_remove_node_specific(), except that the key is a string.
 *  The string is hashed, so that two strings with different addresses will
 *  still match up in the tree.
 */
void tree_remove_node_string_specific
        (/*@notnull@*/ struct tree *tree,
         /*@observer@*/ /*@notnull@*/ char *key,
         /*@null@*/ struct tree_node *node);

/*! \brief Remove a Tree Node with a String Key
 *  \param[in] t The tree to remove the node from.
 *  \param[in] k The key of the node to remove.
 *
 *  A simple wrapper for tree_remove_node_string_specific(), to use whenever you
 *  don't quite care for the particular node that should be removed.
 */
#define tree_remove_node_string(t,k) tree_remove_node_string_specific(t, k, (struct tree_node *)0)

#ifdef __cplusplus
}
#endif

#endif
