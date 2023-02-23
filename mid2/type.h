/********* type.h file ***********/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

u32 *VIC_BASE = (u32 *)0x10140000;
#define VIC_STATUS    0x00/4
#define VIC_INTENABLE 0x10/4
#define VIC_VADDR     0x30/4

u32 *SIC_BASE = (u32 *)0x10003000;
#define SIC_STATUS    0x00/4
#define SIC_INTENABLE 0x08/4
#define SIC_ENSET     0x08/4
#define SIC_PICENSET  0x20/4

char *UART0_BASE = (char *)0x101f1000;
char *UART1_BASE = (char *)0x101f2000;
#define UDR   0x00
#define UFR   0x18
#define UIMSC 0x38

char *KBD_BASE = 0x10006000;
#define KBD_CR 0x00
#define KBD_DR 0x08

#define BLUE   0
#define GREEN  1
#define RED    2
#define CYAN   3
#define YELLOW 4
#define PURPLE 5
#define WHITE  6

#define  SSIZE 1024

#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4

#define  printf  kprintf
 
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
