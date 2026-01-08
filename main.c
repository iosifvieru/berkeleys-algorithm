#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TAG_ELECTION 1
#define TAG_WINNER 2

typedef struct {
    int uuid;
    int leaderId;
} LCRMsg;

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

    // leader election - LCR
    srand(time(NULL) + my_pid);

    int uuid = (rand() % 1000) * 100 + my_pid;
    int recv_uuid = 0;
    int leader_id = 0;
    int max_uuid = uuid;
    
    printf("pid: %d, my UUID: %d\n", my_pid, uuid);

    int left = (my_pid - 1 + world_size) % world_size;
    int right = (my_pid + 1) % world_size;

    for(int i = 0; i < world_size; i++) {
        MPI_Send(&max_uuid, 1, MPI_INT, right, TAG_ELECTION, MPI_COMM_WORLD);
        MPI_Recv(&recv_uuid, 1, MPI_INT, left, TAG_ELECTION, MPI_COMM_WORLD, &status);
        
        if(recv_uuid == uuid){
            leader_id = my_pid;
        }

        if(recv_uuid > max_uuid){
            max_uuid = recv_uuid;
        }
    }

    if(uuid == max_uuid){
        printf("PID %d won the election with the value: %d!!!\n", my_pid, uuid);
        MPI_Send(&leader_id, 1, MPI_INT, right, TAG_WINNER, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&leader_id, 1, MPI_INT, left, TAG_WINNER, MPI_COMM_WORLD, &status);
        if(right != leader_id){
            MPI_Send(&leader_id, 1, MPI_INT, right, TAG_WINNER, MPI_COMM_WORLD);
        }
    }

    printf("pid: %d, my max_uuid is %d, my leader_id = %d\n", my_pid, max_uuid, leader_id);

    // berkeley's clock synchronization algorithm
    double* recvbuf;
    if(my_pid == leader_id){
        recvbuf = (double*) malloc (world_size * sizeof(double));
        
        if(recvbuf == NULL){
            fprintf(stderr, "Unable to allocate memory for recvbuf.\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }
    
    my_clock = MPI_Wtime();

    // add artificial delay
    if(rand() % 10 < 2){
        double random_drift = (rand() % 10000) / 1000.0;
        my_clock += random_drift;
        printf("pid: %d I got random drift: %lf, my clock now: %lf\n", my_pid, random_drift, my_clock);
    }

    MPI_Gather(&my_clock, 1, MPI_DOUBLE, recvbuf, 1, MPI_DOUBLE, leader_id, MPI_COMM_WORLD);

    double adjustment = 0;
    double leader_final_clock = 0;

    double maximum_drift = 1.0;
    int no_excluded_nodes = 0;

    if(my_pid == leader_id){
        double sum_differences = 0;

        for(int i = 0; i < world_size; i++){
            if(i == leader_id) { continue; }
            
            double node_drift = my_clock - recvbuf[i];
            // exclude nodes with high drift from the average sum
            if(fabs(node_drift) > maximum_drift) {
                printf("LEADER: pid %d is excluded due to high clock drift: %lf\n", i, node_drift);
                no_excluded_nodes++;
                continue;
            }

            sum_differences += node_drift;
        }

        double avg_offset = sum_differences / (world_size - no_excluded_nodes);
        printf("LEADER: computed average is %lf\n", avg_offset);

        leader_final_clock = my_clock + avg_offset;

        free(recvbuf);
        recvbuf = NULL; 
    }

    MPI_Bcast(&leader_final_clock, 1, MPI_DOUBLE, leader_id, MPI_COMM_WORLD);

    adjustment = leader_final_clock - MPI_Wtime();

    if(my_pid != leader_id){
        printf("pid %d: my adjustment is: %lf\n", my_pid, adjustment);
        printf("pid %d: my adjusted clock is: %lf\n", my_pid, MPI_Wtime() + adjustment);
    } else {
        printf("pid: %d (LEADER): my adjusted clock is: %lf\n", my_pid, MPI_Wtime() + adjustment);        
    }

    MPI_Finalize();
    return 0;
}
