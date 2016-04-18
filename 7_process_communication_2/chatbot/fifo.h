#ifndef FIFO_H
#define FIFO_H
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
/* ------------------------------------------------------------- */

#define MESSAGE_BUF_SIZE 100
#define FIFO_NAME_BUF_SIZE 100
#define PERM_FILE S_IRUSR|S_IWUSR
#define MAX 100
#define SHORT 10
#define VERYSHORT 2
/* ------------------------------------------------------------- */

char *response[]= {
     "How are you this beautiful day ?",
     "Did you have a happy childhood ?",
     "Did you hate your father ?",
     "Did you have a good friend ?",
     "Did you like your friend ?",
     "What do you think about your friendship ?",
     "I'm not sure I understand .",
     ""
};

char *trans[]={
     "no","Tell me more about the detail .",
     "yes","Tell me more about the detail .",
     "fuck","Don't be so rude again !",
     "shet","Don't be so rude again !",
     "you","Let't not talk about me .",
     "think","Why do you think that ?",
     "hate","So you hate something -- tell me more .",
     "what","Why do you ask ?",
     "want","Why do you want that ?",
     "need","We all need many things -- is this special ?",
     "why","Remember,therapy is good for you .",
     "know","How do you know that ?",
     "bye","Your bill will be mailed to you .",
     "murder","I don't like killing .",
     "kill","It is wrong to kill .",
     "jerk","Don't ever call me a jerk !",
     "can't","Don't be negative -- be positive .",
     "failure","Strive for success .",
     "never","Don't be negative -- be positive .",
     "unhappy","Why are you unhappy ?",
     "beautiful","I'm sure you like her,don't you ?",
     "like","Why do you like that?",
     "love","Remember,love everthing what you love .",
     ""
};
char topics[MAX][80];
char token[80];
char *p_pos;
int res=0;
int head=0;
int tail=0;
int i;

void assert_topic(char *);
void get_token(void);
int is_in(char, char *);
int lookup(char *);
int find_topic(char *);
int in_topics(char *);

/* ------------------------------------------------------------- */


/* Lookup a keyword in translation table  */
int lookup (char *token)
{
    int t;
    t=0;
    while (*trans[t])  {
          if (!strcmp (trans[t],token))  return t ;
          t++;
    }
    return -1;
}

/* place a topic into the topics database  */
void assert_topic (char *t)
{
    if (head==MAX)  head=0;  /* wrap around */
    strcpy(topics[head],t);
    head++;
}

/* retrieve a topic  */
int find_topic (char *t)
{
    if (tail!=head)  {
       strcpy(t,topics[tail]);
       tail++;
       /* wrap around if necessary */
       if (tail==MAX) tail=0;
       return 1;
    }
    return 0;
}

/* see if in topics queue */
int in_topics (char *s)
{
    int t;
    for (t=0;t<MAX;t++)
        if (!strcmp(s,topics[t]))  return 1;
    return 0;
}

/* return a token from the input stream */
void get_token (void)
{
    char *p;
    p=token;
    /* skip spaces */
    while (*p_pos==' ') p_pos++;

    if (*p_pos=='\0') {  /*is end of input*/
       *p++='\0';
       return;
    }
    if (is_in(*p_pos,".!?")) {
       *p=*p_pos;
       p++,p_pos++;
       *p='\0';
       return ;
    }

    /*read word until*/
    while(*p_pos!=' '&&!is_in(*p_pos,".,;?!")&&*p_pos)  {
         *p=tolower(*p_pos++);
         p++;
    }
    *p='\0';
}

int is_in(char c, char *s)
{
    while(*s) {
       if(c==*s) return 1;
       s++;
    }
    return 0;
}

/* ------------------------------------------------------------- */

struct message
{
  pid_t pid;
  char data[MESSAGE_BUF_SIZE];
};
/* ------------------------------------------------------------- */

void make_srv_fifo_queue_name(char* dest, char* basename, size_t namemax)
{
  snprintf(dest, namemax, "fifo_%s", getlogin());
}
/* ------------------------------------------------------------- */

void make_cnt_fifo_queue_name(char* dest, pid_t p, size_t namemax)
{
  snprintf(dest, namemax, "fifo%ld", (long)p);
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

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}
/* ------------------------------------------------------------- */

#endif
