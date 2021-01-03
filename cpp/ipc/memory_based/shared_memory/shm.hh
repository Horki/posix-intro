#ifndef SHM_HH_
#define SHM_HH_

#include <fcntl.h>      // File control; O_CREAT, 0_RDWR, O_RDONLY
#include <semaphore.h>  // sem_t, sem_open, sem_wait, sem_post, sem_unlink
#include <sys/mman.h>   // mmap
#include <unistd.h>     // open, close

#include <cstdio>      // fgetc
#include <filesystem>  // filesystem::current_path
#include <iostream>
#include <string>

namespace SharedMemory {
namespace Exception {
class Exc {
 public:
  virtual std::string what() const = 0;
  virtual ~Exc() {}
};
class Semaphore : public Exc {
 public:
  std::string what() const { return "Semaphore exception"; }
};
class FileDescriptor : public Exc {
 public:
  std::string what() const { return "File descriptor exception"; }
};

}  // namespace Exception
struct Common {
  static constexpr std::streamsize buff{255};
  static constexpr const char *sem_one_name = "one";
  static constexpr const char *sem_two_name = "two";
  static int open_file(const char *path, const bool write = true) {
    std::cout << "Opening file '" << path << "' with read"
              << (write ? "+write" : "") << " perms" << std::endl;
    return open(path,                                 // Filename
                write ? O_CREAT | O_RDWR : O_RDONLY,  // Access
                S_IRUSR | S_IWUSR                     // Access permissions
    );
  }
  static char *map(const int fd, const bool write = true) {
    return static_cast<char *>(mmap(
        nullptr,  // Address space of process
        buff,     // Block size in memory (bytes)
        write ? PROT_READ | PROT_WRITE : PROT_READ,  // Privileges
        MAP_SHARED,  // Flags: shared between processes
        fd,          // File descriptor
        0            // Offset: from beginning
        ));
  }
};

class Task {
 public:
  virtual void run() = 0;
  virtual ~Task() {}
};

class Write : public virtual Task {
  sem_t *sem_one, *sem_two;
  int fd;
  char *c;

 public:
  Write()
      : sem_one{init_sem(Common::sem_one_name)},
        sem_two{init_sem(Common::sem_two_name)} {
    // Look inside /dev/shm
    if (sem_one == SEM_FAILED || sem_two == SEM_FAILED) {
      throw Exception::Semaphore{};
    }
    std::string path =
        std::filesystem::current_path().string() + "/shared_object.txt";
    fd = Common::open_file(path.c_str());
    if (fd == -1) {
      throw Exception::FileDescriptor{};
    }
    c = Common::map(fd);
  }
  void run() {
    std::cout << "Waiting read process!!!" << std::endl;
    do {
      sem_wait(sem_one);
      *c = fgetc(stdin);
      sem_post(sem_two);
    } while (*c++ != '0');
  }
  ~Write() {
    close(fd);
    sem_close(sem_one);
    sem_close(sem_two);
    sem_unlink(Common::sem_one_name);
    sem_unlink(Common::sem_two_name);
    std::cout << "write done" << std::endl;
  }

 private:
  static sem_t *init_sem(const char *name) {
    return sem_open(name,               // Semaphore name
                    O_CREAT,            // Flags
                    S_IRUSR | S_IWUSR,  // Permission
                    0                   // Value
    );
  }
};

class Read : public virtual Task {
  sem_t *sem_one, *sem_two;
  int fd;
  char *c;

 public:
  Read()
      : sem_one{open_sem(Common::sem_one_name)},
        sem_two{open_sem(Common::sem_two_name)} {
    // Look inside /dev/shm
    if (sem_one == SEM_FAILED || sem_two == SEM_FAILED) {
      throw Exception::Semaphore{};
    }
    std::string path =
        std::filesystem::current_path().string() + "/shared_object.txt";
    fd = Common::open_file(path.c_str(), false);
    if (fd == -1) {
      throw Exception::FileDescriptor{};
    }
    c = Common::map(fd, false);
  }
  void run() {
    sem_post(sem_one);
    std::cout << "Started reading" << std::endl;
    do {
      sem_wait(sem_two);
      std::cout << *c;
      sem_post(sem_one);
    } while (*c++ != '0');
  }
  ~Read() {
    close(fd);
    sem_close(sem_one);
    sem_close(sem_two);
    std::cout << "read done" << std::endl;
  }

 private:
  static sem_t *open_sem(const char *name) {
    return sem_open(name,   // Semaphore name
                    O_RDWR  // Flags
    );
  }
};

}  // namespace SharedMemory

#endif
