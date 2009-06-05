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
 *  \brief Basic I/O
 *
 *  Functions from this header file provide basic file in- and output
 *  operations.
 *
 *  All file I/O is performed in non-blocking mode in curie. This is why the
 *  multiplexer is so important -- there are no primitives to perform blocking
 *  I/O.
 *
 *  \note This is the windows version of the I/O header. The POSIX version (or
 *        rather any other non-windows version so far) is slightly different,
 *        due to the lack of file descriptors and all... regular applications
 *        shouldn't see much of a difference, however.
 */

#ifndef LIBCURIE_IO_H
#define LIBCURIE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/constants.h>

/*! \brief I/O mode
 *
 *  The mode of I/O currently being performed on a struct I/O. Switching I/O
 *  modes would effectively invalidate the buffer of an associated struct io.
 */
enum io_type {
    /*! \internal
     *  \brief Undefined I/O Mode
     *
     *  This is the initial value of io->type, and should not be used for
     *  anything else.
     */
    iot_undefined = 0,

    /*! \brief Read I/O Mode
     *
     *  The last access on the associated struct io was a read request. The
     *  buffer contains the data that was read last time, possibly appended to
     *  previous contents.
     */
    iot_read = 1,

    /*! \brief Write I/O Mode
     *
     *  The last access on the associated struct io was a write request. The
     *  buffer contains everything that still needs to be written.
     */
    iot_write = 2,

    /*! \brief "Special" Read I/O Mode
     *
     *  This indicates an I/O struct that can be read from, but it isn't backed
     *  by any real file descriptor, or at least not directly. Instead, data can
     *  be 'read' from this struct after writing to it with the write/collect
     *  functions.
     *
     *  Changing an iot_special_write to this won't invalidate the buffer.
     */
    iot_special_read = 3,

    /*! \brief "Special" Write I/O Mode
     *
     *  This indicates an I/O struct that can be written to, but it'll only
     *  modify the buffer in memory and not flush the data to anywhere.
     *
     *  Changing an iot_special_read to this won't invalidate the buffer.
     */
    iot_special_write = 4
};

/*! \brief Result code for an I/O Operation
 *
 *  This enum is used to specify the outcome of an I/O operation.
 */
enum io_result {
    /*! \internal
     *  \brief Undefined Result
     *
     *  This is only used as the initial value of io->status fields.
     */
    io_undefined = 0,

    /*! \brief End of File
     *
     *  The last request carried out on the associated struct io has hit the end
     *  of the underlying file. Subsequent read or write operations are
     *  pointless (most of the time, anyway).
     */
    io_end_of_file = 1,

    /*! \brief New Data
     *
     *  The last read request has retrieved new data. This is also used for
     *  iot_special_read and iot_special_write to make the multiplexer aware of
     *  file modifications.
     */
    io_changes = 2,

    /*! \brief Unrecoverable Error
     *
     *  The last operation could not be performed, and the file descriptor
     *  associated with the struct io is no longer valid. Operations returning
     *  this code will automatically close (io->fd) and set it to -1.
     */
    io_unrecoverable_error = 3,

    /*! \brief No Changes
     *
     *  The last operation had no effect. This may indicate a recoverable error,
     *  or it may be that the underlying file descriptor is simply not ready to
     *  take new data just yet.
     */
    io_no_change = 4,

    /*! \brief All Operations carried out completely
     *
     *  This return code means that there are no further pending operations for
     *  the given io structure.
     */
    io_complete = 5,

    /*! \brief Some Operations carried out
     *
     *  This return code means that there are pending operations for the given
     *  io structure that still need to be committed.
     */
    io_incomplete = 6,

    /*! \brief No further Data will be accepted
     *
     *  Whenever this code is used, it means that the corresponding io structure
     *  will not take any further modifications. This is used when the structure
     *  is marked for closing, but there are some pending operations.
     */
    io_finalising = 7
};

