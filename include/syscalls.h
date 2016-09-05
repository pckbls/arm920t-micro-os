#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#define SWI_CHAR_READ     0
#define SWI_CHAR_WRITE    1
#define SWI_CREATE_THREAD 2
#define SWI_EXIT          3
#define SWI_BLOCK         4
#define SWI_SLEEP         5

#ifndef __ASSEMBLER__
#include "threads.h"

void syscall_char_read(interrupt_info_t *info);
void syscall_char_write(interrupt_info_t *info);
void syscall_create_thread(interrupt_info_t *info);
void syscall_exit(interrupt_info_t *info);
void syscall_block(interrupt_info_t *info);
void syscall_sleep(interrupt_info_t *info);

#endif

#endif
