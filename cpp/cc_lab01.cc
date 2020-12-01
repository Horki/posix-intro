// C++ lib
#include <iostream>
#include <string_view>
// C lib
#include <cerrno>   // errno
#include <cstring>  // strerror
// Linux lib
#include <sys/wait.h>  // waitpid
#include <unistd.h>    // getpid

void error_msg(const std::string_view &);
void execute_child(pid_t &);
void wait_child(pid_t &);

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
    std::cout << "I'm a parent process[ppid=" << getpid()
              << " waiting for a child\n";
    wait_child(pid);
  }
}

void execute_child(pid_t &ppid) {
  if (ppid) {
    error_msg("Not a child process!");
  }
  std::cout << "Waiting to close child process (gedit[pid=" << getpid()
            << "])\n";
  if (system("/usr/bin/gedit") != 0) {
    error_msg("Can't open gedit");
  }
}

void wait_child(pid_t &pid) {
  if (!pid) std::cerr << "Not a parent process!";
  int pid_status;
  if (waitpid(pid, &pid_status, 0) == -1) {
    error_msg("Error waiting child process");
  }
  std::cout << "Child exited with exit code:" << pid_status << std::endl;
}

void error_msg(const std::string_view &m) {
  std::cerr << m << " : " << strerror(errno) << std::endl;
  exit(EXIT_FAILURE);
}
