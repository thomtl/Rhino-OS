#include <rhino/user/program.h>

extern task_t* taskArray;
extern pdirectory* kernel_directory;

/**
   @brief Loads a program from the INITRD into Memory.
   @param args List of args.  args[0] is the filename of the file to search.
   @return returns a pointer to a struct which contains info over the loaded program, in case of an error it will return 0.
 */
loaded_program_t* load_program(char* filename){
    fs_node_t *node = kopen(filename, O_RDONLY);//0;
    if(node == 0){
        kprint_err("program.c: Could not find program in the filesystem\n");
    	return 0;
	}
    if(node->flags == FS_DIRECTORY){
        kprint_err("program.c: Selected file is a directory\n");
        return 0;
    }
    loaded_program_t* header = kmalloc(sizeof(loaded_program_t));
    uint32_t* buf = kmalloc(node->length);
    header->base = (uint32_t*)buf;
    header->end = (uint32_t*)(buf + node->length);
	header->len = node->length;

    read_fs(node, 0, node->length, (uint8_t*)buf);

	if(elf_is_elf(buf)) header->type = PROGRAM_TYPE_ELF;
	else header->type = PROGRAM_TYPE_UNKOWN;

    return header;
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
	pdirectory* dir;

  	loaded_program_t* header = load_program(prg);
  	if(header == 0){
    	kprint("init.c: failed to load program\n");
    	return;
  	}


  	dir = vmm_clone_dir(kernel_directory);
  	if(header == 0){
    	kprint("init.c: failed to clone kernel page directory\n");
    	return;
  	}

	uint32_t load_address;
	if(header->type == PROGRAM_TYPE_ELF) load_address = elf_get_load_addr(header->base);
	else {
		debug_log("[INIT]: Trying to load unkown file type, assuming raw binary with entry 0x1000\n");
		load_address = PROGRAM_LOAD_ADDRESS;
	}

  	task_t* task = createTask((void*)load_address, task_for_pid(0)->regs.eflags, ((uint32_t)dir - KERNEL_VBASE));

  	vmm_switch_pdirectory(dir);

	uint32_t n_pages;
	if(header->type == PROGRAM_TYPE_ELF) n_pages = IDIV_CEIL(elf_get_file_length(header->base), RHINO_PMM_BLOCK_SIZE);
	else n_pages = 60;

  	for (uint32_t i=0, virt=(uint32_t)load_address; i<n_pages; i++, virt+=4096){ // todo file length
    	void* frame = pmm_alloc_block();
    	vmm_map_page((void*)frame, (void*)virt, 1);
    	task_register_frame(task, frame);
  	}
  
  	vmm_map_page(pmm_alloc_block(), 0x0, 1);
  	task->regs.esp = 0x1000;

	if(header->type == PROGRAM_TYPE_ELF){
		task->regs.eip = (uint32_t)elf_load_file(header->base);
	} 
  	else memcpy((void*)load_address, header->base, header->len);

	task->ram_image.program_entry = 0x0; // Stack start
	task->ram_image.size = (RHINO_PMM_BLOCK_SIZE * n_pages);

  	vmm_switch_pdirectory(kernel_directory);

  	free_program(header);

  	kill_kern();
  	end_task_atomic();
}


/**
   @brief Creates a user process.
   @param args List of args.  args[0] is the file name of the process to init away to. args[1] is the pointer to where to place the pid of the child processs
   @return returns the status.
 */
uint32_t create_process(char* prg, uint32_t* pid){
  	loaded_program_t* header = load_program(prg);
  	if(header == 0){
    	return false;
  	}

  	pdirectory* cur = (pdirectory*)((uint32_t)get_running_task()->regs.cr3 + (uint32_t)KERNEL_VBASE);

  	pdirectory* dir = vmm_clone_dir(kernel_directory);

	uint32_t load_address;
	if(header->type == PROGRAM_TYPE_ELF) load_address = elf_get_load_addr(header->base);
	else {
		debug_log("[INIT]: Trying to load unkown file type, assuming raw binary with entry 0x1000\n");
		load_address = PROGRAM_LOAD_ADDRESS;
	}

  	task_t* task = createTask((void*)load_address, task_for_pid(0)->regs.eflags, ((uint32_t)dir - (uint32_t)KERNEL_VBASE));

  	vmm_switch_pdirectory(dir);

	uint32_t n_pages;
	if(header->type == PROGRAM_TYPE_ELF) n_pages = IDIV_CEIL(elf_get_file_length(header->base), RHINO_PMM_BLOCK_SIZE);
	else n_pages = 60;

  	for (uint32_t i=0, virt=(uint32_t)load_address; i<n_pages; i++, virt+=4096){
    	void* frame = pmm_alloc_block();
    	vmm_map_page((void*)frame, (void*)virt, 1);
    	task_register_frame(task, frame);
  	}

  	vmm_map_page(pmm_alloc_block(), 0x0, 1);
  	task->regs.esp = 0x1000;

	

	if(header->type == PROGRAM_TYPE_ELF){
		task->regs.eip = (uint32_t)elf_load_file(header->base);
	} else memcpy((void*)load_address, header->base, header->len);

	task->ram_image.program_entry = 0x0;
	task->ram_image.size = (RHINO_PMM_BLOCK_SIZE * n_pages);

  	vmm_switch_pdirectory(cur);
  	free_program(header);
  	*pid = task->pid.pid;
  	return true;
}

