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
  PROC *p = running;
  PROC *tmp;
  // 1. erase process user-mode context, e.g. close file descriptors, release resources, deallocate user-mode image memory, etc.
  // skip for now

  // 2. dispose of children processes, if any
  for (int i=0; i<NPROC; i++){
    tmp = &proc[i];
    if (tmp->ppid == p->pid)
    {
      printf("give away children to P1\n");
      tmp->ppid = 1; // send children processes to P1
    }
  }
  // 3. record exitValue in PROC.exitCode for parent to get
  p->exitCode = exitCode;

  // 4. become a ZOMBIE (but do not free the PROC)
  p->status = ZOMBIE;

  // 5. wakeup parent and, if needed, also INIT process P1

  // 6. switch process to give up CPU
  tswitch();
}

int kwait(int *status)  
{
  PROC *p = running;
  PROC *tmp;

  if (p->)
}

int ps()
{
  PROC *p;
  char *tmp;

  printf("PID    PPID    status\n");
  printf("---    ----    ------\n");
  for (int i=0; i<NPROC; i++){
    p = &proc[i];
    if (p->status == 0)
      tmp = "FREE";
    else if (p->status == 1)
      tmp = "READY";
      if (running->pid == p->pid)
        tmp = "RUNNING";
    else if (p->status == 2)
      tmp = "SLEEP";
    else if (p->status == 3)
      tmp = "BLOCK";
    else if (p->status == 4)
      tmp = "ZOMBIE";
    printf(" %d     %d     %s\n", p->pid, p->ppid, tmp);
  }
}