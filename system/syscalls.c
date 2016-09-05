#include "kernel.h"

void syscall_char_read(interrupt_info_t *info)
{
	unsigned char *dest = (unsigned char*)info->u_registers[0];
	
	char c;
	if (serial_rx_nonblocking(&c))
	{
		*dest = (unsigned char)c;
		info->u_registers[0] = 1;
	}
	else
	{
		info->u_registers[0] = 0;
	}
}

void syscall_char_write(interrupt_info_t *info)
{
	unsigned char c = (unsigned char)info->u_registers[0];
	serial_tx(c);
}

void syscall_create_thread(interrupt_info_t *info)
{
	thread_create((thread_func_t*)info->u_registers[0], (void*)info->u_registers[1], info->u_registers[2]);
}

void syscall_exit(interrupt_info_t *info)
{
	/* Hier könnte in Zukunft noch etwas mit dem Rückgabewert des Threads
	 * angestellt werden.
	 */
	UNUSED(info);
	
	thread_exit();
}

void syscall_block(interrupt_info_t *info)
{
	thread_event_t event = info->u_registers[0];
	
	thread_block(event);
}

void syscall_sleep(interrupt_info_t *info)
{
	unsigned int mseconds = info->u_registers[0];
	
	/* Damit die geforderte Wartezeit wirklich nicht unterschritten wird,
	 * muessen wir noch eine Systemtimer-Periode daraufrechnen. */
	mseconds += SYSTEM_TIMER_INTERVAL;
	
	thread_sleep(mseconds);
}