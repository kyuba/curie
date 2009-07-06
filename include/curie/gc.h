/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2009, Kyuba Project Members
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
 *  \brief Garbage Collector
 *
 *  We're trying to implement a rather conservative GC here, all it does is walk
 *  from the stack start address to the current end of it and mark available
 *  objects based on the pointers it sees (assuming all pointers are aligned to
 *  their size, which is a fair assumption).
 *
 *  Note that the gc is never invoked automatically, it needs to be run
 *  manually. This should prevent most problems due to optimisations, and if for
 *  some reason you know that the GC will screw things up, just don't call it.
 */

#ifndef LIBCURIE_GC_H
#define LIBCURIE_GC_H

#include <curie/sexpr.h>

void          gc_add_root    (sexpr *sx);
void          gc_remove_root (sexpr *sx);
void          gc_tag         (sexpr sx);
void          gc_call        (sexpr sx);
unsigned long gc_invoke      ();

unsigned long gc_base_items;

#endif

/*! @} */
