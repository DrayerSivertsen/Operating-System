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

  enqueue(&sleepList, running); // add to sleepList

  tswitch(); // switch process
  int_on(SR); // restore original CPSR
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;


  int SR = int_off(); // disable IQR and return CPSR
  while ((p = dequeue(&sleepList)) != 0) // dequeue each PROC from sleepList
  {
    if (p->status == SLEEP && p->event == event) // save to ready queue if sleeping and correct event
    {
      p->status = READY;
      enqueue(&readyQueue, p); // enter p into readyQueue
    }
    else
    {
      enqueue(&tmp, p); // put back to sleep
    }
  }
  sleepList = tmp; // reset head of queue
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
