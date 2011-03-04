/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include <curie/io-system.h>
#include <curie/memory.h>
#include <windows.h>

#include <stdio.h>

struct io *io_open_special ()
{
    struct io *io = io_create();

    io->handle = (void *)0;
    io->status = io_undefined;
    io->length = 0;
    io->position = 0;
    io->type = iot_special_write;
    io->overlapped = 0;
    io->overlapped_buffer = 0;
    io->overlapped_buffersize = 0;
    io->overlapped_length = 0;

    fprintf (stderr, "opened (special): 0x%x; ----\n", io);

    return io;
}

struct io *io_open (void *handle)
{
    struct io *io = io_create();

    io->handle = handle;
    io->type = iot_undefined;
    io->overlapped = 0;
    io->overlapped_buffer = 0;
    io->overlapped_buffersize = 0;
    io->overlapped_length = 0;

    fprintf (stderr, "opened (normal): 0x%x; ----\n", io);

    return io;
}

struct io *io_open_read (const char *path)
{
    void *handle = CreateFileA
            (path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
             (void *)0, OPEN_EXISTING,
             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, (void *)0);

    struct io *io = io_open(handle);

    fprintf (stderr, "created (read): 0x%x; %s\n", io, path);

    io->type = iot_read;

    return io;
}

struct io *io_open_write (const char *path)
{
    void *handle = CreateFileA
            (path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
             (void *)0, CREATE_ALWAYS,
             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, (void *)0);

    struct io *io = io_open(handle);

    fprintf (stderr, "created (write): 0x%x; %s\n", io, path);

    io->type = iot_write;

    return io;
}

struct io *io_open_create (const char *path, int mode)
{
    return io_open_write (path);
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
    else if ((io->type != iot_special_write) && (io->handle == (void *)0))
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
    unsigned int newsize;

    if ((io->status == io_finalising) ||
        (io->status == io_end_of_file) ||
        (io->status == io_unrecoverable_error) ||
        (io->overlapped && io->overlapped->hEvent))
        return;

    relocate_buffer(io);

    newsize = (io->length + IO_CHUNKSIZE);
    if ((newsize % IO_CHUNKSIZE) != 0) {
        newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
    }

    if (newsize != io->buffersize) {
        io->buffer = resize_mem (io->buffersize, io->buffer, newsize);

        io->buffersize = newsize;
    }

    return;
}

static void destroy_overlapped (struct io *io)
{
    if (io->overlapped->hEvent)
    {
        CloseHandle(io->overlapped->hEvent);
    }
    free_pool_mem (io->overlapped);
    io->overlapped = 0;
}

static void update_overlapped (struct io *io)
{
    static struct memory_pool overlapped_pool
            = MEMORY_POOL_INITIALISER(sizeof (OVERLAPPED));

    if (!io->overlapped)
    {
        io->overlapped = get_pool_mem (&overlapped_pool);

        io->overlapped->Internal     = 0;
        io->overlapped->InternalHigh = 0;
        io->overlapped->Offset       = 0;
        io->overlapped->OffsetHigh   = 0;
        io->overlapped->Pointer      = 0;
    }

    io->overlapped->hEvent           = CreateEvent(0, TRUE, FALSE, 0);
}

enum io_result io_read(struct io *io)
{
    DWORD readrv = 0;

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
    else if ((io->type != iot_special_read) && (io->handle == (void *)0))
    {
        io->status = io_unrecoverable_error;
    }

    if (io->type == iot_special_read)
    {
        relocate_buffer(io);
        return io_no_change;
    }

    if (io->overlapped && io->overlapped->hEvent)
    {
      getresult:
        if (GetOverlappedResult (io->handle, io->overlapped, &readrv, FALSE)
            == 0)
        {
            goto evaluateerror;
        }

        goto evaluate;
    }
    else
    {
        if (io->type == iot_read) {
            relocate_buffer(io);
        } else {
            io->length = 0;
            io->position = 0;

            io->type = iot_read;
            io->status = io_undefined;
        }

        update_overlapped(io);

        if ((io->length + IO_CHUNKSIZE) > io->buffersize) {
            unsigned int newsize = (io->length + IO_CHUNKSIZE);
            if ((newsize % IO_CHUNKSIZE) != 0) {
                newsize = ((newsize / IO_CHUNKSIZE) + 1) * IO_CHUNKSIZE;
            }

            io->buffer = resize_mem (io->buffersize, io->buffer, newsize);

            io->buffersize = newsize;
        }

        if (ReadFile(io->handle, (io->buffer + io->length), IO_CHUNKSIZE, 0,
                     io->overlapped) == 0)
        {
          evaluateerror:
            switch (GetLastError())
            {
                case ERROR_HANDLE_EOF:
                    goto eof;
                case ERROR_IO_PENDING:
                case ERROR_IO_INCOMPLETE:
                    last_error_recoverable_p = (char)1;
                    return io_no_change;
                default:
                    last_error_recoverable_p = (char)0;
                    io->status = io_unrecoverable_error;
                    CloseHandle (io->handle);
                    io->handle = (void *)0;
                    destroy_overlapped(io);
                    return io_unrecoverable_error;
            }
        }

        goto getresult;

      evaluate:
        if (readrv == 0) /* end-of-file */
        {
          eof:
            destroy_overlapped(io);

            io->status = io_end_of_file;
            CloseHandle (io->handle);
            io->handle = (void *)0;
            return io_end_of_file;
        }

//        destroy_overlapped(io);
        if (io->overlapped->hEvent)
        {
            CloseHandle (io->overlapped->hEvent);
            io->overlapped->hEvent = 0;
        }

        io->overlapped->Offset += readrv;
        io->length += readrv;

        return io_changes;
    }
}

