#include "ucode.c"

int ls_file(MINODE *mip, char *name, int dev)
{
    // READ Chapter 11.7.3 HOW TO ls

    char ftime[64];
    char linkname[60];
    int i;
    char *t1 = "xwrxwrxwr-------";
    char *t2 = "----------------";

    if ((mip->INODE.i_mode & 0xF000) == 0x8000)
    { // if (S_ISREG())
        printf("%c", '-');
    }
    if ((mip->INODE.i_mode & 0xF000) == 0x4000)
    { // if (S_ISDIR())
        printf("%c", 'd');
    }
    if ((mip->INODE.i_mode & 0xF000) == 0xA000)
    { // if (S_ISLNK())
        printf("%c", 'l');
    }
    for (i = 8; i >= 0; i--)
    {
        if (mip->INODE.i_mode & (1 << i))
        { // print r|w|x
            printf("%c", t1[i]);
        }
        else
        {
            printf("%c", t2[i]); // or print -
        }
    }
    printf("%4d ", mip->INODE.i_links_count); // link count
    printf("%4d ", mip->INODE.i_gid);         // gid
    printf("%4d ", mip->INODE.i_uid);         // uid

    // print time

    time_t time = (time_t)mip->INODE.i_ctime;
    strcpy(ftime, ctime(&time));  // print time in calendar form
    ftime[strlen(ftime) - 1] = 0; // kill \n at end
    printf("%s ", ftime);

    printf("%8d \t", mip->INODE.i_size); // file size

    // print name
    printf("%s", basename(name)); // print file basename
    // print -> linkname if symbolic file
    if ((mip->INODE.i_mode & 0xF000) == 0xA000)
    {
        // use readlink() to read linkname
        // errno = 0;
        my_readlink(basename(name), linkname);
        // fprintf(stderr, "errno: %d\n", errno);
        printf(" -> %s", linkname); // print linked name
    }

    printf("\t[%d %2d]", mip->dev, mip->ino);

    printf("\n");
}

int ls_dir(MINODE *mip, int dev)
{
    char buf[BLKSIZE], temp[256];
    DIR *dp;
    char *cp;

    get_block(dev, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;
    cp = buf;

    while (cp < buf + BLKSIZE)
    {
        MINODE *current_minode = iget(dev, dp->inode);
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        ls_file(current_minode, temp, dev);
        iput(current_minode);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    printf("\n");
}

int ls(char *pathname)
{
    struct stat* sp; // stat pointer
    stat(pathname, sp);
    int dev, lsIno;

    if (strcmp(pathname, "")) // check if pathname is empty
    {
        lsIno = getino(pathname, &dev);
        if (lsIno == 0)
        {
            printf("ERROR: file/directory not found!\n");
            return -1;
        }

        stat(pathname, sp);

        if (S_ISDIR(sp->st_mode)) // pathname is a directory
        {
            ls_dir(lsINODE, dev);
        }
        else // pathname is a file
        {
            ls_file(lsINODE, name[n - 1], dev);
        }
        iput(lsINODE); // release minode
    }
    else
    {
        ls_dir(running->cwd, dev); // if no pathname call on cwd
    }
}