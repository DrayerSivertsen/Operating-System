
#include "../type.h"

struct buf buffer[NBUF];
struct buf *freelist;
struct semaphore freebuf;
int freelist_wanted = 0; // free list wanted flag

struct devtab devtab[NDEV];

int requests, hits;

struct buf readbuf, writebuf;  // 2 dedicated buffers

int binit()
{
   readbuf.buf = 0x700000;
  writebuf.buf = 0x700000 + 1024;
}

/* getblk: return a buffer=(dev,blk) for exclusive use */
struct buf *getblk(int dev, int blk)
{
  struct buf *bp;
  while (1)
  {
    // (1). search dev_list for a bp=(dev, blk)
    bp = devtab[dev].dev_list;
    // (2). if bp in dev_list
    while (bp)
    {
      if (bp->dev == dev && bp->blk == blk)
      {
        if (bp->busy == 1)
        {
          bp->wanted = 1;
          ksleep(bp); // wait for bp to be released
          continue;
        }
        else
        {
          // bp not busy
          bp = freelist;
          struct buf *prev;
          while (bp)
          {
            if (bp->dev == dev && bp->blk == blk)
            {
              // remove bp from free list
              if (bp == freelist)
              {
                freelist = bp->next_free;
              }
              else
              {
                prev->next_free = bp->next_free;
              }
              break;
            }
            prev = bp;
            bp = bp->next_free;
          }

          bp->busy = 1;
          return bp;
        }
      }
      bp = bp->next_dev;
    }
    // (3). bp not in cache, try to get a free buf from freelist
    if (!freelist)
    {
      freelist_wanted = 1;
      ksleep(freelist); // wait for any free buffer
      continue; // retry the algorithm
    }
    // (4). freelist not empty
    else
    {
      bp = freelist;
      if (bp)
        freelist = bp->next_free;
      bp->busy = 1;
      if (bp->dirty) // bp is for deplayed write
      {
        awrite(bp); // write bp out ASYNC
        continue;
      }
      // (5). reassign bp to (dev,blk); set bp data invalid, etc
      bp->dev = dev;
      bp->blk = blk;
      bp->valid = 0;
    }
  }
} 


struct buf *bread(int dev, int blk)
{
  struct buf *bp = getblk(dev, blk); // get a buffer for (dev,blk)
  if (bp->valid == 0) {
    bp->opcode = 0x18; // READ
    getblock(bp->blk, bp->buf);
    bp->valid = 1;
  }
  return bp;
}

int bwrite(struct buf *bp) // release bp marked VALID but DIRTY
{ 
  bp->valid = 1;
  bp->dirty = 1;
  brelse(bp);
}

int awrite(struct buf *bp) // write DIRTY bp out to disk
{
  bp->opcode = 0x25; // WRITE
  putblock(bp->blk, bp->buf);
  bp->dirty = 0; // turn off DIRTY flag
  brelse(bp); // bp VALID and in buffer cache
}

/* brelse: releases a buffer as FREE to freelist */
int brelse(struct buf *bp)
{
  if (bp->wanted)
    kwakeup(bp); // wakeup ALL proc's sleeping on bp
  if (freelist_wanted)
    kwakeup(freelist); // wakeup ALL proc's sleeping on bp

  // clear free list wanted flags
  bp->wanted = 0;
  freelist_wanted = 0;
  
  struct buf *tmp = freelist;
  while (tmp->next_free)
    tmp = tmp->next_free;
  tmp->next_free = bp;
}

int khits(int y, int z) // syscall hits entry
{
   *(int *)(y) = requests;
   *(int *)(z) = hits;
   return NBUF;  
}

