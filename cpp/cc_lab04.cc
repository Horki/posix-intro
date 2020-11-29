#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

static constexpr size_t NO_TH = 5;
std::mutex              mut;

void airplanes(const size_t);
void wait_threads(std::vector<std::thread> &);

int main() {
  std::vector<std::thread> threads(NO_TH);
  for (size_t i = 0; i < NO_TH; ++i) {
    threads.push_back(std::thread(airplanes, i + 1));
  }
  wait_threads(threads);
}

void airplanes(const size_t n) {
  std::cout << "Airplane " << n << " => Control tower: permission to land\n";
  std::unique_lock<std::mutex> lock(mut);
  std::cout << "Control tower => Airplane " << n << ": Permission granted\n";
  std::cout << "Airplane " << n << " => Control tower: I have landed the runway\n";
  std::cout << "Control tower => everyone: Runway is available\n";
}

void wait_threads(std::vector<std::thread> & threads) {
  for (auto & t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
