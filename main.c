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

    printf("pid: %d: my counter is : %d\n", my_pid, counter);

    int* recvbuf;
    if(my_pid == LEADER){
        recvbuf = (int*) malloc (world_size * sizeof(int));
    }

    MPI_Gather(&counter, 1, MPI_INT, recvbuf, 1, MPI_INT, LEADER, MPI_COMM_WORLD);

    if(my_pid == LEADER){
        // calculate average time
        int leader_time = counter;
        int sum = 0;
        int avg = 0;

        for(int i = 0; i < world_size; i++){
            if(i == LEADER) { continue; }
            
            sum += leader_time - recvbuf[i];
        }

        avg = sum / world_size;
        printf("LEADER: computed average is %d\n", avg);

        counter += avg;
        printf("LEADER: my clock is now: %d\n", counter);

        for(int i = 0; i < world_size; i++){
            if(i == LEADER) { continue; }

            int needed_adj = counter - recvbuf[i];
            printf("pid %d needs to adjust clock with %d\n", i, needed_adj);

            MPI_Isend(&needed_adj, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
        }
    } else {
        int adjustment;
        MPI_Recv(&adjustment, 1, MPI_INT, LEADER, 0, MPI_COMM_WORLD, &status);

        printf("I have got %d from LEADER\n", adjustment);
        printf("pid: %d, My adjusted clock is: %d\n", my_pid, counter + adjustment);
    }

    if(my_pid == LEADER){
        free(recvbuf);
    }
    MPI_Finalize();
    return 0;
}
