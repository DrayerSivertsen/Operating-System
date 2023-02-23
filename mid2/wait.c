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
  PROC *p1;
  // 1. erase process user-mode context, e.g. close file descriptors, release resources, deallocate user-mode image memory, etc.
  // skip for now

  // 2. dispose of children processes, if any
  if (p->child != 0) // child pointer empty
  {
    printf("give away children to P1\n");

    p1 = p;
    while (p1->pid != 1) // find P1
      p1 = p1->parent;

    if (p1->status == SLEEP)
      printf("wakeup P1\n");
      kwakeup(p1->event);

    p = p->child;
    p->parent = p1;
    p->ppid = p1->pid;
    while (p->sibling != 0) // iterate through siblings setting parent to P1
    {
      p->parent = p1;
      p->ppid = p1->pid;
      p = p->sibling;
    }

    
    tmp = p1->child;
    while (tmp->sibling != 0)
      tmp = tmp->sibling;
    tmp->sibling = running->child;
  }

  // 3. record exitValue in PROC.exitCode for parent to get
  running->exitCode = exitCode;

  // 4. become a ZOMBIE (but do not free the PROC)
  running->status = ZOMBIE;

  // 5. wakeup parent and, if needed, also INIT process P1
  kwakeup(running->parent);

  // 6. switch process to give up CPU
  tswitch();
}

int kwait(int *status)  
{
  PROC *p;
  PROC *tmp, *prev;

  if (running->child == 0) // if caller has not child return error
  {
    printf("no child found!\n");
    return -1;
  }


  while(1)
  {
    p = running;
    tmp = p->child;

    if (tmp->status != ZOMBIE) // search for (any) ZOMBIE child
    {
      while (tmp != 0 && tmp->status != ZOMBIE)
      {
        tmp = tmp->sibling;
      }
    }

    if (tmp->status == ZOMBIE)
    {
      printf("zombie found!\n");

      int pid = tmp->pid; // get ZOMBIE child pid

      *status = tmp->exitCode; // copy ZOMBIE child exitCode to *status

      tmp->status = FREE;

      tmp->priority = 0; // reset for freeList

      enqueue(&freeList, tmp); // release child PROC to freeList as FREE

      prev = tmp->parent;
      if (prev->child == tmp) // remove free status: child is free
        prev->child = tmp->sibling;
      else // remove free status: sibling is free
      {
        prev = prev->child;
        while (prev->sibling != tmp)
        {
          prev = prev->sibling;
        }
        prev->sibling = tmp->sibling;
      }

      printf("P%dwaited for a ZOMBIE, P%dexit status=%d\n", running->pid, tmp->pid, *status);

      return pid; // return ZOMBIE child pid
    }

    ksleep(running); // sleep on its PROC address
  }

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