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
 *  \brief S-expressions (internal)
 *  \internal
 */

#ifndef LIBCURIE_SEXPR_INTERNAL_H
#define LIBCURIE_SEXPR_INTERNAL_H

#include <curie/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief S-Expression I/O Structure
 *
 *  Programmes don't need to know how this one looks on the inside. This is just
 *  a wrapper for an input and an output I/O structure for use with sx_read(),
 *  sx_write(), etc.
 */
struct sexpr_io {
    struct io *in;  /*!< \brief Input Structure */
    struct io *out; /*!< \brief Output Structure */
};

struct sexpr_type_descriptor
{
    unsigned int                   type;
    sexpr                        (*serialise)  (sexpr);
    sexpr                        (*unserialise)(sexpr);
    void                         (*tag)        (sexpr);
    void                         (*destroy)    (sexpr);
    void                         (*call)       ();
    sexpr                        (*equalp)     (sexpr, sexpr);
    struct sexpr_type_descriptor  *next;
};

void sx_call_all ();
struct sexpr_type_descriptor *sx_get_descriptor (unsigned int type);

#ifdef __cplusplus
}
#endif

#endif
