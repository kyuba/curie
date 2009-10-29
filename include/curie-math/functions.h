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

#ifndef _LIBCURIE_MATH_FUNCTIONS_H
  #define LIBCURIE_MATH_FUNCTIONS_H
  
  #include <curie-math/constants.h>
  /*! \file 
    \brief Elementary functions
    
    This file contains definitions of functions like powers of arbitrary base/exponent,
    logarithms, roots etc.
    
    */
  
  /*! \brief base^exp */
  double pow (double base, double exp);
  
  /*! \brief Square root of x       
      \return The positive square root of x, if x is >= 0.
      For negative x, ERROR_UNDEFINED will be returned.
  */
  double sqrt (double x); 
  

  double exp (double x);
  
  double ln (double x);
  
  double log2 (double x);
  
  double log10 (double x);
  
  double logarithm (double base, double x);
#endif