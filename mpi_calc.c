#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define INTERVALS 100000

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *to = b;
  double from[INTERVALS];// = a;

  int rank, size;

  from[0] = 1.0;
  from[INTERVALS - 1] = 0.0;
  to[0] = from[0];
  to[INTERVALS - 1] = from[INTERVALS - 1];

  for(long i = 1; i < INTERVALS; i++)
   from[i] = 0.0;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Status status;

  int    time_steps = 100;
  int points_per_domain = INTERVALS / size;

  double from_local[points_per_domain];
  double to_local[points_per_domain];

  int start = 0, end = points_per_domain + 1;

  MPI_Scatter(from, points_per_domain+2, MPI_DOUBLE,
              from_local, points_per_domain+2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  /* Set up initial and boundary conditions. */

  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  {
// printf("the rank is %d\n", rank);
    if (rank > 0) 
      MPI_Recv(&from_local[start], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);

    if (rank < size - 1)
      MPI_Send(&from_local[end-1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);

    if (rank < size - 1)
      MPI_Recv(&from_local[end], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);

    if (rank > 0)
      MPI_Send(&from_local[start+1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);

   for(long i = 1; i <= (end - 1); i++)
    to_local[i] = from_local[i] + 0.1*(from_local[i - 1] - 2*from_local[i] + from[i + 1]);
   
   for(long i = 1; i < (end); i++)
    from_local[i] = to_local[i];

    // printf("the to_local is: %lf\n", from_local[2]);

  }

  MPI_Gather(to_local+1, points_per_domain, MPI_DOUBLE,
             from, points_per_domain, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //            if (rank == 0) {
    //             for(int i = 0; i < INTERVALS; i++)
    // printf("he ans is %lf\n", from[i]);
    //          }

  MPI_Finalize();

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  // printf("Elapsed time (s) = %f\n", time2);

  // for(long i = 2; i < 30; i += 2)
  //  printf("Interval %ld: %f\n", i, to[i]);
  
  return 0;
}                
