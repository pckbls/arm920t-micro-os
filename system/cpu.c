#include "kernel.h"

void get_banked_sp_lr(unsigned int mode, unsigned int regs[2])
{
	if (mode == PSR_USR)
		mode = PSR_SYS;
	
	_get_banked_sp_lr(mode, regs); 
}

void set_banked_sp_lr(unsigned int mode, unsigned int regs[2])
{
	if (mode == PSR_USR)
		mode = PSR_SYS;
	
	_set_banked_sp_lr(mode, regs); 
}
