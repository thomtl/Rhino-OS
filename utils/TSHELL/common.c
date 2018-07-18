#include "common.h"

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx){
  uint32_t ret;
  __asm__ __volatile__ ("int $0x80" : "=a"(ret): "a"(eax), "b"(ebx), "c"(ecx), "d" (edx));
  return ret;
}

void printf(char* m){
    while(*m){
        syscall(1, 1, *m++, 0);
    }
}

void* malloc(size_t sz){
    return (void*)syscall(2, 1, sz, 0);
}
void free(void* addr){
    syscall(2, 2, (uint32_t)addr, 0);
}

void exit(){
    syscall(0, 5, 0, 0);
}

char getchar(){
    return syscall(5, 1, 0, 0);
}

int strcmp(char s1[], char s2[]){
  int i;
  for(i = 0; s1[i] == s2[i]; i++){
    if(s1[i] == '\0') return 0;
  }
  return s1[i] - s2[i];
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
