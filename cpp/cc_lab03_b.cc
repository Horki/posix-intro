#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

static constexpr size_t NO_TH = 2;
static constexpr size_t N = 10;
std::mutex mut;

void func(const size_t);
void print_thread(const size_t);
void wait_threads(std::vector<std::thread> &);

int main() {
  std::vector<std::thread> threads(NO_TH);
  for (size_t i = 0; i < NO_TH; ++i) {
    threads.push_back(std::thread(func, i));
  }
  wait_threads(threads);
}

void func(const size_t n) {
  for (size_t i = 0; i < N; ++i) {
    std::unique_lock<std::mutex> lock{mut};
    print_thread(n);
  }
}

void print_thread(const size_t n) {
  for (size_t i = 0; i < N; ++i) {
    switch (n) {
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
}

void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