enum io_result io_commit (struct io *io)
{
    DWORD rv = 0;
    int pos;
    unsigned int i;
    int evaluateerror = 0;
    int evaluate = 0;

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
            if ((io->length == 0) && (io->overlapped_length == 0))
            {
                return io_complete;
            }
            if (io->buffer != (char *)0)
            {
                if (io->overlapped_buffer)
                {
                  get_result:
                    evaluate =
                        !(evaluateerror = (GetOverlappedResult
                            (io->handle, io->overlapped, &rv, FALSE) == 0));
                }
                else
                {
                    update_overlapped(io);

                    io->overlapped_buffer     = io->buffer;
                    io->overlapped_buffersize = io->buffersize;
                    io->overlapped_length     = io->length;

                    io->buffer     = get_mem (IO_CHUNKSIZE);
                    io->buffersize = IO_CHUNKSIZE;
                    io->length     = 0;

                    evaluate =
                        !(evaluateerror = (WriteFile
                            (io->handle, io->overlapped_buffer,
                             io->overlapped_length, 0,
                             io->overlapped) == 0));

                    if (evaluate)
                    {
                        goto get_result;
                    }
                }
            }
    }

    if (io->buffer == (char *)0)
    {
        io->length = 0;
        return io_unrecoverable_error;
    }

  retry:
    if (evaluateerror) /* potential error */
    {
        switch (GetLastError())
        {
            case ERROR_IO_PENDING:
            case ERROR_IO_INCOMPLETE:
                last_error_recoverable_p = (char)1;
                return io_no_change;
            case ERROR_INVALID_ACCESS:
            case ERROR_INVALID_HANDLE:
                /* we (probably) get these for non-async handles */
                evaluate =
                    !(evaluateerror =
                          (WriteFile (io->handle, io->overlapped_buffer,
                                      io->overlapped_length, &rv,
                                      (void *)0) == 0));

                if (!evaluateerror)
                {
                    goto retry;
                }
                else switch (GetLastError())
                {
                    case ERROR_INVALID_ACCESS:
                    case ERROR_INVALID_HANDLE:
                        break;
                    default:
                        goto retry;
                }
            default:
                last_error_recoverable_p = (char)0;
                destroy_overlapped (io);

                if (io->overlapped_buffer)
                {
                    char *tbuffer = io->buffer;
                    unsigned int tsize = io->buffersize;
                    unsigned int tlength = io->length;

                    io->buffer     = io->overlapped_buffer;
                    io->buffersize = io->overlapped_buffersize;
                    io->length     = io->overlapped_length;

                    io->overlapped_buffer     = 0;
                    io->overlapped_buffersize = 0;
                    io->overlapped_length     = 0;

                    if (tlength != 0)
                    {
                        io_collect (io, tbuffer, tlength);
                    }

                    free_mem (tsize, tbuffer);
                }

                io->status = io_unrecoverable_error;
                CloseHandle (io->handle);
                io->handle = (void *)0;
                return io_unrecoverable_error;
        }
    }

    if (evaluate)
    {
        if (io->overlapped && io->overlapped->hEvent)
        {
            CloseHandle (io->overlapped->hEvent);
            io->overlapped->hEvent = 0;
        }

        if (io->overlapped_buffer)
        {
            fprintf (stderr, "---- written: ---- %i %i %i --\n", rv, io->overlapped_length, io->length);
            fwrite (io->overlapped_buffer, rv, 1, stderr);
            fprintf (stderr, "---- complete: --- %i ---------\n", io->overlapped_length);
            fwrite (io->overlapped_buffer, io->overlapped_length, 1, stderr);
            fprintf (stderr, "---- remaining: -- %i ---------\n", io->length);
            fwrite (io->buffer, io->length, 1, stderr);
            fprintf (stderr, "---------------- (end dump) ----\n");

            io->overlapped->Offset += rv;
            io->overlapped_length -= rv;

            if (io->overlapped_length == 0)
            {
                free_mem (io->overlapped_buffersize, io->overlapped_buffer);

                io->overlapped_buffer     = 0;
                io->overlapped_buffersize = 0;
                io->overlapped_length     = 0;

                return (io->length == 0) ? io_complete : io_incomplete;
            }
            else
            {
                char *tbuffer = io->buffer;
                unsigned int tsize = io->buffersize;
                unsigned int tlength = io->length;

                io->buffer     = io->overlapped_buffer;
                io->buffersize = io->overlapped_buffersize;
                io->length     = io->overlapped_length;

                io->overlapped_buffer     = 0;
                io->overlapped_buffersize = 0;
                io->overlapped_length     = 0;

                for (i = 0, pos = rv; i < io->length; i++, pos++) {
                    io->buffer[i] = io->buffer[pos];
                }

                if (tlength != 0)
                {
                    io_collect (io, tbuffer, tlength);
                }

                free_mem (tsize, tbuffer);
            }
        }
        else
        {
            io->length -= rv;

            for (i = 0, pos = rv; i < io->length; i++, pos++) {
                io->buffer[i] = io->buffer[pos];
            }
        }
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

    if (io->handle != (void *)0)
    {
        CloseHandle (io->handle);
        io->handle = (void *)0;
    }

    if ((io->buffersize > 0) && (io->buffer != (char *)0)) {
        free_mem(io->buffersize, io->buffer);
        io->buffer = (char *)0;
        io->buffersize = 0;
    }

    if (io->overlapped)
    {
        destroy_overlapped(io);
    }

    io_destroy (io);
}
