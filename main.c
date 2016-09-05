#include "lib.h"

#define UNUSED(x) (void)(x)

#define MAX_COUNTER 12

static int counter;
static char c;

void worker_func2(void *arg)
{
	unsigned int local_counter = 0;
	while (counter < MAX_COUNTER)
	{
		counter++;
		local_counter++;
		
		printf("%c%c: %x (%x)\n", c, (char)arg, counter, local_counter);
		
		sleep(100);
	}
}

void worker_func(void *arg)
{
	counter = 0;
	c = (char)arg;
	
	create_thread(worker_func2, (void*)'1', 0);
	create_thread(worker_func2, (void*)'2', 0);
	worker_func2((void*)'3');
}

void main(void* arg)
{
	UNUSED(arg);
	
	printf("Running main()...\n\n");
	
	while (1)
	{
		char c = char_read();
		
		create_thread(worker_func, (void*)c, 1);
	}
}
