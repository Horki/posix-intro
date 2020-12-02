#include <semaphore.h>

#include <iostream>
#include <thread>
#include <vector>

class Semaphore {
  sem_t sem_bin;
  std::vector<std::thread> threads;
  std::size_t no;

 public:
  Semaphore(std::size_t no) : threads(no), no(no) {
    // Init semaphore
    sem_init(&sem_bin, 0, 1);
    add_airplanes();
  }

  ~Semaphore() {
    sem_destroy(&sem_bin);
    std::cout << "semaphore done" << std::endl;
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
    for (std::size_t i = 0; i < no; ++i) {
      threads.push_back(std::thread([=]() {
        std::cout << "Airplane " << i
                  << " => Control tower: permission to land\n";
        sem_wait(&sem_bin);
        std::cout << "Control tower => Airplane " << i
                  << ": Permission granted\n";
        std::cout << "Airplane " << i
                  << " => Control tower: I have landed the runway\n";
        std::cout << "Control tower => everyone: Runway is available\n";
        sem_post(&sem_bin);
      }));
    }
  }
};

int main() {
  Semaphore semaphore(5);
  semaphore.wait();
}
