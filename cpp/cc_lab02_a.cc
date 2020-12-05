#include <cassert>  // assert
#include <iostream>
#include <thread>  // thread

class Data {
 private:
  uint64_t a, b, c, total;

 public:
  Data() : a(10), b(10), c(10), total(0) {}
  Data(uint64_t a, uint64_t b, uint64_t c) : a(a), b(b), c(c), total(0) {}
  // Expensive operation O(n^3)
  void calculate() {
    for (size_t i = 1; i <= a; ++i) {
      for (size_t j = 1; j <= b; ++j) {
        for (size_t k = 1; k <= c; ++k) {
          total += (i * j * k);
        }
      }
    }
  }
  uint64_t get_total() const { return total; }
};

int main() {
  std::cout << "[main] Calculate in separate thread" << std::endl;
  {
    Data d;
    std::thread t([&d] { d.calculate(); });
    std::cout << "[separate] Waiting thread to finish" << std::endl;
    t.join();
    std::cout << "[separate] Result = " << d.get_total() << std::endl;
    assert(d.get_total() == 166375);
    std::cout << "[separate] Thread finished" << std::endl;
  }
  std::cout << "[main] main thread done" << std::endl;
}
