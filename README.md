# operating-systems
# Secure File Access Monitor

## Project Idea

This project modifies xv6 to monitor file access operations.

The system records which process:
- opens files
- reads files
- writes files
- closes files

The main syscalls used are:

- open
- read
- write
- close

---

## Goal

The goal of this project is to understand how file-related system calls work inside xv6 and how monitoring can be added inside the kernel.

---

## Main Files

Important xv6 files used in this project:

```text
kernel/sysfile.c
kernel/syscall.c
kernel/syscall.h
user/user.h
user/usys.pl
```

---

## Team Tasks

### Member 1
- Research and design
- Find hook locations
- Explain filename vs file descriptor issue

### Member 2
- Modify syscalls
- Add hooks inside:
  - sys_open
  - sys_read
  - sys_write
  - sys_close

### Member 3
- Implement log buffer
- Add synchronization if needed

### Member 4
- Implement getlogs() syscall

### Member 5
- Create showlogs.c
- Testing and output verification

---

## System Workflow

1. A user program performs file operations.
2. xv6 system calls handle these operations.
3. Hooks inside the syscalls record the activity.
4. The kernel stores logs inside a buffer.
5. getlogs() transfers logs to user space.
6. showlogs.c prints the logs.

---

## Main Problem

The main issue is that:
- sys_open() knows the filename
- sys_read(), sys_write(), and sys_close() mainly use file descriptors

Example:

```c
open("test.txt")
```

returns:

```text
fd = 3
```

Later operations use:

```c
read(3)
write(3)
close(3)
```

This means read/write/close may only know the file descriptor and not the original filename.

---

## Suggested Log Structure

```c
struct access_log {
    int pid;
    int fd;
    char syscall_name[16];
    char filename[64];
};
```

---

## Expected Output

Example logs:

```text
PID 3 | open  | test.txt
PID 3 | read  | fd=3
PID 3 | write | fd=3
PID 3 | close | fd=3
```

---

## Running xv6

Run xv6 using:

```sh
make qemu
```

Exit xv6 using:

```text
Ctrl + A
X
```

---

## References

- xv6 Repository:
  https://github.com/mit-pdos/xv6-riscv

- xv6 Book:
  https://pdos.csail.mit.edu/6.828/2023/xv6/book-riscv-rev3.pdfS
