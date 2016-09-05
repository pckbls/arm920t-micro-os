#ifndef _CP15_H_
#define _CP15_H_

#define AP_USER_FULL_ACCESS (1<<11) | (1<<10)
#define AP_USER_READ_ONLY   (1<<11) | (0<<10)
#define AP_USER_NO_ACCESS   (0<<11) | (1<<10)

void cp15_write_domain(unsigned int x);
void cp15_write_ttb(unsigned int *table);
void cp15_disable_caches(void);
void cp15_enable_mmu(void);
void cp15_invalidate_tlb();

#endif