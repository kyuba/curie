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

#include <curie/utf-8.h>

unsigned int utf8_get_character (const int_8 *b, unsigned int p, int_32 *c)
{
    unsigned char ra = b[p];

    if (utf8_mbcp(ra))
    {
        switch (ra & 0xf0)
        {
            case 0xf0:
                if ((b[p+1] == 0) || (b[p+2] == 0) || (b[p+3] == 0))
                {
                    *c = 0;
                    return p;
                }

                *c = ((ra & 0x07) << 18) | (((b[p+1]) & 0x3f) << 12)
                      | (((b[p+2]) & 0x3f) << 6) | ((b[p+3]) & 0x3f);

                p += 4; break;

            case 0xe0:
                if ((b[p+1] == 0) || (b[p+2] == 0))
                {
                    *c = 0;
                    return p;
                }

                *c = ((ra & 0x0f) << 12) | (((b[p+1]) & 0x3f) << 6)
                        | ((b[p+2]) & 0x3f);

                p += 3; break;

            case 0xc0:
            case 0xd0:
                if (b[p+1] == 0)
                {
                    *c = 0;
                    return p;
                }

                *c = ((ra & 0x1f) << 6) | ((b[p+1]) & 0x3f);

                p += 2; break;

            default:
                *c = 0;
                p++;
        }
    }
    else
    {
        *c = ra;
        p++;
    }

    return p;
}

unsigned int utf8_encode (int_8 *b, unsigned int c)
{
    if (c <= 0x7f)
    {
        b[0] = c;

        return 1;
    }
    else if (c <= 0x7ff)
    {
        b[0] = 0xc0 | ((c >> 6)  & 0x1f);
        b[1] = 0x80 | ( c        & 0x3f);

        return 2;
    }
    else if (c <= 0xffff)
    {
        b[0] = 0xe0 | ((c >> 12) & 0xf );
        b[1] = 0x80 | ((c >> 6)  & 0x3f);
        b[2] = 0x80 | ( c        & 0x3f);

        return 3;
    }
    else /* if (c <= 0x10ffff) */
    {
        b[0] = 0xf0 | ((c >> 18) & 0x7 );
        b[1] = 0x80 | ((c >> 12) & 0x3f);
        b[2] = 0x80 | ((c >> 6)  & 0x3f);
        b[3] = 0x80 | ( c        & 0x3f);

        return 4;
    }
}
