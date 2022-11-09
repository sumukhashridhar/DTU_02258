#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define INTERVALS 10000000

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  // double *to = b;
  // double *from = a;
  int    time_steps = 100;
  int rank, size;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Status status;

  int points_per_domain = INTERVALS / size;

  // double *from_local, *to_local;

  double *from_local = a; //[points_per_domain + 2];
  double *to_local = b; //[points_per_domain + 2];

  // double a[INTERVALS/size], b[INTERVALS/size];

  /* Set up initial and boundary conditions. */
  int start = 0, end = points_per_domain + 1;
  from_local[start] = 1.0;
  from_local[end] = 0.0;
  to_local[start] = from_local[start];
  to_local[end] = from_local[end];
  
  for(long i = 1; i < end - 1; i++)
   from_local[i] = 0.0;

    // Communication in Left direction
    if (rank > 0) {
        
        MPI_Sendrecv(&from_local[start], 1, MPI_DOUBLE, rank - 1, 0,
                     &from_local[end], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
        
    }

    // Communication in Right direction
    if (rank < size - 1) {

        MPI_Sendrecv(&from_local[end], 1, MPI_DOUBLE, rank + 1, 0,
                     &from_local[start], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
 
    }
  
  // printf("Number of intervals: %ld. Number of time steps: %d\n", INTERVALS, time_steps);

  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  {
   for(long i = 1; i < (end - 1); i++)
    to_local[i] = from_local[i] + 0.1*(from_local[i - 1] - 2*from_local[i] + from_local[i + 1]);

   {
    double* tmp_local = from_local;
    from_local = to_local;
    to_local = tmp_local;
   }
  }

  MPI_Finalize();

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

  for(long i = 2; i < 30; i += 2)
   printf("Interval %ld: %f\n", i, to_local[i]);
  
  return 0;
}                
