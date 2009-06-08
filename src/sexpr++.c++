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

#include <curie++/sexpr.h>

using namespace curiepp;

SExpr::SExpr               ()
{
    value                = sx_nil;
}

SExpr::SExpr               (sexpr sx)
{
    value                = sx;
}

SExpr::~SExpr              ()
{
    sx_destroy             (value);
}

bool SExpr::isNil          ()
{
    return nilp            (value);
}

bool SExpr::isTrue         ()
{
    return truep           (value);
}

bool SExpr::isFalse        ()
{
    return falsep          (value);
}

bool SExpr::isEmpty        ()
{
    return emptyp          (value);
}

bool SExpr::isEol          ()
{
    return eolp            (value);
}

bool SExpr::isEof          ()
{
    return eofp            (value);
}

bool SExpr::isNan          ()
{
    return nanp            (value);
}

bool SExpr::isNex          ()
{
    return nexp            (value);
}

bool SExpr::isDot          ()
{
    return dotp            (value);
}

bool SExpr::isQuote        ()
{
    return quotep          (value);
}

bool SExpr::isQq           ()
{
    return qqp             (value);
}

bool SExpr::isUnquote      ()
{
    return unquotep        (value);
}

bool SExpr::isSplice       ()
{
    return splicep         (value);
}

bool SExpr::isCons         ()
{
    return consp           (value);
}

bool SExpr::isString       ()
{
    return stringp         (value);
}

bool SExpr::isSymbol       ()
{
    return symbolp         (value);
}

bool SExpr::isInteger      ()
{
    return integerp        (value);
}

SExprSymbol::SExprSymbol   (const char *symbol)
{
    value                = make_symbol (symbol);
}

SExprString::SExprString   (const char *string)
{
    value                = make_string (string);
}

SExprInteger::SExprInteger (signed long integer)
{
    value                = make_integer (integer);
}

SExprCons::SExprCons       (SExpr &car, SExpr &cdr)
{
    value                = cons (car.value, cdr.value);
}
