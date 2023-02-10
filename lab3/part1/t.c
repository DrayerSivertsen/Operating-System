#include "type.h"

int color;

#include "string.c"
#include "timer.c"
#include "vid.c"
#include "exceptions.c"

TIMER *tp[4];

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_chandler()
{
    int vicstatus, sicstatus;
    // read VIC SIV status registers to find out which interrupt
    vicstatus = *(VIC_BASE + VIC_STATUS);
    sicstatus = *(SIC_BASE + SIC_STATUS);

    if (vicstatus & 0x0010){   // timer0,1=bit4
            timer_handler(0);
    }
}

int main()
{
   color = YELLOW;
   row = col = 0; 
   fbuf_init();

   // enable VIC to route timer0,1 interrupts at line 4
   *(VIC_BASE + VIC_INTENABLE) |= (1<<4);  // timer0,1 at bit4 
 
   kputs("test timer driver by interrupts\n");
   timer_init();        // initialize timer driver    
   timer_start(0);      // start timer 0
   
   kputs("Enter while(1) loop\n");
   while(1);
}
