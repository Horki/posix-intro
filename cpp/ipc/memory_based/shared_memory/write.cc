#include <iostream>

#include "shm.hh"

using SharedMemory::Write;
using SharedMemory::Exception::Exc;

int main() {
  std::cout << "Write process to shared buffer" << std::endl;
  std::cout << "run /bin/cpp/cc_lab07_ipc_shm_r or ./bin/c/c_lab07_ipc_shm_r"
            << std::endl;

  try {
    Write w;
    w.run();
  } catch (const Exc& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other exception" << std::endl;
  }
}