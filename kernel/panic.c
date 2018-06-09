#include "panic.h"
__attribute__((noreturn))
void panic(){
  clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!");
  while(1){

  }
}

__attribute__((noreturn))
void panic_m(char* m){
  //clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!\n");
  kprint(m);
  while(1){

  }
}
