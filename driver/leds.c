#include "kernel.h"

#define PIOB 0xfffff600
#define PIOC 0xfffff800

#define PIO_PER 0x00
#define PIO_OER 0x10
#define PIO_SODR 0x30
#define PIO_CODR 0x34

void leds_init(void) __attribute__((section(".init")));

static inline
void write_u32(unsigned int addr, unsigned int val)
{
	*(volatile unsigned int *)addr = val;
}

void leds_init(void)
{
	write_u32(PIOB + PIO_PER, LED_YELLOW);
	write_u32(PIOB + PIO_OER, LED_YELLOW);

	write_u32(PIOC + PIO_PER, LED_RED | LED_GREEN);
	write_u32(PIOC + PIO_OER, LED_RED | LED_GREEN);
}

void leds_on(unsigned int leds)
{
	if (leds & LED_YELLOW)
		write_u32(PIOB + PIO_SODR, LED_YELLOW);
	
	if (leds & (LED_GREEN | LED_RED))
		write_u32(PIOC + PIO_SODR, leds & (LED_GREEN | LED_RED));
}

void leds_off(unsigned int leds)
{
	if (leds & LED_YELLOW)
		write_u32(PIOB + PIO_CODR, LED_YELLOW);

	if (leds & (LED_GREEN | LED_RED))
		write_u32(PIOC + PIO_CODR, leds & (LED_GREEN | LED_RED));
}

