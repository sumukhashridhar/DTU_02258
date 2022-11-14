#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define INTERVALS 10000

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double from[INTERVALS];

  int rank, size;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Status status;

  int    time_steps = 100;
  int points_per_domain = INTERVALS / size;

  double from_local[points_per_domain + 2];
  double to_local[points_per_domain + 2];

  int start = 0, end = points_per_domain + 1;

  /* Set up initial and boundary conditions. */

          for(long i = 0; i <= end; i++)
            from_local[i] = 0.0;

          if (rank == 0) {
            from_local[0] = 1.0;
            to_local[0] = from_local[0];
          }

          if (rank == size - 1) {
            from_local[end] = 0.0;
            to_local[end] = from_local[end];
          }

  while(time_steps-- > 0)
  {

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank > 0) 
      MPI_Recv(&from_local[start], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);

    if (rank < size - 1)
      MPI_Send(&from_local[end - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);

    if (rank < size - 1)
      MPI_Recv(&from_local[end], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);

    if (rank > 0)
      MPI_Send(&from_local[start + 1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

      printf("rank is: %d, timeStep is: %d\n", rank, time_steps);

   for(long i = 1; i < (end); i++)
    to_local[i] = from_local[i] + 0.1*(from_local[i - 1] - 2*from_local[i] + from_local[i + 1]);
    
    // printf("the to_local is: %lf\n", from_local[2]);

   for(long i = 1; i < (end); i++)
    from_local[i] = to_local[i];

    MPI_Barrier(MPI_COMM_WORLD);

  }

  MPI_Gather(from_local+1, points_per_domain, MPI_DOUBLE,
             from, points_per_domain, MPI_DOUBLE, 0, MPI_COMM_WORLD);

                 if (rank == 0) {
  for(long i = 2; i < 12; i += 2)
   printf("Interval %ld: %f\n", i, from[i]);
             }

  MPI_Finalize();

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  // printf("Elapsed time (s) = %f\n", time2);


  // for(long i = 2; i < 30; i += 2)
  //  printf("Interval %ld: %f\n", i, to_local[i]);
  
  return 0;
}                
