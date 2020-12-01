#include <arpa/inet.h>   // htons
#include <netdb.h>       // gethostbyname, hostent
#include <netinet/in.h>  // sockaddr_in, in_addr
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // memset
#include <sys/socket.h>  // socket, AF_INET, AF_LOCAL
#include <unistd.h>      // close, read, write

#include "common.h"  // report, HOST, PORT_NUMBER

int main() {
  puts("Socket Client");
  puts("Please run ./bin/c_lab07_ipc_socket_server");
  // Create Kernel-Level socket buffer
  int socket_fd =
      socket(AF_INET,      // Network VS AF_LOCAL
             SOCK_STREAM,  // Reliable bidirectional, arbitrary payload size
             0             // System picks underlying protocol, TCP
      );

  if (socket_fd < 0) {
    report("socket", true);
  }

  // Get the address of the host
  struct hostent *hptr = gethostbyname(HOST);
  if (!hptr) {
    report("gethostbyname", true);
  }
  if (hptr->h_addrtype != AF_INET) {
    report("bad address family", true);
  }

  // Connect to the server: configure server's address list
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

  // Read from client
  char buffer[BUFFER_SIZE + 1];
  puts("Insert message: ");
  fgets(buffer, BUFFER_SIZE, stdin);

  if (write(socket_fd, buffer, strlen(buffer)) > 0) {
    memset(buffer, '\0', sizeof buffer);
    if (read(socket_fd, buffer, sizeof buffer) > 0) {
      printf("Sent: '%s'\n", buffer);
    }
  }

  // Break connection
  puts("Client DONE!");
  close(socket_fd);

  return EXIT_SUCCESS;
}