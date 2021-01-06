#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "common.hh"  // wait_threads

int main() {
  constexpr std::size_t no_th{2};
  constexpr std::size_t n{10};
  std::mutex mut;
  std::vector<std::thread> threads;
  threads.reserve(no_th);
  auto print_thread = [total = n](std::size_t const thread_id) -> void {
    for (std::size_t i{0}; i < total; ++i) {
      switch (thread_id) {
        case 0:
          std::cout << "Thread I : i = " << ((i * 1000) + 1) << std::endl;
          break;
        case 1:
          std::cout << "Thread II : i = " << ((i * 1000) + 1) << std::endl;
          break;
        default:
          std::cerr << "Unknown Thread " << n << std::endl;
      }
    }
  };

  for (std::size_t i{0}; i < no_th; ++i) {
    threads.emplace_back([&m = mut, &pt = print_thread, th = no_th] {
      for (std::size_t i{0}; i < th; ++i) {
        std::unique_lock<std::mutex> lock{m};
        pt(i);
      }  // unlock mutex
    });
  }
  wait_threads(threads);
}
