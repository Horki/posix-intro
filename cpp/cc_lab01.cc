// C++ lib
#include <iostream>

// Linux lib
#include <sys/wait.h>  // waitpid
#include <unistd.h>    // getpid

#include "custom_exception.hh"

void execute_child(pid_t &) noexcept(false);

void wait_child(pid_t &) noexcept(false);

pid_t create_fork() noexcept(false);

int main() {
  try {
    pid_t pid = create_fork();
    if (!pid) {
      // child process
      execute_child(pid);
    } else {
      // parent process
      std::cout << "I'm a parent process[ppid=" << getpid()
                << " waiting for a child\n";
      wait_child(pid);
    }
  } catch (const BaseExcInterface &e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other error" << std::endl;
  }
}

void execute_child(pid_t &ppid) noexcept(false) {
  if (ppid) {
    throw PIDException("Not a child process!");
  }
  std::cout << "Waiting to close child process (gedit[pid=" << getpid() << "])"
            << std::endl;
  if (system("/usr/bin/gedit") != 0) {
    throw new OpenException("Can't open gedit");
  }
}

void wait_child(pid_t &pid) noexcept(false) {
  if (!pid) {
    throw new PIDException("Not a parent process!");
  }
  int pid_status;
  if (waitpid(pid, &pid_status, 0) == -1) {
    throw new PIDException("Error waiting child process");
  }
  std::cout << "Child exited with exit code:" << pid_status << std::endl;
}

pid_t create_fork() noexcept(false) {
  pid_t pid = fork();
  if (pid == -1) {
    throw new PIDException("Can't fork process");
  }
  return pid;
}