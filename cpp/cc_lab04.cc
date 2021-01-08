#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "common.hh"  // wait_threads

int main() {
  constexpr std::size_t no_th{5};
  std::mutex mut;
  std::vector<std::thread> threads;
  threads.reserve(no_th);
  auto airplanes = [&m = mut](std::size_t const airplane_id) -> void {
    {
      std::unique_lock<std::mutex> lock{m};
      std::cout << "Airplane " << airplane_id
                << " => Control tower: permission to land" << std::endl;
    }  // unlock lock
    {
      std::unique_lock<std::mutex> lock{m};
      std::cout << "Control tower => Airplane " << airplane_id
                << ": Permission granted" << std::endl;
      std::cout << "Airplane " << airplane_id
                << " => Control tower: I have landed the runway" << std::endl;
      std::cout << "Control tower => everyone: Runway is available"
                << std::endl;
    }  // unlock lock
  };
  for (std::size_t i{1}; i <= no_th; ++i) {
    threads.emplace_back(airplanes, i);
  }
  wait_threads(threads);
}
