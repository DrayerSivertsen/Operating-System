ENTRY(start)

SECTIONS
{
  . = 0x10000;

  .text :
   {
      ts.o
      t.o
   }
  .data :
   { 
      ts.o(.data) 
      t.o (.data)
   }
  .bss :
   {
     *(.bss)
   }

  . = ALIGN(8);
  . = . + 0x1000;     /* 4kB of SVC mode stack */
  svc_stack_top = .;

}

