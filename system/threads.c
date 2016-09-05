#include "kernel.h"

void threads_init(void) __attribute__((section(".init")));

#ifdef DEBUG_MESSAGES
static inline
unsigned int get_thread_id(tcb_t *thread)
{
	return thread ? thread->id + 1 : 0;
}
#endif

/* Kontrollinformationen der einzelnen Thread */
tcb_t threads[MAX_THREADS];

/* Zeigt auf den aktuell laufenden Thread oder NULL, wenn grad kein Thread aktiv
 * ist. */
static
tcb_t *current_thread;

/* Ringbuffer-Struktur zur verwaltung der Runqueue.
 * Auch die blockierten Threads befinden sich in dieser und werden nicht
 * gesondert behandelt. Zwar werden I/O-lastige Threads dadurch etwas benachteiligt,
 * aber die Implementierung ist einfach, robust und kein Thread kann "absaufen".
 */
static
tcb_t *threads_ringbuf_data[MAX_THREADS + 1];

static
ringbuffer_t threads_ringbuf;

/* Gibt an, wie viel Zeit noch vergehen muss, bis der Scheduler dem aktuellen
 * Thread die Rechenzeit entzieht. */
static
signed int scheduler_time_left;

/* Gibt an, ob waehrend dieser Ausnahmebehandlung die Scheduling-Routine
 * ausgefuehrt werden soll. */
static
int schedule_request;

void threads_init(void)
{
	for (int i = 0; i < MAX_THREADS; i++)
		threads[i].state = THREAD_NOTACTIVE;

	current_thread = NULL;

	ringbuffer_init(&threads_ringbuf, threads_ringbuf_data, MAX_THREADS + 1, sizeof(tcb_t*));
	
	scheduler_time_left = 0;
	
	schedule_request = 0;
}

void thread_request_schedule(void)
{
	schedule_request = 1;
}

/* Erstellt einen neuen Thread und laesst diesen vom Scheduler verwalten. */
void thread_create(thread_func_t *func, void *args, unsigned int new_process)
{
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (threads[i].state != THREAD_NOTACTIVE)
			continue;
		
		tcb_t *thread = &threads[i];
		
		thread->id    = i;
		thread->state = THREAD_READY;
		thread->cpsr  = 0x60000010;
		
		for (int j = 0; j < 16; j++)
			thread->registers[j] = 0;
		thread->registers[0] = (unsigned int)args;
		
		thread->registers[REGISTER_LR] = (unsigned int)exit;
		thread->registers[REGISTER_PC] = (unsigned int)func;
		
		thread->stack_end = (unsigned int*)(&_end_user_stack - i * THREAD_STACK_SIZE);
		thread->registers[REGISTER_SP] = (unsigned int)thread->stack_end;
		
		if (new_process || !current_thread)
			thread->process = process_create();
		else
			thread->process = current_thread->process;
		
		if (!thread->process)
		{
			DEBUG("Kein Prozess mehr frei...\n");
			return;
		}
		
		process_attach_thread(thread->process);
		
		if (current_thread && thread->process == current_thread->process)
			process_switch_context(thread->process);
		
		ringbuffer_write(&threads_ringbuf, (void*)&thread);
		DEBUG("Thread %x erstellt und vom Scheduler erfasst.\n", get_thread_id(thread));
		
		if (!current_thread)
			thread_request_schedule();
		
		break;
	}
}

/* Wir befinden uns in einer Ausnahmebehandlung und moechten den Thread wechseln. */
static
void thread_switch_context(tcb_t *next_thread, interrupt_info_t *info)
{
	if (current_thread)
	{
		DEBUG("Wechsel Thread von %x zu %x.\n", get_thread_id(current_thread), get_thread_id(next_thread));
		
	    /* Wir sichern den Kontext des aktuellen Threads. */
	    for (int i = 0; i < 16; i++)
		    current_thread->registers[i] = info->u_registers[i];
	
	    current_thread->cpsr = info->spsr;
	}
	else
	{
		DEBUG("Wechsel Thread zu %x.\n", get_thread_id(next_thread));
	}
	
	if (!current_thread || (next_thread && current_thread->process != next_thread->process))
		process_switch_context(next_thread->process);
	
	current_thread = next_thread;
	
	if (next_thread != NULL)
	{
		current_thread->state = THREAD_RUNNING;
		
		for (int i = 0; i < 16; i++)
			info->u_registers[i] = current_thread->registers[i];
		info->spsr = current_thread->cpsr;
	}
	else
	{
		info->u_registers[REGISTER_PC] = (unsigned int)busy_wait;
	}
}

