#include "ucode.c"


void l2u(char *filename, char *newfile)
{
    char c;
    char buf[1024];
    int count = 0;

    int fd_old = open(filename, O_RDONLY);
    int fd_new = open(newfile, O_WRONLY|O_CREAT);


    while (c = fgetc(fd_old))
    {
        if (c <= 'z' && c >= 'a')
        {
            c -= 32;
        }

        if (c == 255)
        {
            write(fd_new, buf, count);
            break;
        }

        buf[count] = c;

        if (count == 1023)
        {
            printf("just wrote\n");
            write(fd_new, buf, count);
            count = 0;
        }
        count += 1;

    }

    close(fd_old);
    close(fd_new);
}


main()
{
    if (argc < 3)
    {
        char c;

        while (c = getc())
        {
            if (c <= 'z' && c >= 'a')
            {
                c -= 32;
            }

            if (c == 255)
                break;

            putc(c);
        }
    }
    else
    {
        l2u(argv[1], argv[2]);
    }
}