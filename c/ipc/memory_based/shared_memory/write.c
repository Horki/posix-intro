#include <semaphore.h>  // sem_t, sem_open, sem_wait, sem_post, sem_unlink
//#include <signal.h>     // SIGINT, ...
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "shm_type.h"

// static void close_write(int);

int main() {
  int fd;
  sem_t *sem_one = NULL;
  sem_t *sem_two = NULL;
  char *c = NULL;

  puts("Write process to shared buffer");
  puts("run ./bin/c/c_lab07_ipc_shm_r");
  // SIGINT handler
  // TODO: figure without globals
  //  if (catch_signal(SIGINT, close_write) == -1) {
  //    ERROR_MSG("Can't map the handler");
  //  }

  init_semaphores(&sem_one, &sem_two);
  create_mapping(&fd, &c);

  puts("Waiting read process!!!");
  sem_wait(sem_one);
  printf("Done!\n'%c' for to exit\nStart writing...\n", END_CHAR);
  sem_post(sem_one);

  write_shared(&sem_one, &sem_two, c);

  close_semaphores(&sem_one, &sem_two, true);
  close_mapping(&fd);

  return EXIT_SUCCESS;
}

// static void close_write(int sig) {
//  fprintf(stderr, "Signal: %d, closing write process\n", sig);
//  *c = END_CHAR;
//  sem_post(sem_two);
//  close_semaphores(&sem_one, &sem_two, true);
//  close_mapping(&fd);
//  exit(EXIT_SUCCESS);
//}