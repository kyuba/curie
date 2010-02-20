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

/*! \file
 *  \brief S-Expression Handling (C++)
 *
 *  The objects in this header wrap around the functions in curie/sexpr.h to
 *  provide a more C++-friendly way to work with S-Expressions.
 *
 *  Note: this is quite possibly not compatible with the garbage collector from
 *  curie/gc.h.
 */

#ifndef LIBCURIEPP_SEXPR_H
#define LIBCURIEPP_SEXPR_H

#include <curie/sexpr.h>
#include <curie++/io.h>

namespace curiepp
{
    /*! \brief S-Expression
     *
     *  Basic S-Expression class.
     */
    class SExpr
    {
        public:
            /*! \brief Constructor
             *
             *  This constructor will create a nil value.
             */
            SExpr ();

            /*! \brief Constructor
             *  \param[in] sx S-expression to wrap up.
             *
             *  This constructor will create a new object based on the sx
             *  parameter.
             */
            SExpr (sexpr sx);

            /*! \brief Constructor (for an Integer S-Expression)
             *  \param[in] integer The integer to wrap up.
             *
             *  This constructor will create a new object based on the integer
             *  parameter.
             */
            SExpr (signed long integer);

            /*! \brief Constructor (for a String S-Expression)
             *  \param[in] string The string to wrap up.
             *
             *  This constructor will create a new object based on the string
             *  parameter.
             */
            SExpr (const char *string);

            /*! \brief Constructor (for a new Cons)
             *  \param[in] car The car of the new cons.
             *  \param[in] cdr The cdr of the new cons.
             *
             *  This constructor will create a new cons based on the car and cdr
             *  parameters.
             */
            SExpr (SExpr &car, SExpr &cdr);

            /*! \brief Find out if the object is a nil Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for nilp().
             */
            bool isNil ();

            /*! \brief Find out if the object is a true (\#t) Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for truep().
             */
            bool isTrue ();

            /*! \brief Find out if the object is a false (\#f) Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for nilp().
             */
            bool isFalse ();

            /*! \brief Find out if the object is an empty List Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for emptyp().
             */
            bool isEmpty ();

            /*! \brief Find out if the object is an end-of-list Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for eolp().
             */
            bool isEol ();

            /*! \brief Find out if the object is an end-of-file Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for eofp().
             */
            bool isEof ();

            /*! \brief Find out if the object is a not-a-number Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for nanp().
             */
            bool isNan ();

            /*! \brief Find out if the object is a nonexistent Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for nexp().
             */
            bool isNex ();

            /*! \brief Find out if the object is a dot Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for dotp().
             */
            bool isDot ();

            /*! \brief Find out if the object is a quote Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for quotep().
             */
            bool isQuote ();

            /*! \brief Find out if the object is a quasiquote Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for qqp().
             */
            bool isQq ();

            /*! \brief Find out if the object is a unquote Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for unquotep().
             */
            bool isUnquote();

            /*! \brief Find out if the object is a splice Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for splicep().
             */
            bool isSplice ();

            /*! \brief Find out if the object is a cons Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for consp().
             */
            bool isCons ();

            /*! \brief Find out if the object is a string Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for stringp().
             */
            bool isString ();

            /*! \brief Find out if the object is a symbol Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for symbolp().
             */
            bool isSymbol ();

            /*! \brief Find out if the object is an integer Expression
             *  \return True or false, depending on the object's type.
             *
             *  Wrapper for integerp().
             */
            bool isInteger ();

            /*! \brief (sexpr) Typecast
             *
             *  Typecast to the curie s-expression type.
             */
            operator sexpr();

            /*! \brief (const char *) Typecast
             *
             *  Typecast to a C string (only works for symbols and strings).
             */
            operator const char *();

            /*! \brief Named Constructor (for a Symbol S-Expression)
             *  \param[in] symbol The symbol to wrap up.
             *
             *  This constructor will create a new object based on the symbol
             *  parameter. Since both symbols and strings are really just typed
             *  strings, it wasn't possible to create distinct constructors for
             *  each of the two types of s-expressions.
             */
            static SExpr createSymbol (const char *symbol);

        protected:
            /*! \brief The actual S-Expression Value
             *
             *  This is the C sexpr value that this object wraps around.
             */
            sexpr value;
    };

    /*! \brief S-Expression I/O
     *
     *  Wrapper for all the s-expression i/o functions.
     */
    class SExprIO
    {
        public:
            /*! \brief Dummy Constructor
             *
             *  Dummy constructor for an "empty" s-expression I/O structure.
             *  You probably won't need this.
             */
            SExprIO ();

            /*! \brief Constructor
             *  \param[in] in  Input structure.
             *  \param[in] out Output structure.
             *
             *  Regular constructor, analoguous to sx_open_io();
             */
            SExprIO (IO &in, IO &out);

            /*! \brief Destructor
             *
             *  This will automatically use sx_close_io().
             */
            ~SExprIO ();

            /*! \brief Write S-expression
             *  \param[in] sx The s-expression to write.
             *
             *  Wrapper for sx_write().
             */
            void write (SExpr sx);

            /*! \brief Read S-expression
             *
             *  Wrapper for sx_read().
             */
            SExpr read ();

        protected:
            /*! \brief S-Expression IO Structure
             *
             *  The raw sexpr io structure that this object operates on.
             */
            struct sexpr_io *context;

            /* This friend-relationship is needed because the multiplexer code
               needs to get at the raw sexpr_io structure, but in general the
               structure should not be accessed directly. */
            friend class SExprIOMultiplexer;
    };

    /*! \brief Sexpr Multiplexer
     *
     *  This is a base class to derive your own S-Expression I/O multiplexers
     *  from.
     */
    class SExprIOMultiplexer : public Multiplexer
    {
        public:
            /*! \brief Constructor
             *  \param[in] io The object to use.
             *
             *  Hijack the given SexprIO object by setting its context to
             *  (struct io *)0 and setting up the multiplexer.
             */
            SExprIOMultiplexer (SExprIO &io);

            /*! \brief Destructor
             *
             *  This will close the SexprIO object.
             */
            ~SExprIOMultiplexer ();

        protected:
            /*! \brief Read Callback
             *  \param[in] sx The s-expression that has been read.
             *
             *  This is called when new data comes in. Overwrite when you need
             *  to use this event.
             */
            virtual void onRead (SExpr sx);

            /*! \brief SexprIO Structure
             *
             *  The SexprIO object this multiplexer applies to.
             */
            SExprIO context;

        private:
            /*! \brief Raw Read Callback
             *
             *  This function is passed to multiplex_add_sexpr() as the
             *  on_read() callback.
             */
            static void onReadCallback(sexpr v, struct sexpr_io *io, void *aux);
    };
}

#endif
