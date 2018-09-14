#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void start();
void eexit();

void main(void){
  start();
  printf("DEEEEEEEEER\n");
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