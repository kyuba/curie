/**\file
 * \brief Advanced I/O Features
 *
 * Curie's basic I/O is intended to provide everything you need to get things
 * working. This usually results in the use of single-pass streams, which may
 * not provide optimal performance and require extra copying. In contrast,
 * functions and features in this header are intended to maximize performance,
 * or fall back to streams for unsupported architectures.
 *
 * \note Using functions in this file may require linking against libsyscall.
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
 */

#ifndef LIBSIEVERT_IO_H
#define LIBSIEVERT_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/io.h>

/**\brief Get complete contents of file
 *
 * This function attempts to mmap() the specified path and provide the
 * on_file_read() callback with only a single call with the complete contents
 * of a given file. If that fails then the function will attempt to read the
 * normally, like any other stream.
 *
 * \param[in] path         The file to read
 * \param[in] on_file_read The callback to call after reading from the file.
 * \param[in] aux          Auxiliary data; passed to on_file_read.
 */
void io_get_file_contents
    (const char *path, void (*on_file_read)(void *, unsigned int, void *),
     void *aux);

#ifdef __cplusplus
}
#endif

#endif
