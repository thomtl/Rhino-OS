#include "panic.h"

void panic(){
  clear_screen();
  kprint("KERNEL PANIC!\n");
  kprint("PLEASE REBOOT SYSTEM!");
  while(1){

  }
}
