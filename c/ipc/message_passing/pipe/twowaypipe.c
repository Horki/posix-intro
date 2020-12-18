#include <signal.h>
#include <stdbool.h>  // bool
#include <stdio.h>    // puts, printf, fgets, fprintf
#include <stdlib.h>   // EXIT_SUCCESS, EXIT_FAILURE, exit
#include <string.h>   // strtok, strncmp
#include <unistd.h>   // pipe, read, write, close, fork, pid_t

#define STR_BUF 256
#define PIPES_NO 2

static bool running = true;

static int catch_signal(int sig, void (*)(int));
static void close_write(int);
static void create_pipe(int[PIPES_NO], int);
static void parent_process(int[PIPES_NO], int[PIPES_NO]);
static void child_process(int[PIPES_NO], int[PIPES_NO]);

int main() {
  // SIGINT handler
  if (catch_signal(SIGINT, close_write) == -1) {
    fprintf(stderr, "Can't map the handler");
    exit(EXIT_FAILURE);
  }
  int pipe_fds_1[PIPES_NO], pipe_fds_2[PIPES_NO];

  create_pipe(pipe_fds_1, 1);
  create_pipe(pipe_fds_2, 2);

  puts("Added SIGINT handler!!!");

  pid_t pid = fork();

  if (pid == 0) {
    parent_process(pipe_fds_1, pipe_fds_2);
  } else {
    child_process(pipe_fds_1, pipe_fds_2);
  }

  printf("Pipe %d, done.\n", pid);
  return EXIT_SUCCESS;
}

static void create_pipe(int pipe_fds[PIPES_NO], int pipe_id) {
  int return_status = pipe(pipe_fds);
  if (return_status == -1) {
    fprintf(stderr, "unable to create pipe %d\n", pipe_id);
    exit(EXIT_FAILURE);
  }
}

static void parent_process(int pipe_fds_1[PIPES_NO], int pipe_fds_2[PIPES_NO]) {
  // Parent Process
  puts("Parent Process");
  // close unwanted pipe 1 read side
  close(pipe_fds_1[0]);
  // close unwanted pipe 2 write side
  close(pipe_fds_2[1]);
  char msg[STR_BUF];
  while (true) {
    printf("[PARENT] send message: ");
    if (running) {
      fgets(msg, STR_BUF, stdin);
      strtok(msg, "\n");
      printf("[PARENT] writing to pipe 1: [%s]\n", msg);
    } else {
      strncpy(msg, "0", 1);
    }
    write(pipe_fds_1[1], msg, sizeof msg);
    // Reading from pipe 2
    read(pipe_fds_2[0], msg, sizeof msg);
    printf("[PARENT] reading from pipe 2: [%s]\n", msg);
    if (strncmp(msg, "0", 1) == 0) {
      write(pipe_fds_1[1], msg, sizeof msg);
      break;
    }
  }
  // close unwanted pipe 1 write side
  close(pipe_fds_1[1]);
  // close unwanted pipe 2 read side
  close(pipe_fds_2[0]);
  puts("PARENT done");
}

static void child_process(int pipe_fds_1[PIPES_NO], int pipe_fds_2[PIPES_NO]) {
  // Child Process
  puts("Child Process");
  // close unwanted pipe 1 write side
  close(pipe_fds_1[1]);
  // close unwanted pipe 2 read side
  close(pipe_fds_2[0]);
  // Reading from pipe 1
  char msg[STR_BUF];
  while (true) {
    read(pipe_fds_1[0], msg, sizeof msg);
    if (strncmp(msg, "0", 1) == 0) {
      write(pipe_fds_2[1], msg, sizeof msg);
      break;
    }
    printf("[CHILD] reading from pipe 1: [%s]\n", msg);
    // Write to pipe 1
    printf("[CHILD] send message: ");
    if (running) {
      fgets(msg, STR_BUF, stdin);
      strtok(msg, "\n");
    } else {
      strncpy(msg, "0", 1);
    }
    write(pipe_fds_2[1], msg, sizeof msg);
    printf("[Child] writing to pipe 2: [%s]\n", msg);
  }
  // close unwanted pipe 1 read side
  close(pipe_fds_1[0]);
  // close unwanted pipe 2 write side
  close(pipe_fds_2[1]);
  puts("CHILD done");
}

static int catch_signal(int sig, void (*handler)(int)) {
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  return sigaction(sig, &action, NULL);
}

static void close_write(int sig) {
  fprintf(stderr, "Signal: %d, closing pipeline process\n", sig);
  running = false;
}