all:
	mpicc main.c -o main.out

run:
	mpirun -np 4 main.out

clean:
	rm *.out
