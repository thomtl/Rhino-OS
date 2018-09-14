#include <stdint.h>
#include <stdio.h>
#include <sys.h>

void main(void){
  asm("sti");
  syscall(1, 3, 15, 0);
  printf("Hello World\n");
  uint32_t j = syscall(0,7,0,0);
  syscall(2,4,j,0);
  while(1);
}