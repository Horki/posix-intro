#ifndef COMMON_H_
#define COMMON_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 20

void report(const char *msg, bool terminate) {
  perror(msg);
  if (terminate) {
    exit(EXIT_FAILURE);
  }
}

const char *HOST = "localhost";
const u_int16_t PORT_NUMBER = 8080;

#endif