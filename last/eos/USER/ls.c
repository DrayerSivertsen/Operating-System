#include "ucode.c"
 
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
struct stat mystat, *sp;
 
char* basename(char *dir) 
{
    char *cp = dir;
    char *bname;
 
    while (*cp != 0) 
    {
        bname = cp;
        while (*cp != '/' && *cp != 0) // increment until / is found or end of path
            *cp++;
        *cp = 0;
        *cp++;
    }
 
    return bname;
}
 
void ls_file (char *name)
{
    char linkname[64];
    char ftime[64];
    char *tmp;
    int r;
    sp = &mystat;
 
    if ( (r = stat(name, sp)) < 0)
    {
        printf("failed to ls file\n");
        return;
    }
 
    if ((sp->st_mode & 0xF000) == 0x8000) { // if (S_ISREG())
        tmp = "-";
        printf("%s", tmp);
    }
    if ((sp->st_mode & 0xF000) == 0x4000) { // if (S_ISDIR())
        tmp = "d";
        printf("%s", tmp);
    }
    if ((sp->st_mode & 0xF000) == 0xA000) { // if (S_ISLNK())
        tmp = "l";
        printf("%s", tmp);
    }
    for (int i=8; i >= 0; i--) 
    {
        if (sp->st_mode & (1 << i))
            printc(t1[i]);
        else
            printc(t2[i]);
    }
 
    printf(" %d",sp->st_nlink); // link count
    printf(" %d",sp->st_gid); // gid
    printf(" %d",sp->st_uid); // uid
 
 
    printf(" %d",sp->st_size); // file size
 
    char tmp_line[128];
    strcpy(tmp_line, name);
    printf("%s", basename(tmp_line)); // print file basename
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000)== 0xA000) {
        // use readlink() to read linkname
        readlink(name, linkname);
        printf(" -> %s", linkname); // print linked name
    }
    printf(" \n");
}
 
int ls_dir(char *dname)
{
    char filename[128], buf[1024], tmp[128];
    DIR *dp;
    char *cp;
 
    int fd = open(dname, O_RDONLY);
    read(fd, buf, 1024);
    dp = (DIR *)buf;
    cp = buf;
 
    while (cp < buf + 1024) // loop through all entries in the directory
    {
        strncpy(tmp, dp->name, 128);
        tmp[dp->name_len] = 0;
 
        strcpy(filename, dname);
        strcat(filename, "/");
        strcat(filename, tmp);
        ls_file(filename);
 
        cp += dp->rec_len; // advance to next entry
        dp = (DIR *)cp;
    }
}
int main()
{
    struct stat mystat, *sp;
    sp = &mystat;
 
    int r;
    char *path;
    char filename[1024], cwd[1024];
 
 
    if (argc == 1)
        path = "./";
    else
        path = argv[1];
 
 
    if ((r = stat(path, sp)) < 0)
    {
        exit(1); // path not found
    }
 
    strcpy(filename, path);
    if (path[0] != '/') // relative or absolute path
    {
        getcwd(cwd);
        strcpy(filename, cwd);
        strcat(filename, "/");
        strcat(filename, path);
    }
 
    if ((sp->st_mode & 0xF000) == 0x4000) // check for file or directory
        ls_dir(filename);
    else
        ls_file(filename);
}