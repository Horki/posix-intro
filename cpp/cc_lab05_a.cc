#include <semaphore.h>

#include <iostream>
#include <thread>
#include <vector>

static constexpr size_t NO_TH = 2;
static constexpr size_t MAX_N = 30;
static size_t counter = 1;
static sem_t sem_even, sem_odd;

void init_semaphores();
void print_odd();
void print_even();
void wait_threads(std::vector<std::thread> &);
void close_semaphores();

int main() {
  init_semaphores();
  std::vector<std::thread> threads(NO_TH);
  threads.push_back(std::thread(print_odd));
  threads.push_back(std::thread(print_even));
  wait_threads(threads);
  close_semaphores();
}

void init_semaphores() {
  // 0 = Semaphore is shared between threads of process
  int p_shared = 0;
  // 1 unlocked for 'odd' semaphore
  if (sem_init(&sem_odd, p_shared, 1) != 0) {
    std::cerr << "Error init semaphore for odd lock\n";
  }
  // 0 locked for 'even' semaphore
  if (sem_init(&sem_even, p_shared, 0) != 0) {
    std::cerr << "Error init semaphore for even lock\n";
  }
}

void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void print_odd() {
  while (true) {
    sem_wait(&sem_odd);
    std::cout << "Odd number is " << counter++ << std::endl;
    sem_post(&sem_even);
    if (counter >= MAX_N) break;
  }
}

void print_even() {
  while (true) {
    sem_wait(&sem_even);
    std::cout << "Even number is " << counter++ << std::endl;
    sem_post(&sem_odd);
    if (counter >= MAX_N) break;
  }
}

void close_semaphores() {
  if (sem_destroy(&sem_odd) != 0) {
    std::cerr << "Error destroying Semaphore odd lock\n";
  }
  if (sem_destroy(&sem_even) != 0) {
    std::cerr << "Error destroying Semaphore even lock\n";
  }
}
