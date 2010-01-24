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

#include <curie-math/matrix.h>

vector init (double * data_, unsigned int dimension_) 
{
  vector ret;
  ret.dimension = dimension_;
  ret.data = data_;
  return ret;
	
}

vector vadd(vector v1, vector v2) 
{
  if(v1.dimension != v2.dimension) 
  {
   return UNDEFINED_VECTOR; 
  }
  else 
  {
      double retdata[v1.dimension];
      for(int i = 0; i < v1.dimension; i++) 
      {
		retdata[i] = (v1.data[i] + v2.data[i]);
      }
      vector ret = {retdata, v1.dimension};
      return ret;
  }
}

vector scalarm(double alpha, vector v) 
{
    double *d = v.data;
    
    for(int i = 0; i < v.dimension; i++) 
    {
		d[i] *= alpha;
    }
    
    vector ret = {d, v.dimension};
    return ret;
}

double dot_product (vector v1, vector v2)
{
	if(v1.dimension != v2.dimension) 
	{
		return NEGINFINITY;
	}
	else 
	{
		double ret = 0.0;
		for(int i = 0; i < v1.dimension; i++)
		{
		    ret += (v1.data[i] * v2.data[i]);		
		}
		return ret;
	}
}

vector cross_product (vector v1, vector v2)
{
	if((v1.dimension != v2.dimension) || (v1.dimension != 3))
	{
		return UNDEFINED_VECTOR;
	}
	else 
	{
		double cp[v1.dimension];
		cp[0] = (v1.data[1] * v2.data[2]) - (v1.data[2] * v2.data[1]);
		cp[1] = (v1.data[2] * v2.data[0]) - (v1.data[0] * v2.data[2]);
		cp[2] = (v1.data[0] * v2.data[1]) - (v1.data[1] * v2.data[0]);
		vector ret = {cp, 3};
		return ret;
	}
}

matrix madd(matrix m1, matrix m2)
{
	if ((m1.columns != m2.columns) || (m1.rows != m2.rows))
	{ 
		return UNDEFINED_MATRIX;
	}
	else 
	{
		double d[m1.rows][m1.columns];
		for(int i = 0; i < m1.rows; i++) {
			for(int k = 0; k < m1.columns; k++) {
				d[i][k] = m1.data[i][k] + m2.data[i][k];				
			}
		}
		matrix ret = {d, m1.rows, m1.columns};
		return ret;
	}
}
