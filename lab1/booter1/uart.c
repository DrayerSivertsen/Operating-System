/*********** uart.c file **********/
#define DR 0x00
#define FR 0x18

typedef struct uart{
  char *base;           // base address
  int   unit;           // UART ID number
}UART;

UART *up;

// versatile_epb : UART0 at 0x101F1000

void uart_init()
{
  up->base = (UART *)(0x101F1000);
  up->unit = 0;
}

void uputc(char c)
{
  while ( *(up->base + FR) & 0x20 );  // while FR.bit5 != 0 ;
  *(up->base + DR) = (int)c;          // write c to DR  
}

int ugetc()
{
  while ( *(up->base + FR) & 0x10 ); // while FR.bit4 != 0 ;
  return *(up->base + DR);           // DR has data
}

void uprints(char *s)
{
  while(*s)
    uputc(*s++);
}

int ugets(char *s)
{
  uprints("ugets(): under construction\n\r");
}

// write your own ugets(char s[ ]) to input a string (end by \r)
// example: enter 12345\r ==> s[ ] = "12345";

// Write your own uprintf(char *fmt, ...) for formatter printing as in CS360
