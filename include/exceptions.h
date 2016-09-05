#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#ifndef __ASSEMBLER__
/* Struktur, die bei einer Ausnahme mit Daten gefuellt wird.
 * u_registers und spsr werden am Ende einer Exception-Behandlung
 * zurueckgeschrieben.
 *
 * Diese wird zur Threadverwaltung durchgereicht, welche
 * dort durch Veraenderung der Struktur einen Kontextwechsel
 * durchfuehren kann.
 */
typedef struct
{
	/* Enthaelt die Usermode-Register. */
	unsigned int u_registers[16];

	unsigned int cpsr;
	unsigned int spsr;
} interrupt_info_t;

void exceptions_init(void);
#endif

#endif
