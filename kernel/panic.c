#include "panic.h"
__attribute__((noreturn))
void panic(){
  __asm__ __volatile__("cli");
  clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!");
  while(1){

  }
}

__attribute__((noreturn))
void panic_m(char* m){
  __asm__ __volatile__("cli");
  //clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!\n");
  kprint(m);
  while(1){

  }
}

__attribute__((noreturn))
void panic_assert(char *file, uint32_t line, char *desc){
  __asm__ __volatile__("cli");

  kprint("ASSERTION-FAILED(");
  kprint(desc);
  kprint(") at ");
  kprint(file);
  kprint(":");
  char c[5];
  int_to_ascii(line, c);
  kprint(c);
  kprint("\n");
  while(1);
}
