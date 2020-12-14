#include <iostream>

#include "buffer.hh"

int main() {
  { Custom::ProducerConsumer::Semaphore<10> buff(3, 2); }
  std::cout << "main done " << std::endl;
}
