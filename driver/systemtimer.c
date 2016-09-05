#include "kernel.h"

#define SYSTEM_TIMER_PIMR (65536 * SYSTEM_TIMER_INTERVAL / 2000)

#define PITS (1<<0)

void st_init(void) __attribute__((section(".init")));

static volatile
st_t * const st = (st_t *)ST;

void st_init(void)
{
	st_write_pimr(SYSTEM_TIMER_PIMR);
	st_enable_pit();
}

void st_enable_pit(void)
{
	st->ier |= PITS;
}

void st_disable_pit(void)
{
	st->idr |= PITS;
}

void st_write_pimr(unsigned int x)
{
	st->pimr = x;
}

unsigned int st_read_pimr(void)
{
	return st->pimr;
}

void st_interrupt()
{
	if (st->sr & PITS)
		thread_timer_interrupt();
}
