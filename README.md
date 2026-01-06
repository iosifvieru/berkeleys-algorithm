# Berkeley's Algorithm for clock synchronization

The Accuracy of the Clock Synchronization Achieved by TEMPO in Berkeley UNIX 4.3BSD - Riccardo Gusella, Stefano Zatti can be found [here](https://www2.eecs.berkeley.edu/Pubs/TechRpts/1987/CSD-87-337.pdf).

Berkeley's Algorithm is a clock synchronization algorithm.

#### Steps:

1. A leader is elected.
2. The leader asks the other nodes for their time.
3. The leader observes the round-trip time (RTT) of the messages and estimates the time of each node and its own.
4. The leader averages the clock times, ignoring values far outside a certain threshold.
5. The leader sends out the amount (positive or negative) each node must adjust its clock with. 


### Usage:

Using Makefile:

    make all
    make run

Using mpi:

    mpicc main.c -Wall -o main.out
    mpirun -np <no_threads> --oversubscribe main.out 

### Output

    mpirun -np 8 --oversubscribe main.out
    pid: 3 my clock is : 0.576005
    pid: 3, my UUID: 846
    pid: 2 my clock is : 0.578270
    pid: 2, my UUID: 566
    pid: 4 my clock is : 0.577952
    pid: 4, my UUID: 505
    pid: 0 my clock is : 0.578752
    pid: 5 my clock is : 0.576265
    pid: 5, my UUID: 215
    pid: 7 my clock is : 0.575147
    pid: 7, my UUID: 454
    pid: 0, my UUID: 128
    pid: 1 my clock is : 0.574886
    pid: 1, my UUID: 198
    pid: 6 my clock is : 0.573728
    pid: 6, my UUID: 326
    PID 3 won the election with the value: 846!!!
    pid: 3, my max_uuid is 846, my leader_id = 3
    pid: 4, my max_uuid is 846, my leader_id = 3
    pid: 6, my max_uuid is 846, my leader_id = 3
    pid: 5, my max_uuid is 846, my leader_id = 3
    pid: 7, my max_uuid is 846, my leader_id = 3
    pid: 2, my max_uuid is 846, my leader_id = 3
    pid: 0, my max_uuid is 846, my leader_id = 3
    pid: 1, my max_uuid is 846, my leader_id = 3
    LEADER: computed average is -0.000348
    pid: 3 (LEADER): my adjusted clock is: 0.577159
    pid 0: my adjustment is: -0.003793
    pid 0: my adjusted clock is: 0.577164
    pid 4: my adjustment is: -0.003006
    pid 4: my adjusted clock is: 0.577165
    pid 2: my adjustment is: -0.003368
    pid 2: my adjusted clock is: 0.577164
    pid 6: my adjustment is: 0.001336
    pid 6: my adjusted clock is: 0.577163
    pid 7: my adjustment is: -0.000432
    pid 7: my adjusted clock is: 0.577164
    pid 5: my adjustment is: -0.001558
    pid 5: my adjusted clock is: 0.577164
    pid 1: my adjustment is: -0.000173
    pid 1: my adjusted clock is: 0.577165