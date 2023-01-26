/*********** t.c file **********/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#include "vid.c"
#include "uart.c"

extern char _binary_wsu_bmp_start;

int show_bmp(char *p, int startRow, int startCol)
{ 
   int h, w, pixel, rsize, i, j; 
   unsigned char r, g, b;
   char *pp;
   
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels 
   h = *(q + 1);           // height in pixels
   p += 54;                // p point at pixels now 

   rsize = 4* ((3*w + 3) /4);     // row size is a multiple of 4 bytes  

   for (i=startRow+h/2; i > startRow; i--) // start at last row to print right side up (only need half the height for - 1/2 picture)
   {
      pp = p;
      for (j=startCol; j < startCol+w/2; j++) // (only need half the width for - 1/2 picture)
      {
         b = *pp; g = *(pp+1); r = *(pp+2);
   
         pixel = (b<<16) + (g<<8) + r;
      
         fb[i*640 + j] = pixel;
         pp += 6;    // advance pp to next pixel by 3 bytes (double to skip every other pixel - 1/2 picture)
      }
      p += (rsize*2); // (double to skip every other pixel - 1/2 picture)
   }
   uprintf("\nBMP image height=%d width=%d\n", h, w);
}

int color;
UART *up;

int main()
{
   char *p;

   uart_init();
   up = &uart[0];

   fbuf_init();

   p = &_binary_wsu_bmp_start;
   show_bmp(p, 0, 0); 

   while(1);   // loop here  
}
