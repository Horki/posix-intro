#include <errno.h>  // errno
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // strerror
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // getpid

static void error_msg(const char*);
static void execute_child(pid_t);
static void wait_child(pid_t);

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    error_msg("Can't fork process");
  }

  if (!pid) {
    // child process
    execute_child(pid);
  } else {
    // parent process
    printf("I'm a parent process[ppid=%d] waiting for a child\n", getpid());
    wait_child(pid);
  }

  return EXIT_SUCCESS;
}

static void execute_child(pid_t ppid) {
  if (ppid) {
    error_msg("Not a child process!");
  }
  printf("Waiting to close child process (gedit[pid=%d])\n", getpid());
  if (execl("/usr/bin/gedit", "gedit", NULL) != -1) {
    error_msg("Can't open gedit");
  }
}

static void wait_child(pid_t pid) {
  if (!pid) {
    error_msg("Not a parent process!");
  }
  int pid_status;
  if (waitpid(pid, &pid_status, 0) == -1) {
    error_msg("Error waiting child process");
  }

  printf("Child exited with exit code: %d\n", pid_status);
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s : %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}
