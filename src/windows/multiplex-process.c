/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

#include <curie/multiplex-system.h>
#include <curie/memory.h>
#include <curie/signal.h>

#include <curie/exec-system.h>

#include <windows.h>

static enum multiplex_result mx_f_count(int *r);
static void mx_f_augment(void **rs, int *r);
static void mx_f_callback(void **rs, int r);

static struct multiplex_functions mx_functions = {
    mx_f_count,
    mx_f_augment,
    mx_f_callback,
    (struct multiplex_functions *)0
};

struct exec_cx {
    struct exec_context *context;
    void (*on_death)(struct exec_context *, void *);
    void *data;
    struct exec_cx *next;
};

static struct exec_cx *elements = (struct exec_cx *)0;

static enum multiplex_result mx_f_count(int *r) {
    struct exec_cx *l;

    for (l = elements; l != (struct exec_cx *)0; l = l->next)
    {
        (*r)++;
    }
    
    return mx_ok;
}

static void mx_f_augment(void **rs, int *r)
{
    struct exec_cx *l;

    for (l = elements; l != (struct exec_cx *)0; l = l->next)
    {
        int i, t = *r;
        void *handle = l->context->handle;
        
        if (handle != (void *)0)
        {
            for (i = 0; i < t; i++) {
                if (rs[i] == handle) {
                    goto next;
                }
            }
            rs[i] = handle;
            (*r) += 1;
        }

        next:;
    }
}

static void mx_f_callback(void **rs, int r)
{
    struct exec_cx *l, *q;
    struct exec_cx **p;

    for (l = elements; l != (struct exec_cx *)0; l = l->next)
    {
        struct exec_context *cx = l->context;

        if ((cx != (struct exec_context *)0) && (cx->status == ps_running))
        {
            check_exec_context (cx);
            if (cx->status == ps_terminated) {
                l->on_death (cx, l->data);

                for (q = elements; q != (struct exec_cx *)0; q = q->next)
                {
                    if (q->context == (struct exec_context *)0)
                    {
                        q->on_death (cx, q->data);
                    }
                }
                continue;
            }
        }
    }

    p = (void *)&elements;

    for (l = elements; l != (struct exec_cx *)0; l = l->next)
    {
        struct exec_context *cx = l->context;
        if (cx->status == ps_terminated)
        {
            (*p) = l->next;
            free_pool_mem((void *)cx);
            free_pool_mem((void *)l);
            l = (*p);
            if (l == (struct exec_cx *)0) return;
            continue;
        }

        p = &(l->next);
    }
}

void multiplex_all_processes ()
{
    multiplex_process ();
}

void multiplex_process ()
{
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_add (&mx_functions);
        installed = (char)1;
    }
}

void multiplex_add_process (struct exec_context *context, void (*on_death)(struct exec_context *, void *), void *data) {
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct exec_cx));
    struct exec_cx *element = get_pool_mem (&pool);

    element->context = context;
    element->on_death = on_death;
    element->data = data;
    element->next = elements;
    elements = element;
}
