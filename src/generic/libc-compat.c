/*
 *  libc-compat.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 25/01/2009.
 *  Copyright 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2009, Magnus Deininger All rights reserved.
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

#include <curie/int.h>
#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/main.h>

static struct tree size_map = TREE_INITIALISER;

void *malloc (unsigned long length)
{
    void *r = aalloc(length);

    tree_add_node_value (&size_map, (int_pointer)r, (void *)(int_pointer)length);

    return r;
}

void free(void *ptr)
{
    struct tree_node *node = tree_get_node (&size_map, (int_pointer)ptr);

    afree((unsigned long)node_get_value(node), ptr);
    tree_remove_node (&size_map, (int_pointer)ptr);
}

void *realloc (void *ptr, unsigned long length)
{
    struct tree_node *node = tree_get_node (&size_map, (int_pointer)ptr);
    unsigned long len = (unsigned long)node_get_value(node);

    if (len != length)
    {
         void *r = arealloc (len, ptr, length);

         tree_remove_node (&size_map, (int_pointer)ptr);
         tree_add_node_value (&size_map, (int_pointer)r, (void *)(int_pointer)length);

         ptr = r;
    }

    return ptr;
}

void abort ()
{
    cexit (-1);
}

void *memcpy(void *dest, const void *src, unsigned long n)
{
    const char *sc = (const char *)src;
    char *dc = (char *)dest;

    for (unsigned long i = 0; i < n; i++)
    {
        dc[i] = sc[i];
    }

    return dest;
}

void *memset(void *s, int c, unsigned long n)
{
    char *sc = (char *)s;

    for (unsigned long i = 0; i < n; i++)
    {
        sc[i] = c;
    }

    return s;
}

unsigned long strlen(const char *s)
{
    unsigned long i = 0;

    while (s[i] != (const char)0) i++;

    return i;
}

char *strcpy(char *restrict s1, const char *restrict s2)
{
    for (unsigned int i = 0; s2[i] != 0; i++)
    {
        s1[i] = s2[i];
    }

    return s1;
}

char *strcat(char *restrict s1, const char *restrict s2)
{
    unsigned int i = 0;

    while (s1[i] != (const char)0) i++;

    for (unsigned int j = 0; s2[j]; j++, i++)
    {
        s1[i] = s2[j];
    }

    return s1;
}

int memcmp(const unsigned char *s1, const unsigned char *s2, long count)
{
    for (long i = 0; i < count; i++)
    {
        if (s1[i] == s2[i]) continue;
        return (s2[i] - s1[i]);
    }

    return 0;
}

int strncmp(const unsigned char *s1, const unsigned char *s2, long count)
{
    for (long i = 0; (i < count) && (s1[i] != 0) && (s2[i] != 0); i++)
    {
        if (s1[i] == s2[i]) continue;

        return (s2[i] - s1[i]);
    }

    return 0;
}
