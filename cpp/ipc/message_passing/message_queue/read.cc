#include <iostream>

#include "mqueue.hh"

int main() {
  using MessageQueue::Read;
  std::cout << "Read, listening from 'write' server" << std::endl;
  Read r;
  r.run();
}