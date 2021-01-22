#include <stdbool.h>     // bool
#include <stdio.h>       // puts, printf
#include <stdlib.h>      // EXIT_SUCCESS
#include <string.h>      // memset, strncasecmp
#include <sys/socket.h>  // socklen_t
#include <unistd.h>      // close, read, write, ssize_t

#include "socket.h"  // create_socket, bind_to_server, BUFFER_SIZE, report, sck_in_t

int main() {
  puts("Socket Server");
  puts("Please run ./bin/c_lab07_ipc_socket_client");
  int server_fd = create_socket();

  // Bind the server local memory in address
  bind_to_server(server_fd);

  char buffer[BUFFER_SIZE + 1];
  sck_in_t client_addr;
  socklen_t len = sizeof client_addr;
  memset(&client_addr, 0, len);

  int client_fd = accept(server_fd, (sck_t *)&client_addr, &len);
  if (client_fd < 0) {
    // Don't terminate
    report("accept", true);
  }

  while (true) {
    ssize_t count = read(client_fd, buffer, BUFFER_SIZE);
    if (count > 0) {
      printf("Received from client '%s'\n", buffer);
      // echo as confirmation
      write(client_fd, buffer, BUFFER_SIZE);
    }

    // Break connection
    if (strncasecmp(buffer, "done", 4) == 0) {
      printf("Closing client connection! [%d]\n", client_fd);
      close(client_fd);
      puts("exiting server...");
      break;
    }
  }

  close(server_fd);
  puts("Server DONE");

  return EXIT_SUCCESS;
}
