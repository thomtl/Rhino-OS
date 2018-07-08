#define DEBUG

#if defined(__linux__)
#error "No cross compiler detected you will probably run into trouble"
#endif

#include "common.h"
#include "multiboot.h"
#include "mm/paging.h"
#include "mm/frame.h"
#include "mm/kheap.h"
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
#include "./arch/x86/ports.h"
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

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {

  if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
    PANIC_M(": Kernel not booted by a multiboot compliant bootloader!\n");
    return;
  }
  multibootInfo = mbd;
  if(BIT_IS_SET(mbd->flags, 1) && BIT_IS_SET(mbd->flags, 6)){
    ramAmountMB = ((((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024) / 1024) + 1; // Formula for converting from Kibibyte to Megabyte so: Megabyte = (Kibibyte * 1024) / 1024 / 1024 + 1
    ramAmount = (((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024 / 1024 + 1) * 0x100000;
  } else {
    PANIC_M("bootloader did not supply memory info");
  }
  if(BIT_IS_SET(mbd->flags, 10)){
    kprint_warn("MULTIBOOT Supplied apm table\n");
  } else {
    kprint_err("MULTIBOOT didn't supply APM table\n");
  }
  set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
  kprint("Starting Rhino Copyright 2018 Thomas Woertman, The Netherlands\n");
  char ramString[25] = "";
  int_to_ascii(ramAmountMB, ramString);
  kprint("Detected Memory: ");
  kprint(ramString);
  kprint("MB\n");
  kprint("Enabling Protected Mode..");
  gdt_install();
  isr_install();
  irq_install();
  kprint("done\n");
  kprint("Loading Ramdisk..");
  ASSERT(mbd->mods_count >= 1);
  uint32_t initrd_location = *((uint32_t*)mbd->mods_addr);
  uint32_t initrd_end = *(uint32_t*)(mbd->mods_addr+4);
  placement_address = initrd_end;
  kprint("done\n");
  kprint("Initializing Memory Management..");
  install_paging();
  init_frame_alloc(mbd);
  init_heap();
  kprint("done\n");
  kprint("Initializing Ramdisk..");
  fs_root = initialise_initrd(initrd_location);
  kprint("done\n");
  kprint("Enabling Multitasking..");
  initTasking();
  kprint("done\n");
  #ifndef DEBUG
  clear_screen();
  #endif
  //set_color(VGA_COLOR_WHITE, VGA_COLOR_MAGENTA);
  kprint("Rhino Kernel Internal Shell version 0.0.2");
  set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  kprint("\n$");
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
    kprint("SYSCALL: Simulate a bare syscall\n");
    kprint("MMAP: Print the sections in the BIOS mmap\n");
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
    PANIC_M("Deliberate Kernel Panic");
  }
  if(strcmp(input, "INIT") == 0){
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
  }
  if(strcmp(input, "EXT") == 0){
    loaded_program_t* l = load_program("test.prg", PROGRAM_BINARY_TYPE_BIN);
    if(l == 0){
      kprint_err("load_program errored\n$");
      return;
    }
    char j[25];
    hex_to_ascii((uint32_t)l->base, j);
    kprint(j);
    kprint("\n");
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
  }
  if(strcmp(input, "MMAP") == 0){
    set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    uint8_t entryCount = 1;
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)multibootInfo->mmap_addr;
    uintptr_t *mmap_end = (uintptr_t*)(multibootInfo->mmap_addr + multibootInfo->mmap_length);
    while((uint32_t)mmap < (uint32_t)mmap_end){
      char buf[128] = "";
      kprint("Entry: ");
      int_to_ascii(entryCount, buf);
      kprint(buf);
      for(uint32_t i = 0; i < 128; i++) buf[i] = '\0';
      kprint(" Base: ");
      hex_to_ascii(mmap->addr, buf);
      kprint(buf);
      for(uint32_t i = 0; i < 128; i++) buf[i] = '\0';
      kprint(" - Top: ");
      hex_to_ascii(mmap->addr + mmap->len, buf);
      kprint(buf);
      switch(mmap->type){
        case MULTIBOOT_MEMORY_AVAILABLE:
          kprint(" Available\n");
          break;
        case MULTIBOOT_MEMORY_RESERVED:
          kprint(" Reserved\n");
          break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
         kprint(" ACPI RECLAIM\n");
         break;
        case MULTIBOOT_MEMORY_NVS:
          kprint(" NVS\n");
          break;
        case MULTIBOOT_MEMORY_BADRAM:
          kprint(" BADRAM\n");
          break;
        default:
          break;
      }
      for(uint32_t i = 0; i < 128; i++) buf[i] = '\0';
      mmap = (multiboot_memory_map_t*) ( (uintptr_t)mmap + mmap->size + sizeof(uintptr_t) );
      entryCount++;
    }
    set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprint("$");
    return;
  }
  kprint(input);
  kprint(" is not an executable program.");
  kprint("\n$");
}

void kernel_timer_callback(){
  uptime++;
  char c[128] = "";
  int_to_ascii(uptime, c);
  uint8_t backup = get_color();
  set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
  kprint_at(c, 75, 0, 1);
  set_raw_color(backup);
}
