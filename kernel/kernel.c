#define DEBUG

#include "common.h"
#include "multiboot.h"
#include "paging/paging.h"
#include "../drivers/screen.h"
#include "./../cpu/isr.h"
#include "./../cpu/gdt.h"
#include "kernel.h"
#include "panic.h"
#include "./../libc/include/string.h"
#include "./../libc/function.h"
#include <stdint.h>
//#include "memory/kmalloc.h"
//#include "heap/heap.h"

//KHEAPBM kheap;

uint8_t shouldExit = 0; //set this to 1 to exit the kernel
uint32_t uptime = 0;
multiboot_info_t* multibootInfo;
uint32_t ramAmount = 0; // in MegaBytes
//uint32_t* KERNEL_START = (uint32_t*)0xC0100000;
void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
  /*char kernelVirtualBaseString[25] = "";
  hex_to_ascii((uint32_t)KERNEL_START, kernelVirtualBaseString);
  kprint("KERNEL VIRTUAL BASE: ");
  kprint(kernelVirtualBaseString);
  kprint("\n");
  char kernelPhysicalBaseString[25] = "";
  hex_to_ascii((uint32_t)KERNEL_START - KERNEL_VBASE, kernelPhysicalBaseString);
  kprint("KERNEL PHYSICAL BASE: ");
  kprint(kernelPhysicalBaseString);
  kprint("\n");*/
  if(magic == MULTIBOOT_BOOTLOADER_MAGIC){
    kprint("Multiboot boot successfull\n");
  } else {
    kprint("Error not correctly booted!\n");
    return;
  }
  multibootInfo = mbd;
  if(BIT_IS_SET(mbd->flags, 1)){
    ramAmount = ((((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024) / 1024) + 1; // Formula for converting from Kibibyte to Megabyte so: Megabyte = (Kibibyte * 1024) / 1024 / 1024 + 1
  } else {
    panic_m("Multiboot did not supply memory info");
  }
  kprint("Starting Rhino Copyright 2018 Thomas Woertman, The Netherlands\n");
  kprint("Installing GDT..");
  gdt_install();
  kprint("done\n");
  kprint("Installing Interrupts..");
  isr_install();
  irq_install();
  kprint("done\n");
  char ramString[25] = "HELLO WORLD";
  int_to_ascii(ramAmount, ramString);
  kprint("Detected Memory: ");
  kprint(ramString);
  kprint("MB\n");
  kprint("Enabling Paging..");
  initialise_paging();
  kprint("done\n");
  kprint("Page Directory: ");
  uint32_t cr0;
  __asm__ __volatile__("mov %%cr3, %0": "=r"(cr0));
  char h[25] = "";
  hex_to_ascii(cr0, h);
  kprint(h);
  kprint("\n");
  kprint("Boot successfull!\n");
  #ifndef DEBUG
  clear_screen();
  #endif
  kprint("Rhino Shell version 0.0.1\n$");
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
    #ifdef DEBUG
    kprint("Debug commands:\n");
    kprint("PAGEFAULT: Do a Page fault and Panic\n");
    #endif
    kprint("-------------------------------\n");
    kprint("$");
    return;
  }
  if(strcmp(input, "CLEAR") == 0){
    clear_screen();
    kprint("$");
    return;
  }
  if(strcmp(input, "PAGEFAULT") == 0){
    uint32_t* ptr = (uint32_t*)0xc0000000;
    uint32_t pf = *ptr;
    UNUSED(pf);
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
