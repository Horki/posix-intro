#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define NO_PH 5
#define STR_BUF 255

static pthread_mutex_t mut[NO_PH] = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
};

const char philosophers[NO_PH][STR_BUF] = {
  "Judith Butler",
  "Gilles Deleuze",
  "Karl Marx",
  "Emma Goldman",
  "Michel Foucault",
};

static pthread_t threads[NO_PH];

static void* philosophers_table(void *);
static void wait_threads();
static void error_msg(const char *);

int main() {
  uint8_t thread_no[NO_PH];
  for (uint8_t i = 0; i < NO_PH; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, philosophers_table, (void *) &thread_no[i]) != 0)
      error_msg("Error creating thread for func");
  }

  wait_threads();

  return EXIT_SUCCESS;
}

static void* philosophers_table(void *a) {
  const uint8_t left_fork = *(uint8_t *) a;
  const uint8_t right_fork = (left_fork + 1) % NO_PH;

  printf("%s is thinking\n", philosophers[left_fork]);

  pthread_mutex_lock(&mut[left_fork]); // left
  pthread_mutex_lock(&mut[right_fork]); // right

  printf("%s is eating\n", philosophers[left_fork]);
  sleep(1);

  pthread_mutex_unlock(&mut[right_fork]); // release right
  pthread_mutex_unlock(&mut[left_fork]); // release left

  return NULL;
}

static void wait_threads() {
  for (uint8_t i = 0; i < NO_PH; ++i)
    if (pthread_join(threads[i], NULL) != 0)
      error_msg("Unable to join threads");
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}


