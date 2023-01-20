#include "sdc.h"
u32 base;


int delay()
{
  int i; for (i=0; i<1000; i++);
}

int do_command(int cmd, int arg, int resp)
{
  *(u32 *)(base + ARGUMENT) = (u32)arg;
  *(u32 *)(base + COMMAND)  = 0x400 | (resp<<6) | cmd;
  delay();
}

int sdc_init()
{
  u32 RCA = (u32)0x45670000; // QEMU's hard-coded RCA
  base    = (u32)0x10005000; // PL180 base address
  uprints("sdc_init : ");
  *(u32 *)(base + POWER) = (u32)0xBF; // power on
  *(u32 *)(base + CLOCK) = (u32)0xC6; // default CLK

  // send init command sequence
  do_command(0,  0,   MMC_RSP_NONE);// idle state
  do_command(55, 0,   MMC_RSP_R1);  // ready state  
  do_command(41, 0x0000FFFF,   MMC_RSP_R3);  // argument must not be zero
  do_command(2,  0,   MMC_RSP_R2);  // ask card CID
  do_command(3,  RCA, MMC_RSP_R1);  // assign RCA
  do_command(7,  RCA, MMC_RSP_R1);  // transfer state: must use RCA
  do_command(16, 512, MMC_RSP_R1);  // set data block length
 
  uprints("done\n\r");
}

int getSector(int sector, char *buf)
{
  u32 cmd, arg, status, status_err, *up;
  int i, count = 512;

  *(u32 *)(base + DATATIMER) = 0xFFFF0000;
  // write data_len to datalength reg
  *(u32 *)(base + DATALENGTH) = 512;

  //printf("dataControl=%x\n", 0x93);
  // 0x93=|9|0011|=|9|DMA=0,0=BLOCK,1=Host<-Card,1=Enable
  *(u32 *)(base + DATACTRL) = 0x93;

  cmd = 17;       // CMD17 = READ single block
  arg = (sector*512);
  do_command(cmd, arg, MMC_RSP_R1);

  up = (u32 *)buf;

  status = *(u32 *)(base + STATUS);
  status_err = status & (SDI_STA_DCRCFAIL|SDI_STA_DTIMEOUT|SDI_STA_RXOVERR);

  while (!status_err && count) {
     while(status & (1<<17) == 0); // wait for FIFO full
     // read 16 u32 from FIFO at at time
     for (i = 0; i < 16; i++){
        *(up + i) = *(u32 *)(base + FIFO);
     }
     up += 16;
     count -= 64;
     status = *(u32 *)(base + STATUS); // read status to clear Rx interrupt
     status_err = status & (SDI_STA_DCRCFAIL|SDI_STA_DTIMEOUT|SDI_STA_RXOVERR);
  }
  //printf("write to clear register\n");
  *(u32 *)(base + STATUS_CLEAR) = 0xFFFFFFFF;
}

int getblk(int blk, char *buf)
{
  getSector(2*blk, buf);
  getSector(2*blk + 1, buf + 512);
}


/**************
int putSector(int sector, char *buf)
{
  u32 cmd, arg, status, status_err, *up;
  int i, count = 512;

  *(u32 *)(base + DATATIMER) = 0xFFFF0000;
  *(u32 *)(base + DATALENGTH) = 512;

  cmd = 24;      // CMD24 = Write single block
  arg = (u32)(sector*512);
  do_command(cmd, arg, MMC_RSP_R1);

  //printf("dataControl=%x\n", 0x91);
  // write 0x91=|9|0001|=|9|DMA=0,BLOCK=0,0=Host->Card, Enable
  *(u32 *)(base + DATACTRL) = 0x91; // Host->card

  up = (u32 *)buf;
  status = *(u32 *)(base + STATUS); // read status
  status_err = status & (SDI_STA_DCRCFAIL | SDI_STA_DTIMEOUT);

  while (!status_err && count) {
    while((status & (1<<18))==0); // wait for FIFO empty
    // write 16 u32 at a time
    for (i = 0; i < 16; i++){
        *(u32 *)(base + FIFO) = *(up + i);
    }
    up += 16;
    count -= 64;
    status = *(u32 *)(base + STATUS); // read status to clear Tx interrupt
    status_err = status & (SDI_STA_DCRCFAIL | SDI_STA_DTIMEOUT);
  }
  //printf("write to clear register\n");
  *(u32 *)(base + STATUS_CLEAR) = 0xFFFFFFFF;
}

int putblk(int blk, char *buf)
{
  putSector(2*blk, buf);
  putSector(2*blk + 1, buf + 512);
}
*************/
