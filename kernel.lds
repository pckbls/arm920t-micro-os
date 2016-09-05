/* TODO: eigentlich überflüssig? */
ENTRY(_start)

SECTIONS
{
	ram_origin = 0x20000000;
	ram_size   = 0x04000000;

	fiq_stack_size = 0x100000;
	irq_stack_size = 0x100000;
	svc_stack_size = 0x100000;
	und_stack_size = 0x100000;
	abt_stack_size = 0x100000;
	kernel_stack_size = (fiq_stack_size + irq_stack_size + svc_stack_size
	                  + und_stack_size + abt_stack_size);

	. = ram_origin;
	
	.init :
	{
		_start_init = .;
		loader.o(*);
		kernel.o(.init);
		_end_init = .;
	}
	
	.text :
	{
		_start_text = .;
		kernel.o(.text*);
		kernel.o(.rodata*);
		_end_text = .;
	}
	
	.bss :
	{
		_start_bss = .;
		kernel.o(.bss);
		_end_bss = .;
	}
	
	. = ALIGN(0x100000);
	_start_user = .;
	temp = .;
	
	.user 0x90000000 : AT(temp)
	{
		/* TODO: . = 0x90000000 testen */
		_start_user_virt = .;
		user.o(*);
		_end_user_virt = .;
	}
	
	. = LOADADDR(.user) + SIZEOF(.user);
	_end_user = .;
	
	.data_sections ALIGN(0x100000) :
	{
		_start_data_sections = .;
		. = 8 * 0x100000;
		_end_data_sections = .;
	}
	
	. = ALIGN(0x100000);
	temp = .;
	.user_stack (NOLOAD) :
	{
		_start_user_stack = .;
		
		/* Keine Ahnung, warum man da temp nochmal abziehen muss.
		 * Linkerscripts sind ein wenig... eigenartig.
		 * Die 0x100000 ziehen wir ab, dass der Kernelstack
		 * auf einen 1MB-Block aligned werden kann.
		 */
		. = ram_origin + ram_size - kernel_stack_size - temp - 0x100000;
		. = ALIGN(0x100000);
		
		_end_user_stack = .;
	}
	
	.kernel_stack (NOLOAD) :
	{
		_start_kernel_stack = .;
		
		_start_fiq_stack = .; . += fiq_stack_size; _end_fiq_stack = .;
		_start_fiq_stack = .; . += irq_stack_size; _end_irq_stack = .;
		_start_fiq_stack = .; . += svc_stack_size; _end_svc_stack = .;
		_start_fiq_stack = .; . += und_stack_size; _end_und_stack = .;
		_start_fiq_stack = .; . += abt_stack_size; _end_abt_stack = .;
		
		_end_kernel_stack = .;
	}
}
