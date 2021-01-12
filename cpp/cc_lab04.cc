#include <functional>  // ref
#include <iostream>    // cout, cerr
#include <mutex>       // mutex, unique_lock
#include <thread>      // thread
#include <vector>      // vector

#include "common.hh"  // wait_threads

int main() {
  constexpr std::size_t no_th{5};
  std::mutex mut;
  std::vector<std::thread> threads;
  threads.reserve(no_th);
  auto airplanes = [](std::mutex& m, std::size_t const airplane_id) -> void {
    m.lock();
    std::cout << "Airplane " << airplane_id
              << " => Control tower: permission to land" << std::endl;
    m.unlock();
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
  try {
    for (std::size_t i{1}; i <= no_th; ++i) {
      threads.emplace_back(airplanes, std::ref(mut), i);
    }
    wait_threads(threads);
  } catch (const std::system_error& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << e.code() << std::endl;
  } catch (...) {
    std::cerr << "other" << std::endl;
  }
}
