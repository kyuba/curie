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

#include "curie/io-system.h"

#define TESTDATA       "TEST-DATA"
#define TESTDATA_SIZE  (int)(sizeof(TESTDATA)-1)

int cmain(void) {
/* test the a_create() and a_close() functions */
    int fd, i, j;
    char buffer[TESTDATA_SIZE];

    fd = a_create ("to-curie-test-data", 0660);
    if (fd < 0) return 1;

    i = a_close(fd);
    if (i < 0) return 2;

/* test the a_write() and a_open_write() functions */
    fd = a_open_write ("to-curie-test-data");
    if (fd < 0) return 3;

    i = a_write(fd, TESTDATA, (unsigned int)TESTDATA_SIZE);
    if (i < 0) return 4;
    if (i != TESTDATA_SIZE) return 5;

/* close file descriptor */
    i = a_close(fd);
    if (i < 0) return 6;

/* test the a_read() and a_open_read() functions */
    fd = a_open_read ("to-curie-test-data");
    if (fd < 0) return 7;

    i = a_read (fd, buffer, (unsigned int)TESTDATA_SIZE);
    if (i < 0) return 8;
    if (i != TESTDATA_SIZE) return 9;

    for (j = 0; j < i; j++) {
      if (buffer[j] != TESTDATA[j]) return 10;
    }

    return 0;
}
