#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    printf("pid: %d, hello world\n", my_pid);

    MPI_Finalize();
    return 0;
}