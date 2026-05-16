#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

struct log_entry {
    int pid;
    char proc_name[16];
    char file_name[128];
    int action;
    int tick;
};

#define MAX_LOGS 512

int
main(void)
{
    struct log_entry logs[MAX_LOGS];
    int fd;
    int n, i;
    char buf[32];
    int passed = 0;
    int failed = 0;

    printf("=== File Access Monitor Tests ===\n\n");

    // Test 1: open, write, read, close a file
    printf("Test 1: basic file operations\n");
    fd = open("test.txt", O_CREATE | O_WRONLY);
    write(fd, "hello", 5);
    close(fd);
    fd = open("test.txt", O_RDONLY);
    read(fd, buf, 5);
    close(fd);

    n = getlogs(logs, MAX_LOGS);
    printf("  got %d logs\n", n);
    if(n > 0){ printf("  [PASS]\n"); passed++; }
    else     { printf("  [FAIL]\n"); failed++; }

    // Test 2: check open was logged with filename
    printf("Test 2: open logged with filename\n");
    int found = 0;
    for(i = 0; i < n; i++){
        if(logs[i].action == 0 && strcmp(logs[i].file_name, "test.txt") == 0)
            found = 1;
    }
    if(found){ printf("  [PASS]\n"); passed++; }
    else     { printf("  [FAIL]\n"); failed++; }

    // Test 3: check all 4 actions were logged
    printf("Test 3: all actions logged (open/read/write/close)\n");
    int has[4] = {0, 0, 0, 0};
    for(i = 0; i < n; i++){
        if(logs[i].action >= 0 && logs[i].action <= 3)
            has[logs[i].action] = 1;
    }
    if(has[0] && has[1] && has[2] && has[3]){ printf("  [PASS]\n"); passed++; }
    else { printf("  [FAIL]\n"); failed++; }

    // Test 4: PID is valid
    printf("Test 4: PIDs are valid\n");
    int valid = 1;
    for(i = 0; i < n; i++){
        if(logs[i].pid <= 0) valid = 0;
    }
    if(valid){ printf("  [PASS]\n"); passed++; }
    else     { printf("  [FAIL]\n"); failed++; }

    // Test 5: two files at once
    printf("Test 5: multiple files\n");
    int fd1 = open("a.txt", O_CREATE | O_WRONLY);
    int fd2 = open("b.txt", O_CREATE | O_WRONLY);
    close(fd1);
    close(fd2);
    n = getlogs(logs, MAX_LOGS);
    int found_a = 0, found_b = 0;
    for(i = 0; i < n; i++){
        if(strcmp(logs[i].file_name, "a.txt") == 0) found_a = 1;
        if(strcmp(logs[i].file_name, "b.txt") == 0) found_b = 1;
    }
    if(found_a && found_b){ printf("  [PASS]\n"); passed++; }
    else                  { printf("  [FAIL]\n"); failed++; }

    // Results
    printf("\n %d passed, %d failed n", passed, failed);

    unlink("test.txt");
    unlink("a.txt");
    unlink("b.txt");
    exit(0);
}
