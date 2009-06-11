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

/*! \file
 *  \brief Input and Output Handling (C++)
 */

#ifndef LIBCURIEPP_IO_H
#define LIBCURIEPP_IO_H

#include <curie/int.h>
#include <curie/sexpr.h>

#include <curie++/multiplex.h>

/*! \brief Curie++ Namespace
 *
 *  All Curie++ functionality is kept in this namespace, so that things won't
 *  clash too much.
 */
namespace curiepp
{
    /*! \brief Basic Input and Output
     *
     *  This class performs basic in- and output; it's a wrapper around the code
     *  in curie/io.h. The limitations for curie/io.h still apply, especially
     *  the thing about multiplexing.
     *
     *  This base class provides a memory buffer for I/O, like the
     *  io_open_special() function does for curie, unless you use the
     *  constructor that itself takes a struct io *, in which case you're
     *  supposed to know what you're doing.
     */
    class IO
    {
        public:
            IO ();
            IO (struct io *io);
            ~IO();

            enum io_result read ();
            enum io_result collect (const char *data, int_pointer length);
            enum io_result write (const char *data, int_pointer length);
            enum io_result commit ();
            enum io_result finish ();

            char *getBuffer (int &length);

            void setPosition (int_32 position);

        protected:
            struct io *context;

            friend class IOMultiplexer;
            friend class SExprIO;
    };

    /*! \brief I/O Reader
     *
     *  This class wraps up I/O structures to read from. That is to say, the
     *  structures normally opened with io_open_read().
     */
    class IOReader : public IO
    {
        public:
            IOReader (const char *filename);
            IOReader (sexpr filename);
    };

    /*! \brief I/O Writer
     *
     *  This class wraps up I/O structures to write to. That is to say, the
     *  structures normally opened with io_open_write() or io_open_create().
     */
    class IOWriter : public IO
    {
        public:
            IOWriter (const char *filename);
            IOWriter (const char *filename, int mode);
            IOWriter (sexpr filename);
            IOWriter (sexpr filename, int mode);
    };

    /*! \brief Standard Input
     *
     *  Use this to get a hold of the process's standard input.
     */
    class IOStandardInput : public IO
    {
        public:
            IOStandardInput ();
    };

    /*! \brief Standard Output
     *
     *  Use this to get a hold of the process's standard output.
     */
    class IOStandardOutput : public IO
    {
        public:
            IOStandardOutput ();
    };

    /*! \brief Standard Error
     *
     *  Use this to get a hold of the process's standard error output.
     */
    class IOStandardError : public IO
    {
        public:
            IOStandardError ();
    };

    /*! \brief Standard Error
     *
     *  Use this to get a dummy IO object in case you need one.
     */
    class IONull : public IO
    {
        public:
            IONull ();
    };

    /*! \brief I/O Multiplexer
     *
     *  This is a base class to derive your own I/O multiplexers from.
     */
    class IOMultiplexer: public Multiplexer
    {
        public:
            IOMultiplexer  (IO &io);
            ~IOMultiplexer ();

        protected:
            virtual void onRead  ();
            virtual void onClose ();

            IO context;

        private:
            static void onReadCallback  (struct io *i, void *aux);
            static void onCloseCallback (struct io *i, void *aux);
    };
}

#endif
