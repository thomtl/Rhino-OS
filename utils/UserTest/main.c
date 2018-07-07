#include <stdint.h>
#include <stddef.h>
void printf(char *c);
void putchar(char c);

void main(void){
  putchar('H');
  putchar('e');
  putchar('l');
  putchar('l');
  putchar('o');
  putchar(' ');
  putchar('E');
  putchar('x');
  putchar('t');
  putchar('e');
  putchar('r');
  putchar('n');
  putchar('a');
  putchar('l');
  putchar(' ');
  putchar('W');
  putchar('o');
  putchar('r');
  putchar('l');
  putchar('d');
  return;
}

void printf(char *c){
  while(*c != '\0'){
    putchar(++*c);
  }
}

void putchar(char c){
  __asm__ __volatile__ ("pusha; movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80; popa" : : "d" (c));
}
