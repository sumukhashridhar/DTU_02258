#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define INTERVALS 20

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  // double *to = b;
  // double *from = a;
  int rank, size;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Status status;

  int    time_steps = 100;
  int points_per_domain = INTERVALS / size;

  // double *from_local, *to_local;

  double from_local[points_per_domain + 2];
  double to_local[points_per_domain + 2];

  int start = 0, end = points_per_domain + 1;

  /* Set up initial and boundary conditions. */
  // if (rank == 0) {

      // printf("Rank: %d, start: %d, end: %d, ppd: %d\n", rank, start, end, abs(end - start));

      //   // for (int i = 1; i < size; i++) {

      //     start = rank*points_per_domain;
      //     end = (rank + 1) * points_per_domain;

      //     printf("Rank: %d, start: %d, end: %d, ppd: %d\n", rank, start, end, abs(end - start));

          for(long i = 0; i <= end; i++)
            from_local[i] = 0.0;

          if (rank == 0) {
            from_local[0] = 1.0;
            // from_local[end - 1] = 0.0;
          }
          // from_local[start] = 1.0;
          if (rank == size - 1) {
            from_local[end] = 0.0;
          }
          // // from_local[end] = 0.0;
          // to_local[start] = from_local[start];
          // to_local[end] = from_local[end];
  
  // }
  
  // printf("Number of intervals: %ld. Number of time steps: %d\n", INTERVALS, time_steps);
printf("hey1 %d\n", rank);
  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  {
printf("hey2 %d\n", rank);

    // Communication in Left direction
    if (rank > 0) {
        
        MPI_Sendrecv(&from_local[start], 1, MPI_DOUBLE, rank - 1, 0,
                     &from_local[end], 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD,
                     &status);
        
    }
printf("hey3 %d\n", rank);

    // Communication in Right direction
    if (rank < size - 1) {

        MPI_Sendrecv(&from_local[end], 1, MPI_DOUBLE, rank + 1, 0,
                     &from_local[start], 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD,
                     &status);
 
    }
printf("hey4 %d\n", rank);

   for(long i = 1; i < (end - 1); i++)
    to_local[i] = from_local[i] + 0.1*(from_local[i - 1] - 2*from_local[i] + from_local[i + 1]);
printf("hey5 %d\n", rank);

   {
    // double* tmp_local = from_local;
   for(long i = 0; i < (end); i++)
    from_local[i] = to_local[i];
    // to_local = tmp_local;
   }
printf("hey6 %d\n", rank);

  }

  MPI_Finalize();

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

  // for(long i = 2; i < 30; i += 2)
  //  printf("Interval %ld: %f\n", i, to_local[i]);
  
  return 0;
}                
