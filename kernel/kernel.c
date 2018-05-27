#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "kernel.h"
#include "./../libc/include/string.h"
#include <stdint.h>
#include "memory/kmalloc.h"

void kernel_main() {
  clear_screen();
  kprint("Starting Rhino Copyright 2018 Thomas Woertman, The Netherlands\n");
  kprint("Installing Interrupts..");
  isr_install();
  irq_install();
  kprint("done\n");
  kprint("Starting Memory Manager..");
  initialize_memory_manager();
  kprint("done\n");
  kprint("Boot successfull!");
  clear_screen();
  kprint("Rhino Shell version 0.0.0\n$");
  /*uint32_t* p = kmalloc(16);
  char s[10];
  hex_to_ascii((uint32_t)p, s);
  kprint(s);
  kprint("\n");
  uint32_t* a = kmalloc(16);
  char d[10];
  hex_to_ascii((uint32_t)a, d);
  kprint(d);*/
}
void user_input(char *input){
  if(strcmp(input, "END") == 0){
    clear_screen();
    kprint("Shutting down Rhino");
    __asm__ __volatile__("hlt");
  }
  kprint(input);
  kprint(" is not an executable program.");
  kprint("\n$");
}
