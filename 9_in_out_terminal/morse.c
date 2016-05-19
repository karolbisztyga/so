#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

int errorHandler(char*);
 
int main(void) {

  struct termios t;
  t.c_lflag &= ~ICANON;
  t.c_lflag &= ~ECHO;
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  if(tcsetattr(STDIN_FILENO, TCSANOW, &t)==-1) {
    errorHandler("set attr");
  }

  char *kod[]={".- ","-... ","-.-. ","-.. ",". ","..-. ","--. ",".... ",".. ",".--- ",
	        "-.- ",".-.. ","-- ","-. ","--- ",".--. ","--.- ",".-. ","... ","- ",
	        "..- ","...- ",".-- ","-..- ","-.-- ","--.. "};
  char bufor[6] = "";
  char znak;
 
  while(read(0,&znak,1) && znak != '\n') {
    strcpy(bufor,kod[znak-'a']);
    write(1,bufor, 6);
  }
 
  return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}