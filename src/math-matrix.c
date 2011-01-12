/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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
#include <curie/memory.h>

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

double ** madd_data(double **d1, double** d2, int rows, int columns)
{
	double ** ret = get_mem(rows * sizeof(double *)); 
	
	for(int i = 0; i < rows; i++) {
		ret[i] = get_mem(columns * sizeof(double)); 
		for(int k = 0; k < columns; k++) {
			ret[i][k] = d1[i][k] + d2[i][k]; 
		}
	}
	
	return ret;
}

matrix madd(matrix m1, matrix m2)
{
	if ((m1.columns != m2.columns) || (m1.rows != m2.rows))
	{ 
		return UNDEFINED_MATRIX;
	}
	else 
	{
		double ** data = madd_data( m1.data, m2.data, m1.rows, m1.columns);
		matrix m = {data, m1.rows, m1.columns};
		return m;
	}
}

matrix smmult(double alpha, matrix m)
{
	if(m.data != (void *) 0)
	{
		double ** rdata = get_mem(m.rows * sizeof(double*));
		for(int i = 0; i < m.rows; i++)
		{
			rdata[i] = get_mem(m.columns * sizeof(double));
			for(int k = 0; k < m.columns; k++) 
			{
				rdata[i][k] = alpha * (m.data[i])[k];
			}
		}
		matrix ret = {rdata, m.rows, m.columns};
		return ret; 
	}
	else 
	{
		return UNDEFINED_MATRIX;
	}
}
