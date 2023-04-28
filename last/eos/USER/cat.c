#include "ucode.c"

int cat(char *filename)
{
  char mybuf[1024];
  int n;

  int fd = open(filename, 0);
  while (n = read(fd, mybuf, 1024))
  {
    mybuf[n] = 0; // as a null terminated string
    printf("%s", mybuf);
  }

  close(fd);
}

main()
{
    char buf[1024];
    char tmp[128];
    int n;

    struct stat mystat, st_tty, st0, st1;
    char tty_buf[128];


    gettty(tty_buf);

    stat(tty_buf, &st_tty);
    fstat(0, &st0);
    fstat(1, &st1);

    if (argc < 2)
    {
        if (st_tty.st_ino != st0.st_ino) // stdin has not been redirected
        {
            while (n = getline(buf))
            {
                buf[n] = 0; // as a null terminated string
                printf("%s", buf);
            }
        }
        else
        {
            while (n = gets(buf))
            {
                buf[n] = 0; // as a null terminated string
                printf("%s\n", buf);
            }
        }
 

    }
    else
    {
        if (st_tty.st_ino != st1.st_ino) // stdout has been redirected
        {
            int fd = open(argv[1], 0);
            
            while (n = read(fd, buf, 1024))
            {
                buf[n] = 0;
                write(1, buf, n); // fix the size of print
            }
            
            close(fd);
        }
        else
        {
            cat(argv[1]);
        }
    }
}