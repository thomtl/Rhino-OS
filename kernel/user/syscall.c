#include "syscall.h"
#include "../common.h"
#include "../../drivers/screen.h"
#include "../../libc/include/stdio.h"
#include "../multitasking/task.h"


static inline task_t* sys_fork(){
  return fork();
}

static inline int sys_current_pid(){
  return get_current_pid();
}

static inline void sys_clear_screen(){
  clear_screen();
}

static inline void sys_start_task_atomic(){
  start_task_atomic();
}

static inline void sys_end_task_atomic(){
  end_task_atomic();
}

static inline void sys_yield(){
  yield();
  return;
}

static inline void sys_exec(void(*main)()){
  exec(get_running_task(), main);
  return;
}

static inline void sys_putchar(uint8_t c){
  putchar(c);
  return;
}

void syscall_handler(registers_t *regs){
  switch (regs->eax) {
    case 0:
      switch(regs->ebx){
        case 1:
          sys_yield();
          break;
        case 2:
          sys_start_task_atomic();
          break;
        case 3:
          sys_end_task_atomic();
          break;
        case 4:
          sys_exec((void(*)())regs->ecx);
          break;
        case 5:
          kprint_err("Exit not implemented");
          break;
        case 6:
          regs->eax = (uint32_t)sys_fork();
          break;
        case 7:
          regs->eax = sys_current_pid();
          break;
        default:
          return;
      }
      break;
    case 1:
      switch(regs->ebx){
        case 1:
          sys_putchar(regs->ecx);
          break;
        case 2:
          sys_clear_screen();
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
