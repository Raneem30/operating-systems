#include "types.h"
#include "stat.h"
#include "user.h"

struct log_entry {
    int pid;
    char proc_name[16];
    char file_name[128];
    int action;
    int tick;
};

#define MAX_LOGS 512

char*
action_name(int action)
{
    if(action == 0) return "open";
    if(action == 1) return "read";
    if(action == 2) return "write";
    if(action == 3) return "close";
    return "unknown";
}

int
main(int argc, char *argv[])
{
    struct log_entry logs[MAX_LOGS];
    int n, i;
    int filter_pid = -1;
    int opens = 0, reads = 0, writes = 0, closes = 0;

    if(argc == 2 && strcmp(argv[1], "help") == 0){
        printf("Usage:\n");
        printf("  showlogs       - show all logs\n");
        printf("  showlogs <pid> - show logs for one process\n");
        printf("  showlogs help  - show this message\n");
        exit(0);
    }

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

    printf("PID\tProcess\t\tAction\tTick\tFilename\n");

    for(i = 0; i < n; i++){
        if(filter_pid != -1 && logs[i].pid != filter_pid)
            continue;

        printf("%d\t%s\t\t%s\t%d\t%s\n",
            logs[i].pid,
            logs[i].proc_name,
            action_name(logs[i].action),
            logs[i].tick,
            logs[i].file_name);

        if(logs[i].action == 0) opens++;
        else if(logs[i].action == 1) reads++;
        else if(logs[i].action == 2) writes++;
        else if(logs[i].action == 3) closes++;
    }

    printf("\nopen:%d  read:%d  write:%d  close:%d  total:%d\n",
        opens, reads, writes, closes,
        opens + reads + writes + closes);

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
