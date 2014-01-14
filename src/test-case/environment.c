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

int cmain(void) {
    unsigned int i;

    if (curie_argv == (char **)0)
        return 1;

    for (i = 0; curie_argv[0][i]; i++);

    if (i < 12) return 3;

    if (curie_environment == (char **)0)
        return 2;

    for (i = 0; curie_environment[i] != (char *)0; i++) {
        if (((curie_environment[i][0] == 'P') || (curie_environment[i][0] == 'p')) &&
            ((curie_environment[i][1] == 'A') || (curie_environment[i][1] == 'a')) &&
            ((curie_environment[i][2] == 'T') || (curie_environment[i][2] == 't')) &&
            ((curie_environment[i][3] == 'H') || (curie_environment[i][3] == 'h')) &&
            (curie_environment[i][4] == '=')) {

            return 0;
        }
    }

    return 4; /* reach when there's no PATH= environment variable */
}
