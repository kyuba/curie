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
 *  \brief Process Multiplexer
 *
 *  Multiplex events from different sources, such as I/O reads and writes.
 */

#ifndef LIBCURIE_MULTIPLEX_H
#define LIBCURIE_MULTIPLEX_H

#include <curie/io.h>
#include <curie/sexpr.h>
#include <curie/exec.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief multiplex() Call Result
 *
 *  Result code of a call to the multiplex() function. This is only used by that
 *  one function.
 */
enum multiplex_result {
    /*! \brief Call OK
     *
     *  This is returned when the mutliplex() call returned normally, regardless
     *  of whether any changes have been observed or not.
     */
    mx_ok = 0,

    /*! \brief Nothing to do
     *
     *  Whenever this is returned, it means that either no multiplexers are in
     *  use, or that none of them have anything to watch.
     */
    mx_nothing_to_do = 1,

    /*! \brief Immediate action possible
     *
     *  Used by the ->count() functions to indicate that not only may something
     *  change but in fact things already have changed, so the select() call can
     *  be omitted and the callbacks can be run directly.
     */
    mx_immediate_action = 2
};

/*! \brief Multiplexer Functions
 *
 *  A set of functions that implement a multiplexer. The whole multiplexer code
 *  basically wraps around the POSIX select() call, so all the multiplexers can
 *  do is wait for file descriptor statuses.
 */
struct multiplex_functions {
    /*! \brief Count Function
     *  \return mx_ok or mx_immediate_action if the callbacks can be run
     *          directly without waiting for anything.
     *
     *  This function is used to count the file descriptors that will be needed
     *  for the select() call. Basically any multiplexer will just count its own
     *  file descriptors, and then add the number of fds to check for read
     *  events to the first integer, and the number of fds to check for write
     *  events to the second integer.
     */
    /*@notnull@*/ enum multiplex_result (*count)(int *, int *);

    /*! \brief Augmenter
     *
     *  This is used to add the actual file descriptors to check for to a
     *  pre-allocated buffer. The first argument is a pre-allocated buffer for
     *  the fds to check for reading, the second is the current position in the
     *  buffer, which must be updated after adding things. The third and fourth
     *  argument are the same, but for writing.
     */
    /*@notnull@*/ void (*augment)(int *, int *, int *, int *);

    /*! \brief Select Callback
     *
     *  After the main multiplexer's select() call, this function is called. The
     *  First argument is an array with fds that can now be read from, the
     *  second is the number of elements in the array. The third and fourth
     *  arguments are the same for writable fds.
     */
    /*@notnull@*/ void (*callback)(int *, int, int *, int);

    /*! \brief Next Set of Multiplexer Functions
     *  \internal
     *
     *  Always set this to (struct multiplex_functions *)0; the multiplexer code
     *  uses this to implement a list of multiplexers.
     */
    /*@null@*/ struct multiplex_functions *next;
};

/*! \brief Multiplex
 *  \return mx_ok or mx_nothing_to_do, as per definition.
 *
 *  Calling this function will end up calling the equivalent of select() and
 *  once that call has complete, it will run all the registered multiplexers'
 *  callbacks.
 *
 *  In most programmes you can just call this function in a loop.
 */
enum multiplex_result multiplex ();

/*! \brief Add Multiplexer
 *  \param[in] mx Descriptor for the multiplexer to add.
 *
 *  Adds a new multiplexer to the set of multiplexers currently in use. The
 *  given mx argument will be used by the multiplexer, so make sure not to
 *  deallocate it afterwards or allocate it a volatile function stack.
 */
void multiplex_add (/*@notnull@*/ struct multiplex_functions *mx);

/*! \brief Initialise I/O Multiplexer
 *
 *  Use this function before using the I/O multiplexer, i.e. with one of the
 *  multiplex_add_io(), multiplex_add_io_no_callback() or the multiplex_del_io()
 *  functions.
 */
void multiplex_io ();

/*! \brief Initialise Process Multiplexer
 *
 *  Use this function before using the process multiplexer, i.e. before calling
 *  multiplex_add_process().
 */
void multiplex_process ();

/*! \brief Initialise Process Multiplexer (Reap All Processes)
 *
 *  This is essentially the same as multiplex_process(), but the reaping
 *  strategy that is employed is slightly different. Where multiplex_process()
 *  will only reap children that are specifically specified, this variant will
 *  reap all child processes, even those that have not been specified in any
 *  way.
 *
 *  Theoretically, this variant ought to be faster, since less syscalls are
 *  involved when keeping track of multiple processes.
 */
void multiplex_all_processes ();

/*! \brief Initialise S-Expression I/O Multiplexer
 *
 *  Use this function before using the sexpr multiplexer, i.e. before calling
 *  multiplex_add_sexpr().
 */
void multiplex_sexpr ();

/*! \brief Register Callbacks for an I/O Structure
 *  \param[in] io       The structure to keep track of.
 *  \param[in] on_read  Callback function when new data is available.
 *  \param[in] on_close Callback function when the file is closed.
 *  \param[in] aux      Arbitrary data, passed to the callback functions.
 *
 *  Once this function is called, you shouldn't use the io parameter anymore.
 *  The multiplexer will automatically call io_close() once there's no point in
 *  keeping the io structure around anymore. It will also use io_commit()
 *  whenever needed.
 */
void multiplex_add_io (/*@notnull@*/ /*@only@*/ struct io *io,
                       /*@null@*/ void (*on_read)(struct io *, void *),
                       /*@null@*/ void (*on_close)(struct io *, void *),
                       /*@null@*/ void *aux);

/*! \brief Keep track of an I/O Structure
 *  \param[in] io The structure to keep track of.
 *
 *  This is the same as multiplex_add_io(), except that no callbacks are
 *  registered. Handy for iot_write structures.
 */
void multiplex_add_io_no_callback (/*@notnull@*/ /*@only@*/ struct io *io);

/*! \brief Close an I/O Structure
 *  \param[in] io The structure to close.
 *
 *  Calls io_close() on the io parameter, and stops tracking it with the I/O
 *  multiplexer code.
 */
void multiplex_del_io (/*@notnull@*/ /*@only@*/ struct io *io);

/*! \brief Register Callbacks for a Process
 *  \param[in] context  The process context to keep track of.
 *  \param[in] on_death Callback function when the process dies.
 *  \param[in] aux      Arbitrary data, passed to the callback function.
 *
 *  This function will watch for child termination signals, check if the given
 *  process is still running when a signal like that comes in and if not it'll
 *  reap the process and call the on_death callback.
 */
void multiplex_add_process (/*@notnull@*/ /*@only@*/ struct exec_context *context,
                            /*@notnull@*/ void (*on_death)(struct exec_context*,
                                                           void *),
                            /*@null@*/ void *aux);

/*! \brief Register Callbacks for S-Expression I/O
 *  \param[in] io      The structure to keep track of.
 *  \param[in] on_read Callback function when new data comes in.
 *  \param[in] aux     Arbitrary data, passed to the callback function.
 *
 *  Just like the plain I/O equivalent, except that it will use the sexpr I/O
 *  functions. It will even close the sexpr_io's input and output structures
 *  when appropriate.
 */
void multiplex_add_sexpr (/*@notnull@*/ /*@only@*/ struct sexpr_io *io,
                          /*@null@*/ void (*on_read)(sexpr,
                                                     struct sexpr_io *,
                                                     void *),
                          /*@null@*/ void *aux);

#ifdef __cplusplus
}
#endif

#endif
