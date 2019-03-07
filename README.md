# Posix system development basics

## Originally "win32" tasks, translated into POSIX env, in C and RUST

1. Open a "gedit" program in child process, and output exit signal; ex: use kill, pkill to to quid "gedit"
```console
$ kill -TERM $(pgrep gedit)
```
# | Name | Description
--- | --- | ---
1 | HUP  | Hangup. The controlling term (tty or pty)has exited
2 | INT  | Interrupt. Ctrl-C.
3 | QUIT | User requested core dump.
9 | KILL | Terminate process immediately, can't be ignored.
11 | SEGV | Invalid VM ref, also known as Seg fault
15 | TERM | Request terminator, can be caught by process.