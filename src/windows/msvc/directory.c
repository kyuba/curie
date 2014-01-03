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

#include <curie/directory.h>
#include <curie/memory.h>
#include <sys/types.h>

#include <windows.h>

sexpr read_directory_rx (const char *base, sexpr rx)
{
    char *buffer;
    int i, blength;
    WIN32_FIND_DATA data;
    HANDLE dir;
    sexpr r = sx_end_of_list;

    for (i = 0; base[i]; i++); blength = i + 2;

    buffer = aalloc (blength);

    for (i = 0; base[i]; i++)
    {
        buffer[i] = base[i];
    }

    buffer[i] = '\\';
    buffer[i+1] = '*';
    buffer[i+2] = 0;

    dir = FindFirstFileA (buffer, &data);

    afree (blength, buffer);

    if (dir != INVALID_HANDLE_VALUE)
    {
        do
        {
            char *s = data.cFileName;

            if (truep (rx_match (rx, s)))
            {
                r = cons (make_string (s), r);
            }
        } while (FindNextFileA (dir, &data));

        CloseHandle (dir);
    }

    return r;
}
