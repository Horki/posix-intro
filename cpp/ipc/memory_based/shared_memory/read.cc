#include <iostream>

#include "shm.hh"

using SharedMemory::Read;
using SharedMemory::Exception::Exc;

int main() {
  std::cout << "Read process from shared buffer" << std::endl;
  std::cout << "run ./bin/cpp/cc_lab07_ipc_shm_w or ./bin/c/c_lab07_ipc_shm_w"
            << std::endl;

  try {
    Read r;
    r.run();
  } catch (const Exc& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other exception" << std::endl;
  }
}