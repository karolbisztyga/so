#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX 100
#define SHORT 10
#define VERYSHORT 2

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

int main(int argc, char **argv) {
     int i;
     if(argc <= 1) {
          return 1;
     }
     for(i=1 ; i<argc ; ++i) {
          printf("[%s]",argv[i]);
     }
     for(i=0 ; i<sizeof(trans)/sizeof(trans[0]) ; ++i) {
          printf("%s\n", trans[i]);
     }
     //...
     return 0;
}