/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

/*! \defgroup strings Strings
 *  @{
 */

/*! \file
 *  \brief String Handling -- Advanced Functionality
 *
 *  Additional helper functions to deal with strings, including string sets
 *  (i.e. the typical char ** in regular C).
 */

#ifndef LIBSIEVERT_STRING_H
#define LIBSIEVERT_STRING_H

/*! \defgroup stringsSet String Sets
 *  @{
 */

/*! \brief Add an Element to a Set (String Version)
 *  \param[in] set    The set to add an element to.
 *  \param[in] string The string to add to the set.
 *  \return set ∪ { item }.
 *
 *  This function adds the new item to the set and returns the resulting set,
 *  unless the item is already in the set, in which case the set is returned
 *  unmodified.
 */
char **str_set_add        (char **set, const char *string);

/*! \brief Remove an Element from a Set (String Version)
 *  \param[in] set    The set to remove an element from.
 *  \param[in] string The string to remove from the set.
 *  \return Set ∩ { item }.
 *
 *  This function removes an item from the set, returning the resulting set. If
 *  the item is not in the set, the set is returned unmodified.
 */
char **str_set_remove     (char **set, const char *string);

/*! \brief Merge two Sets (String Version)
 *  \param[in] a      The first set of those to merge.
 *  \param[in] b      The second set of those to merge.
 *  \return a ∪ b.
 *
 *  This function merges a and b, so that the resulting set contains all
 *  elements that are either in a or in b.
 */
char **str_set_merge      (char **a,   char **b);

/*! \brief Calculate the Intersection of two Sets (String Version)
 *  \param[in] a   The first set of those to intersect.
 *  \param[in] b   The second set of those to intersect.
 *  \return a ∩ b.
 *
 *  This function calculates the intersection of a and b, so that the resulting
 *  set will contain all elements that are present in both sets.
 */
char **str_set_intersect  (char **a,   char **b);

/*! \brief Calculate the Difference of two Sets (String Version)
 *  \param[in] a    The first set to work on.
 *  \param[in] b    The second set to work on.
 *  \return (a ∪ b) \ (a ∩ b).
 *
 *  Formally speaking, this function calculates the complement to (a ∩ b) in
 *  (a ∪ b), i.e. the resulting set will contain all elements that are either in
 *  a or in b, but not in both.
 */
char **str_set_difference (char **a,   char **b);

/*! \brief Test if an Item is a Member of a Set (String Version)
 *  \param[in] set    The set to work on.
 *  \param[in] string The string to test for.
 *  \return nonzero if item is in set, 0 otherwise.
 *
 *  Use this to test whether an item is in a set, if you know exactly what that
 *  string is.
 */
int    str_set_memberp    (char **set, const char *string);

/*! \brief Test if an Item is a Member of a Set (Regex, String Version)
 *  \param[in] set   The set to work on.
 *  \param[in] regex A regex describing the item to search for.
 *  \return nonzero if regex matches an element in set, 0 otherwise.
 *
 *  Same as str_set_memberp(), except that instead of looking for a literal item
 *  a regex is used instead, which is matched against the members of the set.
 */
int    str_set_rx_memberp (char **set, const char *regex);

/*! \brief Split a String into a Set (String Version)
 *  \param[in] string    The string to work on.
 *  \param[in] separator The separator to use.
 *  \return string split into a set by separator.
 *
 *  Splits the given string into a set using the given separator.
 */
char **str_split          (const char *string, int separator);

/*! \brief Merge a Set into a String (String Version)
 *  \param[in] set  The set to work on.
 *  \param[in] glue The character to use when collapsing the set.
 *  \return A string that contains the merged version of set.
 *
 *  This function will take each element of the set, and joins them by appending
 *  the next element in the set, separated by the given glue.
 */
const char *str_merge     (char **set, int glue);

/*! @} */

#endif

/*! @} */
