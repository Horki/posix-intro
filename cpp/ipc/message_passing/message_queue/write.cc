#include <iostream>

#include "mqueue.hh"

int main() {
  using MessageQueue::Write;
  std::cout << "Write server, press '0' to terminate." << std::endl;
  Write w;
  w.run();
}