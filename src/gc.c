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

#include <curie/gc.h>
#include <curie/stack.h>
#include <curie/sexpr.h>
#include <curie/sexpr-internal.h>
#include <curie/memory.h>
#include <curie/internal-constants.h>

static sexpr *gc_calls, **gc_roots, *gc_pointer;
static unsigned long gc_call_size, gc_roots_size = 0;
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

static void gc_tag_sub (sexpr sx, sexpr *left, sexpr *right)
{
    sexpr l, r, *p1, *p2;
    int d = 0;

    do
    {
        while ((r = *right) == 0)
        {
            right--;

            if (right < left) return;
        }

        while ((l = *left) == 0)
        {
            left++;

            if (left > right) return;
        }

        if ((l > sx) || (r < sx))
        {
            return;
        }

        if ((l == sx) || (r == sx))
        {
            if (l == sx)
            {
                *left = (sexpr)0;
            }
            else
            {
                *right = (sexpr)0;
            }

            if (consp (sx))
            {
                l = car (sx);
                r = cdr (sx);

                if (pointerp (l))
                {
                    if (pointerp (r))
                    {
                        gc_tag (r);
                    }

                    sx = l;
                    left = gc_calls;
                    right = gc_pointer;
                }
                else if (pointerp (r))
                {
                    sx = r;
                    left = gc_calls;
                    right = gc_pointer;
                }
            }
            else if (customp (sx))
            {
                int type = sx_type (sx);

                struct sexpr_type_descriptor *d = sx_get_descriptor (type);

                if ((d != (struct sexpr_type_descriptor *)0) &&
                    (d->tag != (void *)0))
                {
                    d->tag (sx);
                }

                return;
            }
            else
            {
                return;
            }
        }

        if ((d = (right - left)) > 1)
        {
            p1 = left + (d / 2);
            p2 = p1;

            while ((r = *p1) == 0)
            {
                p1--;

                if (p1 < left) return;
            }

            while ((l = *p2) == 0)
            {
                p2++;

                if (p2 > right) return;
            }

            if (sx >= l)
            {
                left = p2;
            }
            else if (sx <= r)
            {
                right = p1;
            }
            else
            {
                return;
            }
        }
    } while (d > 1);
}

void gc_tag (sexpr sx)
{
    gc_tag_sub (sx, gc_calls, gc_pointer);
}

void gc_call (sexpr sx)
{
    sexpr *map;
    if (cancel) return;

    if (gc_pointer < (sexpr*)((int_pointer)gc_calls + gc_call_size))
    {
      add:
        *gc_pointer = sx;
        gc_pointer++;
        return;
    }

    map = resize_mem (gc_call_size, gc_calls, gc_call_size + LIBCURIE_PAGE_SIZE);

    gc_pointer = (sexpr *)((int_pointer)map +
                   ((int_pointer)gc_pointer - (int_pointer)gc_calls));
    gc_calls = map;
    gc_call_size += LIBCURIE_PAGE_SIZE;

    goto add;
}

static int partition (int left, int right, int pivot)
{
    int index, i;
    sexpr value = gc_calls[pivot], t;

    gc_calls[pivot] = gc_calls[right];
    gc_calls[right] = value;

    index = left;

    for (i = left; i < right; i++)
    {
        if (gc_calls[i] <= value)
        {
            t = gc_calls[i];
            gc_calls[i] = gc_calls[index];
            gc_calls[index] = t;

            index++;
        }
    }

    t = gc_calls[index];
    gc_calls[index] = gc_calls[right];
    gc_calls[right] = t;

    return index;
}

static void sort_calls (int left, int right)
{
    int pivot;

    while (right > left)
    {
        pivot = partition (left, right, left);
        sort_calls (left, pivot - 1);
        left = pivot + 1;
    }
}

static int gc_initialise_memory ()
{
    gc_call_size  = (gc_base_items & (~(LIBCURIE_PAGE_SIZE - 1)))
                  + LIBCURIE_PAGE_SIZE;
    gc_calls     = get_mem (gc_call_size);
    gc_pointer   = gc_calls;

    sx_call_all();
    sx_call_custom();

    if (cancel)
    {
        free_mem (gc_call_size, gc_calls);
        return 0;
    }

    sort_calls (0, gc_pointer - gc_calls - 1);

    if (gc_roots_size != 0)
    {
        sexpr **rootp, **roote;

        for (rootp = gc_roots,
             roote = (sexpr**)((int_pointer)rootp + gc_roots_size);
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

    /* sanity check, if either of these tests fail then either your stack is
       fucked, or your toolchain is useless. */
    if (((stack_growth == sg_down) && (stack_start_address < (void *)l)) ||
        ((stack_growth == sg_up)   && (stack_start_address > (void *)l)))
    {
        return 0;
    }
    /* detect simple alignment errors: */
    if (((int_pointer)l & (~ (sizeof(sexpr) - 1))) != (int_pointer)l) return 0;

    if (!gc_initialise_memory ()) return 0;

    for (t = stack_start_address; t != l; t += step)
    {
        sexpr e = *t;

        if (pointerp (e) && (e != (sexpr)0))
        {
            gc_tag (e);
        }
    }

    for (i = 0, k = (gc_pointer - gc_calls); i < k; i++)
    {
        sexpr sx = gc_calls[i];

        if (sx != (sexpr)0)
        {
            sx_destroy (sx);
            rv++;
        }
    }

    gc_deinitialise_memory ();
    optimise_static_memory_pools ();

    return rv;
}
