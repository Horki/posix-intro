#include <iostream>
#include <thread>
#include <vector>
#include <semaphore.h>

static constexpr size_t NO_TH = 5;
static           sem_t  sem_bin;

void init_semaphore();
void close_semaphore();
void airplanes(const size_t);
void wait_threads(std::vector<std::thread> &);

int main() {
  init_semaphore();
  std::vector<std::thread> threads(NO_TH);
  for (size_t i = 0; i < NO_TH; ++i) {
    threads.push_back(std::thread(airplanes, i + 1));
  }
  wait_threads(threads);
  close_semaphore();
}

void airplanes(const size_t n) {
  std::cout << "Airplane " << n << " => Control tower: permission to land\n";
  sem_wait(&sem_bin);
  std::cout << "Control tower => Airplane " << n << ": Permission granted\n";
  std::cout << "Airplane " << n << " => Control tower: I have landed the runway\n";
  std::cout << "Control tower => everyone: Runway is available\n";
  sem_post(&sem_bin);
}

void wait_threads(std::vector<std::thread> & threads) {
  for (auto & t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void init_semaphore() {
  // init (binary|lock semaphore) with shared between threads of process
  // 1 is for unlocked state    
  if (sem_init(&sem_bin, 0, 1) != 0) {
    std::cerr << "Error init binary semaphore" << std::endl;
  }
}

void close_semaphore() {
  if (sem_destroy(&sem_bin) != 0) {
    std::cerr << "Error destroying binary semaphore" << std::endl;             
  }                
}