/*! \brief I/O Structure
 *
 *  This structure keeps track of the state of any kind of I/O connection. Think
 *  of it as a file descriptor, or as the equivalent of a FILE*.
 */
struct io {
    /*! \brief File Handle
     *
     *  The underlying file handle for this struct.
     */
    void *handle;

    /*! \brief File Data
     *
     *  This is buffer is used to keep the data that was read, or to keep track
     *  of data that still needs to be written.
     */
    char *buffer;

    /*! \brief I/O Type
     *
     *  The type of I/O that was being carried out on this structure the last
     *  time it was used.
     */
    enum io_type   type;

    /*! \brief I/O Status
     *
     *  The result code of the last I/O operation being carried out on this
     *  structure.
     */
    enum io_result status;

    /*! \brief Buffer length
     *
     *  The length of actual data in the buffer field.
     */
    unsigned int length;

    /*! \brief Buffer position
     *
     *  You should modify this field to the position up until which you have
     *  processed the contents of the buffer field. I/O requests use this field
     *  to discard the processed parts of the buffer.
     */
    unsigned int position;

    /*! \brief Buffer size
     *
     *  The size of the buffer field. This is distinct from the usable length of
     *  the field, in that it describes the allocated size of the buffer.
     */
    unsigned int buffersize;
};


/*! \brief Open File Desriptor
 *  \param[in] handle The file handle to use.
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  This will open the given handle, after putting it in non-blocking mode. The
 *  handle itself should not be used afterwards.
 *
 *  \note The returned io structure's type field will still be set to undefined,
 *        and it must be set to either iot_read or iot_write before the struct
 *        I/O is fed to a multiplexer. Functions expecting distinct in- and out-
 *        parameters will set this automatically, but if it is done manually, it
 *        is important that this be set.
 */
struct io *io_open
        (void *handle);

/*! \brief Open Dummy File Desriptor
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  This will open a dummy file descriptor for use with functions that require
 *  an in- as well as an output i/o port. The only reason for this function
 *  is that io_open(-1) only works on non-windows systems.
 */
#define io_open_null io_open((void *)0)

/*! \brief Open Standard Input File Desriptor
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  This will create a regular i/o structurue pointing to the process's standard
 *  input.
 */
struct io *io_open_stdin ( void );

/*! \brief Open Standard Output File Desriptor
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  This will create a regular i/o structurue pointing to the process's standard
 *  output.
 */
struct io *io_open_stdout ( void );

/*! \brief Open Standard Error Output File Desriptor
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  This will create a regular i/o structurue pointing to the process's standard
 *  error output.
 */
struct io *io_open_stderr ( void );

/*! \brief Open File for reading
 *  \param[in] filename The file to open for reading.
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  The given filename is opened for reading. If the file does not exist, or
 *  cannot be read from, a struct io is returned anyway, but it will have an
 *  fd of -1, which means it will be useless. The type is set to iot_read.
 */
struct io *io_open_read
        (const char *filename);

/*! \brief Open File for writing
 *  \param[in] filename The file to open for writing.
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  The given filename is opened for writing. If the file does not exist, or
 *  cannot be written to, a struct io is returned anyway, but it will have an
 *  fd of -1, which means it will be useless. The type is set to iot_write.
 */
struct io *io_open_write
        (const char *filename);

/*! \brief Open File for writing
 *  \param[in] filename The file to open for writing.
 *  \param[in] mode     The mode for the new file.
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  The given filename is opened for writing. The file is created with the
 *  given mode. As usual, a struct io is always created even if the file could
 *  not be created, but in that case it will have an fd of -1.
 */
struct io *io_open_create
        (const char *filename, int mode);

