/*
 *  network.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 06/08/2008.
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

/*! \file
 *  \brief Networking
 *
 *  Basic networking using local unix sockets.
 */

#ifndef LIBCURIE_NETWORK_H
#define LIBCURIE_NETWORK_H

#include <curie/io.h>
#include <curie/sexpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Open Network Loop
 *  \param[out] in  Input I/O structure for the loop.
 *  \param[out] out Output I/O structure for the loop.
 *
 *  This opens a set of io structures, where if you write to the one returned
 *  to the out parameter, it shows up on the in paramter. You know, like pipe()
 *  or socketpair(). Usually socketpair is used, since pipe() is naughty and may
 *  generate SIGPIPEs.
 *
 *  Both in and out may be (struct io *)0 if the loop couldn't be opened.
 */
void net_open_loop (/*@out@*/ /*@notnull@*/ struct io **in,
                    /*@out@*/ /*@notnull@*/ struct io **out);

/*! \brief Connect to Socket
 *  \param[in]  path The socket to connect to.
 *  \param[out] in   I/O structure to read from.
 *  \param[out] out  I/O structure to write to.
 *
 *  Connet to a Unix socket and return proper in/out I/O structures.
 *
 *  Both in and out may be (struct io *)0 if the loop couldn't be opened.
 */
void net_open_socket (/*@notnull@*/ const char *path,
                      /*@out@*/ /*@notnull@*/ struct io **in,
                      /*@out@*/ /*@notnull@*/ struct io **out);

/*! \brief Accept Connections on Network Sockets
 *
 *  Call this function before using either multiplex_add_socket() or
 *  multiplex_add_socket_listener_sx().
 */
void multiplex_network();

/*! \brief Listen on a Unix Socket
 *  \param[in] path       The socket to connect to.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  This multiplexer function will create a new Unix socket at the given path
 *  and call the on_connect() function when a new connection is establish. The
 *  on_connect() gets an input I/O structure as the first argument, and an
 *  output structure as its second argument.
 *
 *  \note The in/out I/O structures aren't added to the I/O multiplexer
 *        automatically, you'll have to do that yourself.
 */
void multiplex_add_socket
        (/*@notnull@*/ const char *path,
         /*@notnull@*/ void (*on_connect)(struct io *, struct io *, void *),
         /*@null@*/ void *aux);

/*! \brief Listen on a Unix Socket with S-Expression I/O
 *  \param[in] path       The socket to connect to.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  Analoguous to multiplex_add_socket(), except that the I/O structures are
 *  fed to the S-Expression code right away.
 *
 *  \note The sexpr_io structure isn't added to the s-expression multiplexer
 *        automatically, you'll have to do that yourself.
 */
void multiplex_add_socket_listener_sx
        (/*@notnull@*/ const char *path,
         /*@notnull@*/ void (*on_connect)(struct sexpr_io *, void *),
         /*@null@*/ void *aux);

/*! \brief Connect to Socket with S-Expression I/O
 *  \param[in] path The socket to connect to.
 *  \return New sexpr_io structure, or (struct sexpr_io *)0 for errors.
 *
 *  Analoguous to net_open_socket(), just for S-Expression I/O.
 */
/*@null@*/ /*@only@*/ struct sexpr_io *sx_open_socket
        (/*@notnull@*/ const char *path);

/*! \brief Open Socket and register Callback for S-Expression I/O
 *  \param[in] path    The socket to connect to.
 *  \param[in] on_read Callback function when new data comes in.
 *  \param[in] aux     Arbitrary data, passed to the callback function.
 *
 *  Same as multiplex_add_sexpr, but it opens the given socket and uses that.
 */
void multiplex_add_socket_sx
        (/*@notnull@*/ const char *path,
         /*@null@*/ void (*on_read)(/*@shared@*/ sexpr, struct sexpr_io *, void *),
         /*@null@*/ void *aux);

#ifdef __cplusplus
}
#endif

#endif
