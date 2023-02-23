int color;

#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"

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
 
   kprintf("Welcome to WANIX in Arm\n");
   kernel_init();
   kfork((int)body, 1);
   printf("P0 switch to P1\n");
   while(1){
     if (readyQueue)
        tswitch();
   }
}
