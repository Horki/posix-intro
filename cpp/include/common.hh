#ifndef COMMON_HH_
#define COMMON_HH_

#include <iostream>
#include <thread>
#include <vector>

void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    } else {
      std::cerr << "ERROR JOINING THREADS!!!!" << std::endl;
    }
  }
}

#endif