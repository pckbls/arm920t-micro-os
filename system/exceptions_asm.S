/* Instruktion, die fuer die Interrupt Vector Table bestimmt ist */
.global ivt_command
ivt_command:
	ldr	pc, [pc, #0x18]

/* Aktiviert Interrupts an der CPU */
.global enable_exceptions
enable_exceptions:
	mrs r0, cpsr
    bic r0, r0, #(1<<7)
	msr cpsr_c, r0
	
	bx lr

/* Die "Trampoline" sichern den Stack und übergeben alle relevanten
 * Interruptinformationen an unsere C-Interrupt-Handler. 
 * Schließlich wird das Programm mit einer Busy-Wait-Schleife abgebrochen. */
.global _unhandled_trampoline
_unhandled_trampoline:
	bl unhandled_handler

	b busy_wait

.global _swi_trampoline
_swi_trampoline:
	stmfd	sp!, {lr}
	
	sub	sp, #(15*4)
	stmia sp, {r0-r14}^
	
	mov r0, sp
	
	bl swi_handler
	
 	ldmia sp, {r0-r14}^
 	nop
 	add sp, sp, #(15*4)
 
 	ldmfd sp!, {pc}^

.global _data_abort_trampoline
_data_abort_trampoline:
	sub lr, lr, #8 /* Pipe-Ausgleich */
	
	stmfd	sp!, {lr}
	
	sub	sp, #(15*4)
	stmia sp, {r0-r14}^
	
	mov r0, sp
	
	bl data_abort_handler
	
 	ldmia sp, {r0-r14}^
 	nop
 	add sp, sp, #(15*4)
 
 	ldmfd sp!, {pc}^

.global _prefetch_abort_trampoline
_prefetch_abort_trampoline:
	sub lr, lr, #4 /* Pipe-Ausgleich */
	
	stmfd	sp!, {lr}
	
	sub	sp, #(15*4)
	stmia sp, {r0-r14}^
	
	mov r0, sp
	
	bl prefetch_abort_handler
	
 	ldmia sp, {r0-r14}^
 	nop
 	add sp, sp, #(15*4)
 
 	ldmfd sp!, {pc}^

.global _undefined_instruction_trampoline
_undefined_instruction_trampoline:
	sub lr, lr, #4 /* Pipe-Ausgleich */
	
	stmfd sp!, {r0-r12,lr}
	
	mov r1, sp
	add r0, sp, #13*4
	bl undefined_instruction_handler
	
	b busy_wait
	
.global _irq_trampoline
_irq_trampoline:
	sub lr, lr, #4 /* Pipe-Ausgleich */
	
	stmfd	sp!, {lr}
	
	sub	sp, #(15*4)
	stmia sp, {r0-r14}^
	
	mov r0, sp
	
	bl irq_handler
	
 	ldmia sp, {r0-r14}^
 	nop
 	add sp, sp, #(15*4)
 
 	ldmfd sp!, {pc}^
