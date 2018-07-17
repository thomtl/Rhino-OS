void start_task_atomic();
void end_task_atomic();
void putchar(char c);
void printf(char *m);


void main(void){
  __asm__ ("sti");
  putchar('x');
  printf("Hello World");
  while(1);
}

void printf(char *m){
  while(*m){
    putchar(*m++);
  }
}

void putchar(char c){
  __asm__ __volatile__ ("pusha; movl $0x1, %%eax; movl $0x1, %%ebx; movl %%edx, %%ecx; int $0x80; popa" : : "d" (c));
}
void start_task_atomic(){
  __asm__ __volatile__ ("pusha; movl $0x0, %eax; movl $0x2, %ebx; int $0x80; popa");
}
void end_task_atomic(){
  __asm__ __volatile__ ("pusha; movl $0x0, %eax; movl $0x3, %ebx; int $0x80; popa");
}
