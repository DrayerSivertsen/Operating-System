/************** uart.c file *******************
Flag register (offset 0x18): status of UART port

   7    6    5    4    3   2   1   0
| TXFE RXFF TXFF RXFE BUSY -   -   - |
  
TXFE=Tx buffer empty
RXFF=Rx buffer full
TXFF=Tx buffer full
RXFE=Rx buffer empty
BUSY = device busy
**********************************************/
  
#define DR 0x00
#define FR 0x18

char *ctable = "0123456789ABCDEF";

typedef struct uart{
  char *base;           // base address
  int  n;               // UART number 
}UART;

UART *up;

// versatile_epb : UART0 at 0x101F1000

void uart_init()
{
  up->base = (UART *)(0x101F1000);
  up->n = 0;
}

void uputc(char c) // output a char to UART
{
  while ( *(up->base + FR) & 0x20 ); // while FR.bit5 != 0 ;  (TX FULL)
  *(up->base + DR) = (int)c;         // write c to DR
}

int ugetc() // return a char from UART
{
  while ( *(up->base + FR) & 0x10 ); // while FR.bit4 != 0 ;  (RX EMPTY)
  return *(up->base + DR);           // get data in DR
}

int uprints(char *s) // print a line to UART
{
  while(*s)
    uputc(*s++);
}


// Lab1 code for printf
void uprintf(char *fmt, ...)
{
  char *cp = fmt;
  int *ip = (int *) (&fmt + 1);
  

  while(*cp)
  {
    if(*cp != '%')
    {
      uputc(*cp);

      if(*cp == '\n')
      {
        uputc('\r');
      }
    }
    else
    {
      cp++;

      if(*cp == 'c')
      {
        printc(*ip);
      }
      if(*cp == 's')
      {
        prints(*ip);
      }
      if(*cp == 'u')
      {
        printu(*ip);
      }
      // if(*cp == 'x')
      // {
      //   printx(*ip);
      // }
      if(*cp == 'd')
      {
        printd(*ip);
      }

      ip++;
        
    }

    cp++;
  }


}

int rpu(u32 x)
{  
  char c;
  if (x){
    c = ctable[x % 10]; // hard coded base 10
    rpu(x / 10); // hard coded base 10
    uputc(c);
  }
}

int printu(u32 x)
{
  (x==0)? uputc('0') : rpu(x);
  uputc(' ');
}

// int printx(u32 x)
// {
//   BASE = 16;
//   uputc('0');
//   uputc('x');
//   (x==0)? uputc('0') : rpu(x);
//   uputc(' ');
//   BASE = 10;
// }

int printd(int x)
{
  if(x < 0)
  {
    uputc('-');
    x = x * -1;
    printu(x);
  }
  else
  {
    printu(x);
  }
}

void printc(char c)
{
  uputc(c);
  uputc(' ');
}

void prints(char *s)
{
  while(*s)
  {
    uputc(*s);
    s++;
  }
  uputc(' ');
}