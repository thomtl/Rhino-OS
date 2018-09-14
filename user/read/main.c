#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

void start();
void eexit();

void main(void){
  start();

  FILE* file = fopen("test.txt");

  char *buf = (char*)malloc(file->length);
  fread(buf, file->length, file);

  for(uint32_t i = 0; i < file->length; i++){
    char b[2] = {buf[i], '\0'};
    printf(b);
  }

  eexit();
}

void start(){
  asm("sti");
  syscall(1, 3, 15, 0);
}

void eexit(){
  syscall(2, 4, syscall(0, 7, 0, 0), 0);
  while(1);
}