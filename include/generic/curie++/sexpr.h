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

 #include <curie++/int.h>
 #include <curie++/io.h>
 #include <curie/sexpr.h>

 namespace curiepp
 {

    class SExpr
    {

      public:

        SExpr();
        //! allows for compatibility with the C types
        SExpr(sexpr);
        ~SExpr();
        unsigned int references;

        virtual SExpr* equalp (SExpr *a);
        virtual void xref();
        virtual void destroy();

    };

    // virtual base class for strings and symbols
    class SExprStringOrSymbol : SExpr
    {
      private:


      public:
        virtual char characterData[];

        virtual SExprStringOrSymbol *join(SExprStringOrSymbol *sx);
    };

    class SExprString : SExprStringOrSymbol
    {
      private:

      public:
        SExprString(const char *data);
        ~SExprString();
    };

    class SExprSymbol : SExprStringOrSymbol
    {
      private:
      public:
        SExprSymbol(const char *data);
        ~SExprSmbol();

    };

    class SExprCons : SExpr
    {
      private:


      public:
        SExprCons (SExpr *car, SExpr *cdr);
        ~SExprCons();
        SExpr *car;
        SExprCons *cdr;

        SExpr *cons (SExpr *car, SExpr *cdr);

        void map(void (*f) (sexpr))
        SExpr *fold(sexpr(*f)(sexpr, sexpr), sexpr seed);
    };

    class SExprIO
    {
      private:
        SExprIO();
        SExprIO(IO *in, IO *out);

        SExprString *readString(unsigned int *i, char *buf, unsigned int length);
        SExprSymbol *readSymbol(unsigned int *i, char *buf, unsigned int length);
        SExpr *readNumber(unsigned int *i, char *buf, unsigned int length);
        SExprCons *readCons(unsigned int *i, char *buf, unsigned int length);
        SExprCons *readConsFinalise(unsigned int *i, char *buf, unsigned int length);
        void readDispatch(unsigned int *i, char *buf, unsigned int length);

        void writeStringOrSymbol(SExprStringOrSymbol *s);
        void writeCons(SExprCons *c);
        void writeInteger(SExpr *i);
        void writeDispatch (SExpr *sexp);


      public:
        SExprIO* openIO(IO *in, IO *out);
        SExprIO* openStdIO();
        ~SExprIO();
        void close();

        SExpr* read();
        void write(SExpr* sx);
    };
 }

 #endif