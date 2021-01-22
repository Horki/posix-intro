#include <stdbool.h>  // true, false
#include <stdio.h>    // puts, printf, fgets
#include <stdlib.h>   // EXIT_SUCCESS
#include <string.h>   // strtok, memset, strncasecmp
#include <unistd.h>   // close, read, write

#include "socket.h"  // create_socket, get_address, connect_to_server, host_t, report, BUFFER_SIZE

int main() {
  puts("Socket Client");
  puts("Please run ./bin/c_lab07_ipc_socket_server");
  int client_fd = create_socket();
  puts("Write 'done' to kill the server");

  // Get the address of the host
  host_t *hptr = get_address();

  connect_to_server(client_fd, hptr);

  // Send to server
  char buffer[BUFFER_SIZE + 1];
  memset(buffer, '\0', BUFFER_SIZE);
  char received[BUFFER_SIZE + 1];
  memset(received, '\0', BUFFER_SIZE);
  while (true) {
    printf("Insert message: ");
    memset(buffer, '\0', BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE, stdin);
    strtok(buffer, "\n");

    if (write(client_fd, buffer, BUFFER_SIZE) == 0) {
      report("Message sending failed", false);
      continue;
    }
    printf("Sent to server: '%s'\n", buffer);
    if (read(client_fd, received, BUFFER_SIZE) == 0) {
      report("Message failed reading", false);
      continue;
    }
    printf("Received from server: '%s'\n", received);

    if (strncasecmp(received, "done", 4) == 0) {
      break;
    }
  }

  // Break connection
  close(client_fd);
  puts("Client DONE!");

  return EXIT_SUCCESS;
}
