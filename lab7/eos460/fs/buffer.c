
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

  // add all buffers into the freelist
  freelist = &buffer[0];
  struct buf *tmp = freelist;

  for (int i = 1; i < NBUF; i++)
  {
    tmp->next_free = &buffer[i];
    tmp->buf = 700000 + i*1024;
    tmp = tmp->next_free;
  }

  // initialize lock
  for (int i = 0; i < NBUF; i++)
  {
    buffer[i].lock.value = 1;
  }

  // initialize freebuf
  freebuf.value = 1;
}

/* getblk: return a buffer=(dev,blk) for exclusive use */
struct buf *getblk(int dev, int blk)
{
  struct buf *bp;

  while (1)
  {
    P(freebuf); // get a free buffer first
    bp = devtab[dev].dev_list;
    // (2). if bp in dev_list
    while (bp)
    {
      if (bp->dev == dev && bp->blk == blk) // bp in dev_list
      {
        printf("bp in dev_list\n");
        if (bp->busy == 0) // bp not BUSY
        {
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

          P(bp->lock);
          return bp;
        }
        // bp in cache but BUSY
        V(freebuf); // give up the free buffer
        P(bp->lock); // wait in bp queue
        return bp;
      }
      bp = bp->next_dev;
    }
    printf("getblk bp not in cache\n");
    // bp not in cache, try to create a bp=(dev, blk)
    bp = freelist;
    if (bp)
      freelist = bp->next_free;

    P(bp->lock);
    if (bp->dirty)
    {
      awrite(bp); // write bp out ASYNC, no wait
      continue;
    }

    // mark bp data invalid, not dirty
    bp->dev = dev;
    bp->blk = blk;
    bp->valid = 0;
    bp->dirty = 0;
    return bp;
  }
} 


struct buf *bread(int dev, int blk)
{
  struct buf *bp = getblk(dev, blk); // get a buffer for (dev,blk)
  if (bp->valid == 0) {
    bp->opcode = 0x18; // READ
    printf("bread bp: dev=%d  blk=%d\n", bp->dev, bp->blk);
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
  if (bp->lock.queue)
  {
    V(bp->lock);
    return;
  }
  if (bp->dirty && freebuf.queue)
  {
    awrite(bp);
    return;
  }

  struct buf *tmp = freelist;
  while (tmp->next_free)
    tmp = tmp->next_free;
  tmp->next_free = bp;

  V(bp->lock);
  V(freebuf);
}

int khits(int y, int z) // syscall hits entry
{
   *(int *)(y) = requests;
   *(int *)(z) = hits;
   return NBUF;  
}

