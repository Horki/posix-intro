#include <iostream>
#include <thread>

class Data {
 private:
  std::size_t a, b, c, total;

 public:
  Data() : a(10), b(10), c(10), total(0) {}
  Data(std::size_t a, std::size_t b, std::size_t c)
      : a(a), b(b), c(c), total(0) {}
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
  std::size_t get_total() const { return total; }
};

int main() {
  Data d;
  std::cout << "Calculate in separate thread" << std::endl;
  std::thread t([&] { d.calculate(); });
  std::cout << "Waiting thread to finish" << std::endl;
  t.join();
  std::cout << "Thread finished" << std::endl;
  std::cout << "Result = " << d.get_total() << std::endl;
}
