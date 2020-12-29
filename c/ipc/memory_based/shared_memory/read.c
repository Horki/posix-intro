#include <semaphore.h>  // sem_t, sem_open, sem_wait, sem_post, sem_unlink
#include <stdbool.h>    // false
#include <stdio.h>      // puts
#include <stdlib.h>     // EXIT_SUCCESS

#include "shm_type.h"  // open_semaphores, open_mapping, print_shared, close_semaphores, close_mapping

int main() {
  int fd;
  sem_t *sem_one = NULL;
  sem_t *sem_two = NULL;
  char *c = NULL;
  puts("Read process from shared buffer");
  puts("run ./bin/c/c_lab07_ipc_shm_w or /bin/cpp/cc_lab07_ipc_shm_w");

  open_semaphores(&sem_one, &sem_two);
  open_mapping(&fd, &c);
  sem_post(sem_one);
  puts("Started reading\n");

  print_shared(&sem_one, &sem_two, c);

  close_semaphores(&sem_one, &sem_two, false);
  close_mapping(&fd);

  puts("read done");
  return EXIT_SUCCESS;
}
