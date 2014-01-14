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
#include <curie/stack.h>
#include <windows.h>

#include <stdlib.h>

char **curie_argv = (char **)0;
char **curie_environment = (char **)0;

int main (int argc, char **argv)
{
    int rv, i, envvsize, indices = 0;
    char **envv;
    const char *env = GetEnvironmentStringsA();

    initialise_stack ();

    for (i = 0; env[i] != (char)0; i++)
    {
        while (env[i] != (char)0)
        {
            i++;
        }

        indices++;
    }

    envvsize = sizeof (char *) * (indices + 1);
    envv     = aalloc (envvsize);

    indices  = 0;

    for (i = 0; env[i] != (char)0; i++)
    {
        envv[indices] = (char *)(env + i);

        while (env[i] != (char)0)
        {
            i++;
        }

        indices++;
    }
    envv[indices] = (char *)0;

    curie_argv          = argv;
    curie_environment   = envv;

    rv = cmain();

    afree (envvsize, envv);

    return rv;
}
