#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define INTERVALS 1000000

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *phi;
  phi = (double*)malloc(INTERVALS*sizeof(double));
  int    time_steps = 100;

  int myrank, P;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &P );
  MPI_Status status, status_right, status_left;
  MPI_Request request, req_left, req_right;

  long int points_per_domain = INTERVALS / P;

    double *my_phi_new, *my_phi;

    int my_start = 0; //myrank*(INTERVALS-2)/P;
    int my_stop = points_per_domain + 1; //(myrank+1)*(INTERVALS-2)/P;
    int my_imax = my_stop-my_start; // including the two ghost points
    
    my_phi_new = (double*)malloc(my_imax*sizeof(double));
    my_phi = (double*)malloc(my_imax*sizeof(double));

  /* Set up initial and boundary conditions. */

    for (int i=1; i<my_imax-1; i++) {
        my_phi[i] = 0.0; // same formula as in the serial computation
    }

          // if (myrank == 0) {
            my_phi[0] = 1.0;
          // }

          // if (myrank == P - 1) {
            my_phi[my_stop] = 0.0;
          // }

  while(time_steps-- > 0)
  {

        if (myrank>0)
            MPI_Irecv(&my_phi[0],1,MPI_DOUBLE,myrank-1,0,MPI_COMM_WORLD,&req_left);

        if (myrank<P-1)
            MPI_Irecv(&my_phi[my_imax-1],1,MPI_DOUBLE,myrank+1,0,MPI_COMM_WORLD,
        &req_right);
        
        if (myrank>0)
            MPI_Send(&my_phi[1],1,MPI_DOUBLE,myrank-1,0,MPI_COMM_WORLD);
        
        if (myrank<P-1)
            MPI_Send(&my_phi[my_imax-2],1,MPI_DOUBLE,myrank+1,0,MPI_COMM_WORLD);
        
        if (myrank>0)
            MPI_Wait(&req_left,&status_left);
        
        if (myrank<P-1)
            MPI_Wait(&req_right,&status_right);

      printf("rank is: %d, timeStep is: %d\n", myrank, time_steps);

   for(long i = 1; i < my_imax-1; i++)
    my_phi_new[i] = my_phi[i] + 0.1*(my_phi[i - 1] - 2*my_phi[i] + my_phi[i + 1]);
    
    // printf("the to_local is: %lf\n", from_local[2]);

    double *temp_ptr = my_phi_new;
    my_phi_new = my_phi;
    my_phi = temp_ptr;

  }

  MPI_Gather(my_phi+1, points_per_domain, MPI_DOUBLE,
             phi, points_per_domain, MPI_DOUBLE, 0, MPI_COMM_WORLD);

                 if (myrank == 0) {
  for(long int i = 2; i < 30; i += 2)
   printf("Interval %ld: %f\n", i, my_phi[i]);
             }

  MPI_Finalize();

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  // printf("Elapsed time (s) = %f\n", time2);


//   for(long i = 2; i < 30; i += 2)
//    printf("Interval %ld: %f\n", i, my_phi[i]);
  
  return 0;
}                
