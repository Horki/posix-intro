# Posix system development basics

## Originally "win32" tasks from my university, translated into POSIX env, in C (and something nearly in RUST)

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
        int a, b, c, total;
    }
    ```

    b) Addition of two matrices, calculate each row in separate thread(no datarace possible)

3. Mutex (Mutual Exclusion)
    * to avoid threads to update shared variable
    * atomic access to any shared resource
    * states: locked and unlocked
    * deadlock: when two or more threads locks mutex

    a) Create two functions(with mutex), each function will iterate counter 50 times, execute each function in 5 threads

    ```c
    // ex func1()
    for (int i = 0; i < 50; ++i)
        printf("Function 1 = %d; Thread = %d\n", counter, thread);
    ```

    b) Create two seperated threads(with mutex) that will execute same function, function should print from which threads it's executed and iterate 10 times with some math

    ```c
    // ex print_thread()
    for (int i = 0; i < 10; ++i) {
        switch(thread) {
            case 1:
                printf("Thread 1 = i = %d\n", (i * 1000) + 1);
                break;
    ```

4. Simulate airport using mutex; 5 airplanes needs to land without crashing into each other
    * Thread Safety
    * thread-safe function: can be safely invokedby multiple threads at same time
    * non-thread-safe function: usually use global or static varibles

5. Semaphores

    a) create two threads that iterate a number, with help of binary semaphores sync threads, so one thread increments *ONLY* odd number, other even

    b) simulate warehouse work, trucks(producers) are storing items into warehouse, while workers(consumers) are taking items from warehouse, using [Producer-Consumer](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem) pattern with semaphores and circular buffer(ring)