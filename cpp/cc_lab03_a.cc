#include <iomanip>   // setfill, setw
#include <iostream>  // cout
#include <mutex>     // mutex, unique_lock
#include <thread>    // thread
#include <vector>    // vector

#include "common.hh"  // wait_threads

int main() {
  constexpr std::size_t no_th{10};
  constexpr std::size_t n_max{50};
  std::size_t counter{1};
  std::mutex mut;
  std::vector<std::thread> threads;
  threads.reserve(no_th);
  auto func = [&cnt = counter, &m = mut, max = n_max](
                  std::size_t const fun_id, std::size_t const n) -> void {
    std::unique_lock<std::mutex> lock{m};
    for (std::size_t i{0}; i < max; ++i) {
      std::cout << "Function " << fun_id << " = " << std::setfill('0')
                << std::setw(3) << cnt++ << "; Thread " << std::setfill('0')
                << std::setw(2) << (n + 1) << std::endl;
    }
  };  // unlock lock
  for (std::size_t i{0}; i < no_th;) {
    threads.emplace_back(func, 1, i++);
    threads.emplace_back(func, 2, i++);
  }
  wait_threads(threads);
}
