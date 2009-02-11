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

#include "curie/immutable.h"

#define STRING1 "hello world"
#define STRING1_LENGTH (unsigned int)(sizeof(STRING1)-1)

int cmain(void) {
    const char *immutable_ref1 = str_immutable_unaligned (STRING1);
    const char *immutable_ref2 = str_immutable_unaligned (immutable_ref1),
               *immutable_ref3 = str_immutable_unaligned (STRING1);
    unsigned int i;

    if (immutable_ref1 == (const char *)0) return 1;
    if (immutable_ref2 == (const char *)0) return 2;
    if (immutable_ref1 != immutable_ref2) return 3;

    for (i = 0; i < STRING1_LENGTH; i++) {
        if (immutable_ref1[i] != STRING1[i]) return 4;
    }

    if (immutable_ref1 != immutable_ref3) return 5;

    return 0;
}
