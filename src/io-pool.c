/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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
#include <curie/constants.h>

static struct io *rpool[IO_STRUCT_POOL_ENTRIES] = { (struct io *)0 };

struct io *io_create ()
{
    struct io *io = 0;

    if (rpool [0] != (struct io *)0)
    {
        int i;

        io = rpool[0];

        for (i = 1; i < IO_STRUCT_POOL_ENTRIES; i++)
        {
            rpool[(i - 1)] = rpool[i];
        }
    }
    else
    {
        static struct memory_pool io_pool
            = MEMORY_POOL_INITIALISER(sizeof(struct io));

        io = get_pool_mem(&io_pool);
    }

    io->buffer = get_mem (IO_CHUNKSIZE);

    io->status = io_undefined;
    io->length = 0;
    io->position = 0;
    io->buffersize = IO_CHUNKSIZE;

    return io;
}

void io_destroy (struct io *io)
{
    int i;

    for (i = 0; i < IO_STRUCT_POOL_ENTRIES; i++)
    {
        if (rpool[i] == (struct io *)0)
        {
            rpool[i] = io;
            return;
        }
    }

    free_pool_mem ((void *)(rpool[0]));

    for (i = 1; i < IO_STRUCT_POOL_ENTRIES; i++)
    {
        rpool[(i - 1)] = rpool[i];
    }

    rpool[(IO_STRUCT_POOL_ENTRIES - 1)] = io;
}

