#include <iostream>  // cout
#include <mutex>     // mutex, unique_lock
#include <thread>    // thread
#include <vector>    // vector

static constexpr size_t NO_TH = 10;
static constexpr size_t N_MAX = 50;
static size_t counter = 1;
static std::mutex mut;

void func1(const size_t);
void func2(const size_t);

int main() {
  std::vector<std::thread> threads(NO_TH);
  for (size_t i = 0; i < NO_TH;) {
    threads.push_back(std::thread(func1, i++));
    threads.push_back(std::thread(func2, i++));
  }
  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void func1(const size_t n) {
  std::unique_lock<std::mutex> lock{mut};
  for (size_t i = 0; i < N_MAX; ++i) {
    std::cout << "Function 1 = " << counter++ << "; Thread " << (n + 1)
              << std::endl;
  }
}

void func2(const size_t n) {
  std::unique_lock<std::mutex> lock{mut};
  for (size_t i = 0; i < N_MAX; ++i) {
    std::cout << "Function 2 = " << counter++ << "; Thread " << (n + 1)
              << std::endl;
  }
}
