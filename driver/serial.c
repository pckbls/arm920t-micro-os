#include <stdarg.h>
#include "kernel.h"

void serial_init(void) __attribute__((section(".init")));

static volatile
dbgu_t * const dbgu = (dbgu_t *)DBGU;

/* Eingabe-Ringbuffer */
unsigned int ringbuf_data[DBGU_RINGBUFFER_SIZE];
ringbuffer_t ringbuf;

void serial_init(void)
{
	ringbuffer_init(&ringbuf, ringbuf_data, DBGU_RINGBUFFER_SIZE, sizeof(unsigned int));
	
	dbgu->ier |= (1<<0);
}

void serial_printf(char *fmt, ...)
{
	va_list ap;
	
	unsigned int ui;
	char c, *s;

	va_start(ap, fmt);

	while (*fmt)
	{
		if (*fmt == '%' && *(fmt + 1))
		{
			fmt++;
			switch (*fmt)
			{
				case '%':
					serial_tx('%');
				break;
			
				case 'c':
					c = (unsigned char)va_arg(ap, int);
					serial_tx(c);
				break;
			
				case 's':
					for (s = va_arg(ap, char*); *s; s++)
						serial_tx(*s);
				break;
				
				/* Da auf der Zielplattform sowohl unsigned ints als auch
				 * void*-Pointer 4 Bytes an Speicher belegen, können die beiden
				 * Datentypen gleich behandelt werden.
				 */
				case 'p':
				case 'x':
					ui = va_arg(ap, unsigned int);
					
					serial_tx('0'); serial_tx('x');
					
					/* Wir extrahieren jeweils 4-Bit aus dem 32-bit-großem
					 * Datum und konvertieren diese in Hexadezimaldarstellung.
					 */
					for (int i = 7; i >= 0; i--)
					{
						/* Die Umrechnung sollte nur im Big-Endian-Modus
						 * funktionieren.
						 */
						unsigned int temp = (ui & (0xF << i*4)) >> i*4;
						serial_tx(itoh(temp));
					}
				break;

				default:
					serial_tx('%');
					serial_tx(*fmt);
				break;
			}
		}
		else
			serial_tx(*fmt);

		fmt++;
	}

	va_end(ap);
}

/* Wartet (interrupt-gesteuert) bis ein Zeichen von der seriellen Schnittstelle empfangen
 * werden kann und gibt dieses zurück.
 */
unsigned char serial_rx(void)
{
	unsigned int data;
	
	while (!ringbuffer_read(&ringbuf, &data));
	
	return (unsigned char)data;
}

unsigned int serial_rx_nonblocking(char *c)
{
	unsigned int data;
	
	if (!ringbuffer_read(&ringbuf, &data))
		return 0;
	
	*c = (unsigned char)data;
	
	return 1;
}

/* Sendet ein Zeichen auf die serielle Schnitstelle sobald diese dazu bereit
 * ist.
 */
void serial_tx(unsigned char c)
{
	while (!(dbgu->sr & TXRDY));
	
	dbgu->thr = (int)c;
}

void serial_interrupt(void)
{
	if (dbgu->sr & (1<<0))
	{
		unsigned int rhr = dbgu->rhr;
		ringbuffer_write(&ringbuf, &rhr);
		
		/* Wir wecken hier alle Threads auf, die auf ein Zeichen der
		 * seriellen Schnittstelle warten.
		 */
		thread_unblock(EVENT_SERIAL_IO);
	}
}
