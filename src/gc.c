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

#if GC_TAG_BUFFER == 1
void gc_tag (sexpr sx)
{
    sexpr *c = gc_calls,
          *k = gc_pointer;

#if defined(GC_KILL_ZEROES)
    sexpr *d = gc_calls,
           v;
#endif

#if defined(GC_KILL_ZEROES)
    while (d < k)
#else
    while (c < k)
#endif
    {
#if defined(GC_KILL_ZEROES)
        if ((*c) == (sexpr)0)
        {
            while ((*d) == (sexpr)0)
            {
                d++;

                if (d == k)
                {
                    goto end;
                }
            }

            v    = (*d);

            (*c) = v;
            *d   = (sexpr)0;

            continue;
        }
        else
#endif
        if ((*c) == sx)
        {
            (*c) = (sexpr)0;

            if (consp (sx))
            {
                sexpr sxcar = car (sx), sxcdr = cdr (sx);

                if (pointerp (sxcar))
                {
                    if (pointerp (sxcdr))
                    {
                        gc_tag (sxcdr);
                        k = gc_pointer;
                    }

                    sx = sxcar;
#if defined(GC_KILL_ZEROES)
                    c = d = gc_calls;
#else
                    c = gc_calls;
#endif
                    continue;
                }
                else if (pointerp (sxcdr))
                {
                    sx = sxcdr;
#if defined(GC_KILL_ZEROES)
                    c = d = gc_calls;
#else
                    c = gc_calls;
#endif
                    continue;
                }
            }
            else if (customp (sx))
            {
                /* implement this once custom sexprs are in */
            }

            break;
        }

        c++;
#if defined(GC_KILL_ZEROES)
        d++;
#endif
    }

#if defined(GC_KILL_ZEROES)
  end:
#endif
    k--;
    while ((k > gc_calls) && ((*k) == (sexpr)0)) { k--; }

    gc_pointer = k + 1;
}
#else
void gc_tag (sexpr sx)
{
    sexpr *c, *d, v,
          *k = gc_pointer;

    sexpr n[GC_TAG_BUFFER] = { sx };
    int nc = 1, np = 0, nce/*, lc*/;

/*    do
    {
        lc = 0;*/
        c = d = gc_calls;

        while (d < k)
        {
          next:
            v = *c;

            if (v == (sexpr)0)
            {
              sn:
                while ((*d) == (sexpr)0)
                {
                    d++;

                    if (d == k)
                    {
                        goto end;
                    }
                }

                v    = (*d);
                (*c) = v;
                (*d) = (sexpr)0;

                goto tk;
            }
            else
            {
              tk:
                np = 0;
                do
                {
                    if (v == n[np])
                    {
                        sx = n[np];
                        (*c) = (sexpr)0;
/*                        lc++;*/

                        nc--;
                        while (np < nc)
                        {
                            n[np] = n[(np + 1)];
                            np++;
                        }

                        nce = nc;

                        if (consp (sx))
                        {
                            sexpr sxcar = car (sx), sxcdr = cdr (sx);

                            if (nc > (GC_TAG_BUFFER - 2))
                            {
                                sx = (sexpr)0;

                                if (pointerp (sxcar))
                                {
                                    if (pointerp (sxcdr))
                                    {
                                        gc_tag (sxcdr);
                                        k = gc_pointer;
                                    }

                                    sx = sxcar;
                                }
                                else if (pointerp (sxcdr))
                                {
                                    sx = sxcdr;
                                }

                                if (sx != (sexpr)0)
                                {
                                    v = (sexpr)0;
                                    for (np = 0; np < nc; np++)
                                    {
                                        if (n[np] > sx)
                                        {
                                            v = n[np];
                                            n[np] = sx;
                                            nc++;

                                            while (np < nc)
                                            {
                                                sx = n[np];
                                                n[np] = v;
                                                v = sx;
                                                np++;
                                            }

                                            v = (sexpr)1;

                                            break;
                                        }
                                    }

                                    if (v == (sexpr)0)
                                    {
                                        n[nc] = sx;
                                        nc++;
                                    }
                                }
                            }
                            else
                            {
                                if (pointerp (sxcar))
                                {
                                    v = (sexpr)0;
                                    for (np = 0; np < nc; np++)
                                    {
                                        if (n[np] > sxcar)
                                        {
                                            v = n[np];
                                            n[np] = sxcar;
                                            nc++;

                                            while (np < nc)
                                            {
                                                sx = n[np];
                                                n[np] = v;
                                                v = sx;
                                                np++;
                                            }

                                            v = (sexpr)1;

                                            break;
                                        }
                                    }

                                    if (v == (sexpr)0)
                                    {
                                        n[nc] = sxcar;
                                        nc++;
                                    }
                                }

                                if (pointerp (sxcdr))
                                {
                                    v = (sexpr)0;
                                    for (np = 0; np < nc; np++)
                                    {
                                        if (n[np] > sxcdr)
                                        {
                                            v = n[np];
                                            n[np] = sxcdr;
                                            nc++;

                                            while (np < nc)
                                            {
                                                sx = n[np];
                                                n[np] = v;
                                                v = sx;
                                                np++;
                                            }

                                            v = (sexpr)1;

                                            break;
                                        }
                                    }

                                    if (v == (sexpr)0)
                                    {
                                        n[nc] = sxcdr;
                                        nc++;
                                    }
                                }
                            }
                        }
                        else if (customp (sx))
                        {
                            /* implement this once custom sexprs are in */
                        }

                        if (nc > nce)
                        {
                            c = d = gc_calls;
                            goto next;
                        }
                        else if (nc == 0)
                        {
                            goto end;
                        }
                        else
                        {
                            goto sn;
                        }
                    }

                    np++;
                } while (np < nc);
            }

            c++;
            d++;
        }
/*    } while ((nc > 0) && (lc > 0));*/

  end:
    k--;
    while ((k > gc_calls) && ((*k) == (sexpr)0)) { k--; }

    gc_pointer = k + 1;
}
#endif

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
    if (map == (sexpr *)0)
    {
        free_mem (gc_call_size, gc_calls);
        cancel = 1;
        gc_calls = (sexpr *)0;
        return;
    }

    gc_pointer = (sexpr *)((int_pointer)map +
                   ((int_pointer)gc_pointer - (int_pointer)gc_calls));
    gc_calls = map;
    gc_call_size += LIBCURIE_PAGE_SIZE;

    goto add;
}

static int gc_initialise_memory ()
{
    gc_call_size  = (gc_base_items & (~(LIBCURIE_PAGE_SIZE - 1)))
                  + LIBCURIE_PAGE_SIZE;
    gc_calls     = get_mem (gc_call_size);
    gc_pointer   = gc_calls;

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

        for (rootp = gc_roots, roote = (sexpr**)((int_pointer)rootp + gc_roots_size);
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
