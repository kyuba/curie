/*
 *  string.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 10/06/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curie/string.h>
#include <curie/memory.h>

/*@-shiftimplementation +charint@*/
/* markos' string hash function requires these. i'm totally not gonna muck
   around with that :D */

/**** begin ***** markos' hashing function ************************************/

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const int_16 *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((int_32)(((const int_8 *)(d))[1])) << 8)\
                       +(int_32)(((const int_8 *)(d))[0]) )
#endif

// Taken from
// http://graphics.stanford.edu/~seander/bithacks.html#ZeroInWord
#define HAS_ZERO_BYTE(v) (~((((v & 0x7F7F7F7FUL) + 0x7F7F7F7FUL) | v) | 0x7F7F7F7FUL))

// data is guarranteed to be 32-bit aligned)
int_32 str_hash(const char *data, unsigned long *len)
{
    int_32 hash = 0, tmp, lw, mask, *p32;
    int rem = 0;
    char *p = (char *) data;
    *len = 0;

    // this unrolls to better optimized code on most compilers
    do {
        p32 = (int_32 *) (p);
        lw = *p32;
        mask = (int_32)HAS_ZERO_BYTE(lw);
        // if mask == 0 p32 has no zero in it
        if (mask == 0) {
            // Do the hash calculation
            hash += get16bits(p);
            tmp = (get16bits(p + 2) << 11) ^ hash;
            hash = (hash << 16) ^ tmp;
            hash += hash >> 11;
            p += sizeof(int_32);
            // Increase len as well
            (*len)++;
        }
    } while (mask == 0);

    // Now we've got out of the loop, because we hit a zero byte,
    // find out which exactly
    if (p[0] == '\0')
        rem = 0;
    if (p[1] == '\0')
        rem = 1;
    if (p[2] == '\0')
        rem = 2;
    if (p[3] == '\0')
        rem = 3;

    /*
     * Handle end cases 
     */
    switch (rem) {
    case 3:
        hash += get16bits(p);
        hash ^= hash << 16;
        hash ^= p[2] << 18;
        hash += hash >> 11;
        break;
    case 2:
        hash += get16bits(p);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1:
        hash += *data;
        hash ^= hash << 10;
        hash += hash >> 1;
    }
    // len was calculated in 4-byte tuples,
    // multiply it by 4 to get the number in bytes
    *len = (*len << 2) + rem;

    /*
     * Force "avalanching" of final 127 bits 
     */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

/**** end ******* markos' hashing function ************************************/

int_32 str_hash_unaligned(const char *string, unsigned long *len)
{
    unsigned int length;

    for (length = 0; string[length] != (char)0; length++);
    length++;

    if (((length % 8) == 0) && ((((int_pointer)string) % 8) == 0)) {
        /* must be suitably aligned */

        return str_hash (string, len);
    } else {
        char *r;
        unsigned int i;
        int_32 rv = 0;

        r = get_mem (length);
        /* the return value of this is always suitable for our purposes. */

        for (i = 0; string[i] != (char)0; i++) r[i] = string[i];

        do {
            r[i] = 0;
            i++;
        } while (i < (length + (8-(length % 8))));

        rv = str_hash (r, len);

        free_mem (length, r);

        return rv;
    }
}
