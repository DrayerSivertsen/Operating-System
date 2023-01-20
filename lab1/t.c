#include "type.h"
#include "string.c"

#include "uart.c"
#include "sdc.c"
#include "boot.c"

int main()
{ 
   uart_init();      
   uprintf("\n\nWelcome to ARM EXT2 Booter\n");

   sdc_init();
   boot();

   uprintf("BACK FROM booter\n");
   
   uprintf("enter a key : ");
   ugetc();

   go();
}
