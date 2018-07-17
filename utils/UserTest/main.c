#include <stdint.h>
int putchar(char c);
void clear_screen();
void printf(char *m);
uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

void main(void){
  __asm__ ("sti");
  syscall(0, 6, 0, 0);
  printf("Hello World");
  while(1);
}

void printf(char *m){
  while(*m){
    putchar(*m++);
  }
}

int putchar(char c){
  //int eax;
  //__asm__ __volatile__ ("movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80" : "=a"(eax): "d" (c));
  //return eax;
  return syscall(1, 1, c, 0);
}


void clear_screen(){
  //__asm__ __volatile__ ("movl $0x1, %eax; movl $0x2, %ebx; int $0x80");
  syscall(1, 2, 0, 0);
}

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx){
  uint32_t ret = 80;
  __asm__ __volatile__ ("int $0x80" : "=a"(ret): "a"(eax), "b"(ebx), "c"(ecx), "d" (edx));
  return ret;
}
