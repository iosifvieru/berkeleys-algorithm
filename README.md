# Berkeley's Algorithm for clock synchronization

The Accuracy of the Clock Synchronization Achieved by TEMPO in Berkeley UNIX 4.3BSD - Riccardo Gusella, Stefano Zatti can be found [here](https://www.cs.ubc.ca/~bestchai/teaching/cs538b_2014w2/clock-synch-in-tempo.pdf).

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