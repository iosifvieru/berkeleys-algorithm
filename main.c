#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEADER 0

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;
    int counter; // will act as time;
    
    MPI_Status status;
    MPI_Request request;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size < 2){
        fprintf(stderr, "You need at least two processes for this program.\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) + my_pid);
    counter = rand() % 100;

    printf("pid: %d my clock is : %d\n", my_pid, counter);

    int* recvbuf;
    if(my_pid == LEADER){
        recvbuf = (int*) malloc (world_size * sizeof(int));
        
        if(recvbuf == NULL){
            fprintf(stderr, "Unable to allocate memory for recvbuf.\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }

    MPI_Gather(&counter, 1, MPI_INT, recvbuf, 1, MPI_INT, LEADER, MPI_COMM_WORLD);

    int leader_final_clock = 0;
    if(my_pid == LEADER){

        int leader_time = counter;
        int sum_differences = 0;

        for(int i = 0; i < world_size; i++){
            if(i == LEADER) { continue; }
            sum_differences += leader_time - recvbuf[i];
        }

        int avg_offset = (int) (sum_differences / (world_size - 1));
        printf("LEADER: computed average is %d\n", avg_offset);

        leader_final_clock = leader_time + avg_offset;
        printf("LEADER: my clock is now: %d\n", leader_final_clock);
        
        free(recvbuf);
        recvbuf = NULL;
    }

    MPI_Bcast(&leader_final_clock, 1, MPI_INT, LEADER, MPI_COMM_WORLD);

    int needed_adj = leader_final_clock - counter;
    printf("pid %d: my adjusted clock is: %d\n", my_pid, counter + needed_adj);

    MPI_Finalize();
    return 0;
}
