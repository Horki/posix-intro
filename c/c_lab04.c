#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define NO_TH 5 

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_t threads[NO_TH];

static void* airplanes(void *);
static void error_msg(const char*);
static void wait_threads();

int main() {
  int32_t thread_no[NO_TH];
  for (int32_t i = 0; i < NO_TH; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, airplanes, (void *) &thread_no[i]) != 0)
      error_msg("Error creating thread for airplanes");
  }

  wait_threads();

  return EXIT_SUCCESS;
}

// NOTE: errno can be unique for each thread
static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void wait_threads() {
  for (int32_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("Unable to join threads");
}

static void* airplanes(void *a) {
  const int32_t n = (*(int32_t *) a) + 1;
  printf("Airplane %d => Control tower: permission to land\n", n);
  pthread_mutex_lock(&mut);
  printf("Control tower => Airplane %d: Permission granted\n", n);
  printf("Airplane %d => Control tower: I have landed to runway\n", n);
  puts("Control tower => everyone: Runway is available");
  pthread_mutex_unlock(&mut);

  return NULL;
}
