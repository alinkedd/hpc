#include <float.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int INPUT_TAG = 0;
const int OUTPUT_TAG = 1;

const int NUMBER_OF_ITERATIONS = 1000;

// Function to write data to file
void append_to_file(int np, double data, bool time) {
  char buffer[1024];

  if (time) {
    snprintf(buffer, sizeof(buffer), "output/lab3_p%d.txt", np);
  } else {
    snprintf(buffer, sizeof(buffer), "output/lab3_result_p%d.txt", np);
  }

  FILE *pFile = fopen(buffer, "a");
  fprintf(pFile, "%lf\n", data);
  fclose(pFile);
}

// Function to read input data from file
double** read_from_file() {
  double rows;
  double columns;

  FILE *pFile = fopen("input/lab3.txt", "r");

  fscanf(pFile, "%d %d\n", rows, columns);

  double** input = malloc(rows * sizeof(double*));
  for (i = 0; i < rows; i++) {
    input[i] = malloc(columns * sizeof(double));
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      fscanf(pFile, "%f\n", &input[i][j]);
    }
  }

  fclose(pFile);

  return input;
}

// Master process (manages data, but also calculates term)
void master(int rank, int np) {
  double det;

  double** matrix = read_from_file();

  clock_t begin = clock();

  for (int k = 0; k < NUMBER_OF_ITERATIONS; k++) {
  }

  clock_t end = clock();
  double total_time = (double)(end - begin) / CLOCKS_PER_SEC;

  append_to_file(np, total_time, true);
  append_to_file(np, det, false);
}

// Slave process (calculates term only)
void slave(int rank, int np, int master_rank) {
  for (int k = 0; k < NUMBER_OF_ITERATIONS; k++) {
  }
}

// Main process (copied by mpi)
int main(int argc, char *argv[]) {
  int rank;
  int np;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if (rank == 0) {
    master(rank, np);
  } else {
    slave(rank, np, 0);
  }

  MPI_Finalize();

  return 0;
}
