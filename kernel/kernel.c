#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "kernel.h"
#include "./../libc/include/string.h"
#include <stdint.h>
#include "memory/kmalloc.h"

void kernel_main() {
  isr_install();
  irq_install();
  initialize_memory_manager();
  kprint("Type something ffs\n >");
  uint32_t* p = kmalloc(16);
  char s[10];
  hex_to_ascii((uint32_t)p, s);
  kprint(s);
  kprint("\n");
  uint32_t* a = kmalloc(16);
  char d[10];
  hex_to_ascii((uint32_t)a, d);
  kprint(d);
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