void thread_schedule(interrupt_info_t *info)
{
	if (!schedule_request)
		return;
	
	schedule_request = 0;
	scheduler_time_left = SCHEDULER_INTERVAL + SYSTEM_TIMER_INTERVAL;
	
	if (current_thread && current_thread->state != THREAD_NOTACTIVE)
		ringbuffer_write(&threads_ringbuf, &current_thread);

	tcb_t *next_thread = NULL, *thread;
	for (int i = threads_ringbuf.elements; i > 0; i--)
	{
		ringbuffer_read(&threads_ringbuf, (void*)&thread);

		if (thread->state == THREAD_READY)
		{
		    next_thread = thread;
			break;
		}

		ringbuffer_write(&threads_ringbuf, (void*)&thread);
	}

	if (next_thread == current_thread)
	{
		DEBUG("Aktueller Thread kann weiterlaufen...\n");
		return;
	}
	
	thread_switch_context(next_thread, info);
}

void thread_timer_interrupt(void)
{
	/* Wir aktualisieren die Wartezeiten der Threads, falls
	 * diese sleep aufgerufen haben.
	 * Falls gerade kein Thread laeuft und mindestens ein
	 * Thread aufgeweckt wurde, loesen wir die
	 * Scheduler-Routine aus. */
	tcb_t *thread;
	for (int i = threads_ringbuf.elements; i > 0; i--)
	{
		ringbuffer_read(&threads_ringbuf, (void*)&thread);

		if (thread->wait_for_event == EVENT_TIME)
		{
			thread->wait_time_left -= SYSTEM_TIMER_INTERVAL;
		
			if (thread->wait_time_left <= 0)
			{
				thread->state = THREAD_READY;
				
				if (!current_thread)
					thread_request_schedule();
			}
		}

		ringbuffer_write(&threads_ringbuf, (void*)&thread);
	}
	
	scheduler_time_left -= SYSTEM_TIMER_INTERVAL;
	if (scheduler_time_left > 0)
		return;
	
	if (current_thread && current_thread->state == THREAD_RUNNING)
	{
		DEBUG("Zeitscheibe von Thread %x ist abgelaufen.\n", get_thread_id(current_thread));
		current_thread->state = THREAD_READY;
	}
	
	thread_request_schedule();
}

/* Wird aufgerufen, wenn sich ein Thread ordnungsgemaess beendet hat. */
void thread_exit(void)
{
	DEBUG("Thread %x hat sich beendet fertig.\n", get_thread_id(current_thread));
	
	current_thread->state = THREAD_NOTACTIVE;
	process_detatch_thread(current_thread->process);
	
	thread_request_schedule();
}

/* Wird aufgerufen, wenn ein Thread block() benutzt hat. */
void thread_block(thread_event_t event)
{
	DEBUG("Thread %x wartet auf ein Event.\n", get_thread_id(current_thread));
	
	current_thread->wait_for_event = event;
	current_thread->state = THREAD_WAITING;
	
	thread_request_schedule();
}

/* Wird aufgerufen, wenn alle Threads, die auf ein Event warten
 * aufgeweckt werden sollen. */
void thread_unblock(thread_event_t event)
{
	tcb_t *thread;
	for (int i = threads_ringbuf.elements; i > 0; i--)
	{
		ringbuffer_read(&threads_ringbuf, (void*)&thread);

		if (thread->state == THREAD_WAITING && thread->wait_for_event == event)
		{
			DEBUG("Thread %x kann nach einem block() weiterarbeiten.\n", get_thread_id(thread));
			thread->state = THREAD_READY;

			if (!current_thread)
				thread_request_schedule();
		}

		ringbuffer_write(&threads_ringbuf, (void*)&thread);
	}
}

void thread_sleep(unsigned int time)
{
	current_thread->wait_time_left = time;
	thread_block(EVENT_TIME);
}

void thread_kill_current(void)
{
	if (!current_thread)
		return;
	
	DEBUG("Aktueller Thread wurde getoetet.\n");
	
	current_thread->state = THREAD_NOTACTIVE;
	process_detatch_thread(current_thread->process);
	
	thread_request_schedule();
}
