#include <stdint.h>
#include <stddef.h>
void printf(char *c);
void putchar(char c);

void main(void){
  putchar('H');
  /*putchar('e');
  putchar('l');
  putchar('l');
  putchar('o');
  putchar(' ');
  putchar('W');
  putchar('o');
  putchar('r');
  putchar('l');
  putchar('d');*/
  printf("Hello External World");
  return;
}

void printf(char *c){
  putchar('Q');
  for(uint8_t i = 0; i < 20; i++){
    putchar('Q');
    putchar(*c);
    c++;
  }
}
void putchar(char c){
  __asm__ __volatile__ ("pusha; movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80; popa" : : "d" (c));
}
