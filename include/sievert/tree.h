/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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
 *  \brief High-level Functionality for Curie BSTs
 *
 *  This functionality used to be part of curie, but it's not really low-level
 *  enough to be justified as part of the core curie library, and it is rarely
 *  used.
 */

#ifndef LIBSIEVERT_TREE_H
#define LIBSIEVERT_TREE_H

#include <curie/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Create Tree Node with a String Key
 *  \param[in] tree The tree to add the node to.
 *  \param[in] key  The lookup key to use.
 *
 *  Analoguous to tree_add_node(), except that the key is a string. The string
 *  is hashed, so that two strings with different addresses will still match up
 *  in the tree.
 */
void tree_add_node_string
        (struct tree *tree, char *key);

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
        (struct tree *tree, char *key, void *aux);

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
struct tree_node * tree_get_node_string
        (struct tree *tree, char *key);

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
        (struct tree *tree, char *key, struct tree_node *node);

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
