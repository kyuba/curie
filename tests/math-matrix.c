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
#include <curie/memory.h>

int vectoradd() 
{
  double d1[3] = {1.0, 0.0, 2.0};
  double d2[3] = {2.0, 1.0, 0.0}; 
  
  vector v1 = {d1, 3};
  vector v2 = {d2, 3};
  
  vector v3 = vadd(v1, v2);
  double* d3 = v3.data;
  if((d3[0] == 3.0) && (d3[1] == 1.0) && (d3[2] == 2.0)) {
      return 0;
  }
  return 1;
}


// test multiplication of vector with real scalar
int sproduct()
{
  double d[3] = {2.0, 1.0, 0.0};	
  vector v = {d, 3};
  
  vector res = scalarm(4.0, v);
  if((res.dimension != 3)) return 2;
  
  double *dres = res.data;
  if((dres[0] != 8.0) || (dres[1] != 4.0) || (dres[2] != 0.0)) return 3;
	
  return 0;
	
}

// dot product of two vectors
int dproduct()
{
	double d1[3] = {1.0, 2.0, 3.0};
	double d2[3] = {3.0, 1.5, 0.5};
	vector v1 = {d1, 3};
	vector v2 = {d2, 3};
	
	double d = dot_product(v1, v2);
	
	if(d != 7.5) return 4;
	return 0;
}


// cross product of two vectors
int cproduct()
{
	double d1[3] = {2.0, 5.0, 1.0}; 
	double d2[3] = {-3.0, 2.0, 4.0};
	
	vector v1 = {d1, 3};
	vector v2 = {d2, 3};
	
	double e1[2] = {1, 0};
	double e2[2] = {0, 1};
	vector v3 = {e1, 2};
	vector v4 = {e2, 2};
	
	vector v0 = cross_product(v3, v4);
	
	if(v0.dimension != UNDEFINED_VECTOR.dimension) return 6;
	
	vector v = cross_product(v1, v2);
	double *d = v.data;
	
	if((d[0] != 18.0) || (d[1] != -11.0) || (d[2] != 19.0)) return 7;
	return 0;
			
}

// checks if a matrix is not an UNDEFINED_MATRIX
int defined (matrix m) 
{
	if((m.data != (double**) 0) && (m.rows > 0) && (m.columns > 0)) {
		return 1;
	}
	else {
		return 0;
	}
}

int matrixadd()
{
	double **d1;
	double **d2;
	
	d1 = get_mem (3 * sizeof(double*)); 
	d2 = get_mem (3 * sizeof(double*)); 
	for (int i = 0; i < 3; i++) 
	{
		d1[i] = get_mem(2 * sizeof(double)); 
		d2[i] = get_mem(2 * sizeof(double)); 
		for(int k = 0; k < 2; k++) 
		{
			d1[i][k] = 1.0;
			d2[i][k] = 2.0; 
		}
	}
	
	int ret = 0;
	
	
	matrix m1 = {d1, 3, 2};
	matrix m2 = {d2, 3, 2};
	
	double ** d3 = get_mem (3 * sizeof(double*)); 
	for(int i = 0; i < 3; i++) {
		d3[i] = get_mem(5*sizeof(double));
	}
	
	double ** d4 = get_mem (4* sizeof(double*)); 
	for(int i = 0; i < 4; i++) {
		d3[i] = get_mem(2*sizeof(double));
	}
	
	matrix m3 = {d3, 3, 5};
	matrix m4 = {d4, 4, 2};
	
	matrix res = madd(m1, m2);
	
	double** data = res.data; 
	
	matrix res_undef1 = madd(m1, m3);
	matrix res_undef2 = madd(m2, m4);
	matrix res_undef3 = madd(m3,  m4); 
	
	if((defined(res_undef1) != 0) 
		|| (defined(res_undef2) != 0) 
		|| (defined(res_undef3) != 0)) {
			return 47;
	}
	
	for(int i = 0; i < 3; i++) 
	{
		for(int k = 0; k < 2; k++)
		{
			if(data[i][k] != 3.0) {
				return i*k +5; 
			}
		}
	}
	
	return ret; 
	
}

int matrixsmult()
{
	double ** d1 = get_mem (3 * sizeof(double*)); 
	for(int i = 0; i < 3; i++) {
		d1[i] = get_mem(5*sizeof(double));
		for(int k = 0; k < 5; k++) {
			d1[i][k] = i;
		}
	}
	
	matrix m = {d1, 3, 5};
	matrix ret = smmult(2.0, m); 
	double **retdata = ret.data;
	
	for(int i = 0; i < 3; i++)
	{
		for(int k = 0; k < 5; k++) 
		{
			if(retdata[i][k] != 2.0*i)
			{
				return 3;
			}
		}
	}
	return 0; 
			
		
}

int cmain (void) 
{
  int i1 = vectoradd();
  if(i1 != 0) return i1;
  
  i1 = sproduct();
  if(i1 != 0) return i1; 
  
  i1 = dproduct();
  if(i1 != 0) return i1;
  
  i1 = cproduct();
  if(i1 != 0) return i1;
  
  i1 = matrixadd();
  if(i1 != 0) return i1;
  
  i1 = matrixsmult();
  if(i1 != 0) return i1;
  
  return 0;
}
