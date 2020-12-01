#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shm_type.h"

sem_t *sem_one, *sem_two;
static void open_semaphores();
static void close_semaphores();

int fd;
char *c;
static void open_mapping();
static void close_mapping();
static void print_shared();
static void close_read(int);

int main() {
  puts("Read process from shared buffer");
  puts("run ./bin/c/c_lab07_ipc_shm_w");
  // SIGINT handler
  if (catch_signal(SIGINT, close_read) == -1) {
    ERROR_MSG("Can't map the handler");
  }

  open_semaphores();
  open_mapping();
  sem_post(sem_one);
  puts("Started reading\n");

  print_shared();

  close_semaphores();
  close_mapping();

  return EXIT_SUCCESS;
}

static void open_semaphores() {
  if ((sem_one = sem_open(SEM_ONE_NAME, O_RDWR)) == SEM_FAILED) {
    ERROR_MSG("Run ./c_lab07_w first, missing[sem_one]");
  }
  if ((sem_two = sem_open(SEM_TWO_NAME, O_RDWR)) == SEM_FAILED) {
    ERROR_MSG("Run ./c_lab07_w first, missing[sem_two]");
  }
}

static void close_semaphores() {
  if (sem_close(sem_one) != 0) {
    ERROR_MSG("Error closing Semaphore one");
  }
  if (sem_close(sem_two) != 0) {
    ERROR_MSG("Error closing Semaphore two");
  }
}

static void open_mapping() {
  char filepath[STR_BUF];
  struct stat sb;
  getcwd(filepath, STR_BUF);
  strncat(filepath, FILENAME, STR_BUF);

  fd = open(filepath, O_RDONLY, S_IRUSR | S_IWUSR);

  if (fstat(fd, &sb) == -1) {
    ERROR_MSG("Missing file");
  }

  c = mmap(NULL, FILE_LEN, PROT_READ, MAP_SHARED, fd, 0);
  printf("Open file '%s' with read perms\n", FILENAME);
}

static void print_shared() {
  do {
    sem_wait(sem_two);
    putchar(*c);
    sem_post(sem_one);
  } while (*c++ != END_CHAR);
}

static void close_mapping() {
  // munmap(fd, FILE_LEN); // Unmpaing pages of memory, done by kernel
  close(fd);
}

static void close_read(int sig) {
  fprintf(stderr, "Signal: %d, closing read process\n", sig);
  *c = END_CHAR;
  sem_post(sem_one);
  close_semaphores();
  close_mapping();
  exit(EXIT_SUCCESS);
}
