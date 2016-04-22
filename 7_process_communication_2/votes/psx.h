#ifndef PSX_H
#define PSX_H
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* ------------------------------------------------------------- */

#define MESSAGE_BUF_SIZE 100
#define PSX_QUEUE_NAME 100
#define PERM_FILE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
/* ------------------------------------------------------------- */

void clear(char *s) {
  int i;
  i=0;
  while(s[i]!=0) {
    s[i++]=0;
  }
}

char* int2CharPt(int x) {
    int length,i;
    char *result;
    length = 1;
    i = x;
    while(i>9) {
        ++length;
        i /= 10;
    }
    result = malloc(length);
    for(i=0;i<length;++i) {
        result[length-i-1] = (x%10)+48;
        x /= 10;
    }
    return result;
}

void generateVotes(char *s) {
  int forvotes, againstvotes;
  forvotes = rand()%100;
  againstvotes = rand()%100;
  clear(s);
  strcat(s,int2CharPt(forvotes));
  s[strlen(s)]=0;
  strcat(s,",");
  s[strlen(s)]=0;
  strcat(s,int2CharPt(againstvotes));
  s[strlen(s)]=0;
}

struct message
{
  pid_t pid;
  char data[MESSAGE_BUF_SIZE];
};
/* ------------------------------------------------------------- */

void make_srv_psx_name(char* dest, char* name, size_t namemax)
{
  snprintf(dest, namemax, "/psxsrv_%s_%s", name, getlogin());
}
/* ------------------------------------------------------------- */

void make_cnt_psx_name(char* dest, pid_t p, size_t namemax)
{
  snprintf(dest, namemax, "/psxcnt_%ld", (long)p);
}
/* ------------------------------------------------------------- */

/* If returns 0 then user want to exit from application */
int stdin_readall(char* dest, int MAXLEN)
{
  char c;
  int bread = 0, b;
  do
    {
      b = scanf("%c", &c);
      dest[bread] = c;
      bread++;
    }
  while((c != '\n') && (b == 1) && (bread < MAXLEN));

  dest[bread-1] = '\0';

  return strcmp(dest, "q");
}
/* ------------------------------------------------------------- */

#endif
