#include <iostream>

#include "buffer.hh"

int main() {
  {
    Custom::Binary::Semaphore<30> semaphore;
    semaphore.run();
  }
  std::cout << "main done" << std::endl;
}