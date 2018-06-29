#include "syscall.h"
#include "../../libc/function.h"
#include "../../drivers/screen.h"
void syscall_handler(registers_t *regs){
  kprint("HELLO SYSCALL WORLD");
  UNUSED(regs);
  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
}
