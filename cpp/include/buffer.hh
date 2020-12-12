#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

#include "common.hh"
#include "posix_semaphore.h"

namespace Custom {
namespace Posix {
using sem = struct rk_sema;
}  // namespace Posix

namespace ProducerConsumer {
enum class Slot : char {
  EMPTY,
  TAKEN,
  RECENTLY,
};

std::ostream &operator<<(std::ostream &os, const Slot &s) {
  switch (s) {
    case Slot::EMPTY:
      os << "0";
      break;
    case Slot::TAKEN:
      os << "+";
      break;
    case Slot::RECENTLY:
      os << "*";
      break;
    default:
      os << "x";
  }
  return os;
}

template <std::size_t BUFFER_LEN>
class Semaphore {
 private:
  std::size_t n_producers, n_consumers;
  std::size_t producer_idx, consumer_idx;
  Posix::sem sem_full, sem_empty, sem_lock;
  std::array<Slot, BUFFER_LEN> cont;
  std::vector<std::thread> threads;

 public:
  Semaphore(std::size_t const n_producers, std::size_t const n_consumers)
      : n_producers{n_producers},
        n_consumers{n_consumers},
        producer_idx{0},
        consumer_idx{0},
        threads(n_producers + n_consumers) {
    // 0 = Semaphore is shared between threads of process
    int p_shared{0};
    if (rk_sema_init(&sem_full, p_shared, 0) != 0) {
      std::cerr << "Error init semaphore for full lock\n";
    }
    if (rk_sema_init(&sem_empty, p_shared, BUFFER_LEN) != 0) {
      std::cerr << "Error init semaphore for empty lock\n";
    }
    if (rk_sema_init(&sem_lock, p_shared, 1) != 0) {
      std::cerr << "Error init semaphore for lock\n";
    }
    cont.fill(Slot::EMPTY);
    std::cout << "Warehouse schema" << std::endl;
    std::cout << "\t0 Empty slot" << std::endl;
    std::cout << "\t* Taken slot" << std::endl;
    std::cout << "\t+ Recently taken by worker" << std::endl << std::endl;
    std::cout << "Number of trucks  (producers): " << n_producers << std::endl;
    std::cout << "Number of workers (consumers): " << n_consumers << std::endl;
  }

  void run() {
    for (std::size_t i = 0; i < n_producers; ++i) {
      threads.push_back(std::thread([this]() { producer(); }));
    }
    for (std::size_t i = 0; i < n_consumers; ++i) {
      threads.push_back(std::thread([this]() { consumer(); }));
    }
  }

  ~Semaphore() {
    wait_threads(threads);
    if (rk_sema_destroy(&sem_full) != 0) {
      std::cerr << "Error destroying Semaphore full lock\n";
    }
    if (rk_sema_destroy(&sem_empty) != 0) {
      std::cerr << "Error destroying Semaphore empty lock\n";
    }
    if (rk_sema_destroy(&sem_lock) != 0) {
      std::cerr << "Error destroying Semaphore lock\n";
    }
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const Semaphore<BUFFER_LEN> &s) {
    os << "[";
    std::copy(s.cont.begin(), s.cont.end(),
              std::ostream_iterator<Slot>(os, ""));
    os << "]" << std::endl;
    return os;
  }

 private:
  void producer() {
    while (true) {
      rk_sema_wait(&sem_empty);
      rk_sema_wait(&sem_lock);
      // Add to buffer
      cont[producer_idx] = Slot::TAKEN;
      std::cout << *this;
      producer_idx = (producer_idx + 1) % BUFFER_LEN;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      rk_sema_post(&sem_lock);
      rk_sema_post(&sem_full);
    }
  }

  void consumer() {
    while (true) {
      rk_sema_wait(&sem_full);
      rk_sema_wait(&sem_lock);
      // Remove from buffer
      cont[consumer_idx] = Slot::RECENTLY;
      consumer_idx = (consumer_idx + 1) % BUFFER_LEN;
      std::cout << *this;
      cont[consumer_idx] = Slot::EMPTY;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      rk_sema_post(&sem_lock);
      rk_sema_post(&sem_empty);
    }
  }
};

}  // namespace ProducerConsumer

namespace Binary {
template <std::size_t LEN>
class Semaphore {
  std::vector<std::thread> threads;
  Posix::sem sem_even, sem_odd;
  std::size_t counter;

 public:
  Semaphore() : threads(2), counter{1} {
    // 0 = Semaphore is shared between threads of process
    int p_shared = 0;
    // 1 unlocked for 'odd' semaphore
    if (rk_sema_init(&sem_odd, p_shared, 1) != 0) {
      std::cerr << "Error init semaphore for odd lock" << std::endl;
    }
    // 0 locked for 'even' semaphore
    if (rk_sema_init(&sem_even, p_shared, 0) != 0) {
      std::cerr << "Error init semaphore for even lock" << std::endl;
    }
  }

  void run() {
    auto print_number = [this](bool const is_odd) -> void {
      std::string msg = is_odd ? "Odd" : "Even";
      do {
        rk_sema_wait(is_odd ? &sem_odd : &sem_even);
        std::cout << msg << " number is " << counter++ << std::endl;
        rk_sema_post(is_odd ? &sem_even : &sem_odd);
      } while (counter < LEN);
    };
    // print odd
    threads.push_back(std::thread(print_number, true));
    // print even
    threads.push_back(std::thread(print_number, false));
  }

  ~Semaphore() {
    wait_threads(threads);
    if (rk_sema_destroy(&sem_odd) != 0) {
      std::cerr << "Error destroying Semaphore odd lock" << std::endl;
    }
    if (rk_sema_destroy(&sem_even) != 0) {
      std::cerr << "Error destroying Semaphore even lock" << std::endl;
    }
  }
};
}  // namespace Binary

}  // namespace Custom

#endif
