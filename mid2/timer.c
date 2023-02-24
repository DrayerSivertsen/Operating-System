// timer.c file
/***** timer confiuguration values *****/
#define CTL_ENABLE          ( 0x00000080 )
#define CTL_MODE            ( 0x00000040 )
#define CTL_INTR            ( 0x00000020 )
#define CTL_PRESCALE_1      ( 0x00000008 )
#define CTL_PRESCALE_2      ( 0x00000004 )
#define CTL_CTRLEN          ( 0x00000002 )
#define CTL_ONESHOT         ( 0x00000001 )

// timer register offsets from base address
/**** byte offsets *******
#define TLOAD   0x00
#define TVALUE  0x04
#define TCNTL   0x08
#define TINTCLR 0x0C
#define TRIS    0x10
#define TMIS    0x14
#define TBGLOAD 0x18
*************************/
/** u32 * offsets *****/
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer{
  u32 *base;            // timer's base address; as u32 pointer
  int tick, hh, mm, ss; // per timer data area
  char clock[16]; 
}TIMER;

volatile TIMER timer[4];  // 4 timers; 2 timers per unit; at 0x00 and 0x20

extern int kpchar(char, int, int);
extern int unkpchar(char, int, int);
extern int kputc(char);

int k;

void timer_init()
{
  int i;
  TIMER *tp;
  kputs("timer_init()\n");

  for (i=0; i<4; i++){
    tp = &timer[i];
    if (i==0) tp->base = (u32 *)0x101E2000; 
    if (i==1) tp->base = (u32 *)0x101E2020; 
    if (i==2) tp->base = (u32 *)0x101E3000; 
    if (i==3) tp->base = (u32 *)0x101E3020;

    *(tp->base+TLOAD) = 0x0;   // reset
    *(tp->base+TVALUE)= 0x0;
    *(tp->base+TRIS)  = 0x0;
    *(tp->base+TMIS)  = 0x0;
    *(tp->base+TCNTL) = 0x62; //011-0000=|En|Pe|IntE|-|scal=00|32-bit|0=wrap|
    *(tp->base+TBGLOAD) = 0xE0000/60;

    tp->tick = tp->hh = tp->mm = tp->ss = 0;
    
    strcpy((char *)tp->clock, "00:00:00");
  }
}

void timer_handler(int n){
  
  TIMER *t = &timer[n];

  t->tick++;

  if (t->tick == 30)
  {
    clrcursor();
  }

  if (t->tick == 60) // increment seconds
  {
    // logic for timer queue
    if (tq != 0)
    {
      tq->time -= 1;
      printf("proc %d running\n", running->pid);
      print_tq();

      if (tq->time == 0)
      {
        TQE* tmp = dequeue_tq();
        printf("kwakeup %d\n", ((PROC*)tmp->proc)->pid);
        kwakeup(((PROC*)tmp->proc)->pid);
      }
    }


    putcursor();

    t->tick = 0; t->ss++;
    if (t->ss == 60) // increment minutes
    {
      t->ss = 0; t->mm++;
      if (t->mm == 60)
      {
        t->mm = 0; t->hh++;
      }
    }

    for (int i=0; i<8; i++)
    {
      unkpchar(t->clock[i], n, 70+i); // to line n of LCD
    }

    t->clock[7] = '0' + (t->ss%10); t->clock[6] = '0' + (t->ss/10);
    t->clock[4] = '0' + (t->mm%10); t->clock[3] = '0' + (t->mm/10);
    t->clock[1] = '0' + (t->hh%10); t->clock[0] = '0' + (t->hh/10);
  }


  // color = 1;
  for (int i=0; i<8; i++)
  {
    kpchar(t->clock[i], n, 70+i); // to line n of LCD
  }

  
  timer_clearInterrupt(n);
}

void timer_start(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];

  *(tp->base+TCNTL) |= 0x80;    // set enable bit 7
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TCNTL) &= 0x7F;  // clear enable bit 7
}
