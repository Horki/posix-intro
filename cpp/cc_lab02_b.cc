#include <algorithm>  // copy
#include <array>      // array
#include <cassert>    // assert
#include <iostream>   // cout
#include <iterator>   // ostream_iterator
#include <numeric>    // iota
#include <stdexcept>  // out_of_range
#include <thread>     // thread
#include <vector>     // vector

#include "common.hh"  // wait_threads

namespace Custom {
using Iter = std::ostream_iterator<int>;

template <std::size_t N, std::size_t M>
class Matrix {
 private:
  std::array<int, N * M> arr;
  std::size_t n;
  std::size_t m;

 public:
  Matrix() : n{N}, m{M} {
    assert(n > 0);
    assert(m > 0);
    for (int i = 0; i < n; ++i) {
      std::iota(arr.begin() + (m * i), arr.begin() + ((m * i) + m), 1);
    }
  }

  Matrix(const int val) : n{N}, m{M} {
    assert(n > 0);
    assert(m > 0);
    arr.fill(val);
  }

  constexpr int const &operator()(const std::size_t x,
                                  const std::size_t y) const {
    return arr.at(x * m + y);
  }

  constexpr int &operator()(const std::size_t x, const std::size_t y) {
    return arr.at(x * m + y);
  }

  constexpr std::size_t const rows() const { return n; }

  constexpr std::size_t const cols() const { return m; }

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
    using LockFreeMatrix = Custom::Matrix<rows, cols>;

    LockFreeMatrix a;
    LockFreeMatrix b(2000);
    LockFreeMatrix res(0);
    {
      assert(a.cols() == b.cols() && a.rows() == b.rows());
      assert(a.cols() == res.cols() && a.rows() == res.rows());
    }
    std::cout << "A:" << std::endl;
    std::cout << a;
    std::cout << "B: " << std::endl;
    std::cout << b;
    for (std::size_t row{0}; row < rows; ++row) {
      threads.emplace_back([&, columns = cols, current_row = row] {
        for (std::size_t col{0}; col < columns; ++col) {
          res(current_row, col) += a(current_row, col) + b(current_row, col);
        }
      });
    }
    std::cout << "wait for join threads" << std::endl;
    wait_threads(threads);

    std::cout << "result: " << std::endl;
    std::cout << res;

  } catch (const std::out_of_range &e) {
    std::cerr << "range: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "something else" << std::endl;
  }
}
