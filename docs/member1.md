# Secure File Access Monitor - Member 1 Design & Research

## Project Overview

The goal of this project is to monitor file access inside xv6.

The monitor should track which process accesses which file using file-related system calls.

The main syscalls to monitor are:

- open
- read
- write
- close

Each log should contain:

- PID
- syscall name
- filename if available
- file descriptor

---

## Important xv6 Files

Main syscall implementations:

- kernel/sysfile.c

Syscall definitions:

- kernel/syscall.h

Syscall dispatch table:

- kernel/syscall.c

User syscall declarations:

- user/user.h
- user/usys.pl

---

## Hook Locations

### sys_open()

File:
- kernel/sysfile.c

Function:
- sys_open()

Hook should be inserted after successful file opening.

Reason:
- log only successful operations

Suggested log:

PID | open | filename | fd

---

### sys_read()

Function:
- sys_read()

Hook should be inserted after successful argfd().

Important:
- sys_read uses file descriptor, not filename.

Suggested log:

PID | read | fd

---

### sys_write()

Function:
- sys_write()

Hook should be inserted after successful argfd().

Suggested log:

PID | write | fd

---

### sys_close()

Function:
- sys_close()

Hook should be inserted before closing the file descriptor.

Suggested log:

PID | close | fd

---

## Main Design Challenge

sys_open() has direct access to the filename.

However:

- sys_read()
- sys_write()
- sys_close()

mainly work using file descriptors.

Possible solutions:

### Option 1

Log file descriptor only.

Example:

PID 4 | read | fd=3

### Option 2

Store fd-to-filename mapping after open.

This is better but more complex.

---

## Suggested Log Structure

```c
#define MAX_LOGS 128
#define MAX_NAME 64

struct access_log {
  int pid;
  int fd;
  char syscall_name[16];
  char filename[MAX_NAME];
};
