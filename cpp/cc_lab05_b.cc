#include <buffer.hh>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

static constexpr size_t NO_TH = 5;
static void wait_threads(std::vector<std::thread> &);

int main() {
  std::cout << "Warehouse schema\n";
  std::cout << "\t0 Empty slot\n";
  std::cout << "\t* Taken slot\n";
  std::cout << "\t+ Recently taken by worker\n\n";
  std::vector<std::thread> threads(NO_TH);
  SemaphoreBuffer<10> buff;
  size_t n_producers = 3;
  size_t n_consumers = 2;
  assert((n_producers + n_consumers) == NO_TH);
  std::cout << "Number of trucks  (producers): " << n_producers << std::endl;
  std::cout << "Number of workers (consumers): " << n_consumers << std::endl;

  for (size_t i = 0; i < n_producers; ++i) {
    std::thread t([&] { buff.producer(i + 1); });
    threads.push_back(std::move(t));
  }
  for (size_t i = 0; i < n_consumers; ++i) {
    std::thread t([&] { buff.consumer(i + 1); });
    threads.push_back(std::move(t));
  }
  wait_threads(threads);
}

void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
