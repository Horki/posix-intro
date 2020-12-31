# [Inter-Process Communication IPC](https://en.wikipedia.org/wiki/Inter-process_communication)

## Write process, read process to shared object(as FILE), with named semaphores (as shared between processes) using system calls, (cp from man pages)

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
