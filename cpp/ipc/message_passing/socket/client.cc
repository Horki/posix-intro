#include <iostream>

#include "sockets.hh"

int main() {
  using Socket::Client;
  using Socket::Exception::Exc;
  std::cout << "Socket Client" << std::endl;
  std::cout << "Please run ./bin/cpp/cc_lab07_ipc_socket_server" << std::endl;
  std::cout << "Write 'done' to kill the server" << std::endl;
  try {
    Client c;
    c.run();
  } catch (const Exc& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "other" << std::endl;
  }
  std::cout << "Client DONE!" << std::endl;
}