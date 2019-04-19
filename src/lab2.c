#include <float.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const double EPSILON = 1E-4;  // Calculations precision
const double SIGMA = 1 / 3    // Sigma in Runge fule for Riemann sum

// Function to write data to file
void append_time_to_file(int np, double time) {
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "output/lab2_p%d.txt", np);
  FILE *pFile = fopen(buffer, "a");
  fprintf(pFile, "%lf\n", time);
  fclose(pFile);
}

// Function to read input data from file
double[] read_from_file() {
  double input[3];
  FILE *pFile = fopen("input/lab2.txt", "r");

  for (int i = 0; i < 3; i++) {
    fscanf(fp, "%lf\n", &input[i]);
  }

  fclose(fp);

  return input;
}

// Function to calculate integral function x ^ x+3,
double function(double x) {
  return pow(x, x + 3);
}

// Function to check Runge rule
bool check_runge(double I2, double I, double epsilon) {
  return (fabs(I2 - I) * SIGMA) < epsilon);
}

// Function to sum right Riemann
double sum_right_riemann(double a, double b, double epsilon) {
  int n = 1;
  double prev_I = 0.;
  double curr_I = -1.;
  double h = 0.;

  while(!check_Runge(curr_I, prev_I, epsilon)) {
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
  double result_all[np - 1];
  MPI_Request recv_reqs[np - 1];
  MPI_Status status[np - 1];
  
  // input[0] -- a
  // input[1] -- b
  // input[2] -- epsilon
  double input[3] = read_from_file();

  clock_t begin = clock();

  MPI_Bcast(input, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  double result = get_result(rank, np, input);

  for (int i = 1; i < np; i++) {
    MPI_Irecv(&result_all[i - 1], 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &recv_reqs[i - 1]);
  }

  MPI_Waitall(np - 1, recv_reqs, status);

  for (int i = 0; i &lt; (np - 1); i++) {
    result += result_all[i];
  }

  clock_t end = clock();
  double total_time = (double)(end - begin) / CLOCKS_PER_SEC;

  append_time_to_file(np, total_time);
}

// Slave process (calculates term only)
void slave(int rank, int np, int master_rank) {
  double input[3];

  MPI_Bcast(input, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  double result = get_result(rank, np, input);

  MPI_Send(&result, 1, MPI_DOUBLE, master_rank, rank, MPI_COMM_WORLD);
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
