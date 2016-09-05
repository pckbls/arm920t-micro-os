#ifndef LIB_H_
#define LIB_H_

#include "threads.h"

char char_read(void);

void char_write(char c);

void printf(char *fmt, ...);

char create_thread(thread_func_t *func, void *args, unsigned int new_process);

void exit(void);

void sleep(unsigned int mseconds);

void block(thread_event_t event);

#endif
