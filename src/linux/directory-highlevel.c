/*
 *  linux/directory-highlevel.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 28/12/2008.
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


#include <curie/directory.h>
#include <curie/directory-system.h>
#include <curie/io-system.h>
#include <linux/dirent.h>

sexpr read_directory_rx (const char *base, struct graph *rx)
{
    int fd = a_open_directory (base);
    sexpr r = sx_end_of_list;

    if (fd >= 0)
    {
        char buffer[0x1000];
        int rc;

        while ((rc = a_getdents64 (fd, &buffer, sizeof(buffer))) > 0)
        {
            unsigned int p = 0;

            for (struct dirent64 *e = (struct dirent64 *)buffer; p < rc;
                 e = (struct dirent64 *)(buffer + (p = p + e->d_reclen)))
            {
                const char *s = e->d_name;

                if (truep (rx_match (rx, s)))
                {
                    r = cons (make_string (s), r);
                }
            }
        }

        a_close (fd);
    }

    return r;
}
