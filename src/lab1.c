#include <float.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const double EPSILON = 1E-8;  // Calculations precision
const int X_TAG = 1;
const int SUM_TAG = 2;

// Function to write data to file
void append_time_to_file(int np, double time) {
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "bin/lab1p%d.txt", np);
  FILE *pFile = fopen(buffer, "a");
  fprintf(pFile, "%lf\n", time);
}

// Function to calculate factorial of n
double factorial(int n) {
  double product = 1.0;

  if (n == 0) { return product; }

  for (int i = 2; i <= n; i++) {
    product *= i;
  }

  return product;
}

// Function to calculate sh series term - x^(2n+1) / !(2n+1)
double calc_sh_term(int n, double x) {
  int odd_n = 2 * n + 1;
  double term = pow(x, odd_n) / factorial(odd_n);

  return term;
}

// Function to calculate sum of terms at indeces [i + (k * P)],
// where i - process' rank, P - number of processes, k = 0..C -
double calc_process_partial_sum(int rank, int np, double x) {
  int n = rank;
  double term = DBL_MAX;
  double p_series_sum = 0.0;

  while (term >= EPSILON) {
    // Calculates term
    term = calc_sh_term(n, x);
    // Adds term to process' partial sum
    p_series_sum += term;
    // Increment term index
    n += np;
  }

  return p_series_sum;
}

// Master process (manages data, but also calculates term)
double master(int rank, int np, double x) {
  double series_sum = 0.0;
  double p_series_sum;

  clock_t begin = clock();

  for (int i = 0; i < np; i++) {
    if (i != rank) {
      // Sends X to slaves
      MPI_Send(&x, 1, MPI_DOUBLE, i, X_TAG, MPI_COMM_WORLD);
    }
  }

  // Calculates process' partial sum
  series_sum += calc_process_partial_sum(rank, np, x);

  for (int i = 0; i < np; i++) {
    if (i != rank) {
      // Receives term from i-th slave
      MPI_Recv(&p_series_sum, 1, MPI_DOUBLE, i, SUM_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      // Adds term to series sum
      series_sum += p_series_sum;
    }
  }

  clock_t end = clock();
  double total_time = (double)(end - begin) / CLOCKS_PER_SEC;

  append_time_to_file(np, total_time);

  return series_sum;
}

// Slave process (calculates term only)
void slave(int rank, int np, int master_rank) {
  double x;
  // Receives X from master
  MPI_Recv(&x, 1, MPI_DOUBLE, master_rank, X_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Calculates process' partial sum
  double series_sum = calc_process_partial_sum(rank, np, x);
  // Sends term to master
  MPI_Send(&series_sum, 1, MPI_DOUBLE, master_rank, SUM_TAG, MPI_COMM_WORLD);
}

// Main process (copied by mpi)
int main(int argc, char *argv[]) {
  int rank;
  int np;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if (rank == 0) {
    double x = 0.5; // main variable
    double series_sum = master(rank, np, x);
    // printf("Partial sum is %.15lf\n", series_sum);
  } else {
    slave(rank, np, 0);
  }

  MPI_Finalize();

  return 0;
}
