#include "kernel.h"

pcb_t processes[MAX_PROCESSES];

extern tcb_t threads[MAX_THREADS];

void process_init(void)
{
	for (unsigned int i = 0; i < MAX_PROCESSES; i++)
	{
		pcb_t *pcb = &processes[i];
		
		pcb->phys_addr = (unsigned int *)(&_start_data_sections + i * 0x100000);
	}
}

pcb_t *process_create(void)
{
	for (unsigned int i = 0; i < MAX_PROCESSES; i++)
	{
		if (processes[i].ref_count == 0)
		{
			pcb_t *pcb = &processes[i];
			
			memcpy(pcb->phys_addr, (void*)&_start_user, &_end_user - &_start_user);
			
			return pcb;
		}
	}
	
	return NULL;
}

void process_attach_thread(pcb_t *process)
{
	if (!process)
		return;
	
	process->ref_count++;
}

void process_detatch_thread(pcb_t *process)
{
	if (!process)
		return;
	
	if (process->ref_count > 0)
		process->ref_count--;
}

void process_switch_context(pcb_t *pcb)
{	
	DEBUG("Wechsel Prozess-Kontext\n");
	
	cp15_invalidate_tlb();
	
	mmu_l1_section((unsigned int)&_start_user_virt, (unsigned int)pcb->phys_addr, PERM_USER_READ_WRITE);
	
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (threads[i].state == THREAD_NOTACTIVE || threads[i].process != pcb)
		{
			unsigned int stack_end = (unsigned int)(&_end_user_stack - i * THREAD_STACK_SIZE - 1);
			DEBUG("Sperre "); mmu_l1_section(stack_end, stack_end, PERM_KERNEL_READ_WRITE);
		}
		else
		{
			unsigned int stack_end = (unsigned int)threads[i].stack_end - 1;
			DEBUG("Freigebe "); mmu_l1_section(stack_end, stack_end, PERM_USER_READ_WRITE);
		}
	}
}
