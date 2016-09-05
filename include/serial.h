#ifndef SERIAL_H_
#define SERIAL_H_

#define DBGU 0xFFFFF200

#define RXRDY (1<<0)
#define TXRDY (1<<1)

typedef struct
{
	unsigned int cr;  // DBGU_CR  0x0000
	char gap_1[0x4];
	unsigned int ier; // DBGU_IER 0x0008
	char gap_2[0x8];
	unsigned int sr;  // DBGU_SR  0x0014
	unsigned int rhr; // DBGU_RHR 0x0018
	unsigned int thr; // DBGU_THR 0x001C
} dbgu_t;

void serial_init(void);
unsigned char serial_rx(void);
unsigned int serial_rx_nonblocking(char *c);
void serial_tx(unsigned char c);
void serial_printf(char *fmt, ...);
void serial_interrupt(void);

#endif