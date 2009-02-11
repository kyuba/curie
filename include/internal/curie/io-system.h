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

/*! \internal
 *
 * @{
 */

/*! \file
 *  \brief Glue Code Header for io.h
 *
 */

#ifndef LIBCURIE_IO_SYSTEM_H
#define LIBCURIE_IO_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif
  int a_read  (int fd,
               /*@notnull@*/ /*@out@*/ void *buf,
               unsigned int count);
  int a_write (int fd,
               /*@notnull@*/ const void *buf,
               unsigned int count);

  int a_open_read (/*@notnull@*/ const char *path);
  int a_open_write (/*@notnull@*/ const char *path);
  int a_create (/*@notnull@*/ const char *path, int mode);
  int a_close (int fd);

  int a_dup (int ofd, int nfd);
  int a_dup_n (int fd);

  int a_make_nonblocking (int fd);

  int a_unlink (/*@notnull@*/ const char *path);

  int a_stat(/*@notnull@*/ const char *path, void *buffer);
  int a_lstat(/*@notnull@*/ const char *path, void *buffer);

  extern char last_error_recoverable_p;
#ifdef __cplusplus
}
#endif

#endif /* LIBCURIE_IO_SYSTEM_H */

/*! @} */
