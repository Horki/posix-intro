#include <signal.h>   // catch_signal
#include <stdbool.h>  // true
#include <stdio.h>    // puts, fprintf, fgets
#include <stdlib.h>   // EXIT_SUCCESS, EXIT_FAILURE, exit
#include <string.h>   // strtok, strncmp
#include <sys/msg.h>  // msgget, msgsnd

#include "type.h"  // msg, BUFFER_SIZE, create_queue, catch_signal, close_ipc

int main() {
  puts("Write to IPC message queue");
  puts("Run ./bin/c/c_lab07_ipc_mq_read to read from IPC MQ");

  // SIGINT handler
  if (catch_signal(SIGINT, close_ipc) == -1) {
    fprintf(stderr, "Can't map the handler");
    exit(EXIT_FAILURE);
  }

  int message_id = create_queue();
  msg.type = 1;

  while (true) {
    printf("Insert data: ");
    fgets(msg.text, BUFFER_SIZE, stdin);
    strtok(msg.text, "\n");
    // send message to message queue
    msgsnd(message_id, &msg, sizeof msg, 0);
    // Display the message
    printf("Data sent to queue '%s'\n", msg.text);
    if (strncmp(msg.text, "0", 1) == 0) {
      break;
    }
  }

  return EXIT_SUCCESS;
}