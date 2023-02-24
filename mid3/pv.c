int P(struct semaphore *s)
{
  int SR = int_off();
  s->value--;
  if (s->value < 0)
    block(s);
  int_on(SR);
}

int block(struct semaphore *s)
{
  running->status = BLOCK;
  enqueue(&s->queue, running);
  tswitch();
}

int V(struct semaphore *s)
{
  int SR = int_off();
  s->value++;
  if (s->value <= 0)
    signal(s);
  int_on(SR);
}

int signal(struct semaphore *s)
{
  PROC *p = dequeue(&s->queue);
  p->status = READY;
  enqueue(&readyQueue, p);
}
