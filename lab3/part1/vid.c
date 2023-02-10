// vid.c file: implement fbuf for the ARM PL110 LCD display
/**************** Reference: ARM PL110 and DUI02241 ********************
Color LCD base address: 0x10120000 - 0x1012FFFF
00    timing0
04    timing1
08    timing2
0C    timing3
10    upperPanelframeBaseAddressRegister // use only ONE panel
14    lowerPanelFrameBaseAddressRegister // some display allows 2 panels
18    interruptMaskClrregister
1C    controlRegister
20    interruptStatusReg
etc
************************************************************************/
#include "font0"

char *tab = "0123456789ABCDF";
u8 cursor;
int volatile *fb;
unsigned char *font;
int row, col;

int fbuf_init()
{
  int x, i;
  /********* for VGA 640x480 ************************/
  *(volatile unsigned int *)(0x1000001c) = 0x2C77;        // LCDCLK SYS_OSCCLK
  *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;    // time0
  *(volatile unsigned int *)(0x10120004) = 0x090B61DF;    // time1
  *(volatile unsigned int *)(0x10120008) = 0x067F1800;    // time2
  *(volatile unsigned int *)(0x10120010) = (2*1024*1024); // fbuf Address
  *(volatile unsigned int *)(0x10120018) = 0x82B;         // control register

  fb = (int *)(2*1024*1024);  // fbuf[] at 2MB area; 
  font = fonts0;              // use fonts0 for char bitmap

  // for 640x480 VGA mode display
  for (x=0; x<640*480; x++)
    fb[x] = 0x00000000;       // clean screen; all pixels are BLACK
  cursor = 127;               // cursor bit map in font0 at 127
}

int clrpix(int x, int y)
{
  int pix = y*640 + x;
  fb[pix] = 0x00000000;
}

int setpix(int x, int y)
{
  int pix = y*640 + x;
  if (color==RED)
     fb[pix] = 0x000000FF;
  if (color==BLUE)
     fb[pix] = 0x00FF0000;
  if (color==GREEN)
     fb[pix] = 0x0000FF00;
  if (color==WHITE)
     fb[pix] = 0x00FFFFFF;
  if (color==YELLOW)
     fb[pix] = 0x0000FFFF;
  if (color==CYAN)
    fb[pix] = 0x00FFFF00;
}

int dchar(unsigned char c, int x, int y)
{
  int r, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;

  for (r=0; r<16; r++){
    byte = *(caddress + r);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  setpix(x+bit, y+r);
    }
  }
}

int undchar(unsigned char c, int x, int y)
{
  int row, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;

  for (row=0; row<16; row++){
    byte = *(caddress + row);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  clrpix(x+bit, y+row);
    }
  }
}

int scroll()
{
  int i;
  for (i=0; i<640*480-640*16; i++){
    fb[i] = fb[i+ 640*16];
  } 
}
  
int kpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;

   dchar(c, x, y);
}

int unkpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;

   undchar(c, x, y);
}


int erasechar()  // erase char at (row,col)
{ 
   int r, bit, x, y;
  unsigned char *caddress, byte;
  x = col*8;
  y = row*16;
  for (r=0; r<16; r++){
     for (bit=0; bit<8; bit++){
        clrpix(x+bit, y+r);
    }
  }
}

int clrcursor()
{
  unkpchar(cursor, row, col);
}

int putcursor()
{
  kpchar(cursor, row, col);
}

int kputc(char c)
{
  clrcursor();
  if (c=='\r'){
    col=0;
    putcursor();
    return;
  }
  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }
    putcursor();
    return;
  }
  if (c=='\b'){
    if (col>0){
      col--;
      erasechar();
      putcursor();
    }
    return;
  }

  // c is ordinary char
  kpchar(c, row, col);
  col++;
  if (col>=80){
    col = 0;
    row++;
    if (row >= 25){
      row = 24;
      scroll();
    }
  }
  putcursor(); 

}
int kputs(char *s)
{
  while(*s){
    kputc(*s);
    if (*s == '\n')
      kputc('\r');
    s++;
  }
}
