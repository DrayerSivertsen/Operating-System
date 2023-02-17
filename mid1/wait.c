// wait.c file

extern PROC *freeList;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *running;

int ksleep(int event)
{
  int SR = int_off();
  running->event = event; // record event value in running PROC.event

  running->status = SLEEP; // change running PROC status to sleep

  tswitch(); // switch process
  int_on(SR); // restore original CPSR
}

int kwakeup(int event)
{
  int SR = int_off(); // disable IQR and return CPSR
  while (p->next)// for each PROC *p do 
  {
    if (p->status == SLEEP && p->event == event)
    {
      p->status = READY;
      enqueue(&readyQueue, p); // enter p into readyQueue
    }
  }
  int_on(SR); // restore original CPSR
}

int kexit(int exitCode) 
{
  // implement this
}

int kwait(int *status)  
{
  // implement this
}
