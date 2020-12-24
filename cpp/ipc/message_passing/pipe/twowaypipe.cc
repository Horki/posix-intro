#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>

class Pipe {
  int pipe1[2];
  int pipe2[2];
  static constexpr std::streamsize buff_size{20};
  char msg[buff_size];

 public:
  Pipe() : msg{} {
    int res1 = pipe(pipe1);
    int res2 = pipe(pipe2);
    if (res1 == -1 || res2 == -1) {
      throw std::bad_exception{};
    }
  }
  ~Pipe() {
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
  }
  void run() {
    pid_t pid = fork();
    if (pid == 0) {
      parent();
    } else {
      child();
    }
    std::cout << "Pipe " << pid << ", done." << std::endl;
  }

 private:
  void parent() {
    // Parent Process
    std::cout << "Parent Process" << std::endl;
    // close unwanted pipe 1 read side
    close(pipe1[0]);
    // close unwanted pipe 2 write side
    close(pipe2[1]);
    while (true) {
      std::cout << "[PARENT] send message: ";
      std::cin.getline(msg, buff_size);
      std::cout << "[PARENT] writing to pipe 1: [" << msg << "]" << std::endl;

      write(pipe1[1], msg, buff_size);
      // Reading from pipe 2
      read(pipe2[0], msg, buff_size);
      std::cout << "[PARENT] reading from pipe 2: [" << msg << "]" << std::endl;
      if (strncmp(msg, "0", 1) == 0) {
        write(pipe1[1], msg, buff_size);
        break;
      }
    }
  }

  void child() {
    // Child Process
    std::cout << "Child Process" << std::endl;
    // close unwanted pipe 1 write side
    close(pipe1[1]);
    // close unwanted pipe 2 read side
    close(pipe2[0]);
    // Reading from pipe 1
    while (true) {
      read(pipe1[0], msg, buff_size);
      if (strncmp(msg, "0", 1) == 0) {
        write(pipe2[1], msg, buff_size);
        break;
      }
      std::cout << "[CHILD] reading from pipe 1: [" << msg << "]" << std::endl;
      // Write to pipe 1
      std::cout << "[CHILD] send message: ";
      std::cin.getline(msg, buff_size);
      write(pipe2[1], msg, buff_size);
      std::cout << "[Child] writing to pipe 2: [" << msg << "]" << std::endl;
    }
  }
};

int main() {
  std::cout << "Press '0' to terminate." << std::endl;
  try {
    Pipe p;
    p.run();
  } catch (const std::bad_exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other exception" << std::endl;
  }
  std::cout << "main done" << std::endl;
}