#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        if (q->proc[0] == NULL){
                q->proc[0] = proc;
                q->size = 1;
        }
        else {
                q->proc[q->size] = proc;
                q->size += 1;
        }
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        struct pcb_t* temp = q->proc[0];
        for (int i = 1; i < q->size; i++) q[i-1] = q[i];
        q->size--;
	return temp;
}