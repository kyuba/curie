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
 *  \brief CPIO Archive Support
 *
 *  CPIO is a very simple archive format used in unix environments. Supporting
 *  an archive format is useful in certain contexts, such as when it would be
 *  beneficial to include several data or source files in a created binary.
 *
 *  The CPIO format was chosen due to its simplicity. Technically it would've
 *  been possible to strip the data even further since most of the file header
 *  information in this format is rather useless to curie applications, but that
 *  would have required inventing yet another new archive file format, so
 *  choosing the simplest archive format in actual use seemed more logical.
 */

#ifndef LIBSIEVERT_CPIO_H
#define LIBSIEVERT_CPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/io.h>

struct cpio
{
    struct io *output;
    struct io *current_file;
    const char *current_file_name;
};

/*! \brief Initialise the CPIO Multiplexer
 *
 *  As is customary with curie, the CPIO code is designed as a stream processor.
 *  This function will make sure the main multiplexer is set up to work properly
 *  with the CPIO code.
 *
 *  \note It's perfectly legitimate to omit the call to multiplex_cpio() if you
 *        only intend to use iot_buffer type I/O structures, as these don't
 *        actually operate on streams.
 */
void multiplex_cpio
    ( void );

/*! \brief Read a CPIO Archive
 *  \param[in] io                The archive to read.
 *  \param[in] regex             Whitelist for filenames.
 *  \param[in] on_new_file       Callback when a new file is found.
 *  \param[in] on_end_of_archive Callback when the archive has ended.
 *  \param[in] aux               Auxiliary data for the callbacks.
 *
 *  This will read a CPIO archive, given as a struct io. Whenever a new file
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
 */
void cpio_read_archive
    ( struct io *io, const char *regex,
      void (*on_new_file) (struct io *io, const char *name, void *aux),
      void (*on_end_of_archive) (void *aux),
      void *aux );

struct cpio *cpio_create_archive
    ( struct io *out );

void cpio_next_file
    ( struct cpio *cpio, const char *filename, struct io *file );

void cpio_close
    ( struct cpio *cpio );

#ifdef __cplusplus
}
#endif

#endif
