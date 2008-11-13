/*
 *  io.c
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

#include <curie/io-system.h>
#include <curie/io.h>
#include <curie/memory.h>

static struct io *io_create ()
{
    static struct memory_pool io_pool = MEMORY_POOL_INITIALISER(sizeof(struct io));
    struct io *io = 0;

    io = get_pool_mem(&io_pool);

    if (io == (struct io *)0) return (struct io *)0;

    /*@-mustfree@*/
    io->buffer = get_mem (IO_CHUNKSIZE);
    /*@=mustfree@*/

    if (io->buffer == (char *)0) {
        free_pool_mem ((void *)io);
        return (struct io *)0;
    }

    io->status = io_undefined;
    io->length = 0;
    io->position = 0;
    io->buffersize = IO_CHUNKSIZE;

    return io;
}

struct io *io_open_special ()
{
    struct io *io = io_create();

    if (io == (struct io *)0) return (struct io *)0;

    io->fd = -1;
    io->type = iot_special_write;

    return io;
}

struct io *io_open (int fd)
{
    struct io *io = io_create();

    (void)a_make_nonblocking (fd);

    io->fd = fd;
    io->type = iot_undefined;

    return io;
}

struct io *io_open_read (const char *path)
{
    int fd = a_open_read (path);
    struct io *io = io_open(fd);

    if (io == (struct io *)0) return (struct io *)0;

    io->type = iot_read;

    return io;
}

struct io *io_open_write (const char *path)
{
    int fd = a_open_write (path);
    struct io *io = io_open(fd);

    if (io == (struct io *)0) return (struct io *)0;

    io->type = iot_write;

    return io;
}

struct io *io_open_create (const char *path, int mode)
{
    int fd = a_create (path, mode);
    struct io *io = io_open(fd);

    if (io == (struct io *)0) return (struct io *)0;

    io->type = iot_write;

    return io;
}

static void relocate_buffer_contents (struct io *io)
{
    if (io->buffer == (char *)0) return;

    if (io->position >= io->length) {
        io->length = 0;
        io->position = 0;
    } else {
        unsigned int i;
        io->length -= io->position;
        for (i = 0; i < io->length; i++) {
            io->buffer[i] = io->buffer[(io->position + i)];
        }
        io->position = 0;
    }
}

#define relocate_buffer(io) if (io->position != 0) relocate_buffer_contents(io)

enum io_result io_collect(struct io *io, const char *data, unsigned int length)
{
    unsigned int i, pos;

    if (io->buffer == (char *)0)
    {
        io->buffersize = 0;
        io->length = 0;
        io->status = io_unrecoverable_error;
        return io_unrecoverable_error;
    }

    if ((io->status == io_finalising) ||
        (io->status == io_end_of_file) ||
        (io->status == io_unrecoverable_error))
        return io->status;

    if (io->type == iot_special_read)
    {
        io->type = iot_special_write;
    }
    else if ((io->type != iot_special_write) && (io->fd == -1))
    {
        io->status = io_unrecoverable_error;
    }

    if ((io->type == iot_write) || (io->type == iot_special_write)) {
        relocate_buffer(io);
    } else {
        io->length = 0;
        io->position = 0;

        io->type = iot_write;
        io->status = io_undefined;
    }

    if ((io->length + length) > io->buffersize) {
        unsigned int newsize = (io->length + length);
        if ((newsize % IO_CHUNKSIZE) != 0) {
            newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
        }

        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);

        if (io->buffer == (char *)0)
        {
            io->buffersize = 0;
            io->length = 0;
            io->status = io_unrecoverable_error;
            return io_unrecoverable_error;
        }

        io->buffersize = newsize;
    }

    for (i = 0, pos = io->length; i < length; i++, pos++) {
        io->buffer[pos] = data[i];
    }

    io->length += length;

    if (io->type == iot_special_write)
    {
        io->status = io_changes;
    }

    return io_incomplete;
}

enum io_result io_write(struct io *io, const char *data, unsigned int length)
{
    enum io_result r = io_collect (io, data, length);
    if (r == io_incomplete) return io_commit(io);
    else return r;
}

