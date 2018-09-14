#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void start();
void eexit();

void main(void){
  start();
  int i = 0;
  FILE *node = 0;
  while ( (node = fread_dir(i)) != 0)
  {
    if ((node->flags&0x7) == RHINO_LIBC_FS_DIRECTORY) syscall(1, 3, 1, 0);
    else syscall(1, 3, 2, 0);
    printf(node->name);
    syscall(1, 3, 15, 0);
    printf("  ");
    i++;
  }
  printf("\n");
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