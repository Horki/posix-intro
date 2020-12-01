#ifndef SHM_TYPE_H_
#define SHM_TYPE_H_

#include <errno.h>      // errno
#include <fcntl.h>      // File control; O_CREAT, 0_RDWR, O_RDONLY, open
#include <semaphore.h>  // sem_t, sem_open, sem_wait, sem_post, sem_unlink
#include <signal.h>     // sigaction, sigemptyset
#include <stdbool.h>    // false
#include <stdlib.h>     // EXIT_FAILURE
#include <string.h>     // strerror, strncat
#include <sys/mman.h>   // mmap, PROT_READ, PROT_WRITE, MAP_SHARED
#include <sys/stat.h>   // S_IRUSR, S_IWUSR, fstat, struct stat
#include <unistd.h>     // getcwd, close

#define FILE_LEN 1024
#define SEM_ONE_NAME "/one"
#define SEM_TWO_NAME "/two"
#define END_CHAR '0'

#define STR_BUF 256
#define FILENAME "/shared_object.txt"

#define ERROR_MSG(MSG)                              \
  do {                                              \
    fprintf(stderr, "%s:%s", MSG, strerror(errno)); \
    exit(EXIT_FAILURE);                             \
  } while (false)

// could be as macro
int catch_signal(int sig, void (*handler)(int)) {
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  return sigaction(sig, &action, NULL);
}

// Semaphore

void init_semaphores(sem_t **one, sem_t **two) {
  // Look inside /dev/shm
  if ((*one = sem_open(SEM_ONE_NAME,       // Semaphore name
                       O_CREAT,            // Flags
                       S_IRUSR | S_IWUSR,  // Permission
                       0                   // Value
                       )) == SEM_FAILED) {
    ERROR_MSG("Error init semaphore one");
  }
  if ((*two = sem_open(SEM_TWO_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0)) ==
      SEM_FAILED) {
    ERROR_MSG("Error init semaphore two");
  }
}

void open_semaphores(sem_t **one, sem_t **two) {
  if ((*one = sem_open(SEM_ONE_NAME, O_RDWR)) == SEM_FAILED) {
    ERROR_MSG("Run ./c_lab07_w first, missing[sem_one]");
  }
  if ((*two = sem_open(SEM_TWO_NAME, O_RDWR)) == SEM_FAILED) {
    ERROR_MSG("Run ./c_lab07_w first, missing[sem_two]");
  }
}

void close_semaphores(sem_t **one, sem_t **two, bool unlink) {
  if (sem_close(*one) != 0) {
    ERROR_MSG("Error closing Semaphore one");
  }
  if (sem_close(*two) != 0) {
    ERROR_MSG("Error closing Semaphore two");
  }
  if (unlink) {
    // Write (main) process in unlinking semaphores
    sem_unlink(SEM_ONE_NAME);
    sem_unlink(SEM_TWO_NAME);
  }
}

void print_shared(sem_t **one, sem_t **two, const char *c) {
  do {
    sem_wait(*two);
    putchar(*c);
    sem_post(*one);
  } while (*c++ != END_CHAR);
}

void write_shared(sem_t **one, sem_t **two, char *c) {
  do {
    sem_wait(*one);
    *c = fgetc(stdin);
    sem_post(*two);
  } while (*c++ != END_CHAR);
}

void open_mapping(int *fd, char **c) {
  char filepath[STR_BUF];
  struct stat sb;
  getcwd(filepath, STR_BUF);
  strncat(filepath, FILENAME, STR_BUF);

  *fd = open(filepath, O_RDONLY, S_IRUSR | S_IWUSR);

  if (fstat(*fd, &sb) == -1) {
    ERROR_MSG("Missing file");
  }

  *c = mmap(NULL, FILE_LEN, PROT_READ, MAP_SHARED, *fd, 0);
  printf("Open file '%s' with read perms\n", filepath);
}

void create_mapping(int *fd, char **c) {
  char filepath[STR_BUF];
  struct stat sb;
  getcwd(filepath, STR_BUF);
  strncat(filepath, FILENAME, STR_BUF);
  *fd = open(filepath,          // Filename
             O_CREAT | O_RDWR,  // Access
             S_IRUSR | S_IWUSR  // Access permissions
  );

  if (fstat(*fd, &sb) == -1) {
    ERROR_MSG("Missing File");
  }

  *c = mmap(NULL,                    // Address space of process
            FILE_LEN,                // Block size in memory (bytes)
                                     //      sb.st_size,
            PROT_READ | PROT_WRITE,  // Privileges
            MAP_SHARED,              // Flags: shared between procesess
            *fd,                     // File descriptor
            0                        // Offset: from beginning
  );
  printf("Open file '%s' with read+write perms\n", filepath);
}

void close_mapping(int *fd) {
  // munmap(fd, FILE_LEN); // Unmpaing pages of memory, done by kernel
  close(*fd);
}

#endif
