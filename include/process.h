#ifndef PROCESS_H_
#define PROCESS_H_

#include "threads.h"

typedef struct
{
	unsigned int ref_count;
	unsigned int *phys_addr;
} pcb_t;

pcb_t *process_create(void);
void process_attach_thread(pcb_t *process);
void process_detatch_thread(pcb_t *process);
void process_switch_context(pcb_t *pcb);

#endif
