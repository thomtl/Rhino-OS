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

  kprint("Creating Task\n");
  task_t* root = task_for_pid(0);
  task_t* task = createTask((void*)PROGRAM_LOAD_ADDRESS, root->regs.eflags, ((uint32_t)dir - KERNEL_VBASE));;
  kprint("Created Task\n");
  kprint("Mapping Pages\n");

  vmm_switch_pdirectory(dir);

  //uint32_t m = (header->len / 4096) + 1;
  //for(uint32_t j = 0; j < m; j++){                               //should be 1024 but page fault investigate
    for (int i=0, virt=(uint32_t)0x0 + (0 * 4096); i<60; i++, virt+=4096){
      void* frame = pmm_alloc_block();
      vmm_map_page((void*)frame, (void*)virt);
      task_register_frame(task, frame);
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
  memcpy((void*)PROGRAM_LOAD_ADDRESS, header->base, header->len);
  vmm_switch_pdirectory(kernel_directory);
  kprint("Program in designated zone\n");

  free_program(header);
  kprint("Loading Space Freed\n");




  kill_kern();
  kprint("Kernel Task Killed\n");

  kprint("Program Loading Done");
  set_raw_color(attr);
  //__asm__ ("cli; hlt");
  end_task_atomic();
  //__asm__ ("cli; hlt");
}

void create_process(char* prg){
  pdirectory* cur = (pdirectory*)((uint32_t)get_running_task()->regs.cr3 + (uint32_t)KERNEL_VBASE);
  pdirectory* dir = vmm_clone_dir(kernel_directory);

  // Set Interrupt bit
  task_t* task = createTask((void*)PROGRAM_LOAD_ADDRESS, task_for_pid(0)->regs.eflags, ((uint32_t)dir - (uint32_t)KERNEL_VBASE));

  vmm_switch_pdirectory(dir);
  for (int i=0, virt=(uint32_t)0x0 + (0 * 4096); i<60; i++, virt+=4096){
    void* frame = pmm_alloc_block();
    vmm_map_page((void*)frame, (void*)virt);
    task_register_frame(task, frame);
  }
  vmm_map_page((void*)0x0, (void*)0xFFFFF000);
  vmm_map_page((void*)0x0, (void*)0x83042000);
  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  memcpy((void*)PROGRAM_LOAD_ADDRESS, header->base, header->len);



  vmm_switch_pdirectory(cur);
  free_program(header);

}
