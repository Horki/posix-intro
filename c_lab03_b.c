#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define NO_TH 2

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void* func(void *);
static void error_msg(const char*);
static void print_thread(const int32_t);
static void wait_threads();

static pthread_t threads[NO_TH];

int main() {
  int32_t thread_no[NO_TH];
  for (int32_t i = 0; i < NO_TH; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, func, (void *) &thread_no[i]) != 0)
      error_msg("Error creating thread for func");
  }

  wait_threads();

  return EXIT_SUCCESS;
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static void print_thread(const int32_t thread) {
  for (int32_t i = 0; i < 10; ++i) {
    switch (thread) {
      case 0:
        printf("Thread I : i = %d\n", (i * 1000) + 1);
        break;
      case 1:
        printf("Thread II : i = %d\n", (i * 1000) + 1);
        break;
      default:
        fprintf(stderr, "Unknown Thread %d\n", thread);
    }
  }
}

static void* func(void *a) {
  const int32_t n = *(int32_t *) a; 
  for (int32_t i = 0; i < 10; ++i) {
    pthread_mutex_lock(&mut);
    print_thread(n);
    pthread_mutex_unlock(&mut);
  }

  return NULL;
}

static void wait_threads() {
  for (int32_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("Unable to join threads");
}
