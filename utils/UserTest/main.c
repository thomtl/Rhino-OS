void start_task_atomic();
void end_task_atomic();
int putchar(char c);
void printf(char *m);
void clear_screen();
#include <stdint.h>


void main(void){
    __asm__ ("sti");
  printf("Hello World");
  while(1);
}

void printf(char *m){
  while(*m){
    putchar(*m++);
  }
}

int putchar(char c){
  int eax;
  __asm__ __volatile__ ("movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80" : "=a"(eax): "d" (c));
  return eax;
}
void clear_screen(){
  __asm__ __volatile__ ("movl $0x1, %eax; movl $0x2, %ebx; int $0x80");
}
void start_task_atomic(){
  __asm__ __volatile__ ("movl $0x0, %eax; movl $0x2, %ebx; int $0x80");
}
void end_task_atomic(){
  __asm__ __volatile__ ("movl $0x0, %eax; movl $0x3, %ebx; int $0x80");
}
