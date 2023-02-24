int color;

#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "timer.c"
#include "pv.c"
#include "message.c"

TIMER *tp[4];

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    vicstatus = *(VIC_BASE + VIC_STATUS); // VIC_STATUS=0x10140000=status reg
    sicstatus = *(SIC_BASE + SIC_STATUS);  

    if (vicstatus & (1 << 31)){
      if (sicstatus & (1 << 3)){
          kbd_handler();
       }
   }

   if (vicstatus & 0x0010){   // timer0,1=bit4
      timer_handler(0);
   }
}

int sender()
{
  char line[128];

  printf("Enter a line for task%d to get : SENDER task %d waits for line from KBD\n", running->pid, running->pid);

  while(1) // use printf() to show actions
  {
    kgetline(line);
    kputs("\n");
    printf("SENDER task %d got a line=[%s]\n", running->pid, line);
    send(line, 3);
  }
}

int receiver()
{
  char line[128];
  int pid;

  while(1) // use printf() to show actions
  {
    printf("RECEIVER task %d try to receive\n", running->pid);
    printf("RUNNING PID: %d\n", running->pid);
    printf("trying to switch\n");
    tswitch();
    printf("RUNNING PID: %d\n", running->pid);
    pid = recv(line);
    printf("RECEIVER task %d received: [%s] from task 1\n");
    // print message contents as sendPID, message
  }
}

int body();

int main()
{ 
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kbd_init();
   
   // allow KBD interrupts   
   *(VIC_BASE + VIC_INTENABLE) |= (1<<31); // allow VIC IRQ31
   *(SIC_BASE + SIC_INTENABLE) |= (1<<3);  // KBD int=3 on SIC
   *(VIC_BASE + VIC_INTENABLE) |= (1<<4);  // timer0,1 at bit4 
 
   kprintf("Welcome to WANIX in Arm\n");
   kernel_init();

   kfork(sender, 1);
   kfork(sender, 1);
   kfork(receiver, 1);

   printList("readyQueue", readyQueue);
   while(1){
     if (readyQueue)
        tswitch();
   }
}
