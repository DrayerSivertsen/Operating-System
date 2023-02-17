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

  enqueue(&sleepList, running);

  tswitch(); // switch process
  int_on(SR); // restore original CPSR
}

int kwakeup(int event)
{
  PROC* p = sleepList;
  PROC* prev = sleepList;

  int SR = int_off(); // disable IQR and return CPSR
  while (p)// for each PROC *p do 
  {
    if (p->status == SLEEP && p->event == event)
    {
      if (p == sleepList) // remove from head
        sleepList = p->next;
      else // remove from middle or end
      {
        prev->next = p->next;
      }
      p->status = READY;
      enqueue(&readyQueue, p); // enter p into readyQueue
    }
    prev = p;
    p = p->next;
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
