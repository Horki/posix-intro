#include <atomic>
#include <cassert>  // assert
#include <iostream>
#include <thread>  // thread

class Data {
 private:
  uint64_t a, b, c, total;
  std::atomic<bool> is_cached{false};

 public:
  Data() : a{10}, b{10}, c{10}, total{0} {}
  Data(const Data&) = delete;
  Data& operator=(const Data&) = delete;
  Data(Data&&) = delete;
  Data& operator=(Data&&) = delete;

  uint64_t get_total() noexcept {
    if (!is_cached) {
      calculate();
      is_cached = true;
    }
    return total;
  }

 private:
  // Expensive operation O(n^3)
  void calculate() noexcept {
    for (size_t i{1}; i <= a; ++i) {
      for (size_t j{1}; j <= b; ++j) {
        for (size_t k{1}; k <= c; ++k) {
          total += (i * j * k);
        }
      }
    }
  }
};

int main() {
  std::cout << "[main] Calculate in separate thread" << std::endl;
  {
    Data d;
    std::thread t([&d] {
      std::cout << "[thread] result is: " << d.get_total() << std::endl;
    });
    std::cout << "[separate] Waiting thread to finish" << std::endl;
    t.join();
    std::cout << "[separate] Result = " << d.get_total() << std::endl;
    assert(d.get_total() == 166375);
    std::cout << "[separate] Thread finished" << std::endl;
  }
  std::cout << "[main] main thread done" << std::endl;
}
