all:
	mpicc main.c -o main.out

run:
	mpirun -np 4 --oversubscribe main.out

clean:
	rm *.out
