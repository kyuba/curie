/*
 *  tests/regex.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 20/12/2008.
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

#include <curie/main.h>
#include <curie/regex.h>

define_string(str_testregex1,  "whatever");
define_string(str_testregex2,  "aab|aaaa");
define_string(str_testregex3,  "(aab|aaaa)");
define_string(str_testregex4,  "aa(b|aa)");
define_string(str_testregex5,  "aa(b|a)a?");
define_string(str_testregex6,  "a*");
define_string(str_testregex7,  "a(a)*");
define_string(str_testregex8,  "ab*");
define_string(str_testregex9,  "a+");
define_string(str_testregex10, "a?a?aa");
define_string(str_testregex11, "a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaaaaaaaaaaa");
define_string(str_testregex12, "a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaavaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

define_string(str_test1,      "whatever");
define_string(str_test2,      "aa");
define_string(str_test3,      "aab");
define_string(str_test4,      "aaa");
define_string(str_test5,      "aaaa");
define_string(str_test6,      "");
define_string(str_test7,      "aa");
define_string(str_test8,      "aaaaaaaaaaaaaaaaaaa");
define_string(str_test9,      "aaaaaaaaaavaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

int cmain()
{
    struct graph *rx1  = rx_compile (str_testregex1),
                 *rx2  = rx_compile (str_testregex2),
                 *rx3  = rx_compile (str_testregex3),
                 *rx4  = rx_compile (str_testregex4),
                 *rx5  = rx_compile (str_testregex5),
                 *rx6  = rx_compile (str_testregex6),
                 *rx7  = rx_compile (str_testregex7),
                 *rx8  = rx_compile (str_testregex8),
                 *rx9  = rx_compile (str_testregex9),
                 *rx10 = rx_compile (str_testregex10),
                 *rx11 = rx_compile (str_testregex11),
                 *rx12 = rx_compile (str_testregex12);

    if (falsep (rx_match (rx1, str_test1))) return 1;

    if (truep  (rx_match (rx2, str_test1))) return 2;
    if (truep  (rx_match (rx2, str_test2))) return 3;
    if (falsep (rx_match (rx2, str_test3))) return 4;
    if (truep  (rx_match (rx2, str_test4))) return 5;
    if (falsep (rx_match (rx2, str_test5))) return 6;
    if (truep  (rx_match (rx2, str_test6))) return 7;

    if (truep  (rx_match (rx3, str_test1))) return 8;
    if (truep  (rx_match (rx3, str_test2))) return 9;
    if (falsep (rx_match (rx3, str_test3))) return 10;
    if (truep  (rx_match (rx3, str_test4))) return 11;
    if (falsep (rx_match (rx3, str_test5))) return 12;
    if (truep  (rx_match (rx3, str_test6))) return 13;

    if (truep  (rx_match (rx4, str_test1))) return 14;
    if (truep  (rx_match (rx4, str_test2))) return 15;
    if (falsep (rx_match (rx4, str_test3))) return 16;
    if (truep  (rx_match (rx4, str_test4))) return 17;
    if (falsep (rx_match (rx4, str_test5))) return 18;
    if (truep  (rx_match (rx4, str_test6))) return 19;

    if (truep  (rx_match (rx5, str_test1))) return 20;
    if (truep  (rx_match (rx5, str_test2))) return 21;
    if (falsep (rx_match (rx5, str_test3))) return 22;
    if (falsep (rx_match (rx5, str_test4))) return 23;
    if (falsep (rx_match (rx5, str_test5))) return 24;
    if (truep  (rx_match (rx5, str_test6))) return 25;

    if (truep  (rx_match (rx6, str_test1))) return 26;
    if (falsep (rx_match (rx6, str_test2))) return 27;
    if (truep  (rx_match (rx6, str_test3))) return 28;
    if (falsep (rx_match (rx6, str_test4))) return 29;
    if (falsep (rx_match (rx6, str_test5))) return 30;
    if (falsep (rx_match (rx6, str_test6))) return 31;

    if (truep  (rx_match (rx7, str_test1))) return 32;
    if (falsep (rx_match (rx7, str_test2))) return 33;
    if (truep  (rx_match (rx7, str_test3))) return 34;
    if (falsep (rx_match (rx7, str_test4))) return 35;
    if (falsep (rx_match (rx7, str_test5))) return 36;
    if (truep  (rx_match (rx7, str_test6))) return 37;

    if (truep  (rx_match (rx8, str_test1))) return 38;
    if (truep  (rx_match (rx8, str_test2))) return 39;
    if (truep  (rx_match (rx8, str_test3))) return 40;
    if (truep  (rx_match (rx8, str_test4))) return 41;
    if (truep  (rx_match (rx8, str_test5))) return 42;
    if (truep  (rx_match (rx8, str_test6))) return 43;

    if (truep  (rx_match (rx9, str_test1))) return 44;
    if (falsep (rx_match (rx9, str_test2))) return 45;
    if (truep  (rx_match (rx9, str_test3))) return 46;
    if (falsep (rx_match (rx9, str_test4))) return 47;
    if (falsep (rx_match (rx9, str_test5))) return 48;
    if (truep  (rx_match (rx9, str_test6))) return 49;

    if (falsep (rx_match (rx10, str_test7))) return 50;
    /* sorta evilish expression */
    if (falsep (rx_match (rx11, str_test8))) return 51;
    /* very evil expression... i think */
    if (falsep (rx_match (rx12, str_test9))) return 52;

    rx_free (rx1);
    rx_free (rx2);
    rx_free (rx3);
    rx_free (rx4);
    rx_free (rx5);
    rx_free (rx6);
    rx_free (rx7);
    rx_free (rx8);
    rx_free (rx9);
    rx_free (rx10);
    rx_free (rx11);

    return 0;
}
