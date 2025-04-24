/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"

#include "string.h" // for strcmp
#include "queue.h" // for queue
#include <stdlib.h> // for free

void remove_matching_processes(struct queue_t *queue, char* proc_name, uint32_t memrg) {
    if (!queue) return;

    struct queue_t temp_queue = { .size = 0 };  
    while (queue->size > 0) {
        struct pcb_t *proc = dequeue(queue);
        char name[100];
        int i = 0;
        uint32_t data = 0;
       
        while (data != -1) {
            libread(proc, memrg, i, &data);
            name[i] = data;
            if (data == -1) name[i] = '\0'; 
            i++;
        }
        
        if (strcmp(name, proc_name) == 0) {
            for (int ind=0; ind<10; ind++) {
                libfree(proc, ind);  
            }
            free(proc);  
        } else {
            enqueue(&temp_queue, proc);  
        }
    }

    while (temp_queue.size > 0) {
        enqueue(queue, dequeue(&temp_queue));
    }
}

void remove_matching_running_processes(struct pcb_t *caller, char* proc_name, uint32_t memrg) {
    struct queue_t *queue = caller->running_list;
    if (!queue) return;

    struct queue_t temp_queue = { .size = 0 }; 
    while (queue->size > 0) {
        struct pcb_t *proc = dequeue(queue);
        if (proc->pid == caller->pid) {
            enqueue(&temp_queue, proc);  
            continue;
        }
        char name[100];
        int i = 0;
        uint32_t data = 0;
        
        while (data != -1) {
            libread(proc, memrg, i, &data);
            name[i] = data;
            if (data == -1) name[i] = '\0';
            i++;
        }

        if (strcmp(name, proc_name) == 0) {
            proc->pc = proc->code->size; 
        } else {
            enqueue(&temp_queue, proc); 
        }
    }

    while (temp_queue.size > 0) {
        struct pcb_t *proc = dequeue(&temp_queue);
        if (proc->pid!=caller->pid)
            enqueue(queue, proc);
    }
    caller->pc = caller->code->size;
}

int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    uint32_t data;

    uint32_t memrg = regs->a1;
    
    /* Get the name of the target process */
    int i = 0;
    data = 0;
    while (data != -1) {
        libread(caller, memrg, i, &data);
        proc_name[i] = data;
        if (data == -1) proc_name[i] = '\0';
        i++;
    }
    
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    for (int prio=0; prio < 140; prio++) {
        struct queue_t *queue = &caller->mlq_ready_queue[prio];
        if (queue == NULL || queue->size == 0) continue;
        remove_matching_processes(queue, proc_name, memrg);
    }
    remove_matching_running_processes(caller, proc_name, memrg);

    return 0;
}
