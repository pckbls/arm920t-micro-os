#ifndef THREADS_H_
#define THREADS_H_

#include "config.h"
#include "exceptions.h"
#include "process.h"

typedef enum
{
	THREAD_READY,
	THREAD_RUNNING,
	THREAD_WAITING,
	THREAD_NOTACTIVE
} thread_state_t;

typedef enum
{
	EVENT_SERIAL_IO,
	EVENT_TIME
} thread_event_t;

typedef struct
{
	/* Die ID (Wert von 0 bis MAX_THREADS-1) des Threads */
	unsigned int id;
	
	/* Status des Threads */
	thread_state_t state;
	
	/* Argumente, die dem Thread uebergeben werden. */
	void *args;
	
	unsigned int registers[16];
	
	unsigned int cpsr;

	/* Falls ein Thread blockiert, wird hier das Event gespeichert,
	 * das ihn wieder aufwachen laesst. */
	thread_event_t wait_for_event;
	
	/* Falls ein Thread auf EVENT_TIME wartet wird hier die Zeit
	 * in ms gespeichert, die noch vergehen muss, bis er aufgeweckt
	 * wird. */
	signed int wait_time_left;
	
	pcb_t *process;
	
	unsigned int *stack_end;
} tcb_t;

typedef void (thread_func_t)(void *);

void threads_init(void);
void thread_create(thread_func_t *func, void *args, unsigned int new_process);
void thread_timer_interrupt(void);
void thread_exit(void);
void thread_block(thread_event_t event);
void thread_unblock(thread_event_t event);
void thread_sleep(unsigned int time);
void thread_schedule(interrupt_info_t *info);
void thread_kill_current(void);

#endif
