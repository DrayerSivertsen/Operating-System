/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

int exec(char *uline)
{
  int i, *ip;
  char *cp, kline[64]; 
  PROC *p = running;
  char file[32], filename[32];
  int *usp, *ustacktop;
  u32 BA, Btop, Busp, uLINE;
  char line[32];

  //printf("EXEC: proc %d uline=%x\n", running->pid, uline);

  // line is in Umode image at p->pgdir[2048]&0xFFF00000=>can access from Kmode
  // char *uimage = (char *)(p->pgdir[2048] & 0xFFF00000);
  BA = (p->pgdir[2048] & 0xFFF00000);
  Btop = BA + 0x100000;  // top of 1MB Uimage
  printf("EXEC: proc %d Uimage at %x\n", running->pid, BA);
 
  uLINE = BA + ((int)uline - 0x80000000);
  kstrcpy(kline, (char *)uLINE);
  // NOTE: may use uline directly 

  printf("EXEC: proc %d line = %s\n", running->pid, kline); 

  // first token of kline = filename
  cp = kline; i=0;
  while(*cp != ' '){
    filename[i] = *cp;
    i++; cp++;
  } 
  filename[i] = 0;
  kstrcpy(file, "/bin/");
  kstrcat(file, filename);

  BA = p->pgdir[2048] & 0xFFF00000;
  kprintf("load file %s to %x\n", file, BA);

  // load filename to Umode image 
  load(file, p);
  //  printf("after loading ");

  // copy kline to high end of Ustack in Umode image
  //BA = p->pgdir[2048] & 0xFFF00000;
  Btop = BA + 0x100000;
  Busp = Btop - 32;

  cp = (char *)Busp;
  strcpy(cp, kline);

  p->usp = (int *)(0x80100000 - 32);

  // set up interrupt frame to execute u2, set up ustack to contain
  // initial string
  // kstack must contain a resume frame FOLLOWed by a goUmode frame
  //  ksp  
  //  -|-----------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 fp ip pc|
  //  -------------------------------------------
  //  28 27 26 25 24 23 22 21 20 19 18  17 16 15
  //  

  //   usp                NOTE: r0 is NOT saved in svc_entry()
  // --|-----goUmode--------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|
  //-------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1

  p->kstack[SSIZE-14] = p->kstack[SSIZE-13] = (int)(0x80100000 - 32); 
  p->kstack[SSIZE-1] = (int)0x80000000;

  kprintf("kexec exit\n");
  return 0;
}
