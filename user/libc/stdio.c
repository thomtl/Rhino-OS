#include "include/stdio.h"
#include "include/sys.h"
#include "include/unistd.h"

uint8_t putchar(uint8_t c){
    syscall(13, c, 0, 0);
    return c;
}

void printf(char* msg){
    while(*msg){
        //syscall(1, 1, *m++, 0);
        putchar(*msg);
        msg++;
    }
}

char getchar(){
    return syscall(23, 0, 0, 0);
}

char* getline(char *lineptr, size_t bufsize){
  for(uint16_t i = 0; i < bufsize; i++){
    lineptr[i] = '\0';
  }

  char c;
  uint32_t pos = 0;
  while(1){

    c = getchar();
    if(c == '\b'){
      lineptr[pos - 1] = ' ';
      pos -= 2;
    } else if(c == '\n'){
      lineptr[pos] = '\0';
      return lineptr;
    } else {
      lineptr[pos] = c;
    }
    pos++;
  }

}

void clear_screen(){
    syscall(14, 0, 0, 0);
}