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

#include <curie++/sexpr.h>

using namespace curiepp;

SExpr::SExpr ()
{
    value = sx_nil;
}

SExpr::SExpr (sexpr sx)
{
    value = sx;
}

inline bool SExpr::isNil ()
{
    return nilp (value);
}

inline bool SExpr::isTrue ()
{
    return truep (value);
}

inline bool SExpr::isFalse ()
{
    return falsep (value);
}

inline bool SExpr::isEmpty ()
{
    return emptyp (value);
}

inline bool SExpr::isEol ()
{
    return eolp (value);
}

inline bool SExpr::isEof ()
{
    return eofp (value);
}

inline bool SExpr::isNan ()
{
    return nanp (value);
}

inline bool SExpr::isNex ()
{
    return nexp (value);
}

inline bool SExpr::isDot ()
{
    return dotp (value);
}

inline bool SExpr::isQuote ()
{
    return quotep (value);
}

inline bool SExpr::isQq ()
{
    return qqp (value);
}

inline bool SExpr::isUnquote ()
{
    return unquotep (value);
}

inline bool SExpr::isSplice ()
{
    return splicep (value);
}

inline bool SExpr::isCons ()
{
    return consp (value);
}

inline bool SExpr::isString ()
{
    return stringp (value);
}

inline bool SExpr::isSymbol ()
{
    return symbolp (value);
}

inline bool SExpr::isInteger ()
{
    return integerp (value);
}

inline SExpr::operator sexpr ()
{
    return value;
}

inline SExpr::operator const char * ()
{
    if (stringp (value))
    {
        return sx_string (value);
    }
    else
    {
        return sx_symbol (value);
    }
}

SExpr SExpr::createSymbol (const char *symbol)
{
    SExpr sx = SExpr (make_symbol (symbol));
    return sx;
}

SExpr::SExpr (const char *string)
{
    value = make_string (string);
}

SExpr::SExpr (signed long integer)
{
    value = make_integer (integer);
}

SExpr::SExpr (SExpr &car, SExpr &cdr)
{
    value = cons (car, cdr);
}

SExprIO::SExprIO ()
{
    context = (struct sexpr_io *)0;
}

SExprIO::SExprIO (IO &in, IO &out)
{
    context = ((in.context  == (struct io *)0) &&
               (out.context == (struct io *)0)) ?
              (struct sexpr_io *)0 :
              sx_open_io   (in.context, out.context);

    in.context = (struct io *)0;
    out.context = (struct io *)0;
}

SExprIO::~SExprIO ()
{
    if (context != (struct sexpr_io *)0)
    {
        sx_close_io (context);
    }
}

void SExprIO::write (SExpr sx)
{
    if (context != (struct sexpr_io *)0)
    {
        sx_write (context, sx);
    }
}

SExpr SExprIO::read ()
{
    if (context != (struct sexpr_io *)0)
    {
        return SExpr (sx_read (context));
    }

    return SExpr (sx_nonexistent);
}

SExprIOMultiplexer::SExprIOMultiplexer (SExprIO &io)
{
    multiplex_sexpr ();

    context = io;
    io.context = (struct sexpr_io *)0;

    if (context.context != (struct sexpr_io *)0)
    {
        multiplex_add_sexpr (context.context, onReadCallback, (void *)this);
    }
}

SExprIOMultiplexer::~SExprIOMultiplexer()
{
    if (context.context != (struct sexpr_io *)0)
    {
        sx_close_io (context.context);
    }
}

void SExprIOMultiplexer::onRead (SExpr sx)
{
}

void SExprIOMultiplexer::onReadCallback(sexpr v, struct sexpr_io *io, void *aux)
{
    SExprIOMultiplexer *m = (SExprIOMultiplexer *)aux;
    m->onRead (v);
}
