#include <algorithm>  // copy
#include <array>      // array
#include <iostream>   // cout
#include <iterator>   // ostream_iterator
#include <numeric>    // iota
#include <thread>     // thread
#include <vector>     // vector

namespace Custom {
using Iter = std::ostream_iterator<int>;

template <int N, int M>
class Matrix {
 private:
  std::array<int, N * M> arr;
  std::size_t n;
  std::size_t m;

 public:
  Matrix() : n(N), m(M) {
    for (int i = 0; i < n; ++i) {
      std::iota(arr.begin() + (m * i), arr.begin() + ((m * i) + m), 1);
    }
  }

  Matrix(int val) : n(N), m(M) { std::fill(arr.begin(), arr.end(), val); }

  constexpr int const &operator()(const std::size_t x,
                                  const std::size_t y) const {
    return arr.at(x * m + y);
  }

  constexpr int &operator()(const std::size_t x, const std::size_t y) {
    return arr.at(x * m + y);
  }

  constexpr std::size_t rows() const { return n; }

  constexpr std::size_t cols() const { return m; }

  friend std::ostream &operator<<(std::ostream &os, const Matrix<N, M> &m) {
    for (int i = 0; i < m.rows(); ++i) {
      std::copy(m.arr.begin() + (m.cols() * i),
                m.arr.begin() + ((m.cols() * i) + m.cols()), Iter(os, " "));
      os << std::endl;
    }
    return os;
  }
};
}  // namespace Custom

int main() {
  try {
    constexpr std::size_t rows{15};
    constexpr std::size_t cols{15};
    std::vector<std::thread> threads;
    threads.reserve(rows);
    Custom::Matrix<rows, cols> a;
    Custom::Matrix<rows, cols> b(2000);
    Custom::Matrix<rows, cols> res(0);
    std::cout << "A:" << std::endl;
    std::cout << a;
    std::cout << "B: " << std::endl;
    std::cout << b;
    for (std::size_t row = 0; row < rows; ++row) {
      threads.push_back(std::thread([&, row] {
        for (std::size_t col = 0; col < cols; ++col) {
          res(row, col) += a(row, col) + b(row, col);
        }
      }));
    }
    std::cout << "wait for join threads" << std::endl;
    for (auto &thread : threads) {
      thread.join();
    }
    std::cout << "result: " << std::endl;
    std::cout << res;

  } catch (const std::out_of_range &e) {
    std::cerr << "range: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "something else" << std::endl;
  }
}
