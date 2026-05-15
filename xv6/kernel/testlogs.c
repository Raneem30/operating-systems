#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

struct log_entry {
    int pid;
    int fd;
    char syscall_name[16];
    char filename[64];
};

#define MAX_LOGS 128

int strcmp_simple(char *a, char *b)
{
    while(*a && *a == *b){ a++; b++; }
    return *a - *b;
}

int passed = 0;
int failed = 0;

void check(int condition, char *name)
{
    if(condition){
        printf("  [PASS] %s\n", name);
        passed++;
    } else {
        printf("  [FAIL] %s\n", name);
        failed++;
    }
}

int find_log(struct log_entry *logs, int n, char *syscall, char *filename)
{
    int i;
    for(i = 0; i < n; i++){
        if(strcmp_simple(logs[i].syscall_name, syscall) == 0){
            if(filename == 0)
                return 1;
            if(strcmp_simple(logs[i].filename, filename) == 0)
                return 1;
        }
    }
    return 0;
}

int main(void)
{
    struct log_entry logs[MAX_LOGS];
    int fd, fd2;
    int n;
    char buf[64];
    int i;

    printf("  File Access Monitor - Test Suite\n");

    // TEST GROUP 1: Basic operations
    printf("[Group 1] Basic file operations\n");

    fd = open("test_basic.txt", O_CREATE | O_WRONLY);
    check(fd >= 0, "open() returns valid fd");

    int w = write(fd, "hello world", 11);
    check(w == 11, "write() writes correct bytes");

    close(fd);

    fd = open("test_basic.txt", O_RDONLY);
    int r = read(fd, buf, sizeof(buf));
    check(r == 11, "read() reads correct bytes");

    close(fd);

    n = getlogs(logs, MAX_LOGS);
    check(n > 0, "getlogs() returns logs");
    check(find_log(logs, n, "open", "test_basic.txt"), "open logged with filename");
    check(find_log(logs, n, "write", 0), "write logged");
    check(find_log(logs, n, "read", 0), "read logged");
    check(find_log(logs, n, "close", 0), "close logged");

    printf("\n");

    // TEST GROUP 2: Multiple files at once
    printf("[Group 2] Multiple files simultaneously\n");

    fd = open("file_a.txt", O_CREATE | O_WRONLY);
    fd2 = open("file_b.txt", O_CREATE | O_WRONLY);
    check(fd >= 0 && fd2 >= 0, "two files opened at same time");
    check(fd != fd2, "different fd for each file");

    write(fd, "aaa", 3);
    write(fd2, "bbb", 3);
    close(fd);
    close(fd2);

    n = getlogs(logs, MAX_LOGS);
    check(find_log(logs, n, "open", "file_a.txt"), "file_a.txt open logged");
    check(find_log(logs, n, "open", "file_b.txt"), "file_b.txt open logged");

    printf("\n");

    
    // TEST GROUP 3: PID is correct
    printf("[Group 3] PID verification\n");

    int my_pid = getpid();
    n = getlogs(logs, MAX_LOGS);

    int pid_correct = 1;
    for(i = 0; i < n; i++){
        if(logs[i].pid <= 0){
            pid_correct = 0;
            break;
        }
    }
    check(pid_correct, "all PIDs are valid (> 0)");

    int found_my_pid = 0;
    for(i = 0; i < n; i++){
        if(logs[i].pid == my_pid){
            found_my_pid = 1;
            break;
        }
    }
    check(found_my_pid, "our PID appears in logs");

    printf("\n");

  // TEST GROUP 4: Child process logging
    printf("[Group 4] Child process logging\n");

    int pid = fork();
    if(pid == 0){
        // child process
        fd = open("child_file.txt", O_CREATE | O_WRONLY);
        write(fd, "child", 5);
        close(fd);
        exit(0);
    } else {
        wait(0);

        n = getlogs(logs, MAX_LOGS);
        check(find_log(logs, n, "open", "child_file.txt"), "child process open logged");

        // check that child has different PID in logs
        int child_pid_found = 0;
        for(i = 0; i < n; i++){
            if(strcmp_simple(logs[i].filename, "child_file.txt") == 0
               && logs[i].pid != my_pid){
                child_pid_found = 1;
                break;
            }
        }
        check(child_pid_found, "child PID differs from parent PID");
    }

    printf("\n");

    // TEST GROUP 5: File descriptor validation
    printf("[Group 5] File descriptor validation\n");

    n = getlogs(logs, MAX_LOGS);
    int fd_valid = 1;
    for(i = 0; i < n; i++){
        if(logs[i].fd < 0){
            fd_valid = 0;
            break;
        }
    }
    check(fd_valid, "all fd values are >= 0");

    printf("\n");

    // TEST GROUP 6: Edge case - read empty file
    printf("[Group 6] Edge case - empty file\n");

    fd = open("empty.txt", O_CREATE | O_WRONLY);
    close(fd);
    fd = open("empty.txt", O_RDONLY);
    r = read(fd, buf, sizeof(buf));
    check(r == 0, "read on empty file returns 0");
    close(fd);

    n = getlogs(logs, MAX_LOGS);
    check(n > 0, "operations on empty file still logged");

    printf("\n");

    // 
    printf("  Results: %d passed, %d failed\n", passed, failed);

    if(failed == 0)
        printf("  ALL TESTS PASSED!\n");
    else
        printf("  Some tests failed. Review output.\n");

    printf("--------------------\n");

    unlink("test_basic.txt");
    unlink("file_a.txt");
    unlink("file_b.txt");
    unlink("child_file.txt");
    unlink("empty.txt");

    exit(0);
}
