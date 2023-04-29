#include "ucode.c"

int more(char *filename)
{
    char mybuf[1024];
    int n;
    int count = 24;
    char input;

    int fd = open(filename, 0);
    while (n = fgetline(fd, mybuf))
    {
        if (count == 0)
            break;

        printf("%s", mybuf);
        count--;
    }

    while (n = fgetline(fd, mybuf))
    {
        if (count == 0)
        {
            input = getc();
            if (input == ' ')
                count = 24;
            else
                count = 1;
        }

        printf("%s", mybuf);
        count--;
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

    int count = 24;
    char input;

    gettty(tty_buf);

    stat(tty_buf, &st_tty);
    fstat(0, &st0);
    fstat(1, &st1);

    if (argc < 2)
    {
        if (st_tty.st_ino != st0.st_ino) // stdin has been redirected
        {
            int fd = dup(0);
            close(0);
            open(tty_buf, 0);

            while (n = fgetline(fd, buf))
            {
                if (count == 0)
                    break;

                printf("%s", buf);
                count--;
            }

            while (n = fgetline(fd, buf))
            {
                if (count == 0)
                {
                    input = getc();
                    if (input == ' ')
                        count = 24;
                    else
                        count = 1;
                }

                printf("%s", buf);
                count--;
            }
        }
        else
        {
            while (n = gets(buf))
            {
                buf[n] = 0; // as a null terminated string

                if (count == 0)
                    break;

                printf("%s", buf);
                count--;
            }

            while (n = gets(buf))
            {
                buf[n] = 0; // as a null terminated string

                if (count == 0)
                {
                    input = getc();
                    if (input == ' ')
                        count = 24;
                    else
                        count = 1;
                }

                printf("%s", buf);
                count--;
            }
        }
    }
    else
    {
        if (st_tty.st_ino != st1.st_ino) // stdout has been redirected
        {
            int fd = open(argv[1], 0);
            
            while (n = fgetline(fd, buf))
            {
                if (count == 0)
                    break;

                write(1, buf, n);
                count--;
            }

            while (n = fgetline(fd, buf))
            {
                if (count == 0)
                {
                    input = getc();
                    if (input == ' ')
                        count = 24;
                    else
                        count = 1;
                }

                write(1, buf, n);
                count--;
            }
            
            close(fd);
        }
        else
        {
            more(argv[1]); // normal case
        }
    }
}