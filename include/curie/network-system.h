/**\internal
 *
 * @{
 */

/**\file
 * \brief Glue Code Header for network.h
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

#ifndef LIBCURIE_NETWORK_SYSTEM_H
#define LIBCURIE_NETWORK_SYSTEM_H

#include <curie/io.h>

enum io_result a_open_loop (int result[]);
enum io_result a_open_socket (int *result, const char *path);
enum io_result a_open_listen_socket (int *result, const char *path);
enum io_result a_open_ip4 (int *result, int_32 addr, int_16 port);
enum io_result a_open_listen_ip4 (int *result, int_32 addr, int_16 port);
enum io_result a_open_ip6 (int *result, int_8 addr[16], int_16 port);
enum io_result a_open_listen_ip6 (int *result, int_8 addr[16], int_16 port);
enum io_result a_accept_socket (int *result, int fd);

#endif

/*! @} */
