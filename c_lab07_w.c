#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // strncat
#include <signal.h>   // SIGINT, ...

#include <sys/mman.h>  // mmap, PROT_READ, PROT_WRITE, MAP_SHARED
#include <sys/stat.h>  // S_IRUSR, S_IWUSR, fstat, struct stat
#include <fcntl.h>     // File control; O_CREAT, 0_RDWR, O_RDONLY, open
#include <unistd.h>    // getcwd, close

#include <semaphore.h> // sem_t, sem_open, sem_wait, sem_post, sem_unlink

#include "types.h"

sem_t *sem_one, *sem_two;
static void init_semaphores();
static void close_semaphores();

int fd;
char *c;
static void create_mapping();
static void close_mapping();
static void write_shared();
static void close_write(int);

int main() {
  puts("Write process to shared buffer");
  puts("run ./c_lab07_r");
  // SIGINT handler
  if (catch_signal(SIGINT, close_write) == -1)
    ERROR_MSG("Can't map the handler");

  init_semaphores();
  create_mapping();

  write_shared();

  close_semaphores();
  close_mapping();

  return EXIT_SUCCESS;
}

static void init_semaphores() {
  // Look inside /dev/shm
  if ((sem_one = sem_open(
    SEM_ONE_NAME,      // Semaphore name
    O_CREAT,           // Flags
    S_IRUSR | S_IWUSR, // Permission
    1                  // Value
    )) == SEM_FAILED)
  	ERROR_MSG("Error init semaphore one");
  if ((sem_two = sem_open(SEM_TWO_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  	ERROR_MSG("Error init semaphore two");
}

static void close_semaphores() {
  if (sem_close(sem_one) != 0)
  	ERROR_MSG("Error closing Semaphore one");
  if (sem_close(sem_two) != 0)
  	ERROR_MSG("Error closing Semaphore two");
  // Write (main) process in unlinking semaphores
  sem_unlink(SEM_ONE_NAME);
  sem_unlink(SEM_TWO_NAME);
}

static void create_mapping() {
  char filepath[STR_BUF];
  struct stat sb;
  getcwd(filepath, STR_BUF);
  strncat(filepath, FILENAME, STR_BUF);
  fd = open(
    filepath,         // Filename
    O_CREAT | O_RDWR, // Access
    S_IRUSR | S_IWUSR // Access permissions
  );

  if (fstat(fd, &sb) == -1)
    ERROR_MSG("Missing File");

  c = mmap(
      NULL,                   // Address space of process
      FILE_LEN,               // Block size in memory (bytes)
//      sb.st_size,
      PROT_READ | PROT_WRITE, // Privileges
      MAP_SHARED,             // Flags: shared between procesess
      fd,                     // File descriptor
      0                       // Offset: from begining
  );
  printf("Open file '%s' with read+write perms\n", FILENAME);
}

static void write_shared() {
  do {
    sem_wait(sem_one);
    *c = fgetc(stdin);
    sem_post(sem_two);
  } while(*c++ != '0');
}

static void close_mapping() {
  // munmap // made by kernel
  close(fd);
}

static void close_write(int sig) {
  fprintf(stderr, "Signal: %d, closing write process\n", sig);
  close_semaphores();
  close_mapping();
  exit(EXIT_SUCCESS);
}
