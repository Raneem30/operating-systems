#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "filelog.h"

struct {
    struct spinlock lock;           
    struct log_entry entries[MAX_LOGS]; 
    int head;   
    int tail;  
    int count; 
} filelog;

void
filelog_init(void)
{
    initlock(&filelog.lock, "filelog");
    filelog.head  = 0;
    filelog.tail  = 0;
    filelog.count = 0;
}

void
filelog_add(int action, char *filename)
{
    struct proc *p = myproc(); 

    acquire(&filelog.lock);    
    struct log_entry *e = &filelog.entries[filelog.tail];

    e->pid    = p->pid;
    e->action = action;
    e->tick   = ticks; 
    safestrcpy(e->proc_name, p->name, MAX_PROCNAME);
    safestrcpy(e->file_name, filename, MAX_FILENAME);
    filelog.tail = (filelog.tail + 1) % MAX_LOGS;

    if (filelog.count < MAX_LOGS) {
        filelog.count++;
    } else {
       
        filelog.head = (filelog.head + 1) % MAX_LOGS;
    }

    release(&filelog.lock);   
}
int
filelog_get(struct log_entry *buf, int max_entries)
{
    acquire(&filelog.lock);

    int n = filelog.count < max_entries ? filelog.count : max_entries;

    for (int i = 0; i < n; i++) {
        int idx = (filelog.head + i) % MAX_LOGS;
        buf[i]  = filelog.entries[idx];
    }

    release(&filelog.lock);

    return n;  
}