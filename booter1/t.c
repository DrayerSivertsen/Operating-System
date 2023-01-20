#include "type.h"
#include "string.c"

#include "uart.c"

int main()
{
  char name[128];
  
  uart_init();       

  while(1){
    
    uprints("What's your name? ");

    ugets(name);

    if ( strcmp(name, "quit")==0 )
	break;
    uprints("name = "); uprints(name); uprints("\n\r");
    ugetc();
  }

  // Write YOUR OWN uprintf(char *fmt, ...) as in CS360 for  
  // uprintf("s=%s u=%u i=%d x=%x\n", "string", 128, -100, 256);

}
