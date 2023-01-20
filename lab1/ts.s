	.text

.global start, main, go

start:
  ldr sp, =svc_stack_top
  bl main
  B .
	
go:
  mov pc, #0x100000
