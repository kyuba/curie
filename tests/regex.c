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

define_string(str_testregex13,  ".+");
define_string(str_testregex14,  "[a]+");
define_string(str_testregex15,  "[ab-z][a-xyz][a-z][a-z][a-z][a-z][a-z][a-z]");

define_string(str_test1,      "whatever");
define_string(str_test2,      "aa");
define_string(str_test3,      "aab");
define_string(str_test4,      "aaa");
define_string(str_test5,      "aaaa");
define_string(str_test6,      "");

#define str_test7 "aa"
#define str_test8 "aaaaaaaaaaaaaaaaaaa"
#define str_test9 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

int cmain()
{
    struct graph *rx1  = rx_compile ("whatever"),
                 *rx2  = rx_compile ("aab|aaaa"),
                 *rx3  = rx_compile ("(aab|aaaa)"),
                 *rx4  = rx_compile ("aa(b|aa)"),
                 *rx5  = rx_compile ("aa(b|a)a?"),
                 *rx6  = rx_compile ("a*"),
                 *rx7  = rx_compile ("a(a)*"),
                 *rx8  = rx_compile ("ab*"),
                 *rx9  = rx_compile ("a+"),
                 *rx10 = rx_compile ("a?a?aa"),
                 *rx11 = rx_compile ("a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaaaaaaaaaaa"),
                 *rx12 = rx_compile ("a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
                 *rx13 = rx_compile_sx (str_testregex13),
                 *rx14 = rx_compile_sx (str_testregex14),
                 *rx15 = rx_compile_sx (str_testregex15);

    if (falsep (rx_match_sx (rx1, str_test1))) return 1;

    if (truep  (rx_match_sx (rx2, str_test1))) return 2;
    if (truep  (rx_match_sx (rx2, str_test2))) return 3;
    if (falsep (rx_match_sx (rx2, str_test3))) return 4;
    if (truep  (rx_match_sx (rx2, str_test4))) return 5;
    if (falsep (rx_match_sx (rx2, str_test5))) return 6;
    if (truep  (rx_match_sx (rx2, str_test6))) return 7;

    if (truep  (rx_match_sx (rx3, str_test1))) return 8;
    if (truep  (rx_match_sx (rx3, str_test2))) return 9;
    if (falsep (rx_match_sx (rx3, str_test3))) return 10;
    if (truep  (rx_match_sx (rx3, str_test4))) return 11;
    if (falsep (rx_match_sx (rx3, str_test5))) return 12;
    if (truep  (rx_match_sx (rx3, str_test6))) return 13;

    if (truep  (rx_match_sx (rx4, str_test1))) return 14;
    if (truep  (rx_match_sx (rx4, str_test2))) return 15;
    if (falsep (rx_match_sx (rx4, str_test3))) return 16;
    if (truep  (rx_match_sx (rx4, str_test4))) return 17;
    if (falsep (rx_match_sx (rx4, str_test5))) return 18;
    if (truep  (rx_match_sx (rx4, str_test6))) return 19;

    if (truep  (rx_match_sx (rx5, str_test1))) return 20;
    if (truep  (rx_match_sx (rx5, str_test2))) return 21;
    if (falsep (rx_match_sx (rx5, str_test3))) return 22;
    if (falsep (rx_match_sx (rx5, str_test4))) return 23;
    if (falsep (rx_match_sx (rx5, str_test5))) return 24;
    if (truep  (rx_match_sx (rx5, str_test6))) return 25;

    if (truep  (rx_match_sx (rx6, str_test1))) return 26;
    if (falsep (rx_match_sx (rx6, str_test2))) return 27;
    if (truep  (rx_match_sx (rx6, str_test3))) return 28;
    if (falsep (rx_match_sx (rx6, str_test4))) return 29;
    if (falsep (rx_match_sx (rx6, str_test5))) return 30;
    if (falsep (rx_match_sx (rx6, str_test6))) return 31;

    if (truep  (rx_match_sx (rx7, str_test1))) return 32;
    if (falsep (rx_match_sx (rx7, str_test2))) return 33;
    if (truep  (rx_match_sx (rx7, str_test3))) return 34;
    if (falsep (rx_match_sx (rx7, str_test4))) return 35;
    if (falsep (rx_match_sx (rx7, str_test5))) return 36;
    if (truep  (rx_match_sx (rx7, str_test6))) return 37;

    if (truep  (rx_match_sx (rx8, str_test1))) return 38;
    if (truep  (rx_match_sx (rx8, str_test2))) return 39;
    if (truep  (rx_match_sx (rx8, str_test3))) return 40;
    if (truep  (rx_match_sx (rx8, str_test4))) return 41;
    if (truep  (rx_match_sx (rx8, str_test5))) return 42;
    if (truep  (rx_match_sx (rx8, str_test6))) return 43;

    if (truep  (rx_match_sx (rx9, str_test1))) return 44;
    if (falsep (rx_match_sx (rx9, str_test2))) return 45;
    if (truep  (rx_match_sx (rx9, str_test3))) return 46;
    if (falsep (rx_match_sx (rx9, str_test4))) return 47;
    if (falsep (rx_match_sx (rx9, str_test5))) return 48;
    if (truep  (rx_match_sx (rx9, str_test6))) return 49;

    if (falsep (rx_match    (rx10, str_test7))) return 50;
    /* sorta evilish expression */
    if (falsep (rx_match    (rx11, str_test8))) return 51;
    /* very evil expression... i think */
    if (falsep (rx_match    (rx12, str_test9))) return 52;

    if (falsep (rx_match_sx (rx13, str_test1))) return 53;
    if (falsep (rx_match_sx (rx13, str_test2))) return 54;
    if (falsep (rx_match_sx (rx13, str_test3))) return 55;
    if (falsep (rx_match_sx (rx13, str_test4))) return 56;
    if (falsep (rx_match_sx (rx13, str_test5))) return 57;
    if (truep  (rx_match_sx (rx13, str_test6))) return 58;
    if (falsep (rx_match    (rx13, str_test7))) return 59;
    if (falsep (rx_match    (rx13, str_test8))) return 60;
    if (falsep (rx_match    (rx13, str_test9))) return 61;

    if (truep  (rx_match_sx (rx14, str_test1))) return 62;
    if (falsep (rx_match_sx (rx14, str_test2))) return 63;
    if (truep  (rx_match_sx (rx14, str_test3))) return 64;
    if (falsep (rx_match_sx (rx14, str_test4))) return 65;
    if (falsep (rx_match_sx (rx14, str_test5))) return 66;
    if (truep  (rx_match_sx (rx14, str_test6))) return 67;

    if (falsep (rx_match_sx (rx15, str_test1))) return 68;
    if (truep  (rx_match_sx (rx15, str_test2))) return 69;
    if (truep  (rx_match_sx (rx15, str_test3))) return 70;
    if (truep  (rx_match_sx (rx15, str_test4))) return 71;
    if (truep  (rx_match_sx (rx15, str_test5))) return 72;
    if (truep  (rx_match_sx (rx15, str_test6))) return 73;

    graph_destroy (rx1);
    graph_destroy (rx2);
    graph_destroy (rx3);
    graph_destroy (rx4);
    graph_destroy (rx5);
    graph_destroy (rx6);
    graph_destroy (rx7);
    graph_destroy (rx8);
    graph_destroy (rx9);
    graph_destroy (rx10);
    graph_destroy (rx11);
    graph_destroy (rx12);
    graph_destroy (rx13);
    graph_destroy (rx14);
    graph_destroy (rx15);

    return 0;
}
