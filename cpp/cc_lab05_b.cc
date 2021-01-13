#include <iostream>

#include "buffer.hh"

int main() {
  {
    using MPMC = Custom::ProducerConsumer::Semaphore<10>;
    MPMC buff(3, 2);
    buff.run();
  }
  std::cout << "main done " << std::endl;
}
