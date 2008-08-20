/*
 *  read-write.c
 *  curie-libc
 *
 *  Created by Magnus Deininger on 08/06/2008.
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

#include "curie/io-system.h"

#define TESTDATA       "TEST-DATA"
#define TESTDATA_SIZE  (int)(sizeof(TESTDATA)-1)

int a_main(void) {
/* test the a_create() and a_close() functions */
    int fd, i, j;
    char buffer[TESTDATA_SIZE];

    fd = a_create ("temporary-curie-test-data", 0660);
    if (fd < 0) return 1;

    i = a_close(fd);
    if (i < 0) return 2;

/* test the a_write() and a_open_write() functions */
    fd = a_open_write ("temporary-curie-test-data");
    if (fd < 0) return 3;

    i = a_write(fd, TESTDATA, (unsigned int)TESTDATA_SIZE);
    if (i < 0) return 4;
    if (i != TESTDATA_SIZE) return 5;

/* close file descriptor */
    i = a_close(fd);
    if (i < 0) return 6;

/* test the a_read() and a_open_read() functions */
    fd = a_open_read ("temporary-curie-test-data");
    if (fd < 0) return 7;

    i = a_read (fd, buffer, (unsigned int)TESTDATA_SIZE);
    if (i < 0) return 8;
    if (i != TESTDATA_SIZE) return 9;

    for (j = 0; j < i; j++) {
      if (buffer[j] != TESTDATA[j]) return 10;
    }

    return 0;
}
