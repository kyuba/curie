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
 *  \brief Networking
 *
 *  Basic networking using local unix sockets.
 */

#ifndef LIBCURIE_NETWORK_H
#define LIBCURIE_NETWORK_H

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