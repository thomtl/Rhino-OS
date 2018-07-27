#include "init.h"
extern uintptr_t page_directory;

void init(char *prg){
  uint8_t attr = get_color();
  uintptr_t dir;
  set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLUE);

  start_task_atomic();

  kprint("Loading ");
  kprint(prg);
  kprint("\n");

  loaded_program_t* header = load_program(prg, PROGRAM_BINARY_TYPE_BIN);
  kprint("Program Loaded\n");

  uint32_t m = (header->len / MM_PAGE_S) + 1;
  kprint("Cloning Dir\n");
  dir = clone_dir(page_directory);
  kprint("Cloned Dir\n");
  kprint("Mapping Pages\n");
  uint32_t j = (uint32_t)PROGRAM_LOAD_ADDRESS;
  for(uint32_t i = FREE_PROG; i < (FREE_PROG + m); i += MM_PAGE_S) {
    map_phys_virt(dir, i, j);
    j += MM_PAGE_S;
  }
  kprint("Mapped Pages\n");
  switch_dir(dir);
  memcpy(PROGRAM_LOAD_ADDRESS, header->base, header->len);
  kprint("Program in designated zone\n");

  free_program(header);
  kprint("Loading Space Freed\n");

  task_t* root = task_for_pid(0);
  createTask(PROGRAM_LOAD_ADDRESS, root->regs.eflags, (uint32_t*)dir);

  kill(0);
  kprint("Kernel Task Killed\n");

  kprint("Program Loading Done");
  set_raw_color(attr);
  //__asm__ ("cli; hlt");
  end_task_atomic();
  //__asm__ ("cli; hlt");
}
