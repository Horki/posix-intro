#include <netinet/in.h>  // sockaddr_in
#include <stdbool.h>     // bool
#include <stdio.h>       // puts, printf
#include <stdlib.h>      // EXIT_SUCCESS
#include <string.h>      // memset, strncasecmp
#include <sys/socket.h>  // socket, AF_INET, AF_LOCAL, bind, listen, socklen_t, accept
#include <unistd.h>  // close, read, write

#include "socket.h"  // create_socket, bind_to_server, BUFFER_SIZE, report

int main() {
  puts("Socket Server");
  puts("Please run ./bin/c_lab07_ipc_socket_client");
  int socket_fd = create_socket();

  // Bind the server local memory in address
  bind_to_server(socket_fd);

  char buffer[BUFFER_SIZE + 1];
  struct sockaddr_in client_addr;
  socklen_t len = sizeof client_addr;

  // Output from client
  while (true) {
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
      printf("Received from client '%s'\n", buffer);
      // echo as confirmation
      write(client_fd, buffer, sizeof buffer);
    }

    // Break connection
    printf("Closing client connection! [%d]\n", client_fd);
    close(client_fd);
    if (strncasecmp(buffer, "done", 4) == 0) {
      puts("exiting server...");
      break;
    }
  }

  puts("Server DONE");
  close(socket_fd);
  return EXIT_SUCCESS;
}