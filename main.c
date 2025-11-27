#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEADER 0

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;
    long current_clock = clock(); // clock time.
    
    MPI_Status status;
    MPI_Request request;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size < 2){
        fprintf(stderr, "You need at least two processes for this program.\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    printf("pid: %d my clock is : %ld\n", my_pid, current_clock);

    long* recvbuf;
    if(my_pid == LEADER){
        recvbuf = (long*) malloc (world_size * sizeof(long));
        
        if(recvbuf == NULL){
            fprintf(stderr, "Unable to allocate memory for recvbuf.\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }

    current_clock = clock();
    MPI_Gather(&current_clock, 1, MPI_LONG, recvbuf, 1, MPI_LONG, LEADER, MPI_COMM_WORLD);

    int adjustment = 0;
    long leader_final_clock = 0;

    if(my_pid == LEADER){
        long sum_differences = 0;

        for(int i = 0; i < world_size; i++){
            if(i == LEADER) { continue; }

            sum_differences += clock() - recvbuf[i];
        }

        long avg_offset = (long) (sum_differences / (world_size - 1));
        printf("LEADER: computed average is %d\n", avg_offset);

        leader_final_clock = clock() + avg_offset;
        printf("LEADER: my clock is now: %d\n", leader_final_clock);

        free(recvbuf);
        recvbuf = NULL; 
    }

    MPI_Bcast(&leader_final_clock, 1, MPI_LONG, LEADER, MPI_COMM_WORLD);

    adjustment = leader_final_clock - clock();

    printf("pid %d: my adjustment is: %ld\n", my_pid, adjustment);
    printf("pid %d: my adjusted clock is: %ld\n", my_pid, clock() + adjustment);

    MPI_Finalize();
    return 0;
}
