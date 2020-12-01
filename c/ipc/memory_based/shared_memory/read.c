#include <semaphore.h>  // sem_t, sem_open, sem_wait, sem_post, sem_unlink
//#include <signal.h>     // SIGINT, ...
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "shm_type.h"

// static void close_read(int);

int main() {
  int fd;
  sem_t *sem_one = NULL;
  sem_t *sem_two = NULL;
  char *c = NULL;
  puts("Read process from shared buffer");
  puts("run ./bin/c/c_lab07_ipc_shm_w");

  // SIGINT handler
  // TODO: figure without globals
  //  if (catch_signal(SIGINT, close_read) == -1) {
  //    ERROR_MSG("Can't map the handler");
  //  }

  open_semaphores(&sem_one, &sem_two);
  open_mapping(&fd, &c);
  sem_post(sem_one);
  puts("Started reading\n");

  print_shared(&sem_one, &sem_two, c);

  close_semaphores(&sem_one, &sem_two, false);
  close_mapping(&fd);

  return EXIT_SUCCESS;
}

// static void close_read(int sig) {
//  fprintf(stderr, "Signal: %d, closing read process\n", sig);
//  *c = END_CHAR;
//  sem_post(sem_one);
//  close_semaphores(&sem_one, &sem_two, false);
//  close_mapping(&fd);
//  exit(EXIT_SUCCESS);
//}
