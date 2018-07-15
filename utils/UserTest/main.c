
void putchar(char c);

void main(void){
  putchar('1');
  putchar('2');
  putchar('3');
  putchar('4');
  putchar('5');
  putchar('6');
  putchar('7');
  putchar('8');
  putchar('9');
  putchar('0');
  return;
}

void putchar(char c){
  __asm__ __volatile__ ("pusha; movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80; popa" : : "d" (c));
}
