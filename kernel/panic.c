#include "panic.h"

void panic(){
  clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!");
  while(1){

  }
}


void panic_m(char* m){
  //clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!\n");
  kprint(m);
  while(1){

  }
}
