#include "syscalls.h"

.global busy_wait
busy_wait:
	b busy_wait

.global _char_read
_char_read:
	swi SWI_CHAR_READ
	bx lr

.global char_write
char_write:
	swi SWI_CHAR_WRITE
	bx lr

.global create_thread
create_thread:
	swi SWI_CREATE_THREAD
	bx lr

.global exit
exit:
	swi SWI_EXIT
	bx lr

.global block
block:
	swi SWI_BLOCK
	bx lr
	
.global sleep
sleep:
	swi SWI_SLEEP
	bx lr