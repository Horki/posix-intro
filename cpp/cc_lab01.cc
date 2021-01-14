// C++ lib
#include <iostream>

// Linux lib
#include <sys/wait.h>  // waitpid
#include <unistd.h>    // getpid

#include <functional>

#include "custom_exception.hh"

int main() {
  std::function<pid_t(void)> create_fork = []() {
    pid_t pid{fork()};
    if (pid == -1) {
      throw new PIDException("Can't fork process");
    }
    return pid;
  };
  try {
    pid_t pid{create_fork()};
    if (!pid) {
      // child process
      auto execute_child = [](pid_t &ppid) -> void {
        if (ppid) {
          throw PIDException("Not a child process!");
        }
        std::cout << "Waiting to close child process (gedit[pid=" << getpid()
                  << "])" << std::endl;
        if (system("/usr/bin/gedit") != 0) {
          throw new OpenException("Can't open gedit");
        }
      };
      execute_child(pid);
    } else {
      // parent process
      auto wait_child = [](pid_t &pid) -> void {
        if (!pid) {
          throw new PIDException("Not a parent process!");
        }
        int pid_status;
        if (waitpid(pid, &pid_status, 0) == -1) {
          throw new PIDException("Error waiting child process");
        }
        std::cout << "Child exited with exit code:" << pid_status << std::endl;
      };
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
