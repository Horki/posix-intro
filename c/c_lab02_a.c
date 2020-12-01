#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct data_s {
  int32_t a, b, c, total;
} data_s;

static void error_msg(const char *);
static void *calculate(void *);
static void create_thread(data_s *);
static void wait_thread();

static pthread_t t;

int main() {
  data_s *d = (data_s *)malloc(sizeof(data_s));
  d->a = 10;
  d->b = 10;
  d->c = 10;
  d->total = 0;
  create_thread(d);
  /* printf("Before thread = %i\n", d->total); // 0 */
  puts("Waiting thread to finish");
  wait_thread();
  printf("Result = %i\n", d->total);
  free(d);
  d = NULL;

  return EXIT_SUCCESS;
}

static void create_thread(data_s *d) {
  if (pthread_create(&t, NULL, calculate, (void *)d) != 0)
    error_msg("Can't create thread t");
}

static void wait_thread() {
  if (pthread_join(t, NULL) != 0) error_msg("Can't join thread t");
}

// Expensive operation O(n^3)
static void *calculate(void *a) {
  struct data_s *res = (struct data_s *)a;
  for (int32_t i = 1; i <= res->a; i++)
    for (int32_t j = 1; j <= res->b; j++)
      for (int32_t k = 1; k <= res->c; k++) res->total += i * j * k;

  puts("Thread finished");

  return NULL;
}

static void error_msg(const char *m) {
  fprintf(stderr, "%s : %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}
