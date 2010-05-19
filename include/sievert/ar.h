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
 *  \brief .a Archive Support
 *
 *  Analoguous to CPIO, .a is a very simple archive format that is perfectly
 *  suited to our needs. The only drawback is that it's ASCII-based, ugh.
 */

#ifndef LIBSIEVERT_AR_H
#define LIBSIEVERT_AR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sievert/metadata.h>
#include <curie/io.h>

/*! \brief .a Archive Handle
 *
 *  This structure is used to keep track of the state of a created archive. The
 *  contents are not really important to users, so the struct's definition is
 *  hidden.
 */
struct ar;

/*! \brief Initialise the .a Multiplexer
 *
 *  As is customary with curie, the .a code is designed as a stream processor.
 *  This function will make sure the main multiplexer is set up to work properly
 *  with this code.
 *
 *  \note It's perfectly legitimate to omit the call to multiplex_ar() if you
 *        only intend to use iot_buffer type I/O structures, as these don't
 *        actually operate on streams.
 */
void multiplex_ar
    ( void );

/*! \brief Read a .a Archive
 *  \param[in] io                The archive to read.
 *  \param[in] regex             Whitelist for filenames.
 *  \param[in] on_new_file       Callback when a new file is found.
 *  \param[in] on_end_of_archive Callback when the archive has ended.
 *  \param[in] aux               Auxiliary data for the callbacks.
 *
 *  This will read a .a archive, given as a struct io. Whenever a new file
 *  header is read from the io struct, its filename is matched against the
 *  passed regex. If it matches, on_new_file() is called with a struct io
 *  containing the file data and the file's name. Once the end of the archive
 *  is read, on_end_of_archive() is called. After this last call, iot_buffer
 *  type io structures passed to on_new_file() may have their data buffers
 *  invalidated.
 *
 *  \note A further note on that last bit; if you actually pass an iot_buffer
 *        type io struct as the first parameter to this function, you don't
 *        have to worry about some remaining buffers getting invalidated halfway
 *        through being processed since you need to free the passed buffer
 *        yourself. On the other hand, when passing a stream, you need to make
 *        sure that you weren't passed an iot_buffer in your on_new_file()
 *        function and accidentally try to use it after you get the
 *        on_end_of_archive() event, or anywhere after that for that matter.
 *
 *  \note The file name and metadata passed to the on_new_file() function are
 *        volatile, meaning that you may only use them before returning from
 *        this function. Afterwards, these pointers will quite likely contain
 *        garbage.
 */
void ar_read_archive
    ( struct io *io, const char *regex,
      void (*on_new_file) (struct io *io, const char *name,
                           struct metadata *metadata, void *aux),
      void (*on_end_of_archive) (void *aux),
      void *aux );

/*! \brief Create a .a Archive
 *  \param[out] out Output file.
 *  \return The new ar structure.
 *
 *  Creates a new .a handle that will write its data to the given output
 *  file.
 */
struct ar *ar_create_archive
    ( struct io *out );

/*! \brief Add a File to a .a Archive
 *  \param[out] ar       The archive to add the file to.
 *  \param[in]  filename Name of the file to create.
 *  \param[in]  metadata File attributes. May be 0 for "sane" defaults.
 *  \param[in]  file     File data to fill the file with.
 *
 *  Use this function to add a file to a .a archive. Most file metadata is
 *  stubbed with more or less sane defaults if you pass a 0-pointer instead of
 *  an actual metadata struct. The file data is added immediately if file is an
 *  iot_buffer type structure, otherwise it is added with the next call to
 *  ar_close() or ar_next_file().
 *
 *  Since .a archives need to have all the files available to be able to create
 *  an archive, this function may not have any effect until ar_close() is
 *  called.
 */
void ar_next_file
    ( struct ar *ar, const char *filename, struct metadata *metadata,
      struct io *file );

/*! \brief Finalise a .a Archive
 *  \param[in] ar The archive to close.
 *
 *  This will write the archive's contents, flush all data and close the output
 *  stream.
 */
void ar_close
    ( struct ar *ar );

#ifdef __cplusplus
}
#endif

#endif
