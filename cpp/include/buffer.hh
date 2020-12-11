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
enum Slot {
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

class IBuffer {
 public:
  virtual void producer(std::size_t const) = 0;

  virtual void consumer(std::size_t const) = 0;

  virtual ~IBuffer() = 0;
};

IBuffer::~IBuffer() {}

template <std::size_t BUFFER_LEN>
class Semaphore : public IBuffer {
 private:
  std::size_t producer_idx, consumer_idx;
  std::array<Slot, BUFFER_LEN> cont;
  Custom::Posix::sem sem_full, sem_empty, sem_lock;

 public:
  Semaphore() : producer_idx{0}, consumer_idx{0} {
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
  }

  ~Semaphore() {
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

  void producer(std::size_t const producer_id) {
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

  void consumer(std::size_t const consumer_id) {
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

  friend std::ostream &operator<<(
      std::ostream &os,
      const Custom::ProducerConsumer::Semaphore<BUFFER_LEN> &s) {
    os << "[";
    std::copy(s.cont.begin(), s.cont.end(),
              std::ostream_iterator<Slot>(os, ""));
    os << "]" << std::endl;
    return os;
  }
};

}  // namespace ProducerConsumer

namespace Binary {
template <std::size_t LEN>
class Semaphore {
  std::vector<std::thread> threads;
  Custom::Posix::sem sem_even, sem_odd;
  std::size_t counter;

 public:
  Semaphore() : threads(2), counter{1} {
    // 0 = Semaphore is shared between threads of process
    int p_shared = 0;
    // 1 unlocked for 'odd' semaphore
    if (rk_sema_init(&sem_odd, p_shared, 1) != 0) {
      std::cerr << "Error init semaphore for odd lock\n";
    }
    // 0 locked for 'even' semaphore
    if (rk_sema_init(&sem_even, p_shared, 0) != 0) {
      std::cerr << "Error init semaphore for even lock\n";
    }
  }

  void run() {
    // print odd
    auto print_number = [&](bool is_odd) {
      std::string msg = is_odd ? "Odd" : "Even";
      while (true) {
        rk_sema_wait(is_odd ? &sem_odd : &sem_even);
        std::cout << msg << " number is " << counter++ << std::endl;
        rk_sema_post(is_odd ? &sem_even : &sem_odd);
        if (counter >= LEN) {
          break;
        }
      }
    };
    // print odd
    threads.push_back(std::thread(print_number, true));
    // print even
    threads.push_back(std::thread(print_number, false));
  }

  ~Semaphore() {
    wait_threads(threads);
    if (rk_sema_destroy(&sem_odd) != 0) {
      std::cerr << "Error destroying Semaphore odd lock\n";
    }
    if (rk_sema_destroy(&sem_even) != 0) {
      std::cerr << "Error destroying Semaphore even lock\n";
    }
  }
};
}  // namespace Binary

}  // namespace Custom

#endif
