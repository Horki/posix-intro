#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h> // IPC_RMID
#include <sys/msg.h> // msgget, msgrcv, msgctl

#include "type.h"    // msg, create_queue

int main() {
    puts("Read from IPC message queue");
    puts("Run ./bin/c/c_lab07_ipc_mq_write to send to IPC MQ");

    int message_id = create_queue();

    // receive the message from queue
    msgrcv(message_id, &msg, sizeof msg, 1, 0);

    // Display the message
    printf("Data received from queue: %s\n", msg.text);

    // Destroy the message queue
    puts("Destroy the message queue!!!");
    msgctl(message_id, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}