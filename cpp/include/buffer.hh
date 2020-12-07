#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

#include "posix_semaphore.h"

enum Slot {
  EMPTY,
  TAKEN,
  RECENTLY,
};

namespace Custom {
namespace Posix {
using sem_t = struct rk_sema;
}  // namespace Posix
}  // namespace Custom

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
class SemaphoreBuffer : public IBuffer {
 private:
  std::size_t producer_idx, consumer_idx;
  std::array<Slot, BUFFER_LEN> cont;
  Custom::Posix::sem_t sem_full, sem_empty, sem_lock;

 public:
  SemaphoreBuffer() : producer_idx{0}, consumer_idx{0} {
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

  ~SemaphoreBuffer() {
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

  friend std::ostream &operator<<(std::ostream &os,
                                  const SemaphoreBuffer<BUFFER_LEN> &s) {
    os << "[";
    std::copy(s.cont.begin(), s.cont.end(),
              std::ostream_iterator<Slot>(os, ""));
    os << "]" << std::endl;
    return os;
  }
};

#endif
