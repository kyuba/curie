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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/sexpr-internal.h>

static struct sexpr_type_descriptor *sxcd = (struct sexpr_type_descriptor *)0;

void sx_register_type
        (unsigned int type,
         sexpr (*serialise) (sexpr), sexpr (*unserialise) (sexpr),
         void (*tag) (sexpr), void (*destroy) (sexpr), void (*call) (),
         sexpr (*equalp) (sexpr, sexpr))
{
    static struct memory_pool pool =
            MEMORY_POOL_INITIALISER(sizeof (struct sexpr_type_descriptor));
    struct sexpr_type_descriptor *d = get_pool_mem (&pool);

    if (d == (struct sexpr_type_descriptor *)0)
    {
        return;
    }

    d->type        = type;
    d->serialise   = serialise;
    d->unserialise = unserialise;
    d->tag         = tag;
    d->destroy     = destroy;
    d->call        = call;
    d->equalp      = equalp;
    d->next        = sxcd;

    sxcd = d;
}

struct sexpr_type_descriptor *sx_get_descriptor (unsigned int type)
{
    struct sexpr_type_descriptor *d = sxcd;

    while (d != (struct sexpr_type_descriptor *)0)
    {
        if (d->type == type)
        {
            return d;
        }

        d = d->next;
    }

    return (struct sexpr_type_descriptor *)0;
}

void sx_call_custom ( void )
{
    struct sexpr_type_descriptor *d = sxcd;

    while (d != (struct sexpr_type_descriptor *)0)
    {
        if (d->call != (void *)0)
        {
            d->call ();
        }

        d = d->next;
    }
}
