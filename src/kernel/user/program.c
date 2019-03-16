#include <rhino/user/program.h>

extern task_t* taskArray;
extern pdirectory* kernel_directory;

/**
   @brief Loads a program from the INITRD into Memory.
   @param args List of args.  args[0] is the filename of the file to search.  args[1] is the binary type of the file.
   @return returns a pointer to a struct which contains info over the loaded program, in case of an error it will return 0.
 */
loaded_program_t* load_program(char* filename, uint8_t type){
    if(type == PROGRAM_BINARY_TYPE_BIN){
      fs_node_t *node = kopen(filename, O_RDONLY);//0;
      //node = finddir_fs(fs_root, filename);
      if(node == 0){
        //kprint_err("program.c: Could not find program on the INITRD\n");
        return 0;
      }
      if((node->flags & 0x7) == FS_DIRECTORY){
        kprint_err("program.c: Selected File is a Directory\n");
        return 0;
      }
      loaded_program_t* header = kmalloc(sizeof(loaded_program_t));
      uint32_t* buf = kmalloc(node->length);
      header->base = (uint32_t*)buf;
      header->end = (uint32_t*)(buf + node->length);
      header->len = node->length;
      header->type = type;
      kprint("REEE");
      read_fs(node, 0, node->length, (uint8_t*)buf);
      kprint("REEE");
      return header;
    } else {
      kprint_err("program.c: ELF and A.OUT types are not yet implemented!\n");
      return 0;
    }
}
/**
   @brief Calls a program in memory.
   @param args List of args.  args[0] is the address to call.
   @return returns nothing.
 */
void run_program(void *address){
  createTask(address, taskArray[0].regs.eflags, taskArray[0].regs.cr3);
  return;
}

/**
   @brief Frees a program in memory.
   @param args List of args.  args[0] is the address of the program info struct.
   @return returns nothing.
 */
void free_program(loaded_program_t* header){
  kfree(header->base);
  kfree(header);
}

/**
   @brief Inits away from the kernel initialization procedure.
   @param args List of args.  args[0] is the file name of the process to init away to.
   @return returns nothing.
 */
void init(char *prg){
  start_task_atomic();
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

  for (int i=0, virt=(uint32_t)PROGRAM_LOAD_ADDRESS; i<60; i++, virt+=4096){
    void* frame = pmm_alloc_block();
    vmm_map_page((void*)frame, (void*)virt, 1);
    task_register_frame(task, frame);
  }
  
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


/**
   @brief Creates a user process.
   @param args List of args.  args[0] is the file name of the process to init away to. args[1] is the pointer to where to place the pid of the child processs
   @return returns the status.
 */
uint32_t create_process(char* prg, uint32_t* pid){
  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  if(header == 0){
    return false;
  }

  pdirectory* cur = (pdirectory*)((uint32_t)get_running_task()->regs.cr3 + (uint32_t)KERNEL_VBASE);

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

  vmm_switch_pdirectory(dir);

  memcpy((void*)PROGRAM_LOAD_ADDRESS, header->base, header->len);

  vmm_switch_pdirectory(cur);
  free_program(header);
  *pid = task->pid.pid;
  return true;
}

