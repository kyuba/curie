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

#ifndef LIBCURIE_MATH_MATH_H
  #define LIBCURIE_MATH_MATH_H

  /*! \file
    \brief Mathematical functions and other math-y goodness
    
    This file pretty much just includes the more specific headers. 
    I want the lib to be rather modular, so that you can include what you need rather than the whole lib.
    Only if you really need the functionality of the whole lib or at least its better part, include this file.
  */

  /*! \brief Trigonometry */
  #include <curie-math/trig.h>

  /*! \brief Matrix and vector operations */
  #include <curie-math/matrix.h>

  // /*! \brief Statistical operations */
  // #include <curie-math/stats.h> 

  /*! \brief Some special functions,  like logarithms, powers, roots and such */
  #include <curie-math/functions.h>

  /*! \brief Mathematical constants */
  #include <curie-math/constants.h>

#endif