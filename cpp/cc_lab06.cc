#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include "common.hh"
#include "posix_semaphore.h"

using sem = struct rk_sema;

class Semaphore {
  sem sem_bin;
  std::vector<std::thread> threads;
  uint16_t no;

 public:
  Semaphore(std::uint16_t no) : no(no) {
    threads.reserve(no);
    // Init semaphore
    int r = rk_sema_init(&sem_bin, 0, 1);
    assert(r == 0);
    add_airplanes();
  }

  ~Semaphore() {
    wait_threads(threads);
    int result{rk_sema_destroy(&sem_bin)};
    std::cout << "semaphore done: " << result << std::endl;
  }

 private:
  void add_airplanes() {
    auto airplane = [this](std::uint16_t const i) -> void {
      rk_sema_wait(&sem_bin);
      std::cout << "Control tower => Airplane " << i << ": Permission granted"
                << std::endl;
      std::cout << "Airplane " << i
                << " => Control tower: I have landed the runway" << std::endl;
      std::cout << "Control tower => everyone: Runway is available"
                << std::endl;
      rk_sema_post(&sem_bin);
    };
    for (uint16_t i{1}; i <= no; ++i) {
      std::cout << "Airplane " << i << " => Control tower: permission to land"
                << std::endl;
      // Capture i by copy, sem_bin by reference
      threads.push_back(std::thread(airplane, i));
    }
  }
};

int main() {
  { Semaphore semaphore(5); }
  std::cout << "main done" << std::endl;
}
