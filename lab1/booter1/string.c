/********** string.c **********/
int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int strncpy(char *dest, char *src, int n)
{
  while(n && *src){
    *dest++ = *src++;
    n--;
  }
  *dest = 0;
}
