#include "kernel.h"

unsigned int l1_table[0x1000] __attribute__((aligned(0x100000)));

void __attribute__((section(".init"))) memory_init(void)
{
	/* Alle nicht genutzten Speicherbereiche sollen gesperrt bleiben. */
	for (unsigned int i = 0x0; i < 0x1000; i++)
		l1_table[i] = 0;
	
	/* Exception table, ROM, SRAM, USB Host Interface */
	for (unsigned int i = 0x000; i <= 0x003; i++)
		l1_table[i] = (i << 20) | AP_USER_NO_ACCESS | (1<<1);
	
	/* RAM */
	for (unsigned int i = 0x200; i < 0x240; i++)
		l1_table[i] = (i << 20) | AP_USER_NO_ACCESS | (1<<1);
	
	/* Peripherie */
	for (unsigned int i = 0xf00; i < 0x1000; i++)
		l1_table[i] = (i << 20) | AP_USER_NO_ACCESS | (1<<1);
	
	cp15_write_domain(1);
	cp15_disable_caches();
	cp15_write_ttb(l1_table);
	cp15_invalidate_tlb();
	cp15_enable_mmu();
}

void mmu_l1_section(unsigned int virt_addr, unsigned int phy_addr, enum mmu_permission permissions)
{
	DEBUG("Mappe %x -> %x (virt -> phys)\n", virt_addr, phy_addr);
	
	unsigned int entry = BLOCK_1MB(phy_addr) << 20;
	entry |= permissions << 10;
	entry |= L1_SECTION;
	
	l1_table[virt_addr >> 20] = entry; 
}
