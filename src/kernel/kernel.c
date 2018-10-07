#if defined(__linux__)
#error "No cross compiler detected you will probably run into trouble"
#endif

#include <rhino/common.h>

#include <libk/string.h>
#include <libk/stdio.h>
#include <libk/stdlib.h>

#include <rhino/multiboot.h>

#include <rhino/mm/vmm.h>
#include <rhino/mm/pmm.h>
#include <rhino/mm/hmm.h>

#include <rhino/fs/vfs.h>
#include <rhino/fs/initrd.h>

#include <rhino/multitasking/task.h>
#include <rhino/multitasking/scheduler.h>

#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/pci.h>
#include <rhino/arch/x86/gdt.h>
#include <rhino/arch/x86/msr.h>
#include <rhino/arch/x86/io.h>
#include <rhino/arch/x86/cmos.h>
#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/arch/x86/drivers/keyboard.h>
#include <rhino/arch/x86/drivers/fdc.h>
#include <rhino/arch/x86/drivers/pcspkr.h>

#include <rhino/kernel.h>
#include <rhino/panic.h>
#include <rhino/acpi/acpi.h>

#include <rhino/Deer/deer.h>

#include <rhino/user/program.h>

#include <rhino/pwr/power.h>



extern uint32_t placement_address;
extern uint32_t _kernel_start;

uint8_t shouldExit = 0;
multiboot_info_t* multibootInfo;
uint32_t ramAmountMB = 0;

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
  kern_base = (kern_base - KERNEL_VBASE);
  hex_to_ascii(kern_base, buf);
  kprint("KERNEL PHYSICAL BASE: ");
  kprint(buf);
  kprint("\n\n");
  #endif

  if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
    PANIC_M(": Kernel was not booted by a multiboot compliant bootloader!\n");
    return;
  }

  multibootInfo = mbd;

  if(BIT_IS_SET(mbd->flags, 1) && BIT_IS_SET(mbd->flags, 6)){
    ramAmountMB = ((((mbd->mem_lower * 1024) + (mbd->mem_upper * 1024)) / 1024) / 1024) + 1; // Formula for converting from Kibibyte to Megabyte so: Megabyte = (Kibibyte * 1024) / 1024 / 1024 + 1
  } else {
    PANIC_M(": Bootloader did not supply memory info");
  }

  set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
  kprint("Starting Rhino 0.3.0, Copyright 2018 Thomas Woertman, The Netherlands\n");
  int_to_ascii(ramAmountMB, buf);
  kprint("Detected Memory: ");
  kprint(buf);
  kprint("MB\n");

  kprint("\nEnabling Protected Mode\n");
  gdt_install();
  isr_install();

  kprint("Initializing Memory Manager\n");

  ASSERT(mbd->mods_count >= 1);
  multiboot_module_t *initrd = (multiboot_module_t*)(mbd->mods_addr + KERNEL_VBASE);
  uint32_t initrd_location = (initrd->mod_start + KERNEL_VBASE);
  uint32_t initrd_end = (initrd->mod_end + KERNEL_VBASE);
  placement_address = initrd_end;

  init_pmm(mbd);
  init_vmm();
  init_heap();

  kprint("Initializing other drivers\n");
  init_fdc();
  fs_root = initialise_initrd(initrd_location);
  pci_check_all_buses();
  init_acpi();
  irq_install();

  kprint("Enabling Multitasking\n");
  initTasking();

  time_t date = read_rtc();
  kprint("Date: ");
  
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

  #ifndef DEBUG
  clear_screen();
  enable_scheduling();
  init("init");
  return;
  #endif
  
  kprint("Rhino Kernel Internal Shell version 0.0.3");
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
    kprint("-------------------------------\n");
    kprint("run: run shell.prg to start the os.");
    kprint("exit: Exit the Kernel.\n");
    kprint("help: To show this Page.\n");
    kprint("clear: To clear the screen.\n");
    kprint("reboot: To reboot the machine.\n");
    #ifdef DEBUG
    kprint("pid: To show the current PID.\n");
    kprint("panic: Panic the kernel.\n");
    kprint("init: To show the files on the initrd.\n");
    kprint("mmap: Print the sections in the BIOS mmap.\n");
    kprint("floppy: Read sector 0 of floppy drive 0.\n");
    kprint("deer: Start the deer display manager.\n");
    #endif
    kprint("-------------------------------\n");
    kprint("$");
    beep(1);
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
  if(strcmp(input, "pid") == 0){
    char c[25] = "";
    int_to_ascii(get_current_pid(), c);
    kprint(c);
    kprint("\n$");
    return;
  }
  if(strcmp(input, "deer") == 0){
    deer_start();
    return;
  }
  if(strcmp(input, "mmap") == 0){
    set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    uint8_t entryCount = 1;
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + KERNEL_VBASE);
    uintptr_t *mmap_end = (uintptr_t*)((multibootInfo->mmap_addr + multibootInfo->mmap_length + KERNEL_VBASE));
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
         kprint(" ACPI Reclaim\n");
         break;
        case MULTIBOOT_MEMORY_NVS:
          kprint(" Non Volatile Storage\n");
          break;
        case MULTIBOOT_MEMORY_BADRAM:
          kprint(" Badram\n");
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
    uint8_t* s = fdc_read_sector(0);
    s = (uint8_t*)((uint32_t)s + (uint32_t)KERNEL_VBASE);
    if(s != 0){
      int i = 0;
      for(int c = 0; c < 2; c++){
        for(int j = 0; j < 128; j++){
          char buf[25] = "";
          hex_to_ascii(s[i + j], buf);
          kprint(buf);
          kprint(" ");
        }
        i+= 128;
      }
    }
    else {
      kprint("ERROR");
    }
    kprint("\n$");
    return;
  }
  if(strcmp(input, "reboot") == 0) reboot();
  if(strcmp(input, "run") == 0){
    init("init");
    kprint("\n$");
    return;
  }
  if(strcmp(input, "") == 0){
    kprint("$");
    return;
  }
  kprint(input);
  kprint(" is not an executable program.");
  kprint("\n$");
}
