.global cp15_write_domain
cp15_write_domain:
	mcr p15, 0, r0, c3, c0, 0
	bx lr
	
.global cp15_write_ttb
cp15_write_ttb:
	mcr p15, 0, r0, c2, c0, 0
	bx lr
	
.global cp15_read_ttb
cp15_read_ttb:
	mrc p15, 0, r0, c2, c0, 0
	bx lr

.global cp15_disable_caches
cp15_disable_caches:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #(1<<12)
	bic r0, r0, #(1<<2)
	mcr p15, 0, r0, c1, c0, 0
	bx lr
	
.global cp15_enable_mmu
cp15_enable_mmu:
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<0)
	mcr p15, 0, r0, c1, c0, 0
	
	bx lr
	
.global cp15_read_c1
cp15_read_c1:
	mrc p15, 0, r0, c1, c0, 0
	bx lr

.global cp15_invalidate_tlb
cp15_invalidate_tlb:
	mcr p15, 0, r0, c8, c7, 0
	bx lr