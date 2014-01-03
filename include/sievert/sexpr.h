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

/*! \defgroup sexprSievert S-Expression Library Functions
 *  \ingroup sexpr
 *  \brief Extended Symbolic Expression Handling
 *  @{
 */

/*! \file
 *  \brief S-expression Library Functions
 *
 *  The split between what's an essential function and what is part of a more
 *  high-level part of the typical library may seem fairly arbitrary; I'm
 *  trying to put the rarely used and higher level functions into libsievert to
 *  clean up the symbol space a bit.
 */

#ifndef LIBSIEVERT_SEXPR_H
#define LIBSIEVERT_SEXPR_H

#include <curie/sexpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief List Map
 *  \param[in] list The list to map.
 *  \param[in] f    The function to apply.
 *
 *  This function applies the given function to each element of f.
 */
void sx_list_map (sexpr list, void (*f)(sexpr));

/*! \brief List Fold
 *  \param[in] list The list to fold.
 *  \param[in] f    The function to apply.
 *  \param[in] seed The seed value to use.
 *  \return The value constructed by f().
 *
 *  This function applies the given function to each element of f, passing the
 *  return value of f to the next function call and ultimately returning the
 *  return value of the last call to f().
 */
sexpr sx_list_fold (sexpr list, sexpr (*f)(sexpr, sexpr), sexpr seed);

/*! \brief Turn Integer Argument into Hex String
 *  \param[in] a The argument to convert.
 *  \return The converted form of the argument.
 *
 *  This is similar to sx_to_string(), except that it only works on integers
 *  and the result is in base-16 instead of base-10.
 */
sexpr sx_integer_to_string_hex (int_pointer_s a);

/*! \defgroup sexprSet Sets (S-expression Version)
 *  @{
 */

/*! \brief Add an Element to a Set (S-expression Version)
 *  \param[in] set  The set to add an element to.
 *  \param[in] item The item to add to the set.
 *  \return set ∪ { item }.
 *
 *  This function adds the new item to the set and returns the resulting set,
 *  unless the item is already in the set, in which case the set is returned
 *  unmodified.
 */
sexpr sx_set_add        (sexpr set,    sexpr item);

/*! \brief Remove an Element from a Set (S-expression Version)
 *  \param[in] set  The set to remove an item from.
 *  \param[in] item The item to remove from the set.
 *  \return Set ∩ { item }.
 *
 *  This function removes an item from the set, returning the resulting set. If
 *  the item is not in the set, the set is returned unmodified.
 */
sexpr sx_set_remove     (sexpr set,    sexpr item);

/*! \brief Merge two Sets (S-expression Version)
 *  \param[in] a    The first set of those to merge.
 *  \param[in] b    The second set of those to merge.
 *  \return a ∪ b.
 *
 *  This function merges a and b, so that the resulting set contains all
 *  elements that are either in a or in b.
 */
sexpr sx_set_merge      (sexpr a,      sexpr b);

/*! \brief Calculate the Intersection of two Sets (S-expression Version)
 *  \param[in] a   The first set of those to intersect.
 *  \param[in] b   The second set of those to intersect.
 *  \return a ∩ b.
 *
 *  This function calculates the intersection of a and b, so that the resulting
 *  set will contain all elements that are present in both sets.
 */
sexpr sx_set_intersect  (sexpr a,      sexpr b);

/*! \brief Calculate the Difference of two Sets (S-expression Version)
 *  \param[in] a    The first set to work on.
 *  \param[in] b    The second set to work on.
 *  \return (a ∪ b) \ (a ∩ b).
 *
 *  Formally speaking, this function calculates the complement to (a ∩ b) in
 *  (a ∪ b), i.e. the resulting set will contain all elements that are either in
 *  a or in b, but not in both.
 */
sexpr sx_set_difference (sexpr a,      sexpr b);

/*! \brief Test if an Item is a Member of a Set (S-expression Version)
 *  \param[in] set  The set to work on.
 *  \param[in] item The item to test for.
 *  \return \#t if item is in set, \#f otherwise.
 *
 *  Use this to test whether an item is in a set, if you know exactly what that
 *  item is.
 */
sexpr sx_set_memberp    (sexpr set,    sexpr item);

/*! \brief Test if an Item is a Member of a Set (Regex, S-expression Version)
 *  \param[in] set   The set to work on.
 *  \param[in] regex A regex describing the item to search for.
 *  \return \#t if regex matches an element in set, \#f otherwise.
 *
 *  Same as sx_set_memberp(), except that instead of looking for a literal item
 *  a regex is used instead, which is matched against string-type and
 *  symbol-type members of the set. Other types of members are ignored.
 */
sexpr sx_set_rx_memberp (sexpr set,    sexpr regex);

/*! \brief Split an Item into a Set (S-expression Version)
 *  \param[in] item      The item to work on.
 *  \param[in] separator The separator to use.
 *  \return item split into a set by separator.
 *
 *  Splits string and symbol-type s-expression values into a set using the given
 *  separator, which may be either a string, symbol or integer. If the separator
 *  is a string or symbol, its first character is used, otherwise it is
 *  interpreted as the ordinal value of a character.
 */
sexpr sx_split          (sexpr item,   sexpr separator);

