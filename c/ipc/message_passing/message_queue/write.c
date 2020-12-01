#include <stdio.h>
#include <stdlib.h>

#include <sys/msg.h> // msgget, msgsend

#include "type.h"    // msg, BUFFER_SIZE, create_queue

int main() {
    puts("Write to IPC message queue");
    puts("Run ./bin/c/c_lab07_ipc_mq_read to read from IPC MQ");

    int message_id = create_queue();
    msg.type = 1;

    puts("Insert data: ");
    fgets(msg.text, BUFFER_SIZE, stdin);

    // send message to message queue
    msgsnd(message_id, &msg, sizeof msg, 0);

    // Display the message
    printf("Data sent to queue %s\n", msg.text);

    return EXIT_SUCCESS;
}