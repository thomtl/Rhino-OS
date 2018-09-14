#include "include/stdio.h"
#include "include/sys.h"
uint8_t putchar(uint8_t c){
    syscall(1, 1, c, 0);
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
    return syscall(5, 1, 0, 0);
}

char* getline(char *lineptr, size_t bufsize){
  //char str[256] = "";
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
    syscall(1, 2, 0, 0);
}

FILE* fopen(char* filename){
  return (FILE*)syscall(3, 1, (uint32_t)filename, 0);
}

void fread(void* ptr, size_t size, FILE* file){
  syscall(4, (uint32_t)file, size, (uint32_t)ptr);
}

FILE* fread_dir(uint32_t index){
  uint32_t dirent = syscall(3, 2, index, 0);
  if(dirent == 0) return 0;

  return fopen(((struct dirent*)dirent)->name);
}