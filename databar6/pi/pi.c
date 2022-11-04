/*

n this exercise you will determine the value                                
 * of PI using the integral  of                                                 
 *  *    4/(1+x*x) between 0 and 1.                                                
 *   *                                                                              
 *    * The integral is approximated by a sum of n intervals.                        
 *     *                                                                              
 *      * The approximation to the integral in each interval is:                       
 *       *    (1/n)*4/(1+x*x).                                                          
 *        */

#include <stdio.h>
#include <time.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif
#include "err_code.h"

#define PI25DT 3.141592653589793238462643

extern double wtime();       // returns time since some fixed past point (wtime.c)


#define INTERVALS 10000000

const char *KernelSource = 
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable			\n"	// this pragma enables doubles in OpenCL
"__kernel void add(  									\n" \
					"__global double* a,  				\n" \
					"__global double* b, 				\n" \
					"__global double* c, 				\n" \
"					const int count){ 					\n" \
"   int i = get_global_id(0);                           \n" \
"   if(i < count)                                       \n" \
"       c[i] = a[i] + b[i];                             \n" \
"}\n" \
"\n";
			

int main(int argc, char **argv)
{
  long int i, intervals = INTERVALS;
  double x, dx, f, sum, pi;


  intervals = INTERVALS;
  printf("Number of intervals: %ld\n", intervals);
  
  dx = 1.0 / (double) intervals;
  sum = 0.0;

  // place before kernel runs the commands
  double rtime = wtime();
  for (i = intervals; i >= 1; i--) {
    x = dx * ((double) (i - 0.5));
    f = 4.0 / (1.0 + x*x);
    sum = sum + f;
  }
  pi = sum * dx;
  
  rtime = wtime() - rtime;
  printf("\nThe kernel ran in %lf seconds\n",rtime);	
  printf("Computed PI %.24f\n", pi);
  printf("The true PI %.24f\n", PI25DT);
  printf("Error       %.24f\n\n", PI25DT-pi);
  return 0;
}                
