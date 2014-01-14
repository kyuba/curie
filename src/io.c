/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include <curie/io-system.h>
#include <curie/memory.h>

struct io *io_open_special ()
{
    struct io *io = io_create();

    io->fd = -1;
    io->status = io_undefined;
    io->length = 0;
    io->position = 0;
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

    io->type = iot_read;

    return io;
}

struct io *io_open_write (const char *path)
{
    int fd = a_open_write (path);
    struct io *io = io_open(fd);

    io->type = iot_write;

    return io;
}

struct io *io_open_create (const char *path, int mode)
{
    int fd = 0;
    struct io *io = (struct io *)0;

    a_unlink (path);

    fd = a_create (path, mode);
    io = io_open(fd);

    io->type = iot_write;

    return io;
}

static void relocate_buffer_contents (struct io *io)
{
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

void io_flush (struct io *io)
{
    if ((io->status == io_finalising) ||
        (io->status == io_end_of_file) ||
        (io->status == io_unrecoverable_error))
        return;

    relocate_buffer(io);

    unsigned int newsize = (io->length + IO_CHUNKSIZE);
    if ((newsize % IO_CHUNKSIZE) != 0) {
        newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
    }

    if (newsize != io->buffersize) {
        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);

        io->buffersize = newsize;
    }

    return;
}

enum io_result io_read(struct io *io)
{
    int readrv;

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

    if (io->type == iot_special_read)
    {
        relocate_buffer(io);
        return io_no_change;
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
        case iot_buffer:
            return io_end_of_file;
        case iot_special_read:
        case iot_read:
            return io_read(io);
        case iot_special_write:
            return io_incomplete;
        case iot_write:
            if (io->length == 0) return io_complete;
            if (io->buffer != (char *)0)
            {
                rv = a_write(io->fd, io->buffer, io->length);
            }
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

void io_close (struct io *io)
{
    if (io->status != io_finalising) (void)io_finish (io);

    if (io->type == iot_write) {
        enum io_result r;

        while (((r = io_commit (io)) == io_incomplete) || (r == io_no_change));

        (void)io_finish (io);
    }

    if (io->fd >= 0)
    {
        (void)a_close (io->fd);
        io->fd = -1;
    }

    if ((io->buffersize > 0) && (io->buffer != (char *)0)) {
        free_mem(io->buffersize, io->buffer);
        io->buffer = (char *)0;
        io->buffersize = 0;
    }

    io_destroy (io);
}
