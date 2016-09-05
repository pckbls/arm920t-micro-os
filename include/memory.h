#ifndef MEMORY_H_
#define MEMORY_H_

enum mmu_permission {
	PERM_KERNEL_READ	= 0,
	PERM_KERNEL_READ_WRITE	= 1,
	PERM_USER_READ		= 2,
	PERM_USER_READ_WRITE	= 3,
};

enum mmu_control {
	MMU_ENABLE		= 1 << 0,
	MMU_ALIGN_CHECK		= 1 << 1,
	MMU_DCACHE		= 1 << 2,
	MMU_SYS_PROTECTION	= 1 << 8,
	MMU_ROM_PROTECTION	= 1 << 9,
	MMU_ICACHE		= 1 << 12,
};

enum mmu_l1_entries {
	L1_FAULT = 0,
	L1_SECTION = 0x12,
};

void *memcpy(void *dest, const void *src, unsigned int n);

void mmu_l1_section(unsigned int virt_addr, unsigned int phy_addr, enum mmu_permission permissions);

#endif
