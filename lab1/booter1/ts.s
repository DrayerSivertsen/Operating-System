	.text

.global start, main

start:

  ldr sp, =svc_stack_top
  bl main

  B .
