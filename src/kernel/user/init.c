#include <rhino/user/init.h>
extern pdirectory* kernel_directory;

void init(char *prg){
  uint8_t attr = get_color();
  pdirectory* dir;
  set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLUE);

  kprint("Loading ");
  kprint(prg);
  kprint("\n");

  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  kprint("Program Loaded\n");


  kprint("Cloning Dir\n");
  dir = vmm_clone_dir(kernel_directory);
  kprint("Cloned Dir\n");
  kprint("Mapping Pages\n");

  vmm_switch_pdirectory(dir);

  //uint32_t m = (header->len / 4096) + 1;
  //for(uint32_t j = 0; j < m; j++){                               //should be 1024 but page fault investigate
    for (int i=0, virt=(uint32_t)PROGRAM_LOAD_ADDRESS + (0 * 4096); i<1023; i++, virt+=4096){
      void* frame = pmm_alloc_block();
      vmm_map_page((void*)frame, (void*)virt);
    }
  //}

  //asm("cli; hlt");


  /*uint32_t j = (uint32_t)PROGRAM_LOAD_ADDRESS;
  for(uint32_t i = FREE_PROG; i < (FREE_PROG + m); i += MM_PAGE_S) {
    map_phys_virt(dir, i, j);
    reserve_addr((void*)i);
    j += MM_PAGE_S;
  }*/


  kprint("Mapped Pages\n");
  vmm_switch_pdirectory(dir);
  memcpy(PROGRAM_LOAD_ADDRESS, header->base, header->len);
  vmm_switch_pdirectory(kernel_directory);
  kprint("Program in designated zone\n");

  free_program(header);
  kprint("Loading Space Freed\n");

  task_t* root = task_for_pid(0);
  createTask(PROGRAM_LOAD_ADDRESS, root->regs.eflags, ((uint32_t)dir - KERNEL_VBASE));

  kill(0);
  kprint("Kernel Task Killed\n");

  kprint("Program Loading Done");
  set_raw_color(attr);
  //__asm__ ("cli; hlt");
  end_task_atomic();
  //__asm__ ("cli; hlt");
}

void create_process(char* prg){
  pdirectory* old = (pdirectory*)((uint32_t)(task_for_pid(1)->regs.cr3) + (uint32_t)KERNEL_VBASE);
  uint8_t attr = get_color();
  set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLUE);

  kprint("Loading ");
  kprint(prg);
  kprint("\n");

  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  kprint("Program Loaded\n");


  kprint("Cloning Dir\n");
  pdirectory* dir = vmm_clone_dir(kernel_directory);
  kprint("Cloned Dir\n");
  kprint("Mapping Pages\n");

  vmm_switch_pdirectory(dir);

  //uint32_t m = (header->len / 4096) + 1;
  //for(uint32_t j = 0; j < m; j++){                               //should be 1024 but page fault investigate
    for (int i=0, virt=(uint32_t)PROGRAM_LOAD_ADDRESS + (0 * 4096); i<1023; i++, virt+=4096){
      void* frame = pmm_alloc_block();
      vmm_map_page((void*)frame, (void*)virt);
    }
  //}

  //asm("cli; hlt");


  /*uint32_t j = (uint32_t)PROGRAM_LOAD_ADDRESS;
  for(uint32_t i = FREE_PROG; i < (FREE_PROG + m); i += MM_PAGE_S) {
    map_phys_virt(dir, i, j);
    reserve_addr((void*)i);
    j += MM_PAGE_S;
  }*/


  kprint("Mapped Pages\n");
  //asm("cli; hlt");
  memcpy(PROGRAM_LOAD_ADDRESS, header->base, header->len);
  kprint("Program in designated zone\n");

  free_program(header);
  kprint("Loading Space Freed\n");

  task_t* root = task_for_pid(0);
  createTask(PROGRAM_LOAD_ADDRESS, root->regs.eflags, ((uint32_t)dir - KERNEL_VBASE));


  kprint("Program Loading Done");
  set_raw_color(attr);

  vmm_switch_pdirectory(old);
  //__asm__ ("cli; hlt");
  yield();
  //__asm__ ("cli; hlt");
}
