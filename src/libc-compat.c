/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

/* weak stubs for stuff in this file, so that when linked with a libc things
 * from the libc are used */

#pragma weak cexit
#pragma weak abort
#pragma weak free
#pragma weak malloc
#pragma weak realloc
#pragma weak strcat
#pragma weak strcpy
#pragma weak strlen
#pragma weak strncmp
#pragma weak memset
#pragma weak memcpy
#pragma weak memcmp
#pragma weak calloc

void cexit (int status) {}

void *malloc (unsigned long length)
{
    void *r = aalloc(length);

    tree_add_node_value (&size_map, (int_pointer)r, (void *)(int_pointer)length);

    return r;
}

void free(void *ptr)
{
    if (ptr != (void *)0)
    {
        struct tree_node *node = tree_get_node (&size_map, (int_pointer)ptr);

        if (node != (struct tree_node *)0)
        {
            afree((unsigned long)node_get_value(node), ptr);
            tree_remove_node (&size_map, (int_pointer)ptr);
        }
    }
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

        if (node != (struct tree_node *)0)
        {
            unsigned long len = (unsigned long)node_get_value(node);

            if (len != length)
            {
                void *r = arealloc (len, ptr, length);

                tree_remove_node (&size_map, (int_pointer)ptr);
                tree_add_node_value (&size_map, (int_pointer)r,
                                     (void *)(int_pointer)length);

                ptr = r;
            }
        }

        return ptr;
    }
}

void abort ( void )
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

int memcmp(const void *s1, const void *s2, unsigned long count)
{
    for (long i = 0; i < count; i++)
    {
        if (((unsigned char *)s1)[i] == ((unsigned char *)s2)[i]) continue;
        return (((unsigned char *)s2)[i] - ((unsigned char *)s1)[i]);
    }

    return 0;
}

int strncmp(const char *s1, const char *s2, unsigned long count)
{
    for (long i = 0; (i < count) && (s1[i] != 0) && (s2[i] != 0); i++)
    {
        if ((unsigned char)(s1[i]) == (unsigned char)(s2[i])) continue;
        return ((unsigned char)(s2[i]) - (unsigned char)(s1[i]));
    }

    return 0;
}

void *calloc (unsigned long length)
{
    void *ptr = malloc (length);

    memset (ptr, 0, length);

    return ptr;
}

/* fuck SSP. learn how to programme, bitches */

#pragma weak __stack_chk_fail
#pragma weak __guard
#pragma weak __stack_smash_handler

unsigned long int __stack_chk_fail = 0;
unsigned long int __guard = 0;
void __stack_smash_handler ( void *p ) {}

/* and this is really only relevant on ARM with recent compilers, but i
 * figured i might as well put it in the general file... */

#pragma weak __aeabi_unwind_cpp_pr0

void __aeabi_unwind_cpp_pr0 ( void ) {}

