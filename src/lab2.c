#include <float.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const double EPSILON = 1E-4;  // Calculations precision
const double SIGMA = 0.33;    // Sigma in Runge fule for Riemann sum

const int INPUT_TAG = 0;
const int OUTPUT_TAG = 1000;

const int NUMBER_OF_ITERATIONS = 1;

// Function to write data to file
void append_time_to_file(int np, double time) {
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "output/lab2_p%d.txt", np);
  FILE *pFile = fopen(buffer, "a");
  fprintf(pFile, "%lf\n", time);
  fclose(pFile);
}

// Function to read input data from file
double* read_from_file(double* input) {
  FILE *pFile = fopen("input/lab2.txt", "r");

  for (int i = 0; i < 3; i++) {
    fscanf(pFile, "%lf\n", &input[i]);
  }

  fclose(pFile);
}

// Function to calculate integral function x ^ x+3,
double function(double x) {
  return pow(x, x + 3);
}

// Function to check Runge rule
bool check_runge(double I2, double I, double epsilon) {
  return ((fabs(I2 - I) * SIGMA) < epsilon);
}

// Function to sum right Riemann
double sum_right_riemann(double a, double b, double epsilon) {
  int n = 1;
  double prev_I = 0.;
  double curr_I = -1.;
  double h = 0.;

  while(!check_runge(curr_I, prev_I, epsilon)) {
    n *= 2;
    h = (b - a) / n;
    prev_I = curr_I;
    curr_I = 0.;

    for (int i = 0; i <= n; i++) {
      curr_I += function(a + i * h) * h;
    }
  }

  return curr_I;
}

// Function to destruct input and get result
double get_result(int rank, int np, double input[]) {
  double a = input[0];
  double b = input[1];
  double epsilon = input[2];
  double step = (b - a) / np;
  return sum_right_riemann(a + rank * step, a + (rank + 1) * step, epsilon / np);
}

// Master process (manages data, but also calculates term)
void master(int rank, int np) {
  double input[3];
  double result_all[np - 1];
  MPI_Request recv_reqs[np - 1];
  MPI_Request send_reqs[np - 1];
  MPI_Status status_recv[np - 1];
  MPI_Status status_send[np - 1];

  // input[0] -- a
  // input[1] -- b
  // input[2] -- epsilon
  read_from_file(input);

  clock_t begin = clock();

  for (int k = 0; k < NUMBER_OF_ITERATIONS; k++) {

    for (int i = 1; i < np; i++) {
      MPI_Isend(input, 3, MPI_DOUBLE, i, INPUT_TAG, MPI_COMM_WORLD, &send_reqs[i - 1]);
    }

    MPI_Waitall(np - 1, send_reqs, status_send);

    double result = get_result(rank, np, input);

    for (int i = 1; i < np; i++) {
      MPI_Irecv(&result_all[i - 1], 1, MPI_DOUBLE, i, OUTPUT_TAG, MPI_COMM_WORLD, &recv_reqs[i - 1]);
    }

    MPI_Waitall(np - 1, recv_reqs, status_recv);

    for (int i = 0; i < (np - 1); i++) {
      result += result_all[i];
    }

    // printf("%f\n", result);
  }

  clock_t end = clock();
  double total_time = (double)(end - begin) / CLOCKS_PER_SEC;

  append_time_to_file(np, total_time);
}

// Slave process (calculates term only)
void slave(int rank, int np, int master_rank) {
  double input[3];
  MPI_Request recv_reqs[1];
  MPI_Request send_reqs[1];
  MPI_Status status[1];

  MPI_Irecv(input, 3, MPI_DOUBLE, master_rank, INPUT_TAG, MPI_COMM_WORLD, recv_reqs);

  MPI_Waitall(1, recv_reqs, status);

  double result = get_result(rank, np, input);

  MPI_Isend(&result, 1, MPI_DOUBLE, master_rank, OUTPUT_TAG, MPI_COMM_WORLD, send_reqs);
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
