EXECS=lab1
MPICC?=mpicc
LIBS=-L/usr/lib/x86_64-linux-gnu -lsndfile -lm

all: ${EXECS}

lab1: src/lab1.c
	mkdir bin
	${MPICC} -o bin/lab1 src/lab1.c $(LIBS)

clean:
	rm -rf bin
