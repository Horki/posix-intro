# Posix system development basics

## Originally "win32" tasks, translated into POSIX env, in C (and something nearly in RUST)

1. Open a "gedit" program in child process, and output exit signal; ex: use kill, pkill to to quit "gedit"

    ```console
    kill -TERM $(pgrep gedit)
    ```

    id | Name | Description
    --- | --- | ---
    1 | HUP  | Hangup. The controlling term (tty or pty)has exited
    2 | INT  | Interrupt. Ctrl-C.
    3 | QUIT | User requested core dump.
    9 | KILL | Terminate process immediately, can't be ignored.
    11 | SEGV | Invalid VM ref, also known as Seg fault
    15 | TERM | Request terminator, can be caught by process.

2. Intro to "pthreads", some advantages over processess:
    * thread creation is faster(cheaper) than process creation
    * sharing data(e.g.: global, heap) between threads is easier
    * context-switch time usually lower for threads

    a) In separate thread multiply numbers of data[1, 10] in struct

    ```c
    struct data {
        int32_t a, b, c, total;
    }
    ```

    b) Addition of two matrices, calculate each row in separate thread(no datarace possible)