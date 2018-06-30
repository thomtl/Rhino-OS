#include <stdint.h>
void printf(char *c);
void putchar(char c);
void _start(void){
  //putchar('Q');
  printf("Hello External World");
  return;
}

void printf(char *c){
  int offset = 0;
  while(c[offset] != '\0'){
    putchar(c[offset++]);
  }
}
void putchar(char c){
  __asm__ __volatile__ ("pusha; movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80; popa" : : "d" (c));
}