/*! \brief Merge a Set into a String or Symbol (S-expression Version)
 *  \param[in] set  The set to collapse.
 *  \param[in] glue The glue to use when collapsing the set.
 *  \return A string or symbol that contains the merged version of set.
 *
 *  This function will merge all elements of the set into a single string or
 *  symbol, with each former element separated by the given glue. The glue
 *  argument is used analoguous to sx_split(), except that for this function it
 *  also determines the return type of the function (a symbol-type glue will
 *  return a symbol, other types will result in a string).
 */
sexpr sx_merge          (sexpr set,    sexpr glue);

/*! \brief Sort the Set according a Custom Function (S-expression Version)
 *  \param[in]     set The set to sort.
 *  \param[in]     gtp The function to sort with.
 *  \param[in,out] aux Passed to gtp().
 *  \return The original set, sorted using gtp().
 *
 *  This will apply a merge-sort algorithm over set, using gtp as the comparison
 *  function for the merge sort. gtp() should return \#t if its first argument
 *  is considered greater than the second. (Thus the name; think of it as
 *  (gt? ...).)
 */
sexpr sx_set_sort_merge
    (sexpr set, sexpr (*gtp)(sexpr, sexpr, void *), void *aux);

/*! \brief Create a List with one Element
 *  \param[in] alpha   The first element.
 *  \return The new list.
 *
 *  This function will create a new list with the given argument as the first
 *  element of that list.
 */
sexpr sx_list1
    (sexpr alpha);

/*! \brief Create a List with two Elements
 *  \param[in] alpha   The first element.
 *  \param[in] beta    The second element.
 *  \return The new list.
 *
 *  This function will create a new list with the given arguments as elements
 *  of that list.
 */
sexpr sx_list2
    (sexpr alpha, sexpr beta);

/*! \brief Create a List with three Elements
 *  \param[in] alpha   The first element.
 *  \param[in] beta    The second element.
 *  \param[in] gamma   The third element.
 *  \return The new list.
 *
 *  This function will create a new list with the given arguments as elements
 *  of that list.
 */
sexpr sx_list3
    (sexpr alpha, sexpr beta, sexpr gamma);

/*! \brief Create a List with four Elements
 *  \param[in] alpha   The first element.
 *  \param[in] beta    The second element.
 *  \param[in] gamma   The third element.
 *  \param[in] delta   The fourth element.
 *  \return The new list.
 *
 *  This function will create a new list with the given arguments as elements
 *  of that list.
 */
sexpr sx_list4
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta);

/*! \brief Create a List with five Elements
 *  \param[in] alpha   The first element.
 *  \param[in] beta    The second element.
 *  \param[in] gamma   The third element.
 *  \param[in] delta   The fourth element.
 *  \param[in] epsilon The fifth element.
 *  \return The new list.
 *
 *  This function will create a new list with the given arguments as elements
 *  of that list.
 */
sexpr sx_list5
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta, sexpr epsilon);

/*! \brief Create a List with six Elements
 *  \param[in] alpha   The first element.
 *  \param[in] beta    The second element.
 *  \param[in] gamma   The third element.
 *  \param[in] delta   The fourth element.
 *  \param[in] epsilon The fifth element.
 *  \param[in] zeta    The sixth element.
 *  \return The new list.
 *
 *  This function will create a new list with the given arguments as elements
 *  of that list.
 */
sexpr sx_list6
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta, sexpr epsilon,
     sexpr zeta);

/*! \brief Get a Value from an Association List via its Key
 *  \param[in] alist The list to look in.
 *  \param[in] key   They key to look for.
 *  \return The key's value if found, sx_nonexistent otherwise.
 *
 *  Given a key and an associative list (i.e. a (... (key . value) ...)
 *  constuct), this function will try to look for the key in the list and return
 *  the value associated with it.
 */
sexpr sx_alist_get (sexpr alist, sexpr key);

/*! \brief Add a Key-Value Pair to an Association List
 *  \param[in] alist The list to modify.
 *  \param[in] key   They key to store the value under.
 *  \param[in] value They value to associate with the key.
 *  \return The new list.
 *
 *  This will introduce a (key . value) pair to the front of the given alist.
 *  The list is not searched for any further occurences of the key, which means
 *  that any duplicates would stay in the list. Make sure to remove duplicates
 *  yourself before adding a new value.
 *
 *  The list passed to this function is not modified. Instead, the new
 *  association list is return.
 */
sexpr sx_alist_add (sexpr alist, sexpr key, sexpr value);

/*! \brief Remove a Value from an Association List via its Key
 *  \param[in] alist The list to modify.
 *  \param[in] key   They key to to remove.
 *  \return The new list.
 *
 *  This will remove any (key . value) pairs with the given key from the given
 *  association list.
 *
 *  The list passed to this function is not modified. Instead, the new
 *  association list is return.
 */
sexpr sx_alist_remove (sexpr alist, sexpr key);

/*! \brief Merge two association lists
 *  \param[in] alist1 The first of the two lists to merge.
 *  \param[in] alist2 The second of the two lists to merge.
 *  \return The new list.
 *
 *  Given two associaton lists, this function will calucalte a combined version
 *  of the two. Elements where the key only occurs in one of the lists are
 *  simply added to the result. For elements where the key occurs in both lists,
 *  the value in the second list overrides the value in the first.
 *
 *  Neither list passed to this function is not modified. Instead, the new
 *  association list is return.
 */
sexpr sx_alist_merge (sexpr alist1, sexpr alist2);

/*! @} */

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