/*! \brief Create Special I/O Structure
 *  \return A new struct io. (struct io *)0 is only returned for memory
 *          allocation errors.
 *
 *  The returned I/O structure is suitable for special in-memory processing. It
 *  isn't backed by any actual file (not from Curie's point of view, anyway),
 *  which means that this I/O structure can be used to process memory buffers
 *  obtained one way or another.
 *
 *  Writing to the resulting IO structure will write to a memory-only buffer,
 *  reading from it will report io_changes once after each write operation.
 */
struct io *io_open_special ( void );

/*! \brief Write Data to I/O Structure
 *  \param[in] io     The I/O structure to write data to.
 *  \param[in] data   The data to write.
 *  \param[in] length The length of the data buffer.
 *  \return Result code of the implicit io_commit().
 *
 *  This function is the same as calling io_collect() and then immediately
 *  calling io_commit().
 */
enum io_result io_write
        (struct io * io,
         const char *data, unsigned int length);

/*! \brief Collect Data to write to I/O Structure
 *  \param[in] io     The I/O structure to write data to.
 *  \param[in] data   The data to write.
 *  \param[in] length The length of the data buffer.
 *  \return io_unrecoverable_error for errors, io_incomplete when the data has
 *          been appended, io_end_of_file or io_finalising if the given io
 *          struct has that as its current status.
 *
 *  The data is appended to the write buffer of the io structure. The type of
 *  the io structure is set to iot_write; if the type was different before the
 *  call, the buffer contents are discarded.
 */
enum io_result io_collect
        (struct io * io,
         const char *data, unsigned int length);

/*! \brief Read Data into I/O structure
 *  \param[in] io The I/O structure to read from.
 *  \return io_unrecoverable_error for erros, io_no_change when no new data is
 *          available, io_end_of_file if the there is nothing further to be read
 *          from the file, io_changes when something has been read,
 *          io_finalising if the struct is in that state.
 *
 *  This function will read data from the io structure's source and append the
 *  data to the buffer.
 */
enum io_result io_read
        (struct io * io);

/*! \brief Reclaim I/O Memory
 *  \param[in] io The I/O structure to flush.
 *
 *  Use this function if you have a buffer that will usually grow quite big
 *  while reading, and you'd like to get the buffer smaller.
 */
void io_flush
        (struct io * io);

/*! \brief Commit pending Oprations
 *  \param[in] io The I/O structure whose data to commit.
 *  \return io_unrecoverable_error for errors, io_undefined if the type of the
 *          io structure is iot_undefined, the return value of io_read() for
 *          iot_read structures. For iot_write structures, io_complete when all
 *          of the buffer has been written, io_end_of_file when nothing can be
 *          written to the file anymore and io_incomplete when there is still
 *          some data left to be read.
 *
 *  Calling this function will either write pending data, or read in new data,
 *  depending on the type of the io argument.
 */
enum io_result io_commit
        (struct io * io);

/*! \brief Set state to io_finalising.
 *  \param[in] io The I/O structure to change.
 *  \return io_finalising.
 *
 *  The io_finalising state is used to prevent further additional io_read() or
 *  io_collect() calls from modifying the buffer. It is used to mark an io
 *  struct for an impending close operation, i.e. when it is known that the
 *  structure will have io_close() called on it soon, and it is not desired that
 *  any multiplexer using this file will keep fetching new data for it or add
 *  new data to write.
 *
 *  Calls to io_commit() for iot_write io structures are still meaningful, as
 *  they flush out parts of the buffer.
 */
enum io_result io_finish
        (struct io * io);

/*! \brief Close I/O structure.
 *  \param[in] io The I/O structure to close.
 *
 *  This will set the I/O structure to io_finalising and then loop until
 *  io_commit() != io_incomplete. After that the fd is closed and the given io
 *  structure is destroyed.
 *
 *  When using a multiplexer, it would be a good idea to only use this function
 *  on iot_write type io structures when there is nothing further to commit, as
 *  this call will loop until there is nothing more to write.
 */
void io_close
        (struct io * io);

#ifdef __cplusplus
}
#endif

#endif
