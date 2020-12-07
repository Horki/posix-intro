#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 15

// Matrix
static int32_t a[N][N];
static int32_t b[N][N];
static int32_t res[N][N];

// Threads
static pthread_t threads[N];

static void error_msg(const char *);
static void *calculate_row(void *);
static void init();
static void print_matrix(int mat[][N]);
static void wait_threads();

int main() {
  init();
  int32_t thread_no[N];
  for (int32_t i = 0; i < N; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, calculate_row,
                       (void *)&thread_no[i]) != 0) {
      error_msg("Can't create thread");
    }
  }

  wait_threads();
  /* print_matrix(a); */
  /* print_matrix(b); */
  print_matrix(res);

  return EXIT_SUCCESS;
}

static void *calculate_row(void *d) {
  int32_t row = *(int32_t *)d;
  printf("Calculating in %d. thread!\n", row + 1);
  // Calculate one matrix row
  for (int32_t j = 0; j < N; ++j) {
    res[row][j] = a[row][j] + b[row][j];
  }

  printf("Calculated in %d. thread! DONE!\n", row + 1);

  return NULL;
}

static void init() {
  for (int32_t i = 0; i < N; ++i) {
    for (int32_t j = 0; j < N; ++j) {
      a[i][j] = 2000;
      b[i][j] = j + 1;
    }
  }
}

static void print_matrix(int32_t mat[][N]) {
  for (int32_t i = 0; i < N; ++i) {
    for (int32_t j = 0; j < N; ++j) {
      printf("%d ", mat[i][j]);
    }
    putchar('\n');
  }
}

static void error_msg(const char *m) {
  fprintf(stderr, "%s : %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void wait_threads() {
  for (int32_t i = 0; i < N; ++i) {
    if (pthread_join(threads[i], NULL) != 0) {
      error_msg("Can't join thread t");
    }
  }
}
