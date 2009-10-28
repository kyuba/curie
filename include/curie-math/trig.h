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

#ifndef LIBCURIE_MATH_TRIG_H
  #define LIBCURIE_MATH_TRIG_H
  /*! \file 
    \brief Trigonometry functions
    
    */
  
  #include <curie-math/constants.h>

  double sin (double x);

  double sinh (double x);
  
  double arcsin (double x);
  
  double arcsinh (double x); 
  
  double cos (double x);
  
  double cosh (double x);
  
  double arccos (double x);

  double arccosh (double x);
  
  double tan (double x);
  
  double tanh (double x);
  
  double arctan (double x);
  
  double arctanh (double x);
  
  double cot (double x);
  
  double coth (double x);
  
  double arccot (double x);
  
  double arccoth (double x); 
#endif