#define DEBUG

#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "./../cpu/gdt.h"
#include "kernel.h"
#include "./../libc/include/string.h"
#include <stdint.h>
//#include "memory/kmalloc.h"
//#include "heap/heap.h"

//KHEAPBM kheap;

uint8_t shouldExit = 0; //set this to 1 to exit the kernel
uint32_t uptime = 0;

void kernel_main() {
  #ifndef DEBUG
  clear_screen();
  #endif
  kprint("Starting Rhino Copyright 2018 Thomas Woertman, The Netherlands\n");
  kprint("Installing GDT..");
  gdt_install();
  kprint("done\n");
  kprint("Installing Interrupts..");
  isr_install();
  irq_install();
  kprint("done\n");
  kprint("Creating Heap..");
  //k_heapBMInit(&kheap);
  //k_heapBMAddBlock(&kheap, 0x100000, 0x100000, 16);
  kprint("done\n");
  kprint("Starting Memory Manager..");
  //initialize_memory_manager();
  kprint("done\n");
  kprint("Boot successfull!\n");
  #ifndef DEBUG
  clear_screen();
  #endif
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
  while(1)
  {
      	if(shouldExit == 1){
          return;
        }
  }
}
void user_input(char *input){
  if(strcmp(input, "EXIT") == 0){
    clear_screen();
    kprint("Shutting down Rhino");
    shouldExit = 1;
    return;
  }
  if(strcmp(input, "HELP") == 0){
    kprint("Showing Commands:\n");
    kprint("-------------------------------\n");
    kprint("EXIT: Exit the Kernel\n");
    kprint("HELP: To show this Page\n");
    kprint("CLEAR: To clear the screen\n");
    kprint("-------------------------------\n");
    kprint("$");
    return;
  }
  if(strcmp(input, "CLEAR") == 0){
    clear_screen();
    kprint("$");
    return;
  }
  /*
  if(strcmp(input, "MEM") == 0){
    kprint("creating mem\n");
    char *j = "";
    char *ptr = (char*)k_heapBMAlloc(&kheap, 265);
    kprint("successfully allocated 265 bytes at ");
    hex_to_ascii((uint32_t)ptr, j);
    kprint(j);
    kprint("\n$");
    return;
  }*/
  kprint(input);
  kprint(" is not an executable program.");
  kprint("\n$");
}

void kernel_timer_callback(){
  uptime++;
}
