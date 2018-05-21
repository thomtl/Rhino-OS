#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "kernel.h"
#include "./../libc/include/string.h"
#include "./../libc/include/stdlib.h"
#include <stdint.h>
void kernel_main() {
  isr_install();
  irq_install();
  kprint("Type something ffs\n >");
  malloc(50);
}
void user_input(char *input){
  if(strcmp(input, "END") == 0){
    kprint("Stopping BootOS Bye :(");
    __asm__ __volatile__("hlt");
  }
  kprint("You said: ");
  kprint(input);
  kprint("\n> ");
}
