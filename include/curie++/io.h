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

/*! \file
 *  \brief Input and Output Handling (C++)
 */

#ifndef LIBCURIEPP_IO_H
#define LIBCURIEPP_IO_H

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
            /*! \brief Constructor ("Special" I/O)
             *
             *  IO objects created with this constructor operate on a memory
             *  buffer, see io_open_special().
             */
            IO ();

            /*! \brief Constructor
             *  \param[in] io The io structure to wrap around.
             *
             *  Use this constructor to create an IO object from a regular curie
             *  io structure.
             */
            IO (struct io *io);

            /*! \brief Destructor
             *
             *  The destructor will automatically close the internal io
             *  structure.
             */
            ~IO();

            /*! \brief Read Data
             *
             *  This will call io_read() on the object's io structure.
             */
            enum io_result read ();

            /*! \brief Collect Data (write without flushing)
             *  \param[in] data   The data to write.
             *  \param[in] length Length of the data to write.
             *
             *  This will call io_collect() on the object's io structure.
             */
            enum io_result collect (const char *data, int_pointer length);

            /*! \brief Write Data (write and flush)
             *  \param[in] data   The data to write.
             *  \param[in] length Length of the data to write.
             *
             *  This will call io_write() on the object's io structure.
             */
            enum io_result write (const char *data, int_pointer length);

            /*! \brief Commit Pending Operations
             *
             *  This will call io_commit() on the object's io structure, thus
             *  flushing any data that still needs to be written.
             */
            enum io_result commit ();

            /*! \brief Commit Pending Operations
             *
             *  This will call io_finish() on the object's io structure, thus
             *  preventing any further io_read() and io_collect() calls from
             *  modifying the buffer.
             */
            enum io_result finish ();

            /*! \brief Get Buffer and its Length
             *  \param[out] length The length of the data buffer.
             *  \return Pointer to the data buffer.
             *
             *  Use this method to get the data that was read earlier using the
             *  multiplexer or the read() method. It may also be used to get
             *  data that still needs to be committed for output structures.
             */
            char *getBuffer (int &length);

            /*! \brief Set Buffer Position
             *  \param[out] position The new position in the output buffer.
             *
             *  Upon reading data, the position in the data buffer must be
             *  adjusted manually. This way, the io structures allow for easier
             *  semi-random access, which is also extremely useful when parsing
             *  special message formats, like textual data.
             *
             *  Use this function to set the new position in the buffer after
             *  processing some part of it. Data before this position may be
             *  discarded on further operations.
             */
            void setPosition (int_32 position);

        protected:
            /*! \brief IO Structure
             *
             *  The raw io structure that this object operates on.
             */
            struct io *context;

            /* These friend-relationships are needed because both the
               multiplexer and the s-expression io code need to get at the raw
               io structures, but in general the structure should not be
               accessed directly. */
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
            /*! \brief Constructor
             *  \param[in] filename The file to open, as a C-string.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
            IOReader (const char *filename);

            /*! \brief Constructor
             *  \param[in] filename The file to open, as an s-expression string.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
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
            /*! \brief Constructor
             *  \param[in] filename The file to open, as a C-string.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
            IOWriter (const char *filename);

            /*! \brief Constructor
             *  \param[in] filename The file to open, as a C-string.
             *  \param[in] mode     File access mode, if it needs to be created.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
            IOWriter (const char *filename, int mode);

            /*! \brief Constructor
             *  \param[in] filename The file to open, as an s-expression string.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
            IOWriter (sexpr filename);

            /*! \brief Constructor
             *  \param[in] filename The file to open, as an s-expression string.
             *  \param[in] mode     File access mode, if it needs to be created.
             *
             *  The file pointed to by the file name is opened immediately. If
             *  the file can't be opened for some reason, the constructor will
             *  still work fine but the first read() call will fail.
             */
            IOWriter (sexpr filename, int mode);
    };

    /*! \brief Standard Input
     *
     *  Use this to get a hold of the process's standard input.
     */
    class IOStandardInput : public IO
    {
        public:
            /*! \brief Constructor
             *
             *  This will really just use io_open_stdin().
             */
            IOStandardInput ();
    };

    /*! \brief Standard Output
     *
     *  Use this to get a hold of the process's standard output.
     */
    class IOStandardOutput : public IO
    {
        public:
            /*! \brief Constructor
             *
             *  This will really just use io_open_stdout().
             */
            IOStandardOutput ();
    };

    /*! \brief Standard Error
     *
     *  Use this to get a hold of the process's standard error output.
     */
    class IOStandardError : public IO
    {
        public:
            /*! \brief Constructor
             *
             *  This will really just use io_open_stderr().
             */
            IOStandardError ();
    };

    /*! \brief I/O Multiplexer
     *
     *  This is a base class to derive your own I/O multiplexers from.
     */
    class IOMultiplexer: public Multiplexer
    {
        public:
            /*! \brief Constructor
             *  \param[in] io The object to use.
             *
             *  Hijack the given IO object by setting its context to
             *  (struct io *)0 and setting up the multiplexer.
             */
            IOMultiplexer  (IO &io);

            /*! \brief Destructor
             *
             *  This will delete the multiplexer entry for the used IO object
             *  (in the process, the io structure is also closed).
             */
            ~IOMultiplexer ();

        protected:
            /*! \brief Read Callback
             *
             *  This is called when new data comes in. Overwrite when you need
             *  to use this event.
             */
            virtual void onRead  ();

            /*! \brief EOF/Error Callback
             *
             *  This is called when the end of file is reached, or an I/O error
             *  occurs. Overwrite when you need to use this event.
             */
            virtual void onClose ();

            /*! \brief IO Structure
             *
             *  The IO object this multiplexer applies to.
             */
            IO context;

        private:
            /*! \brief Raw Read Callback
             *
             *  This function is passed to multiplex_add_io() as the on_read()
             *  callback.
             */
            static void onReadCallback  (struct io *i, void *aux);

            /*! \brief Raw EOF/Error Callback
             *
             *  This function is passed to multiplex_add_io() as the
             *  on_close() callback.
             */
            static void onCloseCallback (struct io *i, void *aux);
    };
}

#endif
