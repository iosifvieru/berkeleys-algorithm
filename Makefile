all:
	mpicc main.c -o main.out

run:
	mpirun main.out

clean:
	rm *.out
