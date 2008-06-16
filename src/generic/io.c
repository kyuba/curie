/*
 *  io.c
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

#include <atomic/nucleus.h>
#include <atomic/io.h>
#include <atomic/memory.h>

/*@-castfcnptr@*/
/* somehow that test didn't work properly on my box... ah well */

/*@-mustfreeonly@*/
/* if i don't specify this, then the line
   io->buffer = get_mem ();
   is invalid. however, it isn't. i simply can't free the storage in there
   before assigning the new address, because there is no address in there
   when i get the chunk from get_pool_mem(). */

/*@null@*/ /*@only@*/ static struct memory_pool * io_pool = 0;

struct io *io_open (int fd)
{
    struct io *io = 0;

    if (io_pool == 0) {
        io_pool = create_memory_pool ((unsigned int)sizeof(struct io));
    }

    io = get_pool_mem(io_pool);

    io->fd = fd;
    io->type = iot_undefined;
    io->status = io_undefined;
    io->position = 0;
    io->length = 0;
    io->cursor = 0;
    io->buffersize = IO_CHUNKSIZE;

    io->buffer = get_mem (IO_CHUNKSIZE);

    /* splint bitches about this one, claiming i'd cast it to int... */
    io->on_data_read = (void (*)(struct io *))0;
    io->on_struct_deallocation = (void (*)(struct io *))0;

    io->arbitrary = (void *)0;

    return io;
}

struct io *io_open_read (const char *path)
{
    int fd = _atomic_open_read (path);
    struct io *io = io_open(fd);

    io->type = iot_read;

    return io;
}

struct io *io_open_write (const char *path)
{
    int fd = _atomic_open_write (path);
    struct io *io = io_open(fd);

    io->type = iot_write;

    return io;
}

enum io_result io_write(struct io *io, char *data, unsigned int length)
{
    unsigned int i, pos;

    if (io->status == io_finalising)
        return io_finalising;

    if (io->type != iot_write) {
        io->position = 0;
        io->length = 0;
        io->cursor = 0;

        io->type = iot_write;
        io->status = io_undefined;
    }

    if ((io->length + length) > io->buffersize) {
        unsigned int newsize = (io->length + length);
        if ((newsize % IO_CHUNKSIZE) != 0) {
            newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
        }

        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);
        io->buffersize = newsize;
    }

    for (i = 0, pos = io->length; i < length; i++, pos++) {
        io->buffer[pos] = data[i];
    }

    io->length += length;

    return io_commit(io);
}

enum io_result io_read(struct io *io)
{
    int readrv;

    if (io->status == io_finalising)
        return io_finalising;

    if (io->type != iot_read) {
        io->position = 0;
        io->length = 0;
        io->cursor = 0;

        io->type = iot_read;
        io->status = io_undefined;
    }

    if ((io->length + IO_CHUNKSIZE) > io->buffersize) {
        unsigned int newsize = (io->length + IO_CHUNKSIZE);
        if ((newsize % IO_CHUNKSIZE) != 0) {
            newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
        }

        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);
        io->buffersize = newsize;
    }

    readrv = _atomic_read(io->fd, (io->buffer + io->length), IO_CHUNKSIZE);

    if (readrv < 0) /* potential error */
    {
        if (last_error_recoverable_p == (char)1) { /* nothing serious,
                                                      just try again later */
            return io_no_change;
        } else { /* source is dead, close the fd */
            io->status = io_unrecoverable_error;
            (void)_atomic_close (io->fd);
            io->fd = -1;
            return io_unrecoverable_error;
        }
    }

    if (readrv == 0) /* end-of-file */
    {
        io->status = io_end_of_file;
        (void)_atomic_close (io->fd);
        io->fd = -1;
        return io_end_of_file;
    }

    io->length += readrv;

    if (io->on_data_read)
        io->on_data_read (io);

    return io_changes;
}

enum io_result io_commit (struct io *io)
{
    int rv = -1, pos;
    unsigned int i;

    switch (io->type) {
        case iot_undefined:
            return io_undefined;
        case iot_read:
            return io_read(io);
        case iot_write:
            rv = _atomic_write(io->fd, io->buffer, io->length);
            break;
    }

    if (rv < 0) /* potential error */
    {
        if (last_error_recoverable_p == (char)1) { /* nothing serious,
                                                      just try again later */
            return io_no_change;
        } else { /* target is dead, close the fd */
            io->status = io_unrecoverable_error;
            (void)_atomic_close (io->fd);
            io->fd = -1;
            return io_unrecoverable_error;
        }
    }

    if (rv == 0) /* end-of-file */
    {
        io->status = io_end_of_file;
        (void)_atomic_close (io->fd);
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

void io_close (struct io *io)
{
    if (io->status != io_finalising) (void)io_finish (io);

    switch (io->type)
    {
        case iot_undefined: break;
        case iot_read: break;
        case iot_write:
            while (io_commit (io) == io_incomplete);
            break;
    }

    if (io->fd >= 0)
        (void)_atomic_close (io->fd);

    if (io->on_struct_deallocation)
        io->on_struct_deallocation(io);

    free_mem(io->buffersize, io->buffer);
    free_pool_mem ((void *)io);
}
