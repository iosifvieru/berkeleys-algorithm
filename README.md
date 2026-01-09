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
    pid: 7 my clock is : 0.559591
    pid: 7, my UUID: 3507
    pid: 5 my clock is : 0.560775
    pid: 5, my UUID: 21305
    pid: 1 my clock is : 0.561513
    pid: 1, my UUID: 90601
    pid: 0 my clock is : 0.561882
    pid: 0, my UUID: 5900
    pid: 4 my clock is : 0.561940
    pid: 4, my UUID: 62404
    pid: 6 my clock is : 0.561513
    pid: 6, my UUID: 38806
    pid: 3 my clock is : 0.561855
    pid: 3, my UUID: 58803
    pid: 2 my clock is : 0.562063
    pid: 2, my UUID: 57902
    PID 1 won the election with the value: 90601!!!
    pid: 3 I got random drift: 1.926000, my clock now: 0.563507
    pid: 5 I got random drift: 5.653000, my clock now: 0.562484
    pid: 0 I got random drift: 7.574000, my clock now: 0.563577
    LEADER: calcualted RTT time for node: 0 is 0.000009
    LEADER: calcualted RTT time for node: 2 is 0.000001
    LEADER: calcualted RTT time for node: 3 is 0.001463
    LEADER: calcualted RTT time for node: 4 is 0.000011
    LEADER: calcualted RTT time for node: 5 is 0.000011
    LEADER: calcualted RTT time for node: 6 is 0.000010
    LEADER: calcualted RTT time for node: 7 is 0.000007
    LEADER: pid 0 is excluded due to high clock drift: 7.574384
    LEADER: pid 3 is excluded due to high clock drift: 1.928513
    LEADER: pid 5 is excluded due to high clock drift: 5.653804
    LEADER: computed average is 0.000696
    pid: 1 (LEADER): my adjusted clock is: 0.563900
    pid 2: my adjustment is: -0.001787
    pid 2: my adjusted clock is: 0.563908
    pid 6: my adjustment is: -0.001354
    pid 6: my adjusted clock is: 0.563908
    pid 0: my adjustment is: -7.575762
    pid 0: my adjusted clock is: 0.563908
    pid 4: my adjustment is: -0.001785
    pid 4: my adjusted clock is: 0.563908
    pid 5: my adjustment is: -5.653733
    pid 5: my adjusted clock is: 0.563907
    pid 3: my adjustment is: -1.927762
    pid 3: my adjusted clock is: 0.563908
    pid 7: my adjustment is: 0.000429
    pid 7: my adjusted clock is: 0.563909