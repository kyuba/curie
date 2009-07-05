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

static sexpr *gc_tags, *gc_calls, **gc_roots;
static unsigned long gc_tag_size, gc_call_size, gc_roots_size = 0, gc_tags_i, gc_calls_i;
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

static void gc_add_to_list
        (sexpr sx, sexpr **index, unsigned long *cursor, unsigned long *len)
{
    if (!cancel)
    {
        sexpr *map = *index;
        unsigned long i = *cursor;

        if ((map + i) < (sexpr*)((char *)map + (*len)))
        {
          add:
            map[i] = sx;
            (*cursor)++;
            return;
        }

        map = resize_mem ((*len), (*index), (*len) + LIBCURIE_PAGE_SIZE);
        if (map == (sexpr *)0)
        {
            cancel = 1;
            return;
        }

        *index = map;
        (*len) += LIBCURIE_PAGE_SIZE;

        goto add;
    }
}

static void remove_dupes (sexpr *index, sexpr *cursor)
{
    sexpr *i, *j;

    for (i = index; i < cursor; i++)
    {
        sexpr deref = *i;

        for (j = i + 1; j < cursor; j++)
        {
            if (deref == (*j))
            {
                *j = (sexpr)0;
            }
        }
    }
}

void gc_tag (sexpr sx)
{
    gc_add_to_list (sx, &gc_tags, &gc_tags_i, &gc_tag_size);
}

void gc_call (sexpr sx)
{
    gc_add_to_list (sx, &gc_calls, &gc_calls_i, &gc_call_size);
}

static int gc_initialise_memory ()
{
    gc_tag_size  = LIBCURIE_PAGE_SIZE;
    gc_tags      = get_mem (LIBCURIE_PAGE_SIZE);
    gc_tags_i    = 0;

    if (gc_tags == (sexpr *)0)
    {
        return 0;
    }

    gc_call_size = LIBCURIE_PAGE_SIZE;
    gc_calls     = get_mem (LIBCURIE_PAGE_SIZE);
    gc_calls_i   = 0;

    if (gc_calls == (sexpr *)0)
    {
        free_mem (LIBCURIE_PAGE_SIZE, gc_tags);
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
    if (gc_tags  != (sexpr *)0)
    {
        free_mem (gc_tag_size,  gc_tags);
    }
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
    unsigned int i, j, k, m;
    unsigned long rv = 0;

    if (!gc_initialise_memory ()) return 0;
    sx_call_all();
    if (cancel)
    {
        goto end;
    }

    for (t = stack_start_address; t != l; t += step)
    {
        sexpr e = *t;

        if (pointerp (e) && (e != (sexpr)0))
        {
            gc_tag (e);
            if (cancel)
            {
                goto end;
            }
        }
    }

    for (j = 0, m = (gc_tag_size / sizeof (sexpr)),
         k = (gc_call_size / sizeof (sexpr)); j < m; j++)
    {
        sexpr sx = gc_tags[j];

        if (sx != (sexpr)0)
        {
            int sx_e = 0;

            for (i = 0; i < k; i++)
            {
                if (gc_calls[i] == sx)
                {
                    sx_e = 1;
                    break;
                }
            }

            if (!sx_e)
            {
                gc_tags[j] = (sexpr)0;
            }
        }
    }

    for (j = 0; j < (gc_tag_size / sizeof (sexpr)); j++)
    {
        sexpr sx = gc_tags[j];

        if (sx != (sexpr)0)
        {
            sx_tag_sub (sx);
            if (cancel)
            {
                goto end;
            }
        }
    }

    remove_dupes (gc_tags,  gc_tags + gc_tags_i);
    remove_dupes (gc_calls, gc_calls + gc_calls_i);

    for (i = 0, k = (gc_call_size / sizeof (sexpr)),
         m = (gc_tag_size / sizeof (sexpr)); i < k; i++)
    {
        sexpr sx = gc_calls[i];

        if (sx != (sexpr)0)
        {
            char sx_e = 0;

            for (j = 0; j < m; j++)
            {
                if (gc_tags[j] == sx)
                {
                    sx_e = 1;
                    break;
                }
            }

            if (!sx_e)
            {
                sx_destroy (sx);
                rv++;
            }
        }
    }

  end:
    gc_deinitialise_memory ();

    return rv;
}
