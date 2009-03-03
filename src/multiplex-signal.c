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
#include <curie/signal.h>
#include <curie/signal-system.h>
#include <curie/tree.h>
#include <curie/memory.h>

struct handler {
    enum signal signal;
    enum signal_callback_result (*handler)(enum signal, void *);
    /*@temp@*/ void *data;
    /*@only@*/ struct handler *next;
};

/*@null@*/ /*@only@*/ static struct handler *signal_handlers
        = (struct handler *)0;

/*@-branchstate@*/
static void invoke (enum signal signal) {
    struct handler *h = signal_handlers, *hp = (struct handler *)0;

    while (h != (struct handler *)0) {
        if ((h->signal == signal) &&
            (h->handler (signal, h->data) == scr_ditch)) {
            /*@-mustfree*/
            if (hp == (struct handler *)0) {
                signal_handlers = h->next;
                afree (sizeof(struct handler), (void *)h);
                h = signal_handlers;
            } else {
                hp->next = h->next;
                afree (sizeof(struct handler), (void *)h);
                h = hp->next;
            }
            /*@=mustfree*/
            continue;
        }

        hp = h;
        h = h->next;
    }
}
/*@=branchstate@*/

static void queue_on_read (struct io *qin, /*@unused@*/ void *u) {
    unsigned int position = (unsigned int)(qin->position / sizeof(enum signal)),
                 length   = (unsigned int)(qin->length / sizeof(enum signal));

    enum signal *buffer = (enum signal *)qin->buffer;
    if (buffer == (enum signal *)0) return;

    while (position < length) {
        invoke (buffer[position]);

        position++;
    }

    qin->position = (unsigned int)(position * sizeof(enum signal));
}

/*@null@*/ /*@only@*/ static struct io *signal_queue = (struct io *)0;

static void queue_on_close (/*@unused@*/ struct io *qin, /*@unused@*/ void *u) {
    signal_queue = io_open_special();
    if (signal_queue != (struct io *)0) {
        multiplex_add_io (signal_queue, queue_on_read, queue_on_close, (void *)0);
    }
}

static void generic_signal_handler (enum signal signal) {
    if (signal_queue == (struct io *)0) return;
    (void)io_write (signal_queue,
                    (char *)&signal,
                    (unsigned int)sizeof(enum signal));
}

/*@-globstate -memtrans@*/
void multiplex_signal () {
    static char installed = (char)0;

    if (installed == (char)0) {
        int i;

        multiplex_io();

        for (i = 0; i < SIGNAL_MAX_NUM; i++) {
            a_set_signal_handler ((enum signal)i, generic_signal_handler);
        }

        if ((signal_queue = io_open_special()) == (struct io *)0)
        {
            return;
        }

        multiplex_add_io (signal_queue, queue_on_read, queue_on_close, (void *)0);

        installed = (char)1;
    }
}
/*@=globstate =memtrans@*/

void multiplex_add_signal (enum signal signal, enum signal_callback_result (*handler)(enum signal, void *), void *data) {
    struct handler *element = aalloc (sizeof(struct handler));

    if (element == (struct handler *)0) return;

    element->signal = signal;
    element->data = data;
    element->handler = handler;

    /*@-mustfree@*/
    element->next = signal_handlers;
    /*@=mustfree@*/
    signal_handlers = element;
}

void send_signal (enum signal signal, int pid) {
    a_kill (signal, pid);
}

void send_signal_self (enum signal signal) {
    a_kill (signal, a_getpid());
}
