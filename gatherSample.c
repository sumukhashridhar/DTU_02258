#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{

int aa[6];

  MPI_Init( &argc, &argv );
int rank;
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
//   MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Status status;

    int a[5] = {1, 2, 3, 4, 5};

  MPI_Gather(a+1, 3, MPI_INT,
             aa, 3, MPI_INT, 0, MPI_COMM_WORLD);

             if (rank == 0) {
                for(int i = 0; i < 6; i++)
    printf("he ans is %d\n", aa[i]);
             }

MPI_Finalize();

return 0;

}

