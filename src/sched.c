#include "queue.h"
 #include "sched.h"
 #include <pthread.h>
 
 #include <stdlib.h>
 #include <stdio.h>
 static struct queue_t ready_queue;
 static struct queue_t run_queue;
 static pthread_mutex_t queue_lock;
 
 static struct queue_t running_list;
 #ifdef MLQ_SCHED
 static struct queue_t mlq_ready_queue[MAX_PRIO];
 static int slot[MAX_PRIO];
 #endif
 
 extern int time_slot;
 int queue_empty(void) {
 #ifdef MLQ_SCHED
 	unsigned long prio;#include "queue.h"
 #include "sched.h"
 #include <pthread.h>
 
 #include <stdlib.h>
 #include <stdio.h>
 static struct queue_t ready_queue;
 static struct queue_t run_queue;
 static pthread_mutex_t queue_lock;
 
 static struct queue_t running_list;
 #ifdef MLQ_SCHED
 static struct queue_t mlq_ready_queue[MAX_PRIO];
 static int slot[MAX_PRIO];
 #endif
 
 extern int time_slot;
 int queue_empty(void) {
 #ifdef MLQ_SCHED
 	unsigned long prio;
 	for (prio = 0; prio < MAX_PRIO; prio++)
 		if(!empty(&mlq_ready_queue[prio])) 
 			return -1;
 #endif
 	return (empty(&ready_queue) && empty(&run_queue));
 }
 
 void init_scheduler(void) {
 #ifdef MLQ_SCHED
     int i;
 
 	for (i = 0; i < MAX_PRIO; i ++) {
 		mlq_ready_queue[i].size = 0;
 		slot[i] = MAX_PRIO - i; 
 	}
 #endif
 	ready_queue.size = 0;
 	run_queue.size = 0;
	running_list.size = 0;
 	pthread_mutex_init(&queue_lock, NULL);
 }
 
 #ifdef MLQ_SCHED

 int period(void) {
	for (int i = MAX_PRIO-1; i >= 0; i--) 
        if (!empty(&mlq_ready_queue[i])) {
			if (slot[i] <= 0) {
				return 1;
			} else return 0;
		}
    return 0;
 }

 /* 
  *  Stateful design for routine calling
  *  based on the priority and our MLQ policy
  *  We implement stateful here using transition technique
  *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
  */
 struct pcb_t * get_mlq_proc(void) {
 	struct pcb_t * proc = NULL;
 	/*TODO: get a process from PRIORITY [ready_queue].
 	 * Remember to use lock to protect the queue.
 	 * */
 	/*Lock process queue for safe access*/	
 	pthread_mutex_lock(&queue_lock);
	if( period() ) //Check if a period has passed
		for (int i = 0; i < MAX_PRIO; i++) 
			if (!empty(&mlq_ready_queue[i]))
				slot[i] = MAX_PRIO - i;

 	for (int i=0; i<MAX_PRIO; i++)
 		if (!empty(&mlq_ready_queue[i]) && slot[i] > 0) {
 			proc = dequeue(&mlq_ready_queue[i]);
 			slot[i] -= time_slot;
 			break;
 		}
 	pthread_mutex_unlock(&queue_lock);
 	return proc;	
 }
 
 void put_mlq_proc(struct pcb_t * proc) {
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&mlq_ready_queue[proc->prio], proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void add_mlq_proc(struct pcb_t * proc) {
 	proc->priority = proc->prio;
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&mlq_ready_queue[proc->prio], proc);
 	pthread_mutex_unlock(&queue_lock);	
 }
 
 struct pcb_t * get_proc(void) {
 	return get_mlq_proc();
 }
 
 void put_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->mlq_ready_queue = mlq_ready_queue;
 	proc->running_list = & running_list;
 	/* TODO: put running proc to running_list */
 	return put_mlq_proc(proc);
 }
 
 void add_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->mlq_ready_queue = mlq_ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 	return add_mlq_proc(proc);
 }
 //dequeue from running_list
 struct pcb_t* dequeue_running(void) {
 	pthread_mutex_lock(&queue_lock);
 	struct pcb_t* tmp = dequeue(&running_list);
 	pthread_mutex_unlock(&queue_lock);
	return tmp;
 }
 //enqueue to running list
 void enqueue_running(struct pcb_t * proc) {
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&running_list, proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void printRunList(void) {
 	struct pcb_t * proc = NULL;
 	pthread_mutex_lock(&queue_lock);
	printf("\t\t\t\\\\\\\\--------------Size:||-%d-||--------------////\n", running_list.size);
 	for (int i = 0; i < running_list.size; i++) {
 		proc = dequeue(&running_list);
 		printf("\t\t\t\tProcess PID: |||----%d----||\n", proc->pid);
 		enqueue(&running_list, proc); // Re-add process to the list
 	}
	printf("\t\t\t\\\\\\\\----------------------------------------////\n");
 	pthread_mutex_unlock(&queue_lock);
 	return;
 } //for debug purpose
 #else
 struct pcb_t * get_proc(void) {
 	struct pcb_t * proc = NULL;
 	/*TODO: get a process from [ready_queue].
 	 * Remember to use lock to protect the queue.
 	 * */
 	return proc;
 }
 
 void put_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&run_queue, proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void add_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 pthread_mutex_lock(&queue_lock);
 	enqueue(&ready_queue, proc);
 	pthread_mutex_unlock(&queue_lock);	
 }
 #endif
 	for (prio = 0; prio < MAX_PRIO; prio++)
 		if(!empty(&mlq_ready_queue[prio])) 
 			return -1;
 #endif
 	return (empty(&ready_queue) && empty(&run_queue));
 }
 
 void init_scheduler(void) {
 #ifdef MLQ_SCHED
     int i;
 
 	for (i = 0; i < MAX_PRIO; i ++) {
 		mlq_ready_queue[i].size = 0;
 		slot[i] = MAX_PRIO - i; 
 	}
 #endif
 	ready_queue.size = 0;
 	run_queue.size = 0;
	running_list.size = 0;
 	pthread_mutex_init(&queue_lock, NULL);
 }
 
 #ifdef MLQ_SCHED
 /* 
  *  Stateful design for routine calling
  *  based on the priority and our MLQ policy
  *  We implement stateful here using transition technique
  *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
  */
 struct pcb_t * get_mlq_proc(void) {
 	struct pcb_t * proc = NULL;
 	/*TODO: get a process from PRIORITY [ready_queue].
 	 * Remember to use lock to protect the queue.
 	 * */
 
 	/*Lock process queue for safe access*/
 	pthread_mutex_lock(&queue_lock);
 	for (int i=0; i<MAX_PRIO; i++) 
 		if (!empty(&mlq_ready_queue[i]) && slot[i] > 0) {
 			proc = dequeue(&mlq_ready_queue[i]);
 			if (proc != NULL){
				slot[i] -= time_slot;
			}
 			break;
 		}
 	pthread_mutex_unlock(&queue_lock);
 	return proc;	
 }
 
 void put_mlq_proc(struct pcb_t * proc) {
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&mlq_ready_queue[proc->prio], proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void add_mlq_proc(struct pcb_t * proc) {
 	proc->priority = proc->prio;
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&mlq_ready_queue[proc->prio], proc);
 	pthread_mutex_unlock(&queue_lock);	
 }
 
 struct pcb_t * get_proc(void) {
 	return get_mlq_proc();
 }
 
 void put_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->mlq_ready_queue = mlq_ready_queue;
 	proc->running_list = & running_list;
 	/* TODO: put running proc to running_list */
 	return put_mlq_proc(proc);
 }
 
 void add_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->mlq_ready_queue = mlq_ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 	return add_mlq_proc(proc);
 }
 //dequeue from running_list
 struct pcb_t* dequeue_running(void) {
 	pthread_mutex_lock(&queue_lock);
 	struct pcb_t* tmp = dequeue(&running_list);
 	pthread_mutex_unlock(&queue_lock);
	return tmp;
 }
 //enqueue to running list
 void enqueue_running(struct pcb_t * proc) {
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&running_list, proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void printRunList(void) {
 	struct pcb_t * proc = NULL;
 	pthread_mutex_lock(&queue_lock);
	printf("\t\t\t\\\\\\\\--------------Size:||-%d-||--------------////\n", running_list.size);
 	for (int i = 0; i < running_list.size; i++) {
 		proc = dequeue(&running_list);
 		printf("\t\t\t\tProcess PID: |||----%d----||\n", proc->pid);
 		enqueue(&running_list, proc); // Re-add process to the list
 	}
	printf("\t\t\t\\\\\\\\----------------------------------------////\n");
 	pthread_mutex_unlock(&queue_lock);
 	return;
 } //for debug purpose
 #else
 struct pcb_t * get_proc(void) {
 	struct pcb_t * proc = NULL;
 	/*TODO: get a process from [ready_queue].
 	 * Remember to use lock to protect the queue.
 	 * */
 	return proc;
 }
 
 void put_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 
 	pthread_mutex_lock(&queue_lock);
 	enqueue(&run_queue, proc);
 	pthread_mutex_unlock(&queue_lock);
 }
 
 void add_proc(struct pcb_t * proc) {
 	proc->ready_queue = &ready_queue;
 	proc->running_list = & running_list;
 
 	/* TODO: put running proc to running_list */
 pthread_mutex_lock(&queue_lock);
 	enqueue(&ready_queue, proc);
 	pthread_mutex_unlock(&queue_lock);	
 }
 #endif
