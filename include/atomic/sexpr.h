/*
 *  sexpr.h
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 01/06/2008.
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

#ifndef ATOMIC_SEXPR_H
#define ATOMIC_SEXPR_H

#include <atomic/parser.h>

enum sx_type {
  sxt_nil,
  sxt_false,
  sxt_true,
  sxt_integer,
  sxt_string,
  sxt_symbol,
  sxt_cons,
  sxt_empty_list,
  sxt_end_of_list,
  sxt_end_of_file,
  sxt_not_a_number
};

struct sexpr {
  enum sx_type type;

  union {
    struct {
	  struct sexpr *car;
	  struct sexpr *cdr;
	} cons;

	const char *string;
	const char *symbol;

	int integer;
  } data;
};

struct sexpr_io {
  struct io *in, *out;
};

struct sexpr_io *sx_open_io(struct io *, struct io *);
struct sexpr_io *sx_open_io_fd(int, int);

struct sexpr *sx_read(struct sexpr_io *);
char sx_write(struct sexpr_io *, struct sexpr *);
void sx_free(struct sexpr *);

struct sexpr *sx_create(enum sx_type);

const struct sexpr * const sx_nil;
const struct sexpr * const sx_false;
const struct sexpr * const sx_true;
const struct sexpr * const sx_empty_list;
const struct sexpr * const sx_end_of_list;
const struct sexpr * const sx_end_of_file;
const struct sexpr * const sx_not_a_number;

#endif
