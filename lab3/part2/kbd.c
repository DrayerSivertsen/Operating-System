#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

int release;       // key release flag
int shiftpressed;
int ctrlpressed;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8
  kp->head = kp->tail = 0;       // circular buffer char buf[128]
  kp->data = 0; kp->room = 128;

  release = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  color = YELLOW;

  scode = *(kp->base + KDATA);   // get scan code of this interrpt

  kputs("kbd interrupt scancode = "); kprintx(scode);


  if (scode == 0x12) // flag for shift
  {
    if (shiftpressed == 1)
      shiftpressed = 0;
    else
      shiftpressed = 1;
  }

  if (scode == 0x14) // flag for ctrl
  {
    if (ctrlpressed == 1)
      ctrlpressed = 0;
    else
      ctrlpressed = 1;
  }

    
  if (scode == 0xF0){  // it's key release 
     release = 1;      // set release flag
     return;
  }

  if (release == 1){   // scan code after 0xF0
     release = 0;      // reset release flag
     return;
  }

  if (shiftpressed == 1) // left-shift pressed
    c = utab[scode]; // uppercase key

  // map scode to ASCII in lowercase 
  if (shiftpressed == 0)
    c = ltab[scode];

  if (scode == 0x21 && ctrlpressed == 1) // ctrl + c
  {
    kputs("kbd interrupt : ");
    if (c != '\r')
      kputs("Control-C key");
    kputs("\n");
    return;
  }

  if (scode == 0x23 && ctrlpressed == 1) // ctrl + d
  {
    kputs("kbd interrupt : ");
    if (c != '\r')
      kputs("EOF");
    kputs("\n");
    return;
  }


  if (scode != 0x12 && scode != 0x14) // don't print ctrl or shift key
  {
    kputs("kbd interrupt : ");
    if (c != '\r')
      kputc(c);
    kputs("\n");

    kp->buf[kp->head++] = c;
    kp->head %= 128;
    kp->data++; kp->room--;
  }

}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();                          // unmask IRQ in case it was masked out
  while(kp->data == 0);              // BUSY wait while kp->data is 0 

  lock();                            // mask out IRQ
    c = kp->buf[kp->tail++]; 
    kp->tail %= 128;                 /*** Critical Region ***/
    kp->data--; kp->room++;
    unlock();                        // unmask IRQ
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s = c;
    s++;
  }
  *s = 0;
}

