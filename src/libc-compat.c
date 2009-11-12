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

#include <curie/int.h>
#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/main.h>

static struct tree size_map = TREE_INITIALISER;

/* weak stubs for some of the bootstrap things */

#pragma weak cexit

void cexit (int status) {}

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
    if (ptr == (void *)0)
    {
        return malloc (length);
    }
    else
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

/* fuck SSP. learn how to programme, bitches */

unsigned long int __stack_chk_fail = 0;
unsigned long int __guard = 0;
void __stack_smash_handler ( void *p ) {}
