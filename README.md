# Posix system development basics

## Originally "win32" tasks from my university, translated into POSIX env, in C (and something nearly in RUST)

1. Open a "gedit" program in child process, and output exit signal; ex: use kill, pkill to to quit "gedit"

    ```console
    kill -TERM $(pgrep gedit)
    ```

    [Signal (IPC)](https://en.wikipedia.org/wiki/Signal_(IPC)) list of most used signals
    id | Name | Description
    --- | --- | ---
    1 | HUP  | Hangup. The controlling term (tty or pty)has exited
    2 | INT  | Interrupt. Ctrl-C.
    3 | QUIT | User requested core dump.
    9 | KILL | Terminate process immediately, can't be ignored.
    11 | SEGV | Invalid VM ref, also known as Seg fault
    15 | TERM | Request terminator, can be caught by process.

    [Proces States](https://en.wikipedia.org/wiki/Process_state#Primary_process_states), from [ps](https://en.wikipedia.org/wiki/Ps_(Unix)) man page
    Code | Name
    --- | ---
    D | uninterruptible sleep (usually IO)
    I | Idle kernel thread
    R | running or runnable (on run queue)
    S | interruptible sleep (waiting for an event to complete)
    T | stopped by job control signal
    t | stopped by debugger during the tracing
    W | paging (not valid since the 2.6.xx kernel)
    X | dead (should never be seen)
    Z | defunct ("zombie") process, terminated but not reaped by its parent

    For BSD formats and when the stat keyword is used, additional characters may be displayed:
    Code | Name
    --- | ---
    < | high-priority (not nice to other users)
    N | low-priority (nice to other users)
    L | has pages locked into memory (for real-time and custom IO)
    s | is a session leader
    l | is multi-threaded (using CLONE_THREAD, like NPTL pthreads do)
    + | is in the foreground process group

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

6. Modify Task 4, by using binary semaphore instead of mutex

7. [IPC](https://en.wikipedia.org/wiki/Inter-process_communication) Write process, read process to shared object(as FILE), with named semaphores (as shared between processes) using system calls, (cp from man pages)

    ```console
    watch -n 1 ls -l /dev/shm # lookup for shared semaphore
    strace ./c_lab07_w        # view system calls
    valgrind -v ./c_lab07_w   # check memory leaks
    ```

    Mapping Types

    Visibility | File | Anonymous
    -- | -- | --
    Private | Init mem from file | Memory alloc
    Shared | Memory Maped I/O; shared IPC | Shared mem between processes

    File flags

    Flags | Description
    -- | --
    O_CREAT | Create file if it does not exists.
    O_RDWR | Open for reading and writing.
    O_RDONLY | Open for reading only.
    O_WRONLY | Open for writing only.

    Access Permission flags

    Name | Numeric Value | Description
    -- | -- | --
    S_IRUSR | 0400 | Read permission, owner
    S_IWUSR | 0200 | Write permission, owner

    MMAP Privileges

    Symbolic Constant | Description
    -- | --
    PROT_READ | Data can be read.
    PROT_WRITE | Data can be written.
    PROT_EXEC | Data can be executed.
    PROT_NONE | Data cannot be accessed.

    MMAP Flags

    Symbolic Constant | Description
    -- | --
    MAP_SHARED | Changes are shared.
    MAP_PRIVATE | Changes are private.
    MAP_FIXED | Interpret *addr* exactly.
