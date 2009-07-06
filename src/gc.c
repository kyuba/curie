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

#include <curie/gc.h>
#include <curie/stack.h>
#include <curie/sexpr.h>
#include <curie/sexpr-internal.h>
#include <curie/memory.h>

static sexpr *gc_calls, **gc_roots;
static unsigned long gc_call_size, gc_roots_size = 0, gc_calls_i;
static char cancel = 0;

void gc_add_root (sexpr *sx)
{
    sexpr **rootp, **roote;

    if (gc_roots_size == 0)
    {
        gc_roots_size = LIBCURIE_PAGE_SIZE;
        gc_roots      = get_mem (LIBCURIE_PAGE_SIZE);
    }

  retry:
    for (rootp = gc_roots, roote = (sexpr**)((char *)rootp + gc_roots_size);
         rootp < roote; rootp++)
    {
        if ((*rootp) == (sexpr *)0)
        {
            *rootp = sx;
            return;
        }
    }

    gc_roots = resize_mem (gc_roots_size, gc_roots,
                           gc_roots_size + LIBCURIE_PAGE_SIZE);
    gc_roots_size += LIBCURIE_PAGE_SIZE;

    goto retry;
}

void gc_remove_root (sexpr *sx)
{
    sexpr **rootp, **roote;

    for (rootp = gc_roots, roote = (sexpr**)((char *)rootp + gc_roots_size);
         rootp < roote; rootp++)
    {
        if ((*rootp) == sx)
        {
            *rootp = (sexpr*)0;
            return;
        }
    }
}

void gc_tag (sexpr sx)
{
    unsigned int i, k;

    for (i = 0, k = (gc_call_size / sizeof (sexpr)); i < k; i++)
    {
        if (gc_calls[i] == sx)
        {
            gc_calls[i] = (sexpr)0;
            sx_tag_sub (sx);
        }
    }
}

void gc_call (sexpr sx)
{
    if (cancel) return;
    sexpr *map = gc_calls;

    if ((map + gc_calls_i) < (sexpr*)((char *)map + gc_call_size))
    {
      add:
        map[gc_calls_i] = sx;
        gc_calls_i++;
        return;
    }

    map = resize_mem (gc_call_size, gc_calls, gc_call_size + LIBCURIE_PAGE_SIZE);
    if (map == (sexpr *)0)
    {
        free_mem (gc_call_size, gc_calls);
        cancel = 1;
        gc_calls = (sexpr *)0;
        return;
    }

    gc_calls = map;
    gc_call_size += LIBCURIE_PAGE_SIZE;

    goto add;
}

static int gc_initialise_memory ()
{
    gc_call_size  = (gc_base_items & (~(LIBCURIE_PAGE_SIZE - 1)))
                  + LIBCURIE_PAGE_SIZE;
    gc_calls     = get_mem (gc_call_size);
    gc_calls_i   = 0;

    if (gc_calls == (sexpr *)0)
    {
        return 0;
    }

    sx_call_all();

    if (cancel)
    {
        free_mem (gc_call_size, gc_calls);
        return 0;
    }

    if (gc_roots_size != 0)
    {
        sexpr **rootp, **roote;

        for (rootp = gc_roots, roote = (sexpr**)((char *)rootp + gc_roots_size);
             rootp < roote; rootp++)
        {
            if ((*rootp) != (sexpr *)0)
            {
                gc_tag (**rootp);
            }
        }
    }

    cancel = 0;

    return 1;
}

static void gc_deinitialise_memory ()
{
    if (gc_calls != (sexpr *)0)
    {
        free_mem (gc_call_size, gc_calls);
    }
}

unsigned long gc_invoke ()
{
    int step = (stack_growth == sg_down) ? -1 : 1;
    sexpr end = sx_end_of_list;
    sexpr *t, *l = &end;
    unsigned int i, k;
    unsigned long rv = 0;

    if (!gc_initialise_memory ()) return 0;

    for (t = stack_start_address; t != l; t += step)
    {
        sexpr e = *t;

        if (pointerp (e) && (e != (sexpr)0))
        {
            gc_tag (e);
        }
    }

    for (i = 0, k = (gc_call_size / sizeof (sexpr)); i < k; i++)
    {
        sexpr sx = gc_calls[i];

        if (sx != (sexpr)0)
        {
            sx_destroy (sx);
            rv++;
        }
    }

    gc_deinitialise_memory ();

    return rv;
}
