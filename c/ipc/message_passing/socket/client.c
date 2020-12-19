#include <netdb.h>   // hostent
#include <stdio.h>   // puts, printf, fgets, fprintf
#include <stdlib.h>  // EXIT_SUCCESS
#include <string.h>  // strtok
#include <unistd.h>  // close, read, write

#include "socket.h"  // create_socket, get_address, connect_to_server

int main() {
  puts("Socket Client");
  puts("Please run ./bin/c_lab07_ipc_socket_server");
  int socket_fd = create_socket();
  puts("Write 'done' to kill the server");

  // Get the address of the host
  struct hostent hptr = get_address();

  connect_to_server(socket_fd, &hptr);

  // Send to server
  char buffer[BUFFER_SIZE + 1];
  printf("Insert message: ");
  fgets(buffer, BUFFER_SIZE, stdin);
  strtok(buffer, "\n");

  if (write(socket_fd, buffer, strlen(buffer)) > 0) {
    puts("Message sent");
    if (read(socket_fd, buffer, sizeof buffer) > 0) {
      printf("Sent: '%s'\n", buffer);
    } else {
      fprintf(stderr, "Message failed reading");
    }
  } else {
    fprintf(stderr, "Message sending failed");
  }

  // Break connection
  puts("Client DONE!");
  close(socket_fd);

  return EXIT_SUCCESS;
}