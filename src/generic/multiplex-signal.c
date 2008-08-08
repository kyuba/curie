/*
 *  multiplex-signal.c
 *  atomic-libc
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

#include <atomic/multiplex.h>
#include <atomic/signal.h>
#include <atomic/signal-system.h>
#include <atomic/network.h>
#include <atomic/tree.h>
#include <atomic/memory.h>

struct handler {
    enum signal signal;
    void (*handler)(enum signal, void *);
    void *data;
    struct handler *next;
};

static struct handler *signal_handlers = (struct handler *)0;

static void invoke (enum signal signal) {
    struct handler *h = signal_handlers;

    while (h != (struct signal_handlers *)0) {
        if (h->signal == signal) {
            h->handler (signal, h->data);
        }

        h = h->next;
    }
}

static void queue_on_read (struct io *qin, void *notused) {
    int position = qin->position / sizeof(enum signal),
        length = qin->length / sizeof(enum signal);

    enum signal *buffer = (enum signal *)qin->buffer;

    while (position < length) {
        invoke (buffer[position]);

        position++;
    }

    qin->position = position * sizeof(enum signal);
}

static struct io *signal_queue_out;

static void generic_signal_handler (enum signal signal) {
    io_write (signal_queue_out, (char *)&signal, sizeof(enum signal));
}

void multiplex_signal () {
    static char installed = (char)0;

    if (installed == (char)0) {
        static struct io *signal_queue_in;
        int i;

        multiplex_io();

        for (i = 0; i < SIGNAL_MAX_NUM; i++) {
            a_set_signal_handler ((enum signal)i, generic_signal_handler);
        }

        net_open_loop (&signal_queue_in, &signal_queue_out);

        multiplex_add_io (signal_queue_in, queue_on_read, (void *)0);
        multiplex_add_io_no_callback (signal_queue_out);

        installed = (char)1;
    }
}

void multiplex_add_signal (enum signal signal, void (*handler)(enum signal, void *), void *data) {
    struct handler *element = aalloc (sizeof(struct handler));

    element->signal = signal;
    element->data = data;
    element->handler = handler;

    element->next = signal_handlers;
    signal_handlers = element;
}

void send_signal (enum signal signal, int pid) {
    a_kill (signal, pid);
}

void send_signal_self (enum signal signal) {
    a_kill (signal, a_getpid());
}
