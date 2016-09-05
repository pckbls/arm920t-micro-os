#include <stdarg.h>
#include "kernel.h"

static
const char itoh_map[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

/* Konvertiert eine Zahl von 0 bis 15 in die entsprechende Hexadezialdarstellung */
unsigned char itoh(unsigned int i)
{
	if (i < sizeof(itoh_map))
		return itoh_map[i];
	
	return '\0';
}
