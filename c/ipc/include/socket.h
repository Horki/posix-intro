#ifndef SOCKET_H_
#define SOCKET_H_

#include <arpa/inet.h>   // htonl, htons
#include <netdb.h>       // gethostbyname, hostent
#include <netinet/in.h>  // sockaddr_in, INADDR_ANY
#include <stdbool.h>     // bool
#include <stdio.h>       // printf
#include <string.h>      // memset
#include <sys/socket.h>  // socket, AF_INET, AF_LOCAL

#define BUFFER_SIZE 20

static const char *HOST = "localhost";
static const u_int16_t PORT_NUMBER = 8080;

void report(const char *msg, bool terminate) {
  perror(msg);
  if (terminate) {
    exit(EXIT_FAILURE);
  }
}

// Create Kernel-Level socket buffer
int create_socket() {
  int socket_fd =
      socket(AF_INET,      // Network VS AF_LOCAL
             SOCK_STREAM,  // Reliable bidirectional, arbitrary payload size
             0             // System picks underlying protocol, TCP
      );

  if (socket_fd < 0) {
    report("socket", true);
  }
  return socket_fd;
}

// Connect to the server: configure server's address list
void connect_to_server(int socket_fd, struct hostent *hptr) {
  struct sockaddr_in socket_addr;
  memset(&socket_addr, 0, sizeof socket_addr);
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_addr.s_addr =
      ((struct in_addr *)hptr->h_addr_list[0])->s_addr;
  // for listening
  socket_addr.sin_port = htons(PORT_NUMBER);

  if (connect(socket_fd, (struct sockaddr *)&socket_addr, sizeof socket_addr) <
      0) {
    report("connect", true);
  }

  // Write some stuff and read the echoes
  printf("Connected to server %s:%i\n", HOST, PORT_NUMBER);
}

// Bind the server local memory in address
void bind_to_server(int socket_fd) {
  struct sockaddr_in socket_addr;
  memset(&socket_addr, 0, sizeof socket_addr);
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // for listening
  socket_addr.sin_port = htons(PORT_NUMBER);

  if (bind(socket_fd, (struct sockaddr *)&socket_addr, sizeof socket_addr) <
      0) {
    report("bind", true);
  }

  // Listening to the socket
  if (listen(socket_fd, 30) < 0) {
    // Listen to clients
    report("listen", true);
  }

  // Write some stuff and read the echoes
  printf("Listening for clients on %s:%i\n", HOST, PORT_NUMBER);
}

// Get the address of the host
struct hostent get_address() {
  struct hostent *hptr = gethostbyname(HOST);
  if (!hptr) {
    report("gethostbyname", true);
  }
  if (hptr->h_addrtype != AF_INET) {
    report("bad address family", true);
  }
  return *hptr;
}

#endif