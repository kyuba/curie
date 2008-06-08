/*
 *  read-write.c
 *  atomic-libc
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

#include "atomic/nucleus.h"

#define TESTDATA       "TEST-DATA"
#define TESTDATA_SIZE  (int)(sizeof(TESTDATA)-1)

int atomic_main(void) {
/* test the _atomic_create() and _atomic_close() functions */
    int fd, i, j;
    char buffer[TESTDATA_SIZE];

    fd = _atomic_create ("temporary-atomic-test-data", 0660);
    if (fd < 0) return 1;

    i = _atomic_close(fd);
    if (i < 0) return 2;

/* test the _atomic_write() and _atomic_open_write() functions */
    fd = _atomic_open_write ("temporary-atomic-test-data");
    if (fd < 0) return 3;

    i = _atomic_write(fd, TESTDATA, TESTDATA_SIZE);
    if (i < 0) return 4;
    if (i != TESTDATA_SIZE) return 5;

/* close file descriptor */
    i = _atomic_close(fd);
    if (i < 0) return 6;

/* test the _atomic_read() and _atomic_open_read() functions */
    fd = _atomic_open_read ("temporary-atomic-test-data");
    if (fd < 0) return 7;

    i = _atomic_read (fd, buffer, TESTDATA_SIZE);
    if (i < 0) return 8;
    if (i != TESTDATA_SIZE) return 9;
    
    for (j = 0; j < i; j++) {
      if (buffer[j] != TESTDATA[j]) return 10;
    }

    return 0;
}
