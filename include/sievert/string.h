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

/*! \file
 *  \brief String Handling -- Advanced Functionality
 *
 *  Additional helper functions to deal with strings, including string sets
 *  (i.e. the typical char ** in regular C).
 */

#ifndef LIBSIEVERT_STRING_H
#define LIBSIEVERT_STRING_H

char **str_set_add        (char **set, const char *string);
char **str_set_remove     (char **set, const char *string);
char **str_set_merge      (char **a,   char **b);
char **str_set_intersect  (char **a,   char **b);
char **str_set_difference (char **a,   char **b);

int    str_set_memberp    (char **set, const char *string);
int    str_set_rx_memberp (char **set, const char *regex);

char **str_split          (const char *string, int separator);
char **str_split_ws       (const char *string);

const char *str_merge     (char **set, int glue);

#endif

