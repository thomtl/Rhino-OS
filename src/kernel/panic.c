#include <rhino/panic.h>
__attribute__((noreturn))
void panic(){
  asm("cli");
  clear_screen();
  kprint_err("KERNEL PANIC!\n");
  kprint_err("PLEASE REBOOT SYSTEM!");
  while(1){

  }
}

__attribute__((noreturn))
void panic_m(char* m, char* f){
  asm("cli");
  //clear_screen();
  kprint_err("KERNEL PANIC!\n");
  kprint_err("PLEASE REBOOT SYSTEM!\n");
  kprint_err(f);
  kprint_err(": ");
  kprint_err(m);
  while(1){

  }
}

__attribute__((noreturn))
void panic_assert(char *file, uint32_t line, char *desc){
  asm("cli");

  kprint_err("ASSERTION-FAILED(");
  kprint_err(desc);
  kprint_err(") at ");
  kprint_err(file);
  kprint_err(":");
  char c[5];
  int_to_ascii(line, c);
  kprint_err(c);
  kprint_err("\n");
  while(1);
}
