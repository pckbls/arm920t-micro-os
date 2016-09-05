#ifndef SYSTEMTIMER_H_
#define SYSTEMTIMER_H_

#include "exceptions.h"

#define ST 0xFFFFFD00

typedef struct
{
	char gap_1[0x04];
	unsigned int pimr;
	char gap_2[0x08];
	unsigned int sr;
	unsigned int ier;
	unsigned int idr;
} st_t;

void st_init(void);
void st_interrupt();
void st_write_pimr(unsigned int x);
void st_enable_pit(void);
void st_disable_pit(void);

#endif
