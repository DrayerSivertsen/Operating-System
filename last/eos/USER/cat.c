int my_read(int fd, char *read_buf, int nbytes)
{
  OFT *oftp = running->fd[fd];
  char kbuf[BLKSIZE];
  int count = 0; // number of bytes read
  int offset = oftp->offset; // byte offset in file to READ
  int avil = oftp->minodePtr->INODE.i_size - offset; // compute bytes available in file
  
  // 2) Compute start byte in block
  while (nbytes && avil)
  {
    int lbk = offset / BLKSIZE; // compute logical block
    int start = offset % BLKSIZE; // start byte in block
    
    // 3) Convert logical block number to physical block number
    int blk = read_map(oftp->minodePtr->INODE, lbk);
    
    //int blk = oftp->minodePtr->INODE.i_block[lbk];
    
    // 4) Read blk into char kbuf[BLKSIZE]
    get_block(dev, blk, kbuf);
    char *cp = kbuf + start;
    int remain = BLKSIZE - start;
    int use_remain = remain;

    if (nbytes - remain < 0)
    {
      use_remain = nbytes;
    }
    else if (avil - remain < 0)
    {
      use_remain = avil;
    }

    memcpy(read_buf, cp, use_remain);
    offset += use_remain; 
    count += use_remain;
    avil -= use_remain;
    nbytes -= use_remain;

    // // // 5) Copy bytes from kbuf[] to buf[]
    // while (remain)
    // {
    //   *read_buf++ = *cp++;
    //   offset++; count++; // inc offset, count;
    //   remain--; avil--; nbytes--; // decrease remain, available nbytes
    //   if (nbytes == 0 || avil == 0) 
    //   {
    //     break;
    //   } 
    // } // end of while(remain)
  } // end of while(nbytes && avil)
  oftp->offset = offset;
  return count;
}

int cat(char *filename)
{
  char mybuf[1024];
  int n;

  int fd = my_open(filename, 0);
  while (n = my_read(fd, mybuf, 1024))
  {
    mybuf[n] = 0; // as a null terminated string
    printf("%s", mybuf); // this works but not great
    // handle \n properly

  }
  //putchar('\n');
  my_close(fd);
}

main()
{
    
}