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
 
 #include "queue.h"
 #include "string.h"
 
 int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
 {
     char proc_name[100];
     uint32_t data;
 
     //hardcode for demo only
     uint32_t memrg = regs->a1;
     
     /* TODO: Get name of the target proc */
     //proc_name = libread..
     int i = 0;
     data = 0;
     while(data != -1){
         libread(caller, memrg, i, &data);
         proc_name[i]= data;
         if(data == -1) proc_name[i]='\0';
         i++;
     }
     printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);
 
     /* TODO: Traverse proclist to terminate the proc
      *       stcmp to check the process match proc_name
      */
     //caller->running_list
     //caller->mlq_ready_queu
 
     struct queue_t* list = caller->mlq_ready_queue;
     for (int i=0; i<list->size; i++) {
         uint32_t d;
         int t=0;
         while (d!=-1) {
             libread(list->proc[i], 1, t, &d);
             list->proc[i]->name[t] = d;
             if (d==-1) list->proc[i]->name[t] = '\0';
             t++; 
         }
         if (strcmp(proc_name, list->proc[i]->name) == 0) {
             //remove proc[i] from list
             for (int j=i; j<list->size; j++) {
                 list->proc[j] = list->proc[j+1];
             }
             list->size--;
         }
         i--;//reserve for i++ later
     }
 
     if (caller->pc < caller->code->size) {
         caller->pc = caller->code->size;
     }
 
     /* TODO Maching and terminating 
      *       all processes with given
      *        name in var proc_name
      */
 
     return 0; 
 }
 