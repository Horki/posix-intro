#ifndef __TYPES_H_
#define __TYPES_H_

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define FILE_LEN 1024
#define SEM_ONE_NAME "/one"
#define SEM_TWO_NAME "/two"
#define END_CHAR '0'

#define STR_BUF 256
#define FILENAME "../../shared_object.txt"

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

#endif
