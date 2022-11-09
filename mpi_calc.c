#include <stdio.h>
#include <time.h>
#include <mpi.h>

// #define INTERVALS 10000000

#define INTERVALS 16

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *to = b;
  double *from = a;
  int    time_steps = 100;
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Status status;

  /* Set up initial and boundary conditions. */
  int points_per_domain = INTERVALS / size;
  from[0] = 1.0;
  from[points_per_domain - 1] = 0.0;
  to[0] = from[0];
  to[points_per_domain - 1] = from[points_per_domain - 1];

  for(long i = 1; i < points_per_domain; i++)
   from[i] = 0.0;

   if (rank == 0) {
      int start = 0;
      int end = points_per_domain + 2;

for (int i = 0; i < size; i++) {
for (int j = 0; j < end; j++) {
  data[j] = from[i*end + j];
  // if (i != 0)
    // MPI_Send(&data, points_per_domain, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
}
  }
   }
  
  // else
    // MPI_Recv(&data, points_per_domain, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
  
  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  {
   for(long i = 1; i < (INTERVALS - 1); i++) 
    to[i] = from[i] + 0.1*(from[i - 1] - 2*from[i] + from[i + 1]);

   {
    double* tmp = from;
    from = to;
    to = tmp;
   }
  }

  MPI_Finalize();

    time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

  // for(long i = 2; i < 30; i += 2) {
  //  printf("Interval %ld: %f\n", i, to[i]);
  return 0;
}                
