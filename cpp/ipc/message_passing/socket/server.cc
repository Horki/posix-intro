#include <iostream>

#include "sockets.hh"

int main() {
  using Socket::Server;
  using Socket::Exception::Exc;
  std::cout << "Socket Server" << std::endl;
  std::cout << "Please run ./bin/cpp/cc_lab07_ipc_socket_client" << std::endl;
  try {
    Server s;
    s.run();
  } catch (const Exc& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other" << std::endl;
  }
  std::cout << "Server DONE" << std::endl;
}