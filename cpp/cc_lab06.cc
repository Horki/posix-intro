#include <semaphore.h>

#include <iostream>
#include <thread>
#include <vector>

class Semaphore {
  sem_t sem_bin;
  std::vector<std::thread> threads;
  uint16_t no;

 public:
  Semaphore(std::uint16_t no) : threads(no), no(no) {
    // Init semaphore
    sem_init(&sem_bin, 0, 1);
    add_airplanes();
  }

  ~Semaphore() {
    //    int result = sem_destroy(&sem_bin);
    //    std::cout << "semaphore done: " << result << std::endl;
  }

  void wait() {
    for (auto &t : threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

 private:
  void add_airplanes() {
    for (uint16_t i = 0; i < no; ++i) {
      // Capture i by copy, sem_bin by reference
      threads.push_back(std::thread([&, i]() {
        std::cout << "Airplane " << i << " => Control tower: permission to land"
                  << std::endl;
        sem_wait(&sem_bin);
        std::cout << "Control tower => Airplane " << i << ": Permission granted"
                  << std::endl;
        std::cout << "Airplane " << i
                  << " => Control tower: I have landed the runway" << std::endl;
        std::cout << "Control tower => everyone: Runway is available"
                  << std::endl;
        sem_post(&sem_bin);
      }));
    }
  }
};

int main() {
  Semaphore semaphore(5);
  semaphore.wait();
}
