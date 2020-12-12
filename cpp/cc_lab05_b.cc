#include <iostream>

#include "buffer.hh"

int main() {
  Custom::ProducerConsumer::Semaphore<10> buff(3, 2);
  buff.run();
}
