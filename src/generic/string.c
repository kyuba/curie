/*
 *  string.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 10/06/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
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

/* this hash function is based on this one:
 * http://www.azillionmonkeys.com/qed/hash.html */

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

static int_32 str_hash_realigned (const char *data, unsigned long len)
{
    int_32 hash = 0, tmp;
    int rem;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 4;
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[2] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

// data is guarranteed to be 32-bit aligned)
int_32 str_hash(const char *data, unsigned long *len)
{
    int_32 hash = 0, tmp, mask;
    int rem = 0;
    unsigned long lent = 0;

    if (((int_pointer)data % 4) != (int_pointer)0)
    {
        unsigned long clen = 0;
        while (data[clen] != (const char)0) clen++;

        if (clen == 0)
        {
            *len = 0;
            return 0;
        }

        char buffer[clen];

        clen = 0;
        while (data[clen] != (const char)0)
        {
            buffer[clen] = data[clen];
            clen++;
        }

        *len = clen;

        return str_hash_realigned (buffer, clen);
    }

    // this unrolls to better optimized code on most compilers
    do {
        mask = HAS_ZERO_BYTE(*((int_32 *)(data)));
        if (mask == 0) {
            // Do the hash calculation
            hash += get16bits(data);
            tmp   = (get16bits(data + 2) << 11) ^ hash;
            hash  = (hash << 16) ^ tmp;
            hash += hash >> 11;
            data += 4;
            // Increase len as well
            lent += 4;
        }
    } while (mask == 0);

    // Now we've got out of the loop, because we hit a zero byte,
    // find out which exactly
    if (data[0] == 0)
        rem = 0;
    else if (data[1] == 0)
        rem = 1;
    else if (data[2] == 0)
        rem = 2;
    else if (data[3] == 0)
        rem = 3;

    /*
     * Handle end cases 
     */
    switch (rem) {
    case 3:
        hash += get16bits(data);
        hash ^= hash << 16;
        hash ^= data[2] << 18;
        hash += hash >> 11;
        break;
    case 2:
        hash += get16bits(data);
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
    *len = lent + rem;

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
