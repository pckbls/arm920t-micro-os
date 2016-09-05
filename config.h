/* Zeit in ms, nach der ein Systemtimer-Interrupt ausgeloest wird */
#define SYSTEM_TIMER_INTERVAL 10

/* Gibt die Zeitscheibenlaenge der Threads in ms an */
#define SCHEDULER_INTERVAL 100

/* Groesse des Eingabe-Puffers der DBGU */
#define DBGU_RINGBUFFER_SIZE 16

/* Anzahl der möglichen Prozess */
#define MAX_PROCESSES 2

/* Anzahl der möglichen Threads */
#define MAX_THREADS 32

/* Größe des Stacks für einen einzelnen Thread in Byte */
#define THREAD_STACK_SIZE 0x100000

/* Debug-Meldungen aktivieren */
#define DEBUG_MESSAGES
