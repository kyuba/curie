/*
 *  io.h
 *  libcurie
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

/*! \file
 *  \brief File I/O
 *
 *  Functions from this header file provide basic file in- and output
 *  operations.
 */

#ifndef LIBCURIE_IO_H
#define LIBCURIE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#define IO_CHUNKSIZE 4096

enum io_type {
  iot_undefined = 0,
  iot_read = 1,
  iot_write = 2
};

enum io_result {
  io_undefined = 0,
  io_end_of_file = 1,
  io_changes = 2,
  io_unrecoverable_error = 3,
  io_no_change = 4,
  io_complete = 5,
  io_incomplete = 6,
  io_finalising = 7
};

struct io {
    int fd;
    /*@notnull@*/ /*@only@*/ char *buffer;
    /* can't use a fixed buffer since we might need to read (or write)
       something larger */

    enum io_type   type;
    enum io_result status;

    unsigned int length;
    unsigned int position;
    unsigned int buffersize;
};

/*@notnull@*/ /*@only@*/ struct io *io_open (int);
/*@notnull@*/ /*@only@*/ struct io *io_open_read (const char *);
/*@notnull@*/ /*@only@*/ struct io *io_open_write (const char *);
enum io_result io_write(/*@notnull@*/ struct io *, const char *, unsigned int);
enum io_result io_collect(/*@notnull@*/ struct io *, const char *, unsigned int);
enum io_result io_read(/*@notnull@*/ struct io *);
enum io_result io_commit (/*@notnull@*/ struct io *);
enum io_result io_finish (/*@notnull@*/ struct io *);
void io_close (/*@notnull@*/ /*@only@*/ struct io *);

#ifdef __cplusplus
}
#endif

#endif
