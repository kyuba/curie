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
 
 namespace curiepp 
 {
 

 
 
    class SExpr 
    {
      
      public:
      SExprHeader *header;
      //! is the sexpr equal to another sexpr a?
      bool equalp (SExpr *a);
      
      void xref();
      virtual void destroy();
      
 /*! \brief Encode an Integer
 *  \param[in] integer The value of the new s-expression.
 *  \return The s-expression.
 *
 *  This macro takes a C integer and returns a new s-expression with the
 *  integer as its value.
 */
#define make_integer(integer)\
        ((SExpr*)((((int_pointer_s)(integer)) << 3) | 0x3))
        
 /*! \brief Encode a Special S-Expression
 *  \param[in] code The value of the new s-expression.
 *  \return The s-expression.
 *
 *  These s-expressions may be used as sentinel values and the like.
 */
#define make_special(code)\
        ((SExpr*)((((int_pointer)(code)) << 3) | 0x5))

#define define_sosi(t,n,s) \
    static const struct sexpr_string_or_symbol sexpr_payload_ ## n =\
        { { t, (unsigned short int)(~0) }, s };\
    static const sexpr n = ((const sexpr)&(sexpr_payload_ ## n))

#define define_string(name,value) define_sosi(sxt_string,name,value)

#define define_symbol(name,value) define_sosi(sxt_symbol,name,value)

/*! \brief NIL (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_nil make_special(1)

/*! \brief Boolean False (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_false make_special(2)

/*! \brief Boolean True (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_true make_special(3)

/*! \brief Empty List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_empty_list make_special(4)

/*! \brief End of List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_end_of_list make_special(5)

/*! \brief End of File (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_end_of_file make_special(6)

/*! \brief Not-a-Number (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_not_a_number make_special(7)

/*! \brief Nonexistent (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_nonexistent make_special(8)

/*! \brief Dot Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
#define sx_dot make_special(9)

/*! \brief S-Expression Pointer Flag
 *
 *  This flag is not set in the memory encoding of an s-expression if it's a
 *  pointer instead of the literal value.
 */
#define sx_mask_no_pointer 0x1

/*! @} */

/*! \defgroup sexprPredicates Predicates
 *  \ingroup sexpr
 *  \brief S-Expression Predicates
 *
 *  Predicates are functions (or function-like macros) that return a boolean
 *  value for the given arguments. For example consp() checks if a given
 *  s-expression is a cons, and equalp() checks if two s-expressions match up.
 *
 *  In Curie we're using the CL naming convention for predicates, in that all
 *  predicates end in 'p'.
 *
 *  @{
 */

/*! \brief Check if the two S-Expressions are equal
 *  \param[in] a S-expression to compare with b.
 *  \param[in] b S-expression to compare with a.
 *  \return sx_true if the two sexprs are equal or sx_false otherwise.
 *
 *  This function is used to determine whether the two s-expressions are the
 *  same, like the C '==' operator. However, the C type operator would only
 *  check if the two s-expressions were the exact same object; this function
 *  will try this test first, and if it fails it will examine the s-expressions
 *  themselves to find out if they're equivalent. If they are, sx_true is
 *  returned.
 *
 *  \note Unlike the type predicates, this function returns an s-expression, so
 *        if(equalp()) will not work in C code. Instead, use if(truep(equalp()))
 *        to test things.
 */
/*@notnull@*/ /*@shared@*/ sexpr equalp
        (/*@notnull@*/ sexpr a,
         /*@notnull@*/ sexpr b);

/*! \brief Check if the S-Expression is a Special Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a special expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define specialp(sx) ((((int_pointer)(sx)) & 0x7) == 0x5)

/*! \brief Check if the S-Expression is a Pointer
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a pointer, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define pointerp(sx) ((((int_pointer)(sx)) & sx_mask_no_pointer) == 0x0)

/*! \brief Check if the S-Expression is the NIL Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nilp(sx)   ((sexpr)(sx) == sx_nil)

/*! \brief Check if the S-Expression is the Boolean True Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is true, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define truep(sx)  ((sexpr)(sx) == sx_true)

/*! \brief Check if the S-Expression is is the Boolean False Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is false, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define falsep(sx) ((sexpr)(sx) == sx_false)

/*! \brief Check if the S-Expression is the empty List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an empty list, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define emptyp(sx) ((sexpr)(sx) == sx_empty_list)

/*! \brief Check if the S-Expression is the End-of-List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the end-of-list expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eolp(sx)   ((sexpr)(sx) == sx_end_of_list)

/*! \brief Check if the S-Expression is the End-of-File Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the end-of-file expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eofp(sx)   ((sexpr)(sx) == sx_end_of_file)

/*! \brief Check if the S-Expression is the Not-a-Number Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the not-a-number expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nanp(sx)   ((sexpr)(sx) == sx_not_a_number)

/*! \brief Check if the S-Expression is the nonexistent Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the nonexistent expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nexp(sx)   ((sexpr)(sx) == sx_nonexistent)

/*! \brief Check if the S-Expression is the Dot Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define dotp(sx)   ((sexpr)(sx) == sx_dot)

/*! \brief Check if the S-Expression is a Cons
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a cons, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define consp(sx)    (pointerp(sx) && (((struct sexpr_cons *)sx_pointer(sx))->header.type == sxt_cons))

/*! \brief Check if the S-Expression is a String
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a string, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define stringp(sx)  (pointerp(sx) && (((struct sexpr_string_or_symbol *)sx_pointer(sx))->header.type == sxt_string))

/*! \brief Check if the S-Expression is a Symbol
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a symbol, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define symbolp(sx)  (pointerp(sx) && (((struct sexpr_string_or_symbol *)sx_pointer(sx))->header.type == sxt_symbol))

/*! \brief Check if the S-Expression is an Integer
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an integer, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define integerp(sx) ((((int_pointer)(sx)) & 0x7) == 0x3)
      
    };
    
    class SExprHeader : SExpr
    {
      public:
      unsigned char type;
      unsigned short int references;
    };
    
    class SExprString : SExpr
    {
      private:
        SExprString(const char *string);
      public:
      
      SExprHeader *header;
      char characterData[];
            
      SExprString* makeString(const char *str);
      
      void destroy();
      SExprString *join(SExprString *sx);
      SExprString *join(SExprSymbol *sx);
    };
    
    class SExprSymbol : SExpr
    {
      private:
        SExprSymbol(const char *symbol);
      
      public:
        SExprHeader *header;
        char characterData[];
            
        SExprSymbol *makeSymbol(const char *symbol);
        void destroy();
        SExprSymbol *join(SExprString *sx);
        SExprSymbol *join(SExprSymbol *sx);
    };
    
    class SExprCons : SExpr
    {
      private:
        SExprCons(SExpr *car, SExprCons *cdr);
      
      public:
        SExprHeader *header;
        SExpr *car;
        SExprCons *cdr;
      
       
        SExpr *cons (SExpr *car, SExprCons *cdr);
    
        void destroy();
        void map(void (*f) (sexpr))
        SExpr fold(sexpr(*f)(sexpr, sexpr), sexpr seed);
    };
    
    class SExprIO 
    {
      private:
        SExprIO();
        SExprIO(IO *in, IO *out);
      
      public:
        SExprIO* openIO(IO *in, IO *out);
        SExprIO* openStdIO();
        void close();
      
        SExpr* read();
        void write(SExpr* sx);
    };
 }
 
 
 #endif