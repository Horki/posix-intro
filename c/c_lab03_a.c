#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_MAX 50
#define NO_TH 10

static int32_t counter = 1;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void *func1(void *);
static void *func2(void *);
static void error_msg(const char *);
static void wait_threads();

static pthread_t threads[NO_TH];

int main() {
  int32_t thread_no[NO_TH];
  for (int32_t i = 0; i < NO_TH;) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, func1, (void *)&thread_no[i]) != 0)
      error_msg("Error creating thread for func1");
    ++i;
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, func2, (void *)&thread_no[i]) != 0)
      error_msg("Error creating thread for func2");
    ++i;
  }

  wait_threads();

  return EXIT_SUCCESS;
}

static void *func1(void *a) {
  const int32_t n = *(int32_t *)a;
  pthread_mutex_lock(&mut);
  for (int32_t i = 0; i < N_MAX; ++i)
    printf("Function 1 = %d; Thread %d\n", counter++, n + 1);
  pthread_mutex_unlock(&mut);

  return NULL;
}

static void *func2(void *a) {
  const int32_t n = *(int32_t *)a;
  pthread_mutex_lock(&mut);
  for (int32_t i = 0; i < N_MAX; ++i)
    printf("Function 2 = %d; Thread %d\n", counter++, n + 1);
  pthread_mutex_unlock(&mut);

  return NULL;
}

// NOTE: errno can be unique for each thread
static void error_msg(const char *m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void wait_threads() {
  for (int32_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("Unable to join threads");
}
