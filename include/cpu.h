#ifndef _CPU_H_
#define _CPU_H_

/* Prozessormode-Bits für das CPSR */
#define PSR_FIQ (1<<4 | 0<<3 | 0<<2 | 0<<1 | 1<<0)
#define PSR_IRQ (1<<4 | 0<<3 | 0<<2 | 1<<1 | 0<<0)
#define PSR_SVC (1<<4 | 0<<3 | 0<<2 | 1<<1 | 1<<0)
#define PSR_ABT (1<<4 | 0<<3 | 1<<2 | 1<<1 | 1<<0)
#define PSR_UND (1<<4 | 1<<3 | 0<<2 | 1<<1 | 1<<0)
#define PSR_SYS (1<<4 | 1<<3 | 1<<2 | 1<<1 | 1<<0)
#define PSR_USR (1<<4 | 0<<3 | 0<<2 | 0<<1 | 0<<0)

#define REGISTER_SP 13
#define REGISTER_LR 14
#define REGISTER_PC 15

#ifndef __ASSEMBLER__
void enable_exceptions(void);
void busy_wait(void);

unsigned int get_cpsr(void); 
unsigned int get_spsr(void); 
void set_spsr(unsigned int mode);
void get_banked_sp_lr(unsigned int mode, unsigned int regs[2]); 
void set_banked_sp_lr(unsigned int mode, unsigned int regs[2]);

void _get_banked_sp_lr(unsigned int mode, unsigned int regs[2]);
void _set_banked_sp_lr(unsigned int mode, unsigned int regs[2]); 
#endif

#endif
