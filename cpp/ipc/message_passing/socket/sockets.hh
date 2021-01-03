#ifndef SOCKETS_HH_
#define SOCKETS_HH_

#include <arpa/inet.h>   // htonl, htons
#include <netdb.h>       // gethostbyname, hostent
#include <netinet/in.h>  // sockaddr_in, in_addr, INADDR_ANY
#include <sys/socket.h>  // socket, socketaddr, AF_INET, AF_LOCAL, SOCK_STREAM
#include <unistd.h>      // close, read, write, ssize_t

#include <cstring>  // memset, strncasecmp
#include <iostream>
#include <string>

namespace Socket {
namespace Exception {
class Exc {
 public:
  virtual std::string what() const = 0;
  virtual ~Exc() {}
};

class Socket : public Exc {
 public:
  std::string what() const { return "Cannot connect to socket"; }
};
class Connection : public Exc {
 public:
  std::string what() const { return "Cannot connect"; }
};
class Bind : public Exc {
 public:
  std::string what() const { return "Cannot bind"; }
};
class Listen : public Exc {
 public:
  std::string what() const { return "Cannot Listen"; }
};
class Host : public Exc {
 public:
  std::string what() const { return "Cannot get host by name"; }
};
class BadAddress : public Exc {
 public:
  std::string what() const { return "Cannot get host by name"; }
};
class Accept : public Exc {
 public:
  std::string what() const { return "Cannot accept"; }
};
}  // namespace Exception
class Task {
 public:
  virtual void run() const = 0;
  virtual ~Task() {}
};

struct Common {
  static constexpr std::streamsize buffer{20};
  static constexpr const char *host = "localhost";
  static constexpr u_int16_t port_number{8080};

  // Create Kernel-Level socket buffer
  static int create_socket() {
    int socket_fd =
        socket(AF_INET,      // Network VS AF_LOCAL
               SOCK_STREAM,  // Reliable bidirectional, arbitrary payload size
               0             // System picks underlying protocol, TCP
        );

    if (socket_fd < 0) {
      throw Exception::Socket{};
    }
    return socket_fd;
  }
  static bool terminate(const char *msg) {
    return strncasecmp(msg, "done", 4) == 0;
  }
};

class Client : public virtual Task {
  int client_fd;
  struct hostent *h_ptr;
  mutable char msg[Common::buffer];
  mutable char received[Common::buffer];

 public:
  Client()
      : client_fd{Common::create_socket()},
        h_ptr{get_address()},
        msg{},
        received{} {}
  ~Client() { close(client_fd); }
  void run() const {
    connect_to_server();
    do {
      std::cout << "Insert message: ";
      std::cin.getline(msg, Common::buffer);

      if (write(client_fd, msg, Common::buffer) == 0) {
        std::cerr << "Message sending failed" << std::endl;
        continue;
      }
      std::cout << "Sent to server: '" << msg << "'" << std::endl;
      if (read(client_fd, received, Common::buffer) == 0) {
        std::cerr << "Message failed reading" << std::endl;
        continue;
      }
      std::cout << "Received from server: '" << received << "'" << std::endl;
    } while (!Common::terminate(msg));
  }

 private:
  static struct hostent *get_address() {
    struct hostent *hptr = gethostbyname(Common::host);
    if (!hptr) {
      throw Exception::Host{};
    }
    if (hptr->h_addrtype != AF_INET) {
      // bad address family
      throw Exception::BadAddress{};
    }
    return hptr;
  }
  void connect_to_server() const {
    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr =
        ((struct in_addr *)h_ptr->h_addr_list[0])->s_addr;
    // for listening
    socket_addr.sin_port = htons(Common::port_number);

    if (connect(client_fd, (struct sockaddr *)&socket_addr,
                sizeof socket_addr) < 0) {
      throw Exception::Connection{};
    }

    // Write some stuff and read the echoes
    std::cout << "Connected to server " << Common::host << ":"
              << Common::port_number << std::endl;
  }
};

class Server : public virtual Task {
  int client_fd;
  int server_fd;
  mutable char msg[Common::buffer];

 public:
  Server() : server_fd{Common::create_socket()}, msg{} {
    bind_to_server();
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    memset(&client_addr, 0, len);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0) {
      throw Exception::Accept{};
    }
  }
  ~Server() {
    std::cout << "Closing client connection! " << client_fd << std::endl;
    close(server_fd);
    close(client_fd);
    std::cout << "Exiting server..." << std::endl;
  }
  void run() const {
    do {
      ssize_t count = read(client_fd, msg, Common::buffer);
      if (count > 0) {
        std::cout << "Received from client '" << msg << "'" << std::endl;
        // echo as confirmation
        write(client_fd, msg, Common::buffer);
      }
    } while (!Common::terminate(msg));
  }

 private:
  // Bind the server local memory in address
  void bind_to_server() const {
    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // for listening
    socket_addr.sin_port = htons(Common::port_number);

    if (bind(server_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) <
        0) {
      throw Exception::Bind{};
    }

    // Listening to the socket
    if (listen(server_fd, 30) < 0) {
      throw Exception::Listen{};
    }

    // Write some stuff and read the echoes
    std::cout << "Listening for client " << Common::host << ":"
              << Common::port_number << std::endl;
  }
};

}  // namespace Socket

#endif