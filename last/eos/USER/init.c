#include "ucode.c"
int console, ttyS0, ttyS1;
int parent() // P1's code
{
    int pid, status;
    while(1)
    {
        printf("INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid == console)
        {
            printf("INIT: forks a new console login\n");
            console = fork(); // fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }
        else if (pid == ttyS0)
        {
            printf("INIT: forks a new console login\n");
            ttyS0 = fork(); // fork another one
            if (ttyS0)
                continue;
            else
                exec("login /dev/ttyS0"); // new console login process
        }
        if (pid == ttyS1)
        {
            printf("INIT: forks a new console login\n");
            ttyS1 = fork(); // fork another one
            if (ttyS1)
                continue;
            else
                exec("login /dev/ttyS1"); // new console login process
        }

        printf("INIT: I just buried an orphan child proc %d\n");
    }
}

main()
{
    int in_console, out_console; // file descriptors for terminal I/O
    int in_ttyS0, out_ttyS0; // file descriptors for S0
    int in_ttyS1, out_ttyS1; // file descriptors for S1

    // set file descriptors 0, 1
    in_console = open("dev/tty0", O_RDONLY);
    out_console = open("dev/tty0", O_WRONLY);
    in_ttyS0 = open("dev/ttyS0", O_RDONLY);
    out_ttyS0 = open("dev/ttyS0", O_WRONLY);
    in_ttyS1 = open("dev/ttyS1", O_RDONLY);
    out_ttyS1 = open("dev/ttyS1", O_WRONLY);

    printf("INIT: fork a login proc on console\n");
    console = fork();

    if (console) // parent to fork processes
    {
        ttyS0 = fork();
        if (getpid() == 1)
        {
            ttyS1 = fork();
        }
    }

    if (getpid() == 2)
    {
        exec("login /dev/tty0");
    }
    else if (getpid() == 3)
    {
        exec("login /dev/ttyS0");
    }
    else if (getpid() == 4)
    {
        exec("login /dev/ttyS1");
    }
    else // parent
    {
        parent();
    }

}