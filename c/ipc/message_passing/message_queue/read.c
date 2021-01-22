#include <signal.h>   // catch_signal
#include <stdbool.h>  // true
#include <stdio.h>    // puts, fprintf, printf
#include <stdlib.h>   // EXIT_SUCCESS, EXIT_FAILURE, exit
#include <string.h>   // strncmp
#include <sys/ipc.h>  // IPC_RMID
#include <sys/msg.h>  // msgget, msgrcv, msgctl

#include "type.h"  // msg, create_queue, close_ipc

int main() {
  puts("Read from IPC message queue");
  puts("Run ./bin/c/c_lab07_ipc_mq_write to send to IPC MQ");

  // SIGINT handler
  if (catch_signal(SIGINT, close_ipc) == -1) {
    fprintf(stderr, "Can't map the handler");
    exit(EXIT_FAILURE);
  }

  int message_id = create_queue();

  // receive the message from queue
  while (true) {
    msgrcv(message_id, &msg, sizeof msg, 1, 0);

    // Display the message
    printf("Data received from queue: %s\n", msg.text);
    if (strncmp(msg.text, "0", 1) == 0) {
      break;
    }
  }

  puts("Destroy the message queue!!!");
  msgctl(message_id, IPC_RMID, NULL);

  return EXIT_SUCCESS;
}
