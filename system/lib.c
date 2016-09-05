#include <stdarg.h>
#include "lib.h"

int _char_read(char *c);

char char_read(void)
{
	char c;

	/* Wir warten so lang, bis wir ein Zeichen
	 * aus der seriellen Schnittstelle auslesen koennen.
	 * Falls mehrere Threads auf ein Zeichen warten,
	 * bekommt der Thread das Zeichen, der als erstes
	 * vom Scheduler erfasst wird. */
	while (!_char_read(&c))
		block(EVENT_SERIAL_IO);
	
	return c;
}

static
const char itoh_map[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

/* Konvertiert eine Zahl von 0 bis 15 in die entsprechende Hexadezialdarstellung */
static
unsigned char itoh(unsigned int i)
{
	if (i < sizeof(itoh_map))
		return itoh_map[i];
	
	return '\0';
}


void printf(char *fmt, ...)
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
					char_write('%');
				break;
					
				case 'c':
					c = (unsigned char)va_arg(ap, int);
					char_write(c);
				break;
					
				case 's':
					for (s = va_arg(ap, char*); *s; s++)
						char_write(*s);
				break;
					
				/* Da auf der Zielplattform sowohl unsigned ints als auch
				 * void*-Pointer 4 Bytes an Speicher belegen, können die beiden
				 * Datentypen gleich behandelt werden.
				 */
				case 'p':
				case 'x':
					ui = va_arg(ap, unsigned int);
					
					char_write('0'); char_write('x');
					
					/* Wir extrahieren jeweils 4-Bit aus dem 32-bit-großem
					 * Datum und konvertieren diese in Hexadezimaldarstellung.
					 */
					for (int i = 7; i >= 0; i--)
					{
						/* Die Umrechnung sollte nur im Big-Endian-Modus
						 * funktionieren.
						 */
						unsigned int temp = (ui & (0xF << i*4)) >> i*4;
						char_write(itoh(temp));
					}
				break;
					
				default:
					char_write('%');
					char_write(*fmt);
				break;
			}
		}
		else
			char_write(*fmt);
		
		fmt++;
	}
	
	va_end(ap);
}
