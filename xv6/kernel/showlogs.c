#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct log_entry {
    int pid;
    int fd;
    char syscall_name[16];
    char filename[64];
};

#define MAX_LOGS 128

int
main(int argc, char *argv[])
{
    struct log_entry logs[MAX_LOGS];
    int n, i;
    int filter_pid = -1;
    int opens = 0, reads = 0, writes = 0, closes = 0;

    // help
    if(argc == 2 && strcmp(argv[1], "help") == 0){
        printf("Usage:\n");
        printf("  showlogs       - show all logs\n");
        printf("  showlogs <pid> - show logs for one process\n");
        printf("  showlogs help  - show this message\n");
        exit(0);
    }

    // pid filter
    if(argc == 2)
        filter_pid = atoi(argv[1]);

    n = getlogs(logs, MAX_LOGS);
    if(n < 0){
        printf("error reading logs\n");
        exit(1);
    }
    if(n == 0){
        printf("no logs recorded\n");
        exit(0);
    }

    printf("PID\tSyscall\t\tFD\tFilename\n");
    printf("---\t-------\t\t--\t--------\n");

    for(i = 0; i < n; i++){
        if(filter_pid != -1 && logs[i].pid != filter_pid)
            continue;

        printf("%d\t%s\t\t%d\t%s\n",
            logs[i].pid,
            logs[i].syscall_name,
            logs[i].fd,
            logs[i].filename);

        if(strcmp(logs[i].syscall_name, "open") == 0) opens++;
        else if(strcmp(logs[i].syscall_name, "read") == 0) reads++;
        else if(strcmp(logs[i].syscall_name, "write") == 0) writes++;
        else if(strcmp(logs[i].syscall_name, "close") == 0) closes++;
    }

    // summary
    printf("\nopen:%d  read:%d  write:%d  close:%d  total:%d\n",
        opens, reads, writes, closes,
        opens + reads + writes + closes);

    // warning if too many ops from one process
    if(filter_pid == -1){
        for(i = 0; i < n; i++){
            int count = 0;
            int j;
            for(j = 0; j < n; j++){
                if(logs[j].pid == logs[i].pid)
                    count++;
            }
            if(count > 15){
                printf("[WARNING] PID %d has %d operations\n",
                    logs[i].pid, count);
                break;
            }
        }
    }

    exit(0);
}
