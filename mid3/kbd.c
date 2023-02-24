// kbd.c file

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#include "keymap2"

typedef struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

KBD kbd;
int release;

int keyset;
int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  release = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

  if (scode == 0xF0){ // key release 
    release = 1;
    return;
  }
  if (release){       // 2nd interrupt of key release
    release = 0;
    return;
  }

  c = ltab[scode];

  if (c != '\r')
    printf("%c", c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
  kwakeup(&kp->data);
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
  while (kp->data == 0);

  lock();
   c = kp->buf[kp->tail++];
   kp->tail %= 128;
   kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
