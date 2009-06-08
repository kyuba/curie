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

#ifndef LIBCURIEPP_SEXPR_H
#define LIBCURIEPP_SEXPR_H

#include <curie/sexpr.h>
#include <curie++/int.h>
#include <curie++/io.h>
#include <curie++/multiplex.h>

namespace curiepp
{

    class SExpr
    {
        public:
            SExpr         ();
            SExpr         (sexpr sx);
            ~SExpr        ();

            bool isNil    ();
            bool isTrue   ();
            bool isFalse  ();
            bool isEmpty  ();
            bool isEol    ();
            bool isEof    ();
            bool isNan    ();
            bool isNex    ();
            bool isDot    ();
            bool isQuote  ();
            bool isQq     ();
            bool isUnquote();
            bool isSplice ();
            bool isCons   ();
            bool isString ();
            bool isSymbol ();
            bool isInteger();

        protected:
            sexpr value;
            friend class SExprCons;
    };

    class SExprSymbol : public SExpr
    {
        public:
            SExprSymbol   (const char *symbol);
    };

    class SExprString : public SExpr
    {
        public:
            SExprString   (const char *string);
    };

    class SExprInteger : public SExpr
    {
        public:
            SExprInteger  (signed long integer);
    };

    class SExprCons    : public SExpr
    {
        public:
            SExprCons     (SExpr &car, SExpr &cdr);
    };

    class SExprIO
    {
        public:
            SExprIO       (IO *in, IO *out);
            ~SExprIO      ();

            void write    (SExpr sx);
            SExpr read    ();

        protected:
            struct sexpr_io *context;
    };

    class SExprIOMultiplexer : public Multiplexer
    {
        public:
            SExprIOMultiplexer
                          (SExprIO *io);
            ~SExprIOMultiplexer
                          ();

            void on_read  (SExpr sx);
            void on_close ();

            void clearContext();

        protected:
            SExprIO *context;
    };
}

#endif
