#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include "buffer.hh"

int main() {
  constexpr std::size_t no_th{5};
  constexpr std::size_t n_producers{3};
  constexpr std::size_t n_consumers{2};
  assert((n_producers + n_consumers) == no_th);
  std::vector<std::thread> threads(no_th);
  SemaphoreBuffer<10> buff;

  std::cout << "Warehouse schema" << std::endl;
  std::cout << "\t0 Empty slot" << std::endl;
  std::cout << "\t* Taken slot" << std::endl;
  std::cout << "\t+ Recently taken by worker" << std::endl << std::endl;
  std::cout << "Number of trucks  (producers): " << n_producers << std::endl;
  std::cout << "Number of workers (consumers): " << n_consumers << std::endl;

  for (std::size_t i = 0; i < n_producers; ++i) {
    threads.push_back(std::thread(
        [&b = buff, producer_id = i + 1] { b.producer(producer_id); }));
  }
  for (std::size_t i = 0; i < n_consumers; ++i) {
    threads.push_back(std::thread(
        [&b = buff, consumer_id = i + 1] { b.consumer(consumer_id); }));
  }
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
