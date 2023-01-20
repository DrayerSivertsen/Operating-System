int boot()
{ 
  char *name[2];  // name[0]="boot";   name[1]="eos";
  char *address = (char *) 0x100000;  // EOS kernel loading address     

  /*****************************
   sdimage is a disk image containing an EXT2 FS
   EOS kernel image is the file /boot/eos

   t.c:      booter();

             -------- YOUR task is to these ------- 
   booter(): FIND the file /boot/eos
             load its data blocks to 1MB = 0x100000
             return 1 to caller
             -------------------------------------
   caller:   call go() in ts.s:
             mov pc, 0x100000

  ****************************/

  name[0] = "boot";
  name[1] = "eos";

  // 1. get inodes start block # from GD in block 2

  GD *gp;
  INODE *ip;
  DIR *dp;
  char *cp;
  char buf[1024], buf1[1024], buf2[1024];
  char temp[256];

  getblk(2, buf);
  gp = (GD *)buf;
  int iblk = gp->bg_inode_table; // print as %d to see its value
  uprintf("iblk=%d\n", iblk);


  // 2. get the block iblk containing root inode into buf[]
  getblk(iblk, buf);
  ip = (INODE *)buf + 1;

  // 3. search for "boot" in data block of / INODE
  int blk = ip->i_block[0];
  getblk(blk, buf);
  cp = buf;

  uprintf("search name=boot\n");
  
  while (cp < buf + BLKSIZE && strcmp(temp, name[0])) // loop block size starting at buf(start point) or until "boot" found
  {
    cp += dp->rec_len;
    dp = (DIR *)cp;
    strncpy(temp, dp->name, dp->name_len); // make name a string
    temp[dp->name_len] = 0; // null terminate the string
    // uprintf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
    uprintf("%s", temp);
  }
  // uprintf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
  uprintf("found boot\n");

  // 4. user Mailman's algorithm to get INODE of boot
  int inodes_start_block = iblk; // bg_inode_table points to the inodes_start_block (we set it to blk earlier)
  blk = inodes_start_block + (dp->inode-1)/8;
  int offset = (dp->inode-1) % 8;
  getblk(blk, buf);

  ip = (INODE *)buf + offset; // ip is set to /boot at offset
  blk = ip->i_block[0];
  getblk(blk, buf);
  cp = buf; // start cp at buf
  dp = (DIR *)buf; // start dp at buf

  // 5. search for "eos" in data block of /boot's INODE to get its ino

  uprintf("search name=eos\n");
  
  while (cp < buf + BLKSIZE && strcmp(temp, name[1])) // loop block size starting at buf(start point) or until "boot" found
  {
    cp += dp->rec_len;
    dp = (DIR *)cp;
    strncpy(temp, dp->name, dp->name_len); // make name a string
    temp[dp->name_len] = 0; // null terminate the string
    // uprintf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
    uprintf("%s", temp);
  }

  // uprintf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
  uprintf("found eos\n");

  // 6. load eos's INODE block into a buf[] and let ip = INODE of /boot/eos
  blk = inodes_start_block + (dp->inode-1)/8;
  offset = (dp->inode-1) % 8;
  getblk(blk, buf);

  ip = (INODE *)buf + offset; // ip is set to /boot at offset

  // 7. ip->i_block[0]-[11] are direct blocks
  uprintf("load the kernel\n");

  if (ip->i_block[12])
  {
    getblk(ip->i_block[12], buf1);
  }

  char *location = (char *) 0x100000;

  for (int i = 0; i < 12; i++)
  {
    getblk(ip->i_block[i], location);
    uputc('*');
    location += 1024;
  }

  // 8. ip->i_block[12] are indirect blocks; load them into u32 ubuf[256] indirect blocks will be ubuf[0], ubud[1],..., until 0
  u32 *up;

  if (ip->i_block[12])
  {
    up = (u32 *)buf1;
    while (*up)
    {
      getblk(*up, location);
      uputc('.');
      location += 1024;
      up++;
    }
  }

  uprintf("\nloading done\n");

  // 9. when all blocks are loaded, return to assembly code
  return 0;
}  

