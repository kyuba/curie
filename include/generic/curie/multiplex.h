/*
 *  multiplex.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 03/06/2008.
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

/*! \file
 *  \brief Process Multiplexer
 *
 *  Multiplex events from different sources, such as I/O reads and writes.
 */

#ifndef LIBCURIE_MULTIPLEX_H
#define LIBCURIE_MULTIPLEX_H

#include <curie/io.h>
#include <curie/sexpr.h>
#include <curie/exec.h>

#ifdef __cplusplus
extern "C" {
#endif

struct multiplex_functions {
    /*@notnull@*/ void (*count)(int *, int *);
    /*@notnull@*/ void (*augment)(int *, int *, int *, int *);
    /*@notnull@*/ void (*callback)(int *, int, int *, int);

    /*@null@*/ struct multiplex_functions *next;
};

enum multiplex_result {
    mx_ok = 0,
    mx_nothing_to_do = 1
};

enum multiplex_result multiplex ();

void multiplex_add (/*@notnull@*/ struct multiplex_functions *);

void multiplex_io ();
void multiplex_process ();
void multiplex_sexpr ();

void multiplex_add_io (/*@notnull@*/ /*@only@*/ struct io *io,
                       /*@null@*/ void (*on_read)(struct io *, void *),
                       /*@null@*/ void (*on_close)(struct io *, void *),
                       /*@null@*/ void *d);
void multiplex_add_io_no_callback (/*@notnull@*/ /*@only@*/ struct io *io);
void multiplex_del_io (/*@notnull@*/ /*@only@*/ struct io *io);

void multiplex_add_process (/*@notnull@*/ /*@only@*/ struct exec_context *context,
                            /*@null@*/ void (*on_death)(struct exec_context *, void *),
                            /*@null@*/ void *d);
void multiplex_add_sexpr (/*@notnull@*/ /*@only@*/ struct sexpr_io *io,
                          /*@null@*/ void (*on_read)(struct sexpr *, struct sexpr_io *, void *),
                          /*@null@*/ void *d);

#ifdef __cplusplus
}
#endif

#endif
