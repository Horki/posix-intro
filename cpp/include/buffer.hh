#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <semaphore.h>

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

enum Slot {
  EMPTY,
  TAKEN,
  RECENTLY,
};

std::ostream& operator<<(std::ostream& os, const Slot& s) {
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
  virtual void producer(const size_t) = 0;
  virtual void consumer(const size_t) = 0;
  virtual void print_buffer() const = 0;
  virtual ~IBuffer() = 0;
};
IBuffer::~IBuffer() {}

template <size_t BUFFER_LEN>
class SemaphoreBuffer : public IBuffer {
 private:
  size_t producer_idx, consumer_idx;
  std::array<Slot, BUFFER_LEN> cont;
  sem_t sem_full, sem_empty, sem_lock;

 public:
  SemaphoreBuffer() : producer_idx(0), consumer_idx(0) {
    // 0 = Semaphore is shared between threads of process
    int p_shared = 0;
    if (sem_init(&sem_full, p_shared, 0) != 0) {
      std::cerr << "Error init semaphore for full lock\n";
    }
    if (sem_init(&sem_empty, p_shared, BUFFER_LEN) != 0) {
      std::cerr << "Error init semaphore for empty lock\n";
    }
    if (sem_init(&sem_lock, p_shared, 1) != 0) {
      std::cerr << "Error init semaphore for lock\n";
    }
    cont.fill(Slot::EMPTY);
  }
  ~SemaphoreBuffer() {
    if (sem_destroy(&sem_full) != 0) {
      std::cerr << "Error destroying Semaphore full lock\n";
    }
    if (sem_destroy(&sem_empty) != 0) {
      std::cerr << "Error destroying Semaphore empty lock\n";
    }
    if (sem_destroy(&sem_lock) != 0) {
      std::cerr << "Error destroying Semaphore lock\n";
    }
  }
  void producer(const size_t producer_id) {
    while (true) {
      sem_wait(&sem_empty);
      sem_wait(&sem_lock);
      // Add to buffer
      cont[producer_idx] = Slot::TAKEN;
      print_buffer();
      producer_idx = (producer_idx + 1) % BUFFER_LEN;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      sem_post(&sem_lock);
      sem_post(&sem_full);
    }
  }
  void consumer(const size_t consumer_id) {
    while (true) {
      sem_wait(&sem_full);
      sem_wait(&sem_lock);
      // Remove from buffer
      cont[consumer_idx] = Slot::RECENTLY;
      consumer_idx = (consumer_idx + 1) % BUFFER_LEN;
      print_buffer();
      cont[consumer_idx] = Slot::EMPTY;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      sem_post(&sem_lock);
      sem_post(&sem_empty);
    }
  }
  void print_buffer() const {
    std::cout << "[";
    std::copy(cont.begin(), cont.end(),
              std::ostream_iterator<Slot>(std::cout, ""));
    std::cout << "]" << std::endl;
  }
};

#endif
