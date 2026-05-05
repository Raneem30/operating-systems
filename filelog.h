// kernel/filelog.h

#ifndef FILELOG_H
#define FILELOG_H

#define MAX_LOGS [512]    
#define MAX_FILENAME [128]  
#define MAX_PROCNAME [16]

#define ACTION_OPEN  [0]
#define ACTION_READ  [1]
#define ACTION_WRITE [2]
#define ACTION_CLOSE [3]

struct log_entry {
    int  pid;                    
    char proc_name[MAX_PROCNAME]; 
    char file_name[MAX_FILENAME]; 
    int  action;                 
    int  tick;                   
};

#endif