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

#include <curie/multiplex.h>
#include <curie++/io.h>

using namespace curiepp;

IO::IO()
{
    context = io_open_special ();
}

IO::IO(struct io *io)
{
    context = io;
}

IO::~IO()
{
    if (context != (struct io *)0)
    {
        io_close (context);
    }
}

enum io_result IO::read ()
{
    if (context != (struct io *)0)
    {
        return io_read (context);
    }

    return io_unrecoverable_error;
}

enum io_result IO::collect (const char *data, int_pointer length)
{
    if (context != (struct io *)0)
    {
        return io_collect (context, data, length);
    }

    return io_unrecoverable_error;
}

enum io_result IO::write (const char *data, int_pointer length)
{
    if (context != (struct io *)0)
    {
        return io_write (context, data, length);
    }

    return io_unrecoverable_error;
}

enum io_result IO::commit ()
{
    if (context != (struct io *)0)
    {
        return io_commit (context);
    }

    return io_unrecoverable_error;
}

enum io_result IO::finish ()
{
    if (context != (struct io *)0)
    {
        return io_finish (context);
    }

    return io_unrecoverable_error;
}

char *IO::getBuffer (int &length)
{
    if (context != (struct io *)0)
    {
        context->buffer + context->position;
        length = context->length - context->position;
    }

    return (char *)0;
}

void IO::setPosition (int_32 position)
{
    if (context != (struct io *)0)
    {
        context->position += position;
    }
}

IOReader::IOReader (const char *filename)
{
    context = io_open_read (filename);
}

IOReader::IOReader (sexpr filename)
{
    const char *f = sx_string (filename);
    context = io_open_read (f);
}

IOWriter::IOWriter (const char *filename)
{
    context = io_open_write (filename);
}

IOWriter::IOWriter (const char *filename, int mode)
{
    context = io_open_create (filename, mode);
}

IOWriter::IOWriter (sexpr filename)
{
    const char *f = sx_string (filename);
    context = io_open_write (f);
}

IOWriter::IOWriter (sexpr filename, int mode)
{
    const char *f = sx_string (filename);
    context = io_open_create (f, mode);
}

IOStandardInput::IOStandardInput()
{
    context = io_open_stdin();
}

IOStandardOutput::IOStandardOutput()
{
    context = io_open_stdout();
}

IOStandardError::IOStandardError()
{
    context = io_open_stderr();
}

IOMultiplexer::IOMultiplexer(IO &io)
{
    multiplex_io ();

    context    = io;
    io.context = (struct io *)0;

    if (context.context != (struct io *)0)
    {
        multiplex_add_io (context.context, onReadCallback, onCloseCallback,
                          (void *)this);
    }
}

IOMultiplexer::~IOMultiplexer()
{
    if (context.context != (struct io *)0)
    {
        multiplex_del_io (context.context);

        context.context = (struct io *)0;
    }
}

void IOMultiplexer::onReadCallback (struct io *i, void *aux)
{
    IOMultiplexer *m = (IOMultiplexer *)aux;
    m->onRead ();
}

void IOMultiplexer::onCloseCallback (struct io *i, void *aux)
{
    IOMultiplexer *m = (IOMultiplexer *)aux;
    m->onClose ();

    if (m->context.context != (struct io *)0)
    {
        m->context.context = (struct io *)0;
    }
}

void IOMultiplexer::onRead()  {}
void IOMultiplexer::onClose() {}
