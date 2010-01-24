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


int matrixadd()
{
	double d1[3][2] = {{1, 0}, {0, 1}, {0, 0}};
	double d2[3][2] = {{4, 3}, {1, 0}, {23, 42}};
	double d3[3][2] = {{5, 3}, {1, 1}, {23, 42}};
	int ret = 0;
	
	
	matrix m1 = {d1, 3, 2};
	matrix m2 = {d2, 3, 2};
	
	matrix res = madd(m1, m2);
	
	for(int i = 0; i < 3; i++) 
	{
		for (int k = 0; k < 2; k++) 
		{
			if(res.data[i][k] != d3[i][k]	) {
				ret = 8;
			}
		}
	}
	return ret;
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
  
  return 0;
}
