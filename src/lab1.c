#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const double EPSILON = 1E-8;  // Calculations precision
const int X_TAG = 1;
const int N_TAG = 2;
const int TERM_TAG = 3;
const int FINISH_TAG = 4;

// Function to calculate factorial of n
double factorial(int n) {
  double product = 1.0;

  if (n == 0) { return product; }

  for (int i = 2; i <= n; i++) {
    product *= i;
  }

  return product;
}

// Function to calculate sh series term - x^(2n-1) / !(2n-1)
double calc_sh_term(int n, double x) {
  int odd_n = 2 * (n + 1) - 1; // n starts from 0, so increment by 1
  double term = pow(x, odd_n) / factorial(odd_n);

  return term;
}

void master(int np) {
  int finish = false;
  int next_n = 0;
  int n;
  double x = 0.5; // main variable
  double series_sum = 0.0;
  double term;

  for (int i = 1; i < np; i++) {
    MPI_Send(&x, 1, MPI_DOUBLE, i, X_TAG, MPI_COMM_WORLD);
  }

  while(!finish) {
    n = next_n++;

    for (int i = 1; i < np; i++) {
      MPI_Send(&next_n, 1, MPI_INT, i, N_TAG, MPI_COMM_WORLD);
      next_n++;
    }

    term = calc_sh_term(n, x);
    series_sum += term;

    if (term < EPSILON) {
      finish = true;
    } else {
      for (int i = 1; i < np; i++) {
        MPI_Recv(&term, 1, MPI_DOUBLE, i, TERM_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        series_sum += term;

        if (term < EPSILON) {
          finish = true;
          break;
        }
      }
    }

    for (int i = 1; i < np; i++) {
      MPI_Send(&finish, 1, MPI_INT, i, FINISH_TAG, MPI_COMM_WORLD);
    }
  }

  printf("%.15lf\n", series_sum);
}

void slave() {
  int finish = false;
  int n;
  double x;
  double term;

  MPI_Recv(&x, 1, MPI_DOUBLE, 0, X_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  while (!finish) {
    MPI_Recv(&n, 1, MPI_INT, 0, N_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    term = calc_sh_term(n, x);

    MPI_Send(&term, 1, MPI_DOUBLE, 0, TERM_TAG, MPI_COMM_WORLD);
    MPI_Recv(&finish, 1, MPI_INT, 0, FINISH_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int np;
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if (rank == 0) {
    master(np);
  } else {
    slave();
  }

  MPI_Finalize();

  return 0;
}
