#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NO_TH 5

static sem_t sem_bin;
static pthread_t threads[NO_TH];

static void* airplanes(void *);
static void error_msg(const char*);
static void wait_threads();
static void init_semaphore();
static void close_semaphore();

int main() {
  init_semaphore();

  int32_t thread_no[NO_TH];
  for (int32_t i = 0; i < NO_TH; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, airplanes, (void *) &thread_no[i]) != 0)
      error_msg("Error creating thread for airplanes");
  }

  wait_threads();
  close_semaphore();

  return EXIT_SUCCESS;
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void wait_threads() {
  for (int32_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("unable to join threads");
}

static void* airplanes(void *a) {
  const int32_t n = (*(int32_t *) a) + 1;
  printf("Airplane %d => Control tower: permission to land\n", n);
  sem_wait(&sem_bin);
  printf("Control tower => Airplane %d: Permission granted\n", n);
  printf("Airplane %d => Control tower: I have landed to runway\n", n);
  puts("Control tower => everyone: Runway is available");
  sem_post(&sem_bin);

  return NULL;
}

static void init_semaphore() {
  // init (binary|lock semaphore) with shared between threads of process
  // 1 is for unlocked state
  if (sem_init(&sem_bin, 0, 1) != 0)
    error_msg("Error init binary semaphore");
}

static void close_semaphore() {
  if (sem_destroy(&sem_bin) != 0)
    error_msg("Error destroying binary semaphore");
}
