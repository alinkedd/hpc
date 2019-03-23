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
const int FINISH_TAG = 3;

// Function to write data to file
void append_time_to_file(int np, double x, double time) {
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "output/lab1_p%d_x%lf.txt", np, x);
  FILE *pFile = fopen(buffer, "a");
  fprintf(pFile, "%lf\n", time);
  fclose(pFile);
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

  while (fabs(term) >= EPSILON) {
    // Calculates term
    term = calc_sh_term(n, x);
    // Adds term to process' partial sum
    p_series_sum += term;
    // Increment term index
    n += np;
  }

  return p_series_sum;
}

// Function to send from master finish signal
void send_finish(int rank, int np, int finish) {
  for (int i = 0; i < np; i++) {
    if (i != rank) {
      MPI_Send(&finish, 1, MPI_INT, i, FINISH_TAG, MPI_COMM_WORLD);
    }
  }
}

// Master process (manages data, but also calculates term)
void master(int rank, int np) {
  int finish = false;
  double series_sum = 0.0;
  double p_series_sum;
  double x;

  FILE *pFile = fopen("input/x.txt", "r");

  while( fscanf(pFile, "%lf\n", &x) != EOF ) {
    send_finish(rank, np, false);

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

    append_time_to_file(np, x, total_time);

    // printf("Partial sum is %.15lf\n", series_sum);
  };

  send_finish(rank, np, true);

  fclose(pFile);
}

// Slave process (calculates term only)
void slave(int rank, int np, int master_rank) {
  int finish = false;
  double x;
  double series_sum;

  // Receives if calculations continue
  MPI_Recv(&finish, 1, MPI_INT, master_rank, FINISH_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  while (!finish) {
    // Receives X from master
    MPI_Recv(&x, 1, MPI_DOUBLE, master_rank, X_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Calculates process' partial sum
    series_sum = calc_process_partial_sum(rank, np, x);

    // Sends term to master
    MPI_Send(&series_sum, 1, MPI_DOUBLE, master_rank, SUM_TAG, MPI_COMM_WORLD);

    // Receives if calculations stop
    MPI_Recv(&finish, 1, MPI_INT, master_rank, FINISH_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
