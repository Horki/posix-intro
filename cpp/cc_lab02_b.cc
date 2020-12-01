#include <algorithm>  // copy
#include <iostream>   // cout
#include <iterator>   // ostream_iterator
#include <numeric>    // iota
#include <thread>     // thread
#include <vector>     // vecot

static constexpr size_t N = 15;

std::vector<std::vector<int>> v1(N, std::vector<int>(N, 2000));
std::vector<std::vector<int>> v2(N, std::vector<int>(N));
std::vector<std::vector<int>> res(N, std::vector<int>(N));

void init();
void print_matrix(const std::vector<std::vector<int>> &);
void calculate(const size_t);

int main() {
  init();
  std::vector<std::thread> threads(N);
  std::cout << "Matrix A:\n";
  print_matrix(v1);
  std::cout << "Matrix B:\n";
  print_matrix(v2);
  for (size_t i = 0; i < N; ++i) {
    threads.push_back(std::thread(calculate, i));
  }
  for (auto &th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }
  std::cout << "Matrix A + B\n";
  print_matrix(res);
}

void print_matrix(const std::vector<std::vector<int>> &v) {
  for (const auto &row : v) {
    std::copy(row.begin(), row.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
  }
}

void init() {
  for (auto &row : v2) {
    std::iota(row.begin(), row.end(), 1);
  }
}

void calculate(const size_t i) {
  std::cout << "Calculating in thread id:" << i << std::endl;
  for (size_t j = 0; j < N; ++j) {
    res[i][j] = v1[i][j] + v2[i][j];
  }
}
