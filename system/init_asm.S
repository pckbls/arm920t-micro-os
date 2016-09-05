#include "config.h"
#include "cpu.h"

.section .init

.extern _end_fiq_stack
.extern _end_irq_stack
.extern _end_svc_stack
.extern _end_und_stack
.extern _end_abt_stack

.extern _end_user_stack

.global _init
_init:
	bl leds_init
	bl stacks_init
	bl exceptions_init
	bl memory_init
	bl process_init
	bl threads_init
	bl st_init
	bl serial_init
	
	ldr r0, =main
	mov r1, #0
	bl thread_create
	
	bl enable_exceptions

	/* Diese Stelle sollten wir niemals erreichen, da durch die erste Exception
	 * automatisch der erste Kontextwechsel durchgeführt wird.
	 */
	b busy_wait

/* Initialisiert die Stacks */
.global stacks_init
stacks_init:
	 /* Sichert die Rücksprungadresse aus dem Supervisor-Mode, in dem wir uns
	  * anfangs befinden, damit diese später im System-Mode weiterverwendet
	  * werden kann. */
	mov r3, lr

	mov r2, pc
	mrs r0, cpsr
	
	/* FIQ Modus */
	bic r0, #31
	orr r0, #PSR_FIQ
	msr cpsr_c, r0
	ldr sp, =_end_fiq_stack
	
	/* IRQ Modus */
	bic r0, #31
	orr r0, #PSR_IRQ
	msr cpsr_c, r0
	ldr sp, =_end_irq_stack
	
	/* Undefined Modus */
	bic r0, #31
	orr r0, #PSR_UND
	msr cpsr_c, r0
	ldr sp, =_end_und_stack
	
	/* Abort Modus */
	bic r0, #31
	orr r0, #PSR_ABT
	msr cpsr_c, r0
	ldr sp, =_end_abt_stack
	
	/* Supervisor Modus */
	bic r0, #31
	orr r0, #PSR_SVC
	msr cpsr_c, r0
	ldr sp, =_end_svc_stack
	
	/* System/User Modus */
	bic r0, #31
	orr r0, #PSR_SYS
	msr cpsr_c, r0
	ldr sp, =_end_user_stack
	
	bx r3
