/*
 *  multiplex-signal.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 07/08/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curie/multiplex.h>
#include <curie/signal.h>
#include <curie/signal-system.h>
#include <curie/network.h>
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

    while (position < (length / sizeof (enum signal)))
    {
        invoke (buffer[position]);

        position++;
    }

    qin->position = (unsigned int)(position * sizeof(enum signal));
}

/*@null@*/ /*@only@*/ static struct io *signal_queue_out = (struct io *)0;

static void queue_on_close (/*@unused@*/ struct io *qin, /*@unused@*/ void *u) {
    if (signal_queue_out == (struct io *)0) return;
    multiplex_del_io (signal_queue_out);
    signal_queue_out = (struct io *)0;
}

static void generic_signal_handler (enum signal signal) {
    if (signal_queue_out == (struct io *)0) return;
    (void)io_collect (signal_queue_out,
                      (char *)&signal,
                      (unsigned int)sizeof(enum signal));
}

/*@-globstate -memtrans@*/
void multiplex_signal () {
    static char installed = (char)0;

    if (installed == (char)0) {
        static struct io *signal_queue_in, *out;
        int i;

        multiplex_io();

        for (i = 0; i < SIGNAL_MAX_NUM; i++) {
            a_set_signal_handler ((enum signal)i, generic_signal_handler);
        }

        net_open_loop (&signal_queue_in, &out);

        if ((signal_queue_in == (struct io *)0) ||
            (out == (struct io *)0))
        {
            if (signal_queue_in != (struct io *)0) io_close (signal_queue_in);
            if (out != (struct io *)0) io_close (out);
            return;
        }

        /*@-mustfree@*/
        signal_queue_out = out;
        /*@=mustfree@*/

        multiplex_add_io (signal_queue_in, queue_on_read, queue_on_close, (void *)0);
        multiplex_add_io_no_callback (signal_queue_out);

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
