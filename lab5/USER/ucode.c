int umenu()
{
  uprintf("-----------------------------------------------------------\n");
  uprintf("getpid getppid ps chname switch kfork sleep wakeup exit wait\n");
  uprintf("-----------------------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  uprintf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ufork()
{
  uprintf("syscall to kernel kfork, ");
  char s[32];
  uprintf("enter a filename : ");
  ugetline(s);
  uprintf("\n");
  return syscall(5,s,0,0);
}

int usleep()
{
  char* input;
  int event;
  uprintf("syscall to kernel ksleep, ");
  uprintf("input an event value to sleep : ");
  ugetline(input);
  event = atoi(input);
  uprintf("\n");
  return syscall(6,event,0,0);
}

int uwakeup()
{
  char* input;
  int event;
  uprintf("syscall to kernel kwakeup, ");
  uprintf("input an event value to wakeup : ");
  ugetline(input);
  event = atoi(input);
  uprintf("\n");
  return syscall(7,event,0,0);
}

int uexit()
{
  char* input;
  int exitCode;
  uprintf("syscall to kernel kexit, ");
  uprintf("input an exitCode value : ");
  ugetline(input);
  exitCode = atoi(input);
  uprintf("\n");
  return syscall(8,exitCode,0,0);
}

int uwait()
{
  char *status;
  uprintf("syscall to kernel kwait\n");
  return syscall(9,status,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}

