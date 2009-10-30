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

#include <curie/hash.h>

/* --- MurmurHash2 ---------------------------------------------------------- */

int_32 hash_murmur2_32 ( const void * key, int len, unsigned int seed )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    int_32 h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

/* since this is the the file to use on 64-bit platforms, we use the optimised
   version of this hash for 64-bit platforms */

int_64 hash_murmur2_64 ( const void * key, int len, unsigned int seed )
{
    const int_64 m = 0xc6a4a7935bd1e995;
    const int r = 47;

    int_64 h = seed ^ (len * m);

    const int_64 * data = (const int_64 *)key;
    const int_64 * end = data + (len/8);

    while(data != end)
    {
        int_64 k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
    case 7: h ^= ((int_64)(data2[6])) << 48;
    case 6: h ^= ((int_64)(data2[5])) << 40;
    case 5: h ^= ((int_64)(data2[4])) << 32;
    case 4: h ^= ((int_64)(data2[3])) << 24;
    case 3: h ^= ((int_64)(data2[2])) << 16;
    case 2: h ^= ((int_64)(data2[1])) << 8;
    case 1: h ^= ((int_64)(data2[0]));
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

/* see comment above for hash_murmur2_64().
   note that this is a copy of the "corresponding" hash function, because the
   murmurhash2 is pretty small to begin with, so small that the overhead of
   indirecting the function call would probably be worse than just copying
   the function. of course modern compilers are supposed to inline... but meh,
   it's not that big of a deal anyway. */

int_pointer hash_murmur2_pt ( const void * key, int len, unsigned int seed )
{
    const int_64 m = 0xc6a4a7935bd1e995;
    const int r = 47;

    int_64 h = seed ^ (len * m);

    const int_64 * data = (const int_64 *)key;
    const int_64 * end = data + (len/8);

    while(data != end)
    {
        int_64 k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
    case 7: h ^= ((int_64)(data2[6])) << 48;
    case 6: h ^= ((int_64)(data2[5])) << 40;
    case 5: h ^= ((int_64)(data2[4])) << 32;
    case 4: h ^= ((int_64)(data2[3])) << 24;
    case 3: h ^= ((int_64)(data2[2])) << 16;
    case 2: h ^= ((int_64)(data2[1])) << 8;
    case 1: h ^= ((int_64)(data2[0]));
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

/* --- END --- MurmurHash2 -------------------------------------------------- */

/* --- SuperFastHash -------------------------------------------------------- */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const int_16 *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((int_32)(((const int_8 *)(d))[1])) << 8)\
                       +(int_32)(((const int_8 *)(d))[0]) )
#endif

int_32      hash_superfast ( const int_8 * data, int len )
{
    int_32 hash = len, tmp;
    int rem;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (int_16);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (int_16)] << 18;
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

/* --- END --- SuperFastHash ------------------------------------------------ */
