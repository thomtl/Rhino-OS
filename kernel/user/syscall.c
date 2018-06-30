#include "syscall.h"
#include "../common.h"
#include "../../drivers/screen.h"
#include "../../libc/include/stdio.h"
#include "../multitasking/task.h"


static inline void sys_yield(){
  yield();
  return;
}

static inline void sys_exec(void(*main)()){
  exec(get_running_task(), main);
  return;
}

void syscall_handler(registers_t *regs){
  kprint("\nSYSCALL RECEIVED!\n");
  switch (regs->eax) {
    case 0:
      switch(regs->ebx){
        case 1:
          sys_yield();
          break;
        case 2:
          sys_exec((void(*)())regs->ecx);
          break;
        default:
          return;
      }
      break;
    case 1:
      switch(regs->ebx){
        case 1:
          putchar(regs->ecx);
          break;
        default:
          return;
      }
      break;
    default:
      return;
  }
  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
}
