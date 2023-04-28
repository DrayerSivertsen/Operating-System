#include "ucode.c"
char cmdline[128];
char *cmd;
int pid, status;
char tmp_line[128];
 
char head[128];
char tail[128];
 
int scan(char *cmdLine)
{
    int i = argc; // set to number of args
    while(i > 0)
    {
        i--;
        if(strcmp(argv[i], "|") == 0)
        {
            strcpy(tail, argv[i+1]);
 
            if(argv[i+2])
            {
                strcat(tail, " ");
                strcat(tail, argv[i+2]);
            }
            int j = 0;
            strcpy(head, argv[j]);
            strcat(head, " ");
            j++;
            while(j < i)
            {
                strcat(head, argv[j]);
                strcat(head, " ");
                j++;
            }
 
            return 1;
        }
    }
 
    strcpy(head, argv[0]);
 
    if(argv[1])
    {
        strcat(head, " ");
        strcat(head, argv[1]);
    }
 
    strcpy(tail, "\0");
 
    return 0;
}
 
void do_command(char *command)
{
    strncpy(tmp_line, command, 128);
    token(tmp_line);
    // Check for redirection
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "<") == 0) // redirect stdin
        {
            close(0);
            int fd = open(argv[i+1], O_RDONLY);
            argv[i] = '\0';
        }
        else if(strcmp(argv[i], ">") == 0) // redirect stdout
        {
            close(1);
            int fd = open(argv[i+1], O_WRONLY|O_CREAT);
            argv[i] = '\0';
        }
        else if(strcmp(argv[i], ">>") == 0)
        {
            close(1);
            int fd = open(argv[i+1], O_WRONLY|O_CREAT|O_APPEND);
            argv[i] = '\0';
        }
    }
 
    exec(command);
}
 
void do_pipe(char *cmdLine, int *pd)
{
    printf("child sh %d running : cmd=%s\n", getpid(), cmdLine);
    if (pd)
    {
        close(pd[0]);
        dup2(pd[1], 1);
        close(pd[1]);
    }
 
    // divide cmdLine into head, tail by rightmost pipe symbol
    strncpy(tmp_line, cmdLine, 128);
    token(tmp_line);
    int hasPipe = scan(cmdLine);
    printf("head: %s, tail: %s\n", head, tail);
 
    if(hasPipe)
    {
        int lpd[2];
        pipe(lpd);
        int pid = fork();
        if(pid)
        {
            close(lpd[1]);
            dup2(lpd[0], 0);
            close(lpd[0]);
            printf("tail before doCommand: %s\n", tail);
            do_command(tail);
        }
        else
        {
            printf("head before do_pipe: %s\n", head);
            do_pipe(head, lpd);
        }
    }
    else
    {
        do_command(cmdLine);
    }
}
 
 
main()
{
    signal(2, 1);

    while (1)
    {
        // get command line from user
        printf("sh %d# ", getpid());
        gets(cmdline);
 
        strncpy(tmp_line, cmdline, 128);
        token(tmp_line);
 
        // printf("cmdline: %s\n", cmdline);
        cmd = argv[0];

        if (line[0] == 0)
            continue;
 
        // if command is non-trivial (not cd or exit)
        if (strcmp(cmd, "cd") == 0)
        {
            if (argv[1])
                chdir(argv[1]);
            else
                chdir(home);
            continue;
        }
        else if (strcmp(cmd, "logout") == 0)
            exit(0);
        else if (strcmp(cmd, "exit") == 0)
            exit(0);
 
        // fork a child process to execute the command line and waits for the child to terminate
        pid = fork();
        if (pid)
        {
            printf("parent sh %dforked a child %d\n", getpid(), pid);
            pid = wait(&status);
            printf("child %dexit status = %d\n", pid, status);
        }
        else
            do_pipe(cmdline, 0);
 
    }
 
 
}