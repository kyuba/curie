/*
 *  multiplex-process.c
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
#include <curie/memory.h>
#include <curie/signal.h>

struct exec_cx {
    struct exec_context *context;
    void (*on_death)(struct exec_context *, void *);
    /*@temp@*/ void *data;
};

static struct memory_pool list_pool = MEMORY_POOL_INITIALISER(sizeof (struct exec_cx));

/*@-memtrans@*/
static enum signal_callback_result sig_chld_signal_handler
        (/*@unused@*/ enum signal signal, void *e)
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
/*@=memtrans@*/

void multiplex_process () {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_signal();
        installed = (char)1;
    }
}

/*@-mustfree@*/
void multiplex_add_process (struct exec_context *context, void (*on_death)(struct exec_context *, void *), void *data) {
    struct exec_cx *element = get_pool_mem (&list_pool);

    if (element == (struct exec_cx *)0) return;

    element->context = context;
    element->on_death = on_death;
    element->data = data;

    multiplex_add_signal (sig_chld, sig_chld_signal_handler, (void *)element);
}
/*@=mustfree@*/
