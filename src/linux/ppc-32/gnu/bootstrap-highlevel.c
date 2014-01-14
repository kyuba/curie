/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include <curie/main.h>
#include <curie/memory.h>

char ** __do_startup (char **y, int argc, int envc) {
    int i, j;

    curie_argv = get_mem ((argc + 1) * sizeof (char *));
    curie_environment = get_mem ((envc + 1) * sizeof (char *));

    for (i = 1, j = 0; y[i] != (char *)0; i++) {
        if (y[i][0] != (char)0) {
            curie_argv[j] = y[i];
            j++;
        }
    }
    curie_argv[j] = (char *)0;

    for (i++, j = 0; y[i] != (char *)0; i++) {
        if (y[i][0] != (char)0) {
            curie_environment[j] = y[i];
            j++;
        }
    }
    curie_environment[j] = (char *)0;

    return y;
}
