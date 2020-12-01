#ifndef TYPE_H_
#define TYPE_H_

#include <sys/ipc.h>

#define BUFFER_SIZE 20
#define PROJECT_ID 65

struct message_buffer {
    long type;
    char text[BUFFER_SIZE];
} msg;

int create_queue() {
    // ftok to generate a random key
    key_t key = ftok("progfile", PROJECT_ID);
    // msgget creates a message queue, and returns identifier
    return msgget(key, 0666 | IPC_CREAT);
}

#endif