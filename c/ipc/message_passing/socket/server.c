#include <arpa/inet.h>   // htonl, htons
#include <netinet/in.h>  // sockaddr_in, INADDR_ANY
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // memset, strncmp
#include <sys/socket.h>  // socket, AF_INET, AF_LOCAL, bind, listen, socklen_t, accept
#include <unistd.h>  // close, read, write

#include "common.h"  // report, HOST, PORT_NUMBER

int main() {
  puts("Socket Server");
  puts("Please run ./bin/c_lab07_ipc_socket_client");
  int socket_fd =
      socket(AF_INET,      // Network VS AF_LOCAL
             SOCK_STREAM,  // Reliable bidirectional, arbitrary payload size
             0             // System picks underlying protocol, TCP
      );

  if (socket_fd < 0) {
    report("socket", true);
  }

  // Bind the server local memory in address
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

  // Read from client
  while (true) {
    char buffer[BUFFER_SIZE + 1];
    struct sockaddr_in client_addr;
    socklen_t len = sizeof client_addr;
    // Accept blocks
    int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0) {
      // Don't terminate
      report("accept", false);
      continue;
    }
    memset(buffer, '\0', sizeof buffer);
    ssize_t count = read(client_fd, buffer, sizeof buffer);
    if (count > 0) {
      puts(buffer);
      // echo as confirmation
      write(client_fd, buffer, sizeof buffer);
    }

    // Break connection
    puts("Closing client connection!");
    close(client_fd);
    if (strncmp(buffer, "done", 4) == 0) {
      puts("exiting server...");
      break;
    }
  }

  puts("Server DONE");
  close(socket_fd);
  return EXIT_SUCCESS;
}