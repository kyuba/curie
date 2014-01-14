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
#include <curie/constants.h>

static struct memory_pool *io_struct_pool = (struct memory_pool *)0;
static struct io *(*get_io_struct)();

static struct io *get_io_struct_real ()
{
    return get_pool_mem (io_struct_pool);
}

static struct io *get_io_struct_init ()
{
    io_struct_pool = create_memory_pool (sizeof (struct io));
    get_io_struct = get_io_struct_real;
    return get_io_struct_real();
}

static struct io *(*get_io_struct)() = get_io_struct_init;

struct io *io_open_buffer (void *buffer, unsigned int size)
{
    struct io *io = get_io_struct ();

    io->buffer = buffer;

    io->type = iot_buffer;
    io->status = io_end_of_file;
    io->length = size;
    io->position = 0;
    io->buffersize = 0; /* set to 0 so other code will not try to manage the
                           buffer by resizing or freeing it */
#if defined(_WIN32)
    io->handle = (void *)0;
#else
    io->fd = -1;
#endif

    return io;
}

struct io *io_create ()
{
    struct io *io = get_io_struct ();

    io->buffer = get_mem (IO_CHUNKSIZE);

    io->status = io_undefined;
    io->length = 0;
    io->position = 0;
    io->buffersize = IO_CHUNKSIZE;

    return io;
}

void io_destroy (struct io *io)
{
    free_pool_mem ((void *)(io));
}

