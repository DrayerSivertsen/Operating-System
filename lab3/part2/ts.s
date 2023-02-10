.text
.code 32

.global reset_handler
.global vectors_start, vectors_end
.global int_off, int_on
.global lock, unlock

reset_handler:
  LDR sp, =svc_stack
  BL copy_vectors

  MSR cpsr, #0x12
  LDR sp, =irq_stack

  MSR cpsr, #0x13
  BL main
  B .

.align 4
irq_handler:
  sub	lr, lr, #4
  stmfd	sp!, {r0-r12, lr}
  bl	IRQ_chandler  
  ldmfd	sp!, {r0-r12, pc}^


// int_on()/int_off()
int_off: // sr = int_off();
  MRS r4, cpsr
  mov r0, r4
  BIC r4, r4, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r4
  mov pc, lr          // return original SR	

int_on: // int_on(SR); 
  MSR cpsr, r0        // restore original SR
  mov pc, lr	

lock:                 // mask out IRQ
  mrs r4, cpsr     
  orr r4, r4, #0x80
  msr cpsr, r4
  mov pc, lr

unlock:               // mask in IRQ
  MRS r4, cpsr
  BIC r4, r4, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r4
  mov pc, lr	
	
vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

	.end
