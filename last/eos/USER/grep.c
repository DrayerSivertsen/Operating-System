#include "ucode.c"

int grep(char *filename)
{
    char mybuf[1024];
    int n;

    int fd = open(filename, 0);
    while (n = fgets(fd, mybuf))
    {
        if (strlen(mybuf) == strlen(argv[1]))
        {
            if (strcmp(mybuf, argv[1]) == 0)
                printf("%s", mybuf);
        }
        else
        {
            if (strstr(mybuf, argv[1]))
                printf("%s", mybuf);
        }
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

    if (argc < 3)
    {
        if (st_tty.st_ino != st0.st_ino) // stdin has been redirected
        {
            while (n = getline(buf))
            {
                buf[n] = 0; // as a null terminated string

                if (strlen(buf) == strlen(argv[1]))
                {
                    if (strcmp(buf, argv[1]) == 0)
                        printf("%s", buf);
                }
                else
                {
                    if (strstr(buf, argv[1]))
                        printf("%s", buf);
                }
            }
        }
        else
        {
            while (n = gets(buf))
            {
                buf[n] = 0; // as a null terminated string

                if (strlen(buf) == strlen(argv[1]))
                {
                    if (strcmp(buf, argv[1]) == 0)
                        printf("%s\n", buf);
                }
                else
                {
                    if (strstr(buf, argv[1]))
                        printf("%s\n", buf);
                }


            }
        }
    }
    else // three arguments
    {
        if (st_tty.st_ino != st1.st_ino) // stdout has been redirected
        {
            int fd = open(argv[2], 0);
            
            while (n = fgets(fd, buf))
            {
                buf[n] = 0;
                if (strlen(buf) == strlen(argv[1]))
                {
                    if (strcmp(buf, argv[1]) == 0)
                        write(1, buf, n);
                }
                else
                {
                    if (strstr(buf, argv[1]))
                        write(1, buf, n);
                }
            }
            
            close(fd);
        }
        else
        {
            grep(argv[2]);
        }
    }
}