enum io_result io_read(struct io *io)
{
    int readrv;

    if (io->buffer == (char *)0)
    {
        io->buffersize = 0;
        io->length = 0;
        io->status = io_unrecoverable_error;
        return io_unrecoverable_error;
    }

    if ((io->status == io_finalising) ||
        (io->status == io_end_of_file) ||
        (io->status == io_unrecoverable_error))
        return io->status;

    if (io->type == iot_special_write)
    {
        io->type = iot_special_read;
        relocate_buffer(io);
        if (io->status == io_changes)
        {
          io->status = io_no_change;
          return io_changes;
        }
        return io_no_change;
    }
    else if ((io->type != iot_special_read) && (io->fd == -1))
    {
        io->status = io_unrecoverable_error;
    }

    if (io->type == iot_read) {
        relocate_buffer(io);
    } else {
        io->length = 0;
        io->position = 0;

        io->type = iot_read;
        io->status = io_undefined;
    }

    if ((io->length + IO_CHUNKSIZE) > io->buffersize) {
        unsigned int newsize = (io->length + IO_CHUNKSIZE);
        if ((newsize % IO_CHUNKSIZE) != 0) {
            newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
        }

        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);

        if (io->buffer == (char *)0)
        {
            io->buffersize = 0;
            io->length = 0;
            io->status = io_unrecoverable_error;
            return io_unrecoverable_error;
        }

        io->buffersize = newsize;
    }

    readrv = a_read(io->fd, (io->buffer + io->length), IO_CHUNKSIZE);

    if (readrv < 0) /* potential error */
    {
        if (last_error_recoverable_p == (char)1) { /* nothing serious,
                                                      just try again later */
            return io_no_change;
        } else { /* source is dead, close the fd */
            io->status = io_unrecoverable_error;
            (void)a_close (io->fd);
            io->fd = -1;
            return io_unrecoverable_error;
        }
    }

    if (readrv == 0) /* end-of-file */
    {
        io->status = io_end_of_file;
        (void)a_close (io->fd);
        io->fd = -1;
        return io_end_of_file;
    }

    io->length += readrv;

    return io_changes;
}

enum io_result io_commit (struct io *io)
{
    int rv = -1, pos;
    unsigned int i;

    switch (io->type) {
        case iot_undefined:
            return io_undefined;
        case iot_special_read:
        case iot_read:
            return io_read(io);
        case iot_special_write:
            return io_incomplete;
        case iot_write:
            if (io->length == 0) return io_complete;
            if (io->buffer != (char *)0)
                rv = a_write(io->fd, io->buffer, io->length);
    }

    if (io->buffer == (char *)0)
    {
        io->length = 0;
        return io_unrecoverable_error;
    }

    if (rv < 0) /* potential error */
    {
        if (last_error_recoverable_p == (char)1) { /* nothing serious,
                                                      just try again later */
            return io_no_change;
        } else { /* target is dead, close the fd */
            io->status = io_unrecoverable_error;
            (void)a_close (io->fd);
            io->fd = -1;
            return io_unrecoverable_error;
        }
    }

    if (rv == 0) /* end-of-file */
    {
        io->status = io_end_of_file;
        (void)a_close (io->fd);
        io->fd = -1;
        return io_end_of_file;
    }

    if (rv == (int)io->length)
    {
        io->length = 0;
        return io_complete;
    }

    io->length -= rv;

    for (i = 0, pos = rv; i < io->length; i++, pos++) {
        io->buffer[i] = io->buffer[pos];
    }

    return io_incomplete;
}

enum io_result io_finish (struct io *io)
{
    io->status = io_finalising;
    return io_finalising;
}

/*@-branchstate@*/
void io_close (struct io *io)
{
    if (io->status != io_finalising) (void)io_finish (io);

    if (io->type == iot_write) {
        while (io_commit (io) == io_incomplete);
    }

    if (io->fd >= 0)
        (void)a_close (io->fd);

    if ((io->buffersize > 0) && (io->buffer != (char *)0)) {
        free_mem(io->buffersize, io->buffer);
    }

    free_pool_mem ((void *)io);
}
/*@=branchstate@*/
