EXECS=lab2
MPICC?=mpicc
LIBS=-L/usr/lib/x86_64-linux-gnu -lsndfile -lm

all: ${EXECS}

lab1: src/lab1.c
	mkdir bin && mkdir output && mkdir averages && mkdir terms_number
	${MPICC} -o bin/lab1 src/lab1.c $(LIBS)

lab2: src/lab2.c
	mkdir bin && mkdir output && mkdir averages
	${MPICC} -o bin/lab2 src/lab2.c $(LIBS)

clean:
	rm -rf bin && rm -rf output && rm -rf averages && rm -rf terms_number
