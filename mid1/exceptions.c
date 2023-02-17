// only handle IRQ interrupts
/* all other exception handlers are infinite loops */
void undef_handler()          { while(1); }
void swi_handler()            { while(1); }
void prefetch_abort_handler() { while(1); }
void data_abort_handler()     { while(1); }
void fiq_handler()            { while(1); }

