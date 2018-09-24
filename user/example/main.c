#include <stdint.h>
#include <stdio.h>
#include <sys.h>

void main(void){
  syscall(0,3,0,0);
  syscall(1, 3, 15, 0);
  printf("Hello World\n");
  uint32_t j = syscall(0,7,0,0);
  syscall(0,14,j,0);
  while(1);
}