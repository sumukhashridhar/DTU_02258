#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include "err_code.h"

//pick up device type from compiler command line or from
//the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif


extern double wtime();       // returns time since some fixed past point (wtime.c)
extern int output_device_info(cl_device_id );


#define INTERVALS 10000000

double a[INTERVALS], b[INTERVALS];

const char *KernelSource = "\n" \
"__kernel void stencil(                                                 \n" \
"   __global double* from,                                                  \n" \
"   __global double* to,                                                  \n" \
"   __global double* temp,                                                  \n" \
"   const unsigned int count)                                           \n" \
"   const int time_steps)                                           \n" \
"{                                                                      \n" \
"     while(time_steps-- > 0) {                                           \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)    {                                                   \n" \
"           to[i] = from[i] + 0.1*(from[i - 1] - 2*from[i] + from[i + 1]);     \n" \
"}                                                                      \n" \
"   if(i < count)    {                                                   \n" \
"            from[i] = to[i];  \n" \
"}                                                                      \n" \

"}                                                                      \n" \

"}                                                                      \n" \
"\n";

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *to = b;
  double *from = a;
  int    time_steps = 100;
  int i = 0;
  long int count = INTERVALS;

  /* Set up initial and boundary conditions. */
  from[0] = 1.0;
  from[INTERVALS - 1] = 0.0;
  to[0] = from[0];
  to[INTERVALS - 1] = from[INTERVALS - 1];
  
  for(long i = 1; i < INTERVALS; i++)
   from[i] = 0.0;

  // openCL
  
    int          err;               // error code returned from OpenCL calls

    unsigned int correct;           // number of correct results

    size_t global;                  // global domain size

    cl_device_id     device_id;     // compute device id
    cl_context       context;       // compute context
    cl_command_queue commands;      // compute command queue
    cl_program       program;       // compute program
    cl_kernel        ko_stencil;       // compute kernel

    cl_mem d_from;                     // device memory used for the input  a vector
    cl_mem d_to;                     // device memory used for the input  b vector
    cl_mem d_temp;                     // device memory used for the output c vector

    // Set up platform and GPU device

    cl_uint numPlatforms;

    // Find number of platforms
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    checkError(err, "Finding platforms");
    if (numPlatforms == 0)
    {
        printf("Found 0 platforms!\n");
        return EXIT_FAILURE;
    }

    // Get all platforms
    cl_platform_id Platform[numPlatforms];
    err = clGetPlatformIDs(numPlatforms, Platform, NULL);
    checkError(err, "Getting platforms");

    // Secure a CPU
    for (i = 0; i < numPlatforms; i++)
    {
        err = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
        if (err == CL_SUCCESS)
        {
            break;
        }
    }

    if (device_id == NULL)
        checkError(err, "Finding a device");

    err = output_device_info(device_id);
    checkError(err, "Printing device output");

    // Create a compute context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    checkError(err, "Creating context");

    // Create a command queue
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    checkError(err, "Creating command queue");

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    checkError(err, "Creating program");

    // Build the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n%s\n", err_code(err));
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    // Create the compute kernel from the program
    ko_stencil = clCreateKernel(program, "stencil", &err);
    checkError(err, "Creating kernel");

    // Create the input (a, b) and output (c) arrays in device memory
    d_from  = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(double) * count, NULL, &err);
    checkError(err, "Creating buffer d_from");

    d_to  = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(double) * count, NULL, &err);
    checkError(err, "Creating buffer d_to");

    d_temp  = clCreateBuffer(context,  CL_MEM_READ_WRITE, sizeof(double) * count, NULL, &err);
    checkError(err, "Creating buffer d_temp");

    // Write a and b vectors into compute device memory
    err = clEnqueueWriteBuffer(commands, d_from, CL_TRUE, 0, sizeof(double) * count, from, 0, NULL, NULL);
    checkError(err, "Copying h_a to device at d_from");

    err = clEnqueueWriteBuffer(commands, d_to, CL_TRUE, 0, sizeof(double) * count, to, 0, NULL, NULL);
    checkError(err, "Copying h_b to device at d_to");

    // Set the arguments to our compute kernel
    err  = clSetKernelArg(ko_stencil, 0, sizeof(cl_mem), &d_from);
    err |= clSetKernelArg(ko_stencil, 1, sizeof(cl_mem), &d_to);
    err |= clSetKernelArg(ko_stencil, 2, sizeof(cl_mem), &d_temp);
    err |= clSetKernelArg(ko_stencil, 3, sizeof(unsigned int), &count);
    err |= clSetKernelArg(ko_stencil, 4, sizeof(int), &time_steps);
    checkError(err, "Setting kernel arguments");

    double rtime = wtime();

    // Execute the kernel over the entire range of our 1d input data set
    // letting the OpenCL runtime choose the work-group size
    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_stencil, 1, NULL, &global, NULL, 0, NULL, NULL);
    checkError(err, "Enqueueing kernel");

    // Wait for the commands to complete before stopping the timer
    err = clFinish(commands);
    checkError(err, "Waiting for kernel to finish");

    rtime = wtime() - rtime;
    printf("\nThe kernel ran in %lf seconds\n",rtime);

    // Read back the results from the compute device
    err = clEnqueueReadBuffer( commands, d_to, CL_TRUE, 0, sizeof(double) * count, to, 0, NULL, NULL );  
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        exit(1);
    }

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

  for(long i = 2; i < 30; i += 2)
   printf("Interval %ld: %f\n", i, to[i]);
  
    // cleanup then shutdown
    clReleaseMemObject(d_from);
    clReleaseMemObject(d_to);
    clReleaseMemObject(d_temp);
    clReleaseProgram(program);
    clReleaseKernel(ko_stencil);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

  return 0;
}                
