#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posix_semaphore.h"

#define NO_TH 2
#define MAX_N 30

static struct rk_sema sem_even, sem_odd;
static pthread_t threads[NO_TH];
static int32_t counter = 1;

static void* print_odd(void*);
static void* print_even(void*);
static void error_msg(const char*);
static void wait_threads();
static void init_semaphores();
static void close_semaphores();

int main() {
  init_semaphores();

  if (pthread_create(&threads[0], NULL, print_odd, NULL) != 0)
    error_msg("Error creating thread for print_odd");

  if (pthread_create(&threads[1], NULL, print_even, NULL) != 0)
    error_msg("Error creating thread for print_even");

  wait_threads();
  close_semaphores();

  return EXIT_SUCCESS;
}

static void wait_threads() {
  for (int32_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("Unable to join threads");
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void* print_odd(void* a) {
  bool end = false;
  do {
    rk_sema_wait(&sem_odd);
    printf("Odd number is %d\n", counter++);
    if (counter >= MAX_N) end = true;
    rk_sema_post(&sem_even);
  } while (!end);

  return NULL;
}

static void* print_even(void* a) {
  bool end = false;
  do {
    rk_sema_wait(&sem_even);
    printf("Even number is %d\n", counter++);
    if (counter >= MAX_N) end = true;
    rk_sema_post(&sem_odd);
  } while (!end);

  return NULL;
}

static void init_semaphores() {
  // 0 = Semaphore is shared between threads of process
  int32_t p_shared = 0;
  // 1 unlocked for 'odd' semaphore
  if (rk_sema_init(&sem_odd, p_shared, 1) != 0)
    error_msg("Error init semaphore for odd lock");
  // 0 locked for 'even' semaphore
  if (rk_sema_init(&sem_even, p_shared, 0) != 0)
    error_msg("Error init semaphore for even lock");
}

static void close_semaphores() {
  if (rk_sema_destroy(&sem_odd) != 0)
    error_msg("Error destroying Semaphore odd lock");
  if (rk_sema_destroy(&sem_even) != 0)
    error_msg("Error destroying Semaphore even lock");
}
