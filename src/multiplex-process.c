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

#include <curie/multiplex.h>
#include <curie/memory.h>
#include <curie/signal.h>

#include <curie/exec-system.h>

struct exec_cx {
    struct exec_context *context;
    void (*on_death)(struct exec_context *, void *);
    void *data;
    struct exec_cx *next;
};

static struct exec_cx *elements = (struct exec_cx *)0;
static char multiplexer_installed = (char)0;

static enum signal_callback_result sig_chld_signal_handler
        (enum signal signal, void *e)
{
    struct exec_cx *cx = (struct exec_cx *)e;

    if (cx->context->status == ps_running) {
        check_exec_context (cx->context);
        if (cx->context->status == ps_terminated) {
            cx->on_death (cx->context, cx->data);
            free_pool_mem((void *)cx);
            return scr_ditch;
        }
    }

    return scr_keep;
}

/* this is a combined handler that should be nicer on the resources, but it has
   the disadvantage of reaping all child processes. */
static enum signal_callback_result sig_chld_combined_handler
        (enum signal signal, void *e)
{
    int pid, q;

    while ((pid = a_wait_all(&q)) > 0)
    {
        struct exec_cx *cx = elements;
        struct exec_cx **prev = &elements;

        while (cx != (struct exec_cx *)0)
        {
            if ((cx->context->pid == pid) &&
                (cx->context->status == ps_running))
            {
                cx->context->exitstatus = q;
                cx->context->status = ps_terminated;
                cx->on_death (cx->context, cx->data);

                *prev = cx->next;
                free_pool_mem((void *)cx);

                break;
            }

            prev = &(cx->next);
            cx = cx->next;
        }
    }

    return scr_keep;
}

void multiplex_process () {
    if (multiplexer_installed == (char)0) {
        multiplex_signal();
        multiplexer_installed = (char)1;
    }
}

void multiplex_all_processes () {
    if (multiplexer_installed == (char)0) {
        multiplex_signal();
        multiplex_add_signal (sig_chld, sig_chld_combined_handler, (void *)0);
        multiplexer_installed = (char)2;
    }
}

void multiplex_add_process (struct exec_context *context, void (*on_death)(struct exec_context *, void *), void *data) {
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct exec_cx));
    struct exec_cx *element = get_pool_mem (&pool);

    if (element == (struct exec_cx *)0) return;

    element->context = context;
    element->on_death = on_death;
    element->data = data;
    element->next = (struct exec_cx *)0;

    if (multiplexer_installed == (char)1)
    {
        multiplex_add_signal (sig_chld, sig_chld_signal_handler, (void *)element);
    }
    else
    {
        if (elements == (struct exec_cx *)0)
        {
            elements = element;
        }
        else
        {
            struct exec_cx *cx = elements;

            while ((cx->next) != (struct exec_cx *)0)
            {
                cx = cx->next;
            }

            cx->next = element;
        }
    }
}
