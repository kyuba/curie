/*
 *  io++.c++
 *  libcurie++
 *
 *  Created by Magnus Deininger on 21/10/2008.
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

#include <curie++/io.h>

using namespace curiepp;

IO::IO()
{
    filename = sx_nonexistent;
}

IO::IO(sexpr f) : filename (f)
{
    sx_xref (filename);
    filename = sx_nonexistent;
}

IO::IO(char *f)
{
    filename = make_string (f);
}

IO::IO (int fd)
{
    context = io_open (fd);
    filename = sx_nonexistent;
}

IO::IO (int fd, io_type type)
{
    context = io_open (fd);
    if (context != (struct io *)0) context->type = type;
    filename = sx_nonexistent;
}

IO::~IO()
{
    if (context != (struct io *)0) io_close (context);
    sx_destroy(filename);
}

void IO::open()
{
    this->open (filename, iot_read);
}

void IO::open (sexpr &f)
{
    this->open (f, iot_read);
}

void IO::open (io_type type)
{
    this->open (filename, type);
}

void IO::open (sexpr &f, io_type type)
{
    if (!stringp(f)) return;

    switch (type)
    {
        case iot_read:
            context = io_open_read(sx_string(f));
            return;
        case iot_write:
            context = io_open_write(sx_string(f));
            return;
        default:
            return;
    }
}

enum io_result IO::read ()
{
    if (context == (struct io *)0) this->open(iot_read);
    if (context == (struct io *)0) return io_unrecoverable_error;

    return io_read(context);
}

enum io_result IO::collect (const char *data, int_pointer length)
{
    if (context == (struct io *)0) this->open(iot_write);
    if (context == (struct io *)0) return io_unrecoverable_error;

    return io_collect (context, data, length);
}

enum io_result IO::write (const char *data, int_pointer length)
{
    if (context == (struct io *)0) this->open(iot_write);
    if (context == (struct io *)0) return io_unrecoverable_error;

    return io_write (context, data, length);
}

enum io_result IO::commit ()
{
    if (context == (struct io *)0) return io_complete;

    return io_commit(context);
}

enum io_result IO::finish ()
{
    if (context == (struct io *)0) return io_complete;

    return io_finish(context);
}

void IO::close ()
{
    io_close (context);
}
