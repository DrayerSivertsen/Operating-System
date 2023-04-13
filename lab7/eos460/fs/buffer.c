
#include "../type.h"

struct buf buffer[NBUF];
struct buf *freelist;
struct semaphore freebuf;

struct devtab devtab[NDEV];

int requests, hits;

struct buf readbuf, writebuf;  // 2 dedicated buffers

int binit()
{
   readbuf.buf = 0x700000;
  writebuf.buf = 0x700000 + 1024;
}

struct buf *getblk(int dev, int blk)
{
  // just need a buffer, so let it be the writebuf
  struct buf *bp = &writebuf;
  bp->dev = dev; bp->blk = blk;
  bp->dirty = 0;
  return bp;
} 

struct buf *bread(int dev, int blk)
{
  // use readbuf to read in a block; return readbuf pointer
  //printf("bread %d %d ", dev, blk);
  
  struct buf *bp = &readbuf;
  bp->dev = dev; bp->blk = blk;
  getblock(blk, bp->buf);
  //printf("end bread\n");
  return bp;
}

int bwrite(struct buf *bp)
{ 
  // write out bp->buf to bp->blk
  putblock(bp->blk, bp->buf);
}

int awrite(struct buf *bp)
{
  // no async write; all writes are immediate
  bwrite(bp);
}

int brelse(struct buf *bp)
{
  // no need to release any buffer
}

int khits(int y, int z) // syscall hits entry
{
   *(int *)(y) = requests;
   *(int *)(z) = hits;
   return NBUF;  
}

