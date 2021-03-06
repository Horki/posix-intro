#include <algorithm>  // copy
#include <cassert>    // assert
#include <future>     // async, future
#include <iostream>   // cout
#include <iterator>   // ostream_iterator
#include <numeric>    // iota
#include <stdexcept>  // out_of_range
#include <valarray>   // valarray, slice
#include <vector>     // vector

namespace Custom {
using Iter = std::ostream_iterator<int>;

template <std::size_t N, std::size_t M>
class Matrix {
  std::valarray<int> arr;

 public:
  Matrix() : arr(N * M) {
    assert(N > 0);
    assert(M > 0);
    std::valarray<int> v(M);
    std::iota(std::begin(v), std::end(v), 1);
    for (std::size_t i{0}; i < N; ++i) {
      row(i) = v;
    }
  }

  explicit Matrix(const int val) : arr(val, N * M) {
    assert(N > 0);
    assert(M > 0);
  }

  Matrix(const Matrix &) = delete;

  Matrix &operator=(const Matrix &) = delete;

  Matrix(Matrix &&) = delete;

  Matrix &operator=(Matrix &&) = delete;

  constexpr int const &operator()(const std::size_t x,
                                  const std::size_t y) const {
    if (x >= N || y >= M) {
      throw std::out_of_range{"const () out of range"};
    }
    return arr[x * M + y];
  }

  constexpr int &operator()(const std::size_t x, const std::size_t y) {
    if (x >= N || y >= M) {
      throw std::out_of_range{"() out of range"};
    }
    return arr[x * M + y];
  }

  decltype(auto) row(std::size_t n) {
    if (n >= N) {
      throw std::out_of_range{"row out of range"};
    }
    return arr[std::slice(n * M, M, 1)];
  }

  decltype(auto) row(std::size_t n) const {
    if (n >= N) {
      throw std::out_of_range{"const row out of range"};
    }
    return arr[std::slice(n * M, M, 1)];
  }

  decltype(auto) col(std::size_t n) {
    if (n >= M) {
      throw std::out_of_range{"col out of range"};
    }
    return arr[std::slice(n, N, M)];
  }

  decltype(auto) col(std::size_t n) const {
    if (n >= M) {
      throw std::out_of_range{"const col out of range"};
    }
    return arr[std::slice(n, N, M)];
  }

  constexpr std::size_t rows() const noexcept { return N; }

  constexpr std::size_t cols() const noexcept { return M; }

  friend std::ostream &operator<<(std::ostream &os, const Matrix<N, M> &m) {
    for (std::size_t i{0}; i < m.rows(); ++i) {
      std::copy(&m(i, 0), &m(i, 0) + m.cols(), Iter(os, " "));
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
    using LockFreeMatrix = Custom::Matrix<rows, cols>;
    auto fn_task = [](LockFreeMatrix &res, const LockFreeMatrix &a,
                      const LockFreeMatrix &b, const std::size_t n) -> void {
      res.col(n) += a.col(n);
      res.col(n) += b.col(n);
    };

    std::vector<std::future<void>> futures;
    futures.reserve(rows);

    LockFreeMatrix a{};
    LockFreeMatrix b{2000};
    LockFreeMatrix res{0};
    {
      assert(a.cols() == b.cols() && a.rows() == b.rows());
      assert(a.cols() == res.cols() && a.rows() == res.rows());
    }
    std::cout << "A:" << std::endl;
    std::cout << a;
    std::cout << "B: " << std::endl;
    std::cout << b;
    for (std::size_t c{0}; c < cols; ++c) {
      // TODO: investigate!
      futures.push_back(
          std::async(fn_task, std::ref(res), std::cref(a), std::cref(b), c));
    }
    std::cout << "wait for promises" << std::endl;
    for (auto &f : futures) {
      f.get();
    }

    std::cout << "result: " << std::endl;
    std::cout << res;

  } catch (const std::out_of_range &e) {
    std::cerr << "range: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "something else" << std::endl;
  }
}
