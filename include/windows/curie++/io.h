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
     *  in curie/io.h.
     *
     *  The limitations for curie/io.h still apply, especially the thing about
     *  multiplexing.
     */
    class IO
    {
        public:
            IO ();
            IO (char *filename);
            IO (sexpr filename);
            IO (void *handle);
            IO (void *handle, io_type type);
            ~IO();

            void open ();
            void open (sexpr &filename);
            void open (io_type type);
            void open (sexpr &filename, io_type type);

            enum io_result read ();
            enum io_result collect (const char *data, int_pointer length);
            enum io_result write (const char *data, int_pointer length);
            enum io_result commit ();
            enum io_result finish ();
            void close ();

            char *getBuffer ();

            void setPosition (int_32 position);

        protected:
            struct io *context;

        private:
            sexpr filename;
    };

    /*! \brief I/O Multiplexer
     *
     *  This is a base class to derive your own I/O multiplexers from.
     */
    class IOMultiplexer: public IO, public Multiplexer
    {
      public:
        IOMultiplexer();

        void Add(void (*on_read)(struct io *, void *),
                     void (*on_close)(struct io *, void *),
                     void *aux);

        void AddNoCallback();

        void Delete();
    };
}

#endif
