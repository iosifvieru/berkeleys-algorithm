#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TAG_ELECTION 100
#define TAG_WINNER 101

#define RTT_REQUEST 10
#define RTT_RESPONSE 11

double clock_drift = 0;

double get_current_clock_with_drift(){
    return MPI_Wtime() + clock_drift;
}

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int my_pid, world_size;
    double my_clock = get_current_clock_with_drift();
    
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

    // printf("pid: %d, my max_uuid is %d, my leader_id = %d\n", my_pid, max_uuid, leader_id);

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
    
    my_clock = get_current_clock_with_drift();

    // add artificial delay
    if(rand() % 10 <= 2){
        clock_drift = (rand() % 10000) / 1000.0;
        printf("pid: %d I got random drift: %lf, my clock now: %lf\n", my_pid, clock_drift, my_clock);
    }

    if(my_pid == leader_id){
        int ping = my_pid; // ping message
        for(int i = 0; i < world_size; i++) {
            if(i == leader_id){
                recvbuf[i] = my_clock;
                continue;
            }

            // calculate RTT (round trip time)
            double start_time = MPI_Wtime();

            // ping the node "i" to send their clock;
            MPI_Send(&ping, 1, MPI_INT, i, RTT_REQUEST, MPI_COMM_WORLD);
            // printf("LEADER: I have sent ping message to %d at start_time: %lf\n", i, start_time);

            double node_clock;
            MPI_Recv(&node_clock, 1, MPI_DOUBLE, i, RTT_RESPONSE, MPI_COMM_WORLD, &status);
            
            double rtt = MPI_Wtime() - start_time;
            printf("LEADER: calcualted RTT time for node: %d is %lf\n", i, rtt);

            recvbuf[i] = node_clock + (rtt / 2.0);
        }
         
    } else {
        int pong;
        // send to leader my clock
        MPI_Recv(&pong, 1, MPI_INT, leader_id, RTT_REQUEST, MPI_COMM_WORLD, &status);
        double current_time = get_current_clock_with_drift();
        // printf("pid: %d I have received my pong message at %lf\n", my_pid, current_time);

        MPI_Send(&current_time, 1, MPI_DOUBLE, leader_id, RTT_RESPONSE, MPI_COMM_WORLD);
    }

    // MPI_Gather(&my_clock, 1, MPI_DOUBLE, recvbuf, 1, MPI_DOUBLE, leader_id, MPI_COMM_WORLD);

    double adjustment = 0;
    double leader_final_clock = 0;

    double maximum_drift = 1.0;
    int no_excluded_nodes = 0;

    if(my_pid == leader_id){
        double sum_differences = 0;

        for(int i = 0; i < world_size; i++){
            if(i == leader_id) { continue; }
            
            double node_drift = recvbuf[i] - my_clock;
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

    adjustment = leader_final_clock - get_current_clock_with_drift();

    if(my_pid != leader_id){
        printf("pid %d: my adjustment is: %lf\n", my_pid, adjustment);
        printf("pid %d: my adjusted clock is: %lf\n", my_pid, get_current_clock_with_drift() + adjustment);
    } else {
        printf("pid: %d (LEADER): my adjusted clock is: %lf\n", my_pid, get_current_clock_with_drift() + adjustment);        
    }

    MPI_Finalize();
    return 0;
}
