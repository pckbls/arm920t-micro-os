#include "kernel.h"

#define MC 0xFFFFFF00
#define MC_RCR 0x0

#define AIC 0xFFFFF000
#define AIC_EOICR 0x130
#define AIC_IECR 0x120

void exceptions_init(void) __attribute__((section(".init")));

extern unsigned int ivt_command;

extern void _swi_trampoline(void);
extern void _data_abort_trampoline(void);
extern void _undefined_instruction_trampoline(void);
extern void _irq_trampoline(void);
extern void _prefetch_abort_trampoline(void);
extern void _unhandled_trampoline(void);

/* Die Reihenfolge dieser Tabelle muss mit den SWI_-Defines in syscalls.h
 * uebereinstimmen! */
typedef void (swi_handler_t)(interrupt_info_t *);
static
swi_handler_t *swi_handlers[] = {
	syscall_char_read,
	syscall_char_write,
	syscall_create_thread,
	syscall_exit,
	syscall_block,
	syscall_sleep
};

static
void dump_details(unsigned int spsr, unsigned int pc, unsigned int *registers)
{
	serial_printf("PC: %x\n", pc);
	serial_printf("SPSR: %x\n", spsr);
	
	serial_printf("Register dump:\n");
	serial_printf("  r15 : %x r14 : %x r13 : %x r12 : %x\n",
				  registers[15], registers[14], registers[13], registers[12]);
	serial_printf("  r11 : %x r10 : %x  r9 : %x  r8 : %x\n",
				  registers[11], registers[10], registers[9], registers[8]);
	serial_printf("   r7 : %x  r6 : %x  r5 : %x  r4 : %x\n",
				  registers[7], registers[6], registers[5], registers[4]);
	serial_printf("   r3 : %x  r2 : %x  r1 : %x  r0 : %x\n",
				  registers[3], registers[2], registers[1], registers[0]);
	serial_printf("\n");
}

static
void dump_info_struct(interrupt_info_t *info)
{
	serial_printf("Dumping interrupt info structure!\n");
	dump_details(info->spsr, info->u_registers[REGISTER_PC], info->u_registers);
}

static
void interrupt_fill_info_struct(unsigned int *registers, interrupt_info_t *info)
{
	for (int i = 0; i <= 15; i++)
		info->u_registers[i] = registers[i];
	
	info->cpsr = get_cpsr();
	info->spsr = get_spsr();
}

static
void interrupt_restore_context(unsigned int *registers, interrupt_info_t *info)
{
	for (int i = 0; i <= 15; i++)
		registers[i] = info->u_registers[i];
	
	set_spsr(info->spsr);
}

void exceptions_init(void)
{
	// Memory remap
	volatile unsigned int *mc_rcr = (unsigned int *)(MC + MC_RCR);
	*mc_rcr |= (1 << 0);
	
	volatile unsigned int *p;
	
	// Wir schreiben den Sprungbefehl, den wir in der arm.S definiert haben
	// an die richtigen Adressen in der IVT.
	p = (unsigned int *)0x00; *p = ivt_command; // Reset
	p = (unsigned int *)0x04; *p = ivt_command; // Undefined Instruction
	p = (unsigned int *)0x08; *p = ivt_command; // Software interrupt
	p = (unsigned int *)0x0C; *p = ivt_command; // Prefetch abort
	p = (unsigned int *)0x10; *p = ivt_command; // Data abort
	p = (unsigned int *)0x18; *p = ivt_command; // Hardware Interrupt
	p = (unsigned int *)0x1C; *p = ivt_command; // Fast Interrupt
	
	// Damit die Sprungbefehle funktionieren, benötigen wir noch die
	// Adressen der Trampoline als WORD hinter der IVT.
	p = (unsigned int *)0x20; *p = (unsigned int)_unhandled_trampoline;
	p = (unsigned int *)0x24; *p = (unsigned int)_undefined_instruction_trampoline;
	p = (unsigned int *)0x28; *p = (unsigned int)_swi_trampoline;
	p = (unsigned int *)0x2C; *p = (unsigned int)_prefetch_abort_trampoline;
	p = (unsigned int *)0x30; *p = (unsigned int)_data_abort_trampoline;
	p = (unsigned int *)0x38; *p = (unsigned int)_irq_trampoline;
	p = (unsigned int *)0x3C; *p = (unsigned int)_unhandled_trampoline;
	
	volatile unsigned int *aic_iecr = (unsigned int*)(AIC + AIC_IECR);
	*aic_iecr |= (1<<1);
}

void swi_handler(unsigned int *registers)
{
	interrupt_info_t info;
	interrupt_fill_info_struct(registers, &info);

	unsigned int swi_opcode = *(unsigned int *)(info.u_registers[REGISTER_PC]- 4) & ~(0xff000000);
	
	if (swi_opcode < sizeof(swi_handlers) / sizeof(swi_handler_t *))
		swi_handlers[swi_opcode](&info);
	else
	{
		serial_printf("Unhandled software interrupt (Code: %x)!\n", swi_opcode);
		dump_info_struct(&info);
	}
	
	thread_schedule(&info);
	
	interrupt_restore_context(registers, &info);
}

void irq_handler(unsigned int *registers)
{
	interrupt_info_t info;
	interrupt_fill_info_struct(registers, &info);
	
	serial_interrupt();
	st_interrupt();
	
	thread_schedule(&info);

	interrupt_restore_context(registers, &info);
	
	volatile unsigned int *aic_eoicr = (unsigned int*)(AIC + AIC_EOICR);
	*aic_eoicr = 1;
}

void data_abort_handler(unsigned int *registers)
{
	interrupt_info_t info;
	interrupt_fill_info_struct(registers, &info);
	
	serial_printf("Data abort!\n");
	dump_info_struct(&info);
	
	thread_kill_current();
	
	thread_schedule(&info);
	
	interrupt_restore_context(registers, &info);
}

void undefined_instruction_handler(unsigned int *pc, unsigned int *registers)
{
	serial_printf("Undefined instruction!\n");
	dump_details(get_spsr(), *pc, registers);
}

void prefetch_abort_handler(unsigned int *registers)
{
	interrupt_info_t info;
	interrupt_fill_info_struct(registers, &info);
	
	serial_printf("Prefetch abort!\n");
	dump_info_struct(&info);
	
	thread_kill_current();
	
	thread_schedule(&info);
	
	interrupt_restore_context(registers, &info);
}

void unhandled_handler(void)
{
	serial_printf("Unhandled exception!\n");
}
