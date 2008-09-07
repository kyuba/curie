/*
 *  io-system.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 26/05/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  int    a_read  (int fd, /*@out@*/ void *buf, unsigned int count);
  int    a_write (int fd, const void *buf, unsigned int count);

  int    a_open_read (const char *path);
  int    a_open_write (const char *path);
  int    a_create (const char *path, int mode);
  int    a_close (int fd);

  int    a_dup (int ofd, int nfd);
  int    a_dup_n (int fd);

  int    a_make_nonblocking (int fd);

  int    a_unlink (const char *path);

  extern char last_error_recoverable_p;
#ifdef __cplusplus
}
#endif

#endif /* LIBCURIE_IO_SYSTEM_H */

/*! @} */
