#define DEBUG

#include "common.h"
#include "multiboot.h"
#include "paging/paging.h"
#include "multitasking/task.h"
#include "fs/vfs.h"
#include "fs/initrd.h"
#include "multitasking/task.h"
#include "user/program.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "./arch/x86/isr.h"
#include "./arch/x86/gdt.h"
#include "./arch/x86/msr.h"
#include "security/test_security.h"
#include "kernel.h"
#include "panic.h"
#include "./../libc/include/string.h"
#include "./../libc/include/stdio.h"
#include <stdint.h>

extern uint32_t placement_address;
uint8_t shouldExit = 0; //set this to 1 to exit the kernel
uint32_t uptime = 0;
multiboot_info_t* multibootInfo;
uint32_t ramAmountMB = 0; // in MegaBytes
uint32_t ramAmount = 0;
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
    ramAmountMB = ((((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024) / 1024) + 1; // Formula for converting from Kibibyte to Megabyte so: Megabyte = (Kibibyte * 1024) / 1024 / 1024 + 1
    ramAmount = (((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024 / 1024 + 1) * 0x100000;
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
  char ramString[25] = "";
  int_to_ascii(ramAmountMB, ramString);
  kprint("Detected Memory: ");
  kprint(ramString);
  kprint("MB\n");
  kprint("Loading Ramdisk..");
  ASSERT(mbd->mods_count >= 1);
  //uint32_t initrd_location = *((uint32_t*)mbd->mods_addr);
  uint32_t initrd_end = *(uint32_t*)(mbd->mods_addr+4);
  placement_address = initrd_end;
  kprint("done\n");
  kprint("Enabling Paging..");
  install_paging();
  kprint("done\n");
  /*__asm__ __volatile__ ("cli; hlt");
  kprint("Initializing Ramdisk..");
  fs_root = initialise_initrd(initrd_location);
  kprint("done\n");
  kprint("Enabling Multitasking..");
  initTasking();
  kprint("done\n");
  kprint("Boot successfull!\n");*/
  #ifndef DEBUG
  clear_screen();
  #endif
  kprint("Rhino Shell version 0.0.1\n$");
  while(1)
  {
    if(shouldExit == 1){
        return;
    }
    char c[256] = "";
    getline(c, 256);
    user_input(c);
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
    kprint("PID: To show the current tasks PID\n");
    kprint("TASK: To yield\n");
    #ifdef DEBUG
    kprint("Debug commands:\n");
    kprint("PANIC: Panic the kernel\n");
    kprint("INIT: To show the files on the initrd\n");
    kprint("STACKSMASH: To smash the stack and test the stack smash protection\n");
    kprint("TICK: To show the current tick of the CPU\n");
    kprint("SYSCALL: Simulate a bare syscall\n");
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
  if(strcmp(input, "PANIC") == 0){
    panic_m("Deliberate Kernel Panic");
  }
  /*if(strcmp(input, "INIT") == 0){
    int i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
      kprint("Found file ");
      kprint(node->name);
      fs_node_t *fsnode = finddir_fs(fs_root, node->name);

      if ((fsnode->flags&0x7) == FS_DIRECTORY){
        kprint("\n\t(directory)\n");
      } else {
        kprint("\n\t contents: \"");
        uint8_t buf[256];
        int sz = read_fs(fsnode, 0, 256, buf);
        int j;
        for (j = 0; j < sz; j++){
          char hgh[2] = {(buf[j]), '\0'};
          kprint(hgh);
        }


        kprint("\"\n");
      }
      i++;
    }
    kprint("$");
    return;
  }
  if(strcmp(input, "STACKSMASH") == 0){
    do_stacksmash("BLUBBLUBBLUBBLUBLUB");
    return;
  }
  if(strcmp(input, "TASK") == 0){
    kprint("Switching to other task\n");
    yield();
    kprint("ready\n$");
    return;
  }
  if(strcmp(input, "PID") == 0){
    char c[25] = "";
    int_to_ascii(get_current_pid(), c);
    kprint(c);
    kprint("\n$");
    return;
  }*/
  if(strcmp(input, "TICK") == 0){
    char c[128] = "";
    int_to_ascii(uptime, c);
    kprint(c);
    kprint("\n$");
    return;
  }
  /*if(strcmp(input, "EXT") == 0){
    loaded_program_t* l = load_program("test.prg", PROGRAM_BINARY_TYPE_BIN);
    if(l == 0){
      kprint("load_program errored\n$");
      return;
    }
    char j[25];
    hex_to_ascii((uint32_t)l->base, j);
    kprint(j);
    j[0] = '\0';
    run_program(l->base);
    free_program(l);
    kprint("\n$");
    return;
  }
  if(strcmp(input, "SYSCALL") == 0){
    __asm__ __volatile__ ("int $0x80");
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
