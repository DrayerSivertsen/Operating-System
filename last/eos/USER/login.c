// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
#include "ucode.c"
int in, out, err; 
char username[128], password[128];
char buf[256], tmp[256];
 
main(int argc, char *argv[])
{
    // (1). close file descriptors 0,1 inherited from INIT.
    close(0);
    close(1);
 
    // (2). open argv[1] 3 times as in(0), out(1), err(2)
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_RDWR);
    printf("LOGIN : open %s as stdin=%d, stdout=%d, stderr=%d\n", argv[1], O_RDONLY, O_WRONLY, O_RDWR);
 
    // (3). set tty name string in PROC.tty
    settty(argv[1]);
 
    // (4). open /etc/passwd
    int fd = open("etc/passwd", O_RDONLY);
 
    while (1)
    {
        // get user login and password
        printf("login:"); gets(username);
        printf("password: "); gets(password);
 
        read(fd, buf, 256);
        char *bp = buf;
 
        while (*bp != 0) // loop through all lines in /etc/passwd
        {
            char *cp = tmp;
 
            while (*bp != '\n') // next line
            {
                *cp++ = *bp++;
            }
            *cp = 0;
            *bp++;
 
            token_login(tmp); // tokenize login
 
            if (strcmp(username, argv[0]) == 0 && strcmp(password, argv[1]) == 0)
            {
                printf("login successful\n");
 
                chuid(atoi(argv[2]), atoi(argv[3])); // change uid, gid to user's uid, gid
                chdir(argv[5]); // change cwd to user's home dir
                home = argv[5];
                close(fd); // close /etc/passwd
                exec(argv[6]); // exec program in user account
            }
        }
        printf("login failed, try again\n");
    }
}
 
void token_login(char *line)
{
  char *cp;
  cp = line;
  argc = 0;
 
  while (*cp != 0)
  {
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           argv[argc++] = cp;         
       while (*cp != ':' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break;
       cp++;
  }
  argv[argc] = 0;
}