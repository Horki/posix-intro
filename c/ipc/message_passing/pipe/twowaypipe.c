#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // pipe, read, write, close

#define BUFFER_SIZE 20
#define PIPES_NO 2

static void create_pipe(int[PIPES_NO], int);

int main() {
    int pipe_fds_1[PIPES_NO], pipe_fds_2[PIPES_NO];
    const char pipe1_write_msg[BUFFER_SIZE] = "Hi from pipe 1";
    const char pipe2_write_msg[BUFFER_SIZE] = "Hello from pipe 2";
    char read_msg[BUFFER_SIZE];

    create_pipe(pipe_fds_1, 1);
    create_pipe(pipe_fds_2, 2);

    pid_t pid = fork();

    if (pid == 0) {
        // Parent Process
        puts("Parent Process");
        // close unwanted pipe 1 read side
        close(pipe_fds_1[0]);
        // close unwanted pipe 2 write side
        close(pipe_fds_2[1]);
        printf("[Parent] writing to pipe 1: [%s]\n", pipe1_write_msg);
        write(pipe_fds_1[1], pipe1_write_msg, sizeof pipe1_write_msg);
        // Reading from pipe 2
        read(pipe_fds_2[0], read_msg, sizeof read_msg);
        printf("[Parent] reading from pipe 2: [%s]\n", read_msg);
    } else {
        // Child Process
        puts("Child Process");
        // close unwanted pipe 1 write side
        close(pipe_fds_1[1]);
        // close unwanted pipe 2 read side
        close(pipe_fds_2[0]);
        // Reading from pipe 1
        read(pipe_fds_1[0], read_msg, sizeof read_msg);
        printf("[Child] reading from pipe 1: [%s]\n", read_msg);
        // Write to pipe 1
        write(pipe_fds_2[1], pipe2_write_msg, sizeof pipe2_write_msg);
        printf("[Child] writing to pipe 2: [%s]\n", pipe2_write_msg);
    }

    printf("Pipe %d, done.\n", pid);
    return EXIT_SUCCESS;
}

static void create_pipe(int pipe_fds[PIPES_NO], int pipe_id) {
    int return_status = pipe(pipe_fds);
    if (return_status == -1) {
        fprintf(stderr, "unable to create pipe %d\n", pipe_id);
        exit(EXIT_FAILURE);
    }
}