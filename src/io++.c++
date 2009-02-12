/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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
