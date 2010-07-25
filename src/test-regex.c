/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include <curie/main.h>
#include <curie/memory.h>
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

define_string(str_test10,     "λ");
define_string(str_test11,     "λy.x");

int cmain()
{
    sexpr rx1  = rx_compile ("whatever"),
          rx2  = rx_compile ("aab|aaaa"),
          rx3  = rx_compile ("(aab|aaaa)"),
          rx4  = rx_compile ("aa(b|aa)"),
          rx5  = rx_compile ("aa(b|a)a?"),
          rx6  = rx_compile ("a*"),
          rx7  = rx_compile ("a(a)*"),
          rx8  = rx_compile ("ab*"),
          rx9  = rx_compile ("a+"),
          rx10 = rx_compile ("a?a?aa"),
          rx11 = rx_compile ("a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaaaaaaaaaaa"),
          rx12 = rx_compile ("a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
          rx13 = rx_compile_sx (str_testregex13),
          rx14 = rx_compile_sx (str_testregex14),
          rx15 = rx_compile_sx (str_testregex15),
          rx16 = rx_compile ("[α-ω]");

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

    if (falsep (rx_match_sx (rx16, str_test10)))return 74;
    if (truep  (rx_match_sx (rx16, str_test11)))return 75;
    if (truep  (rx_match_sx (rx16, str_test6))) return 76;

    sx_destroy (rx1);
    sx_destroy (rx2);
    sx_destroy (rx3);
    sx_destroy (rx4);
    sx_destroy (rx5);
    sx_destroy (rx6);
    sx_destroy (rx7);
    sx_destroy (rx8);
    sx_destroy (rx9);
    sx_destroy (rx10);
    sx_destroy (rx11);
    sx_destroy (rx12);
    sx_destroy (rx13);
    sx_destroy (rx14);
    sx_destroy (rx15);
    sx_destroy (rx16);

    optimise_static_memory_pools();

    return 0;
}
