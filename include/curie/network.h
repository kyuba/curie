/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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
 */
void net_open_loop (struct io **in, struct io **out);

/*! \brief Connect to Socket
 *  \param[in]  path The socket to connect to.
 *  \param[out] in   I/O structure to read from.
 *  \param[out] out  I/O structure to write to.
 *
 *  Connet to a Unix socket and return proper in/out I/O structures.
 */
void net_open_socket (const char *path, struct io **in, struct io **out);

/*! \brief Connect via IPv4
 *  \param[in]  addr The host to connect to.
 *  \param[in]  port The port to connect to.
 *  \param[out] in   I/O structure to read from.
 *  \param[out] out  I/O structure to write to.
 *
 *  Connet to a (possibly remote) host and return proper in/out I/O structures.
 */
void net_open_ip4 (int_32 addr, int_16 port, struct io **in, struct io **out);

/*! \brief Connect via IPv6
 *  \param[in]  addr The host to connect to.
 *  \param[in]  port The port to connect to.
 *  \param[out] in   I/O structure to read from.
 *  \param[out] out  I/O structure to write to.
 *
 *  Connet to a (possibly remote) host and return proper in/out I/O structures.
 */
void net_open_ip6 (int_8 addr[16], int_16 port, struct io **in, struct io **out);

/*! \brief Accept Connections on Network Sockets
 *
 *  Call this function before using either multiplex_add_socket() or
 *  multiplex_add_socket_sx().
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
        (const char *path, void (*on_connect)(struct io *, struct io *, void *),
         void *aux);

/*! \brief Listen for IPv4 Connections
 *  \param[in] addr       The host/ip address to bind to.
 *  \param[in] port       The port to listen on.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  This multiplexer function will bind to a new IPv4 port, possibly on the
 *  given host and call the on_connect() function when a new connection is
 *  establish. The on_connect() gets an input I/O structure as the first
 *  argument, and an output structure as its second argument.
 *
 *  \note The in/out I/O structures aren't added to the I/O multiplexer
 *        automatically, you'll have to do that yourself.
 *  \note You may omit the host to bind to by using (const char *)0 or an empty
 *        string. This would bind to all interfaces.
 */
void multiplex_add_ip4
        (int_32 addr, int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux);

/*! \brief Listen for IPv6 Connections
 *  \param[in] addr       The host/ip address to bind to.
 *  \param[in] port       The port to listen on.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  This multiplexer function will bind to a new IPv6 port, possibly on the
 *  given host and call the on_connect() function when a new connection is
 *  establish. The on_connect() gets an input I/O structure as the first
 *  argument, and an output structure as its second argument.
 *
 *  \note The in/out I/O structures aren't added to the I/O multiplexer
 *        automatically, you'll have to do that yourself.
 *  \note You may omit the host to bind to by using (const char *)0 or an empty
 *        string. This would bind to all interfaces.
 */
void multiplex_add_ip6
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux);

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
void multiplex_add_socket_sx
        (const char *path, void (*on_connect)(struct sexpr_io *, void *),
         void *aux);

/*! \brief Listen for IPv4 Connections for S-Expression I/O
 *  \param[in] addr       The host/ip address to bind to.
 *  \param[in] port       The port to listen on.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  Analoguous to multiplex_add_ip4(), except that the I/O structures are
 *  fed to the S-Expression code right away.
 *
 *  \note The sexpr_io structure isn't added to the s-expression multiplexer
 *        automatically, you'll have to do that yourself.
 */
void multiplex_add_ip4_sx
        (int_32 addr, int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux);

/*! \brief Listen for IPv Connections for S-Expression I/O
 *  \param[in] addr       The host/ip address to bind to.
 *  \param[in] port       The port to listen on.
 *  \param[in] on_connect Called when a new connection is established.
 *  \param[in] aux        Passed to the callback function.
 *
 *  Analoguous to multiplex_add_ip6(), except that the I/O structures are
 *  fed to the S-Expression code right away.
 *
 *  \note The sexpr_io structure isn't added to the s-expression multiplexer
 *        automatically, you'll have to do that yourself.
 */
void multiplex_add_ip6_sx
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux);

/*! \brief Connect to Socket with S-Expression I/O
 *  \param[in] path The socket to connect to.
 *  \return New sexpr_io structure, or (struct sexpr_io *)0 for errors.
 *
 *  Analoguous to net_open_socket(), just for S-Expression I/O.
 */
struct sexpr_io *sx_open_socket
        (const char *path);

/*! \brief Connect via IPv4 for S-Expression I/O
 *  \param[in] addr The host to connect to.
 *  \param[in] port The port to connect to.
 *  \return New sexpr_io structure, or (struct sexpr_io *)0 for errors.
 *
 *  Analoguous to net_open_ip4(), just for S-Expression I/O.
 */
struct sexpr_io *sx_open_ip4
        (int_32 addr, int_16 port);

/*! \brief Connect via IPv6 for S-Expression I/O
 *  \param[in] addr The host to connect to.
 *  \param[in] port The port to connect to.
 *  \return New sexpr_io structure, or (struct sexpr_io *)0 for errors.
 *
 *  Analoguous to net_open_ip6(), just for S-Expression I/O.
 */
struct sexpr_io *sx_open_ip6
        (int_8 addr[16], int_16 port);

/*! \brief Open Socket and register Callback for S-Expression I/O
 *  \param[in] path    The socket to connect to.
 *  \param[in] on_read Callback function when new data comes in.
 *  \param[in] aux     Arbitrary data, passed to the callback function.
 *
 *  Same as multiplex_add_sexpr, but it opens the given socket and uses that.
 */
void multiplex_add_socket_client_sx
        (const char *path, void (*on_read)(sexpr, struct sexpr_io *, void *),
         void *aux);

/*! \brief Open Connection via IPv4 and register Callback for S-Expression I/O
 *  \param[in] addr    The host to connect to.
 *  \param[in] port    The port to connect to.
 *  \param[in] on_read Callback function when new data comes in.
 *  \param[in] aux     Arbitrary data, passed to the callback function.
 *
 *  Same as multiplex_add_sexpr, but it connects to the given host on the given
 *  port.
 */
void multiplex_add_ip4_client_sx
        (int_32 addr, int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux);

/*! \brief Open Connection via IPv6 and register Callback for S-Expression I/O
 *  \param[in] addr    The host to connect to.
 *  \param[in] port    The port to connect to.
 *  \param[in] on_read Callback function when new data comes in.
 *  \param[in] aux     Arbitrary data, passed to the callback function.
 *
 *  Same as multiplex_add_sexpr, but it connects to the given host on the given
 *  port.
 */
void multiplex_add_ip6_client_sx
        (int_8 addr[16], int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux);

#ifdef __cplusplus
}
#endif

#endif
