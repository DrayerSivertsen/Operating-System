// kernel.c file

#define NPROC 9
/*********** in type.h ***************
typedef struct proc{
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    priority;
  int    status;
  int    event;
  int    exitCode;

  struct proc *parent;
  struct proc *child;
  struct proc *sibling;
  
  int    kstack[SSIZE];
}PROC;
***************************************/
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
TQE *tq, tqe[NPROC]; // tq = timer queue pointer
int procsize = sizeof(PROC);

int tqe_init()
{
  int i;
  PROC *p;
  kprintf("tqe_init()\n");
  // for (i=0; i<NPROC; i++)
  // {
  //   p = &tqe[i];
  //   int time = 0;
  //   p->proc = &proc[i];
  //   p->next = p + 1;
  // }
}

int enqueue_tq(int time)
{
  TQE *cur = tq;
  TQE *prev;

  if (tq == 0) // first element and empty
  {
    // printf("first element and empty\n");
    tq = &tqe[running->pid];
    tq->time = time;
    tq->proc = &proc[running->pid];
    tq->next = 0;
  }
  else
  {
    while (time > cur->time && cur != 0) // time greater than cur move to next in queue
    {
      time -= cur->time;
      prev = cur;
      cur = cur->next;
    }

    if (cur == tq) // first element not empty
    {
      // printf("first element not empty\n");
      tq = &tqe[running->pid];
      tq->next = cur;
      tq->time = time;
      tq->proc = &proc[running->pid];
      cur->time -= time;
    }
    else if (cur == 0) // inserted at end
    {
      // printf("inserted at end\n");
      cur = &tqe[running->pid];
      prev->next = cur;
      cur->time = time;
      cur->proc = &proc[running->pid];
      cur->next = 0;
    }
    else // middle element not empty
    {
      // printf("middle element not empty\n");
      prev->next = &tqe[running->pid];
      prev = prev->next;
      prev->time = time;
      prev->proc = &proc[running->pid];
      prev->next = cur;
      cur->time -= time;
    }
  }
}

TQE* dequeue_tq()
{
  TQE* tmp = tq;
  tq = tq->next;

  return tmp;
}

int print_tq()
{
  TQE *cur = tq;
  printf("timerQueue = ");
  while(cur){
    printf("[%d,%d]->", ((PROC*)cur->proc)->pid, cur->time);
    cur = cur->next;
  }
  printf("NULL\n");
}

int body();

int kernel_init()
{
  int i;
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->ppid = 0;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; 
  freeList = &proc[0];    // freeList = ALL free procs 
  // printList("freeList", freeList);
  
  readyQueue = 0;
  sleepList = 0;
  
  // creat P0 as running process
  p = dequeue(&freeList); // take proc[0] off freeList
  p->priority = 0;
  p->status = READY;
  p->ppid = 0;
  p->parent = p;
  p->child = p->sibling = 0;
  running = p;           // running -> proc[0] with pid=0

  kprintf("running = %d\n", running->pid);
  // printList("freeList", freeList);
}

int kfork(int func, int priority)
{
  PROC *tmp;
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running; // set parent pointer
  p->child = p->sibling = 0;

  // implement process family tree as BINARY tree by using the child, sibling, parent pointers
  if (running->child == 0) // child pointer empty
  {
    running->child = p; // set child pointer
  }
  else // child pointer not empty
  {
    tmp = running->child;
    while (tmp->sibling != 0) // iterate through siblings
      tmp = tmp->sibling;
    tmp->sibling = p; // set sibling pointer
  }
  
  // set kstack for new proc to resume to func()
  // stmfd sp!, {r0-r12, lr} saves regs in stack as
  // stack = lr r12 r11 r10 r9 r8 r7 r6 r5  r4  r3  r2  r1  r0
  // HIGH    -1 -2  -3  -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14   LOW
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-1] = (int)func;  // saved regs in dec address ORDER !!!

  p->ksp = &(p->kstack[SSIZE-14]);

  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  // printList("readyQueue", readyQueue);
  return p->pid;
}

int scheduler()
{
  // kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  // kprintf("next running = %d\n", running->pid);
  if (running->pid){
     color = running->pid;
  }
}


// code of processes
int body()
{
  char c, cmd[64];
  char* input, *status;
  int event, exitCode;
  PROC *tmp;

  while(1)
  {
    
  }
}
