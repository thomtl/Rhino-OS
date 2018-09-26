#include <rhino/user/init.h>
#include <rhino/mm/hmm.h>
extern pdirectory* kernel_directory;

void init(char *prg){
  uint8_t attr = get_color();
  pdirectory* dir;
  set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLUE);

  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  if(header == 0){
    kprint("init.c: failed load program aborting\n");
    return;
  }


  dir = vmm_clone_dir(kernel_directory);
  if(header == 0){
    kprint("init.c: failed clone dir aborting\n");
    return;
  }

  task_t* root = task_for_pid(0);
  task_t* task = createTask((void*)PROGRAM_LOAD_ADDRESS, root->regs.eflags, ((uint32_t)dir - KERNEL_VBASE));

  vmm_switch_pdirectory(dir);

  //uint32_t m = (header->len / 4096) + 1;
  //for(uint32_t j = 0; j < m; j++){                               //should be 1024 but page fault investigate
  for (int i=0, virt=(uint32_t)PROGRAM_LOAD_ADDRESS; i<60; i++, virt+=4096){
    void* frame = pmm_alloc_block();
    vmm_map_page((void*)frame, (void*)virt, 1);
    task_register_frame(task, frame);
  }
  
  //kfree((void*)(task->regs.esp - 0x1000));
  vmm_map_page(pmm_alloc_block(), 0x0, 1);
  task->regs.esp = 0x1000;

  vmm_switch_pdirectory(dir);
  memcpy((void*)PROGRAM_LOAD_ADDRESS, header->base, header->len);
  vmm_switch_pdirectory(kernel_directory);

  free_program(header);

  kill_kern();

  set_raw_color(attr);
  end_task_atomic();
}

uint32_t create_process(char* prg, uint32_t* pid){
  pdirectory* cur = (pdirectory*)((uint32_t)get_running_task()->regs.cr3 + (uint32_t)KERNEL_VBASE);
  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  if(header == 0){
    return false;
  }
  pdirectory* dir = vmm_clone_dir(kernel_directory);

  //TODO: Set Interrupt bit
  task_t* task = createTask((void*)PROGRAM_LOAD_ADDRESS, task_for_pid(0)->regs.eflags, ((uint32_t)dir - (uint32_t)KERNEL_VBASE));

  vmm_switch_pdirectory(dir);
  for (int i=0, virt=(uint32_t)PROGRAM_LOAD_ADDRESS; i<60; i++, virt+=4096){
    void* frame = pmm_alloc_block();
    vmm_map_page((void*)frame, (void*)virt, 1);
    task_register_frame(task, frame);
  }

  vmm_map_page(pmm_alloc_block(), 0x0, 1);
  task->regs.esp = 0x1000;

  memcpy((void*)PROGRAM_LOAD_ADDRESS, header->base, header->len);

  vmm_switch_pdirectory(cur);
  free_program(header);
  *pid = task->pid.pid;
  return true;
}
