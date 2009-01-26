/*
 *  sexpr.h
 *  libcurie++
 *
 *  Created by Magnus Deininger on 01/01/2009.
 *  Copyright 2008/2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008/2009, Magnus Deininger All rights reserved.
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