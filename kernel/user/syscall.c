#include "syscall.h"
#include "../../libc/function.h"
#include "../../drivers/screen.h"
#include "../../libc/include/stdio.h"
void syscall_handler(registers_t *regs){
  kprint("\nSYSCALL RECEIVED!\n");
  UNUSED(regs);
  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
}
