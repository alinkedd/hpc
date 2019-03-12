EXECS=lab1
MPICC?=mpicc

all: ${EXECS}

lab1: src/lab1.c
	mkdir bin
	${MPICC} -o bin/lab1 src/lab1.c

clean:
	rm -rf bin
