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
#include <curie/memory.h>

static sexpr *gc_tags,  *gc_tags_pointer,
             *gc_calls, *gc_calls_pointer,
             **gc_roots;
static int_pointer gc_tag_size,
                   gc_call_size,
                   gc_roots_size = 0;

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
    if ((sexpr *)((char *)gc_tags + gc_tag_size) > gc_tags_pointer)
    {
        tag:
        *gc_tags_pointer = sx;
        gc_tags_pointer++;
        return;
    }

    gc_tags = resize_mem (gc_tag_size, gc_tags,
                          gc_tag_size + LIBCURIE_PAGE_SIZE);
    gc_tag_size += LIBCURIE_PAGE_SIZE;
    goto tag;
}

void gc_call (sexpr sx)
{
    if ((sexpr *)((char *)gc_calls + gc_call_size) > gc_calls_pointer)
    {
        tag:
        *gc_calls_pointer = sx;
        gc_calls_pointer++;
        return;
    }

    gc_calls = resize_mem (gc_call_size, gc_calls,
                           gc_call_size + LIBCURIE_PAGE_SIZE);
    gc_call_size += LIBCURIE_PAGE_SIZE;
    goto tag;
}

static void gc_initialise_memory ()
{
    sexpr **rootp, **roote;

    gc_tag_size      = LIBCURIE_PAGE_SIZE;
    gc_call_size     = LIBCURIE_PAGE_SIZE;

    gc_tags          = get_mem (LIBCURIE_PAGE_SIZE);
    gc_calls         = get_mem (LIBCURIE_PAGE_SIZE);
    gc_tags_pointer  = gc_tags;
    gc_calls_pointer = gc_calls;

    for (rootp = gc_roots, roote = (sexpr**)((char *)rootp + gc_roots_size);
         rootp < roote; rootp++)
    {
        if ((*rootp) != (sexpr *)0)
        {
            gc_tag (**rootp);
        }
    }
}

static void gc_deinitialise_memory ()
{
    free_mem (gc_tag_size,  gc_tags);
    free_mem (gc_call_size, gc_calls);
}

void gc_invoke ()
{
    int step = (stack_growth == sg_down) ? -1 : 1;
    sexpr end = sx_end_of_list;
    sexpr *t, *l = &end;

    gc_initialise_memory ();

    for (t = stack_start_address; t != l; t += step)
    {
        sexpr e = *t;

        if (pointerp (e))
        {
            gc_tag (e);
        }
    }

    gc_deinitialise_memory ();
}
