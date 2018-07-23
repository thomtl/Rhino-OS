#define DEBUG

#if defined(__linux__)
#error "No cross compiler detected you will probably run into trouble"
#endif

#include "common.h"
#include "multiboot.h"
#include "mm/paging.h"
#include "mm/phys.h"
#include "mm/kheap.h"
#include "multitasking/task.h"
#include "fs/vfs.h"
#include "fs/initrd.h"
#include "multitasking/task.h"
#include "multitasking/scheduler.h"
#include "user/program.h"
#include "user/init.h"
#include "power/power.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/floppy.h"
#include "./arch/x86/isr.h"
#include "./arch/x86/gdt.h"
#include "./arch/x86/msr.h"
#include "./arch/x86/ports.h"
#include "./arch/x86/cmos.h"
#include "security/test_security.h"
#include "kernel.h"
#include "panic.h"
#include "./../libk/include/string.h"
#include "./../libk/include/stdio.h"
#include <stdint.h>

extern uint32_t placement_address;
extern uint32_t _kernel_start;
uint8_t shouldExit = 0; //set this to 1 to exit the kernel
multiboot_info_t* multibootInfo;
uint32_t ramAmountMB = 0; // in MegaBytes
uint32_t ramAmount = 0;

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
  char buf[25] = "";
  #ifndef DEBUG
  clear_screen();
  #endif
  #ifdef DEBUG
  set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
  uint32_t kern_base = (uint32_t)&_kernel_start;
  hex_to_ascii(kern_base, buf);
  kprint("KERNEL VIRTUAL BASE: ");
  kprint(buf);
  kprint("\n");
  buf[0] = '\0';
  kern_base = virt_to_phys(kern_base);
  hex_to_ascii(kern_base, buf);
  kprint("KERNEL PHYSICAL BASE: ");
  kprint(buf);
  kprint("\n");
  #endif
  if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
    PANIC_M(": Kernel was not booted by a multiboot compliant bootloader!\n");
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
  kprint("Starting Rhino 0.2, Copyright 2018 Thomas Woertman, The Netherlands\n");
  int_to_ascii(ramAmountMB, buf);
  kprint("Detected Memory: ");
  kprint(buf);
  kprint("MB\n");

  kprint("Date: ");
  time_t date = read_rtc();
  int_to_ascii(date.day, buf);
  kprint(buf);
  kprint("/");
  int_to_ascii(date.month, buf);
  kprint(buf);
  kprint("/");
  int_to_ascii(date.year, buf);
  kprint(buf);
  kprint(" ");
  int_to_ascii(date.hour, buf);
  kprint(buf);
  kprint(":");
  if(date.minute < 9) kprint("0");
  int_to_ascii(date.minute, buf);
  kprint(buf);
  kprint("\n");


  kprint("\nEnabling Protected Mode..");
  gdt_install();
  isr_install();
  irq_install();
  kprint("done\n");

  kprint("Loading Ramdisk..");
  ASSERT(mbd->mods_count >= 1);
  multiboot_module_t *initrd = (multiboot_module_t*)phys_to_virt(mbd->mods_addr);
  uint32_t initrd_location = phys_to_virt(initrd->mod_start);//*((uint32_t*)phys_to_virt(mbd->mods_addr));
  uint32_t initrd_end = phys_to_virt(initrd->mod_end);//*(uint32_t*)(phys_to_virt(mbd->mods_addr)+ 4);
  placement_address = initrd_end;
  kprint("done\n");


  kprint("Initializing Memory Manager..");
  init_mm_phys_manager(mbd);
  init_mm_paging();
  kprint("Memory Manager initialized\n");

  kprint("Initializing Ramdisk..");
  fs_root = initialise_initrd(initrd_location);
  kprint("done\n");

  kprint("Enabling Multitasking..");
  initTasking();
  kprint("done\n");

  #ifndef DEBUG
  clear_screen();
  #endif

  kprint("Rhino Kernel Internal Shell version 0.0.2");
  set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  kprint("\n$");
  enable_scheduling();
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
  if(strcmp(input, "exit") == 0){
    clear_screen();
    kprint("Shutting down Rhino");
    shouldExit = 1;
    return;
  }
  if(strcmp(input, "help") == 0){
    kprint("Showing Commands:\n");
    kprint("-------------------------------\n");
    kprint("run: run shell.prg to start the os");
    kprint("exit: Exit the Kernel\n");
    kprint("help: To show this Page\n");
    kprint("clear: To clear the screen\n");
    kprint("pid: To show the current tasks PID\n");
    kprint("task: To yield\n");
    kprint("ext: To run an external program on the INITRD\n");
    kprint("reboot: To reboot the machine\n");
    #ifdef DEBUG
    kprint("Debug commands:\n");
    kprint("panic: Panic the kernel\n");
    kprint("init: To show the files on the initrd\n");
    kprint("stacksmash: To smash the stack and test the stack smash protection\n");
    kprint("syscall: Simulate a bare syscall\n");
    kprint("mmap: Print the sections in the BIOS mmap\n");
    kprint("floppy: Show the floppy configuration of this machine\n");
    #endif
    kprint("-------------------------------\n");
    kprint("$");
    return;
  }
  if(strcmp(input, "clear") == 0){
    clear_screen();
    kprint("$");
    return;
  }
  if(strcmp(input, "panic") == 0){
    PANIC_M("Deliberate Kernel Panic");
  }
  if(strcmp(input, "init") == 0){
    int i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
      kprint("Found file ");
      kprint(node->name);
      fs_node_t *fsnode = finddir_fs(fs_root, node->name);

      if ((fsnode->flags&0x7) == FS_DIRECTORY){
        kprint("\n(directory)\n");
      } else {
        kprint("\n    contents: \"");
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
  if(strcmp(input, "stacksmash") == 0){
    do_stacksmash("BLUBBLUBBLUBBLUBLUB");
    return;
  }
  if(strcmp(input, "task") == 0){
    kprint("Switching to other task\n");
    yield();
    kprint("ready\n$");
    return;
  }
  if(strcmp(input, "pid") == 0){
    char c[25] = "";
    int_to_ascii(get_current_pid(), c);
    kprint(c);
    kprint("\n$");
    return;
  }
  if(strcmp(input, "ext") == 0){
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
  if(strcmp(input, "syscall") == 0){
    __asm__ __volatile__ ("int $0x80");
    kprint("\n$");
    return;
  }
  if(strcmp(input, "mmap") == 0){
    set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    uint8_t entryCount = 1;
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)phys_to_virt(multibootInfo->mmap_addr);
    uintptr_t *mmap_end = (uintptr_t*)(phys_to_virt(multibootInfo->mmap_addr + multibootInfo->mmap_length));
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
  if(strcmp(input, "floppy") == 0){
    display_floppy_drive_info();
    kprint("$");
    return;
  }
  if(strcmp(input, "reboot") == 0) reboot();
  if(strcmp(input, "run") == 0){
    init("shell.prg");
    kprint("\n$");
    return;
  }
  kprint(input);
  kprint(" is not an executable program.");
  kprint("\n$");
}
