#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEADER 2

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;
    double my_clock = MPI_Wtime();
    
    MPI_Status status;
    MPI_Request request;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size < 2){
        fprintf(stderr, "You need at least two processes for this program.\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    printf("pid: %d my clock is : %lf\n", my_pid, my_clock);

    // berkeley's clock synchronization algorithm
    double* recvbuf;
    if(my_pid == LEADER){
        recvbuf = (double*) malloc (world_size * sizeof(double));
        
        if(recvbuf == NULL){
            fprintf(stderr, "Unable to allocate memory for recvbuf.\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }
    
    my_clock = MPI_Wtime();
    MPI_Gather(&my_clock, 1, MPI_DOUBLE, recvbuf, 1, MPI_DOUBLE, LEADER, MPI_COMM_WORLD);

    double adjustment = 0;
    double leader_final_clock = 0;

    if(my_pid == LEADER){
        double sum_differences = 0;

        for(int i = 0; i < world_size; i++){
            if(i == LEADER) { continue; }

            sum_differences += my_clock - recvbuf[i];
        }

        double avg_offset = (sum_differences / (world_size - 1));
        printf("LEADER: computed average is %lf\n", avg_offset);

        leader_final_clock = my_clock + avg_offset;

        free(recvbuf);
        recvbuf = NULL; 
    }

    MPI_Bcast(&leader_final_clock, 1, MPI_DOUBLE, LEADER, MPI_COMM_WORLD);

    adjustment = leader_final_clock - MPI_Wtime();

    if(my_pid != LEADER){
        printf("pid %d: my adjustment is: %lf\n", my_pid, adjustment);
        printf("pid %d: my adjusted clock is: %lf\n", my_pid, MPI_Wtime() + adjustment);
    } else {
        printf("pid: %d (LEADER): my adjusted clock is: %lf\n", my_pid, MPI_Wtime() + adjustment);        
    }

    MPI_Finalize();
    return 0;
}
