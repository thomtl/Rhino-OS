#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "kernel.h"
#include "./../libc/string.h"
#include <stdint.h>
void kernel_main() {
  isr_install();
  irq_install();
  kprint("Type something ffs");
}
void user_input(char *input){
  if(strcmp(input, "END") == 0){
    kprint("Stopping BootOS Bye :(");
    asm volatile("hlt");
  }
  kprint("You said: ");
  kprint(input);
  kprint("\n> ");
}
