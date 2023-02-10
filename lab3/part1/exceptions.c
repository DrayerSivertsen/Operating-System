// exceptions.c file
/* all other exception handlers are while(1) loops */
void __attribute__((interrupt)) undef_handler(void) { while(1); }
void __attribute__((interrupt)) swi_handler(void) { while(1); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { while(1); }
void __attribute__((interrupt)) data_abort_handler(void) { while(1); }
void __attribute__((interrupt)) fiq_handler(void) { while(1); }
