#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#ifndef MLQ_SCHED
#define MLQ_SCHED
#endif

#define MAX_PRIO 140

int queue_empty(void);
int period(void);

void init_scheduler(void);
void finish_scheduler(void);

/* Get the next process from ready queue */
struct pcb_t * get_proc(void);

/* Put a process back to run queue */
void put_proc(struct pcb_t * proc);

/* Add a new process to ready queue */
void add_proc(struct pcb_t * proc);
void printRunList(void); //for debug purpose
void enqueue_running(struct pcb_t * proc);
void dequeue_running();
#endif
