#ifndef TYPE_H_
#define TYPE_H_

#include <signal.h>   // struct sigaction, sigemptyset, sigaction
#include <stdbool.h>  // true
#include <stdio.h>    // fprintf
#include <string.h>   // strncpy
#include <sys/ipc.h>  // key_y, ftok, IPC_CREAT
#include <sys/msg.h>  // msgget, msgsnd

#define BUFFER_SIZE 20
#define PROJECT_ID 65

struct message_buffer {
  long type;
  char text[BUFFER_SIZE];
} msg;

static bool running = true;

int create_queue() {
  // ftok to generate a random key
  key_t key = ftok("progfile", PROJECT_ID);
  // msgget creates a message queue, and returns identifier
  return msgget(key, 0666 | IPC_CREAT);
}

int catch_signal(int sig, void (*handler)(int)) {
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  return sigaction(sig, &action, NULL);
}

void close_ipc(int sig) {
  fprintf(stderr, "\nSignal: %d, closing IPC message passing processes\n", sig);
  running = false;
  int msg_id = create_queue();
  strncpy(msg.text, "0", 1);
  msgsnd(msg_id, &msg, sizeof msg, 0);
}

#endif
