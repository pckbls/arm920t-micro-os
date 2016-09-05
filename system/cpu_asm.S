.global _get_banked_sp_lr
_get_banked_sp_lr:
    /*  
     * In gewünschten Modus wechseln (mittels Read-Modify-Write, 
     * siehe AARM A4.1.39), alten Wert von CPSR in r2 halten.
     */
    mrs r2, cpsr
    bic r3, r2, #0x1f
    orr r3, r3, r0
    msr cpsr_c, r3
 
    /* Stack und Link Register lesen */ 
    stmia   r1, {sp, lr} 
 
    /* Modus mittels r2 zur�cksetzen */ 
    msr cpsr_c, r2
 
    mov pc, lr
    
.global _set_banked_sp_lr
_set_banked_sp_lr:
    /*  
     * In gewünschten Modus wechseln (mittels Read-Modify-Write, 
     * siehe AARM A4.1.39), alten Wert von CPSR in r2 halten.
     */
    mrs r2, cpsr
    bic r3, r2, #0x1f
    orr r3, r3, r0
    msr cpsr_c, r3
 
    /* Stack und Link Register schreiben */ 
    ldr sp, [r1, #0]
    ldr lr, [r1, #4]
 
    /* Modus mittels r2 zur�cksetzen */ 
    msr cpsr_c, r2
 
    mov pc, lr

.global get_cpsr
get_cpsr:
  	mrs	r0, cpsr
  	mov	pc, lr

.global get_spsr
get_spsr:
  	mrs	r0, spsr
  	mov	pc, lr

.global set_spsr
set_spsr:
	msr spsr, r0
	mov pc, lr
