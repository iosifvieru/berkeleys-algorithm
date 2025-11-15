#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size < 2){
        fprintf(stderr, "You need at least two processes for this program.\n");

        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    MPI_Finalize();
    return 0;
}
