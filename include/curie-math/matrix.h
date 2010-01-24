/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010 Kyuba Project Members
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

#ifndef LIBCURIE_MATH_MATRIX_H
  #define LIBCURIE_MATH_MATRIX_H
  
  /*! \file 
    \brief Vector and matrix operations */
  
  #include <curie-math/constants.h>
  
  #ifdef __cplusplus
  namespace curiemath {
    extern "C" {
  #endif
      /*! Curie-math one-dimensional vectors are elements of {Q^n | n \in N, n >= 1}. 
	We are aware there are other vector spaces out there, but for our purposes a vector is just an array of rational numbers
	[I'd say real numbers but you can't really use irrational numbers in a computer. For starters,  your memory's finite).
	Its dimension is memorized for convenience reasons. 
	
	Users should  not alter the vector itself, but instead create a  new one, especially if its dimension is going to change.*/
    struct vector {
	  double* data;
	  unsigned int dimension;
    };
     typedef struct vector vector;
      
      #define UNDEFINED_VECTOR (struct vector) {(void*) 0, 0}
      
      /*! Vector addition. 
      
      Vectors are added component-wise. Let x and y be 2 vectors, and let x = (x1, x2) and y = (y1, y2).
      Then x+y = (x1+y1, x2+y2).
      
      If the vectors to be added have different dimensions, UNDEFINED_VECTOR will be returned.*/
      // todo: variable number of arguments could come in handy here, too
      vector vadd(vector v1, vector v2);
            
      /*! Scalar multiplication of a vector with a scalar from Q*/
      vector scalarm(double alpha, vector v1);

      vector cross_product(vector v1, vector v2);
	  
	  double dot_product(vector v1, vector v2);
      
      vector init (double * d, unsigned int dimension);
	  
	 
	 /*! Matrix operations
	  *
	  * */
	  
	  
	  struct matrix {
		double **data;
		unsigned int rows;
		unsigned int columns;
	  };
	  typedef struct matrix matrix;
	  
	  #define UNDEFINED_MATRIX (struct matrix) {(void *) 0, 0, 0}
	  
	  matrix madd(matrix m1, matrix m2);
	  
	  matrix mmult(matrix m1, matrix m2);
	  
	  matrix smmult(double alpha, matrix m);
	  
	  double det(matrix m);
	  
	  matrix invert(matrix m);
	  

  #ifdef __cplusplus
    }
  }
#endif
   

#endif
