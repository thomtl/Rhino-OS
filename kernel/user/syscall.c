#include "syscall.h"
#include "../common.h"
#include "../mm/kheap.h"
#include "../../drivers/screen.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/string.h"
#include "../multitasking/task.h"
#include "../fs/vfs.h"
#include "../fs/initrd.h"
#include "../../drivers/keyboard.h"

static inline char sys_getchar(){
  return kbd_getchar();
}

static inline fs_node_t* sys_finddir_fs(char* file){
  return finddir_fs(fs_root, file);
}

static inline void* sys_malloc(size_t size){
  return kmalloc(size);
}

static inline void sys_free(void* addr){
  kfree(addr);
}

static inline uint32_t sys_read_fs(fs_node_t* node, uint32_t size, uint8_t* buffer){
  return read_fs(node, 0, size, buffer);
}

static inline task_t* sys_fork(uint32_t eip){
  return fork_sys(eip);
}

static inline void sys_exit(){
  task_t* t = get_running_task();
  kill(t->pid.pid);
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
  char buf[25] = "";
  hex_to_ascii(regs->eip, buf);
  kprint(buf);
  kprint("\n");
  for(int i = 0; i < 25; i++) buf[i] = '\0';
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
          sys_exit();
          break;
        case 6:
          regs->eax = (uint32_t)sys_fork(regs->eip);
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
    case 2:
      switch(regs->ebx){
        case 1:
          regs->eax = (uint32_t)sys_malloc(regs->ecx);
          break;
        case 2:
          sys_free((void*)regs->ecx);
          break;
        default:
          break;
      }
      break;
    case 3:
      switch(regs->ebx){
        case 1:
          regs->eax = (uint32_t)sys_finddir_fs((char*)regs->ecx);
          break;
        default:
          break;
      }
      break;
    case 4:
      regs->eax = sys_read_fs((fs_node_t*)regs->ebx, regs->ecx, (uint8_t*)regs->edx);
      break;
    case 5:
      switch(regs->ebx){
        case 1:
          regs->eax = sys_getchar();
          break;
        default:
          break;
      }
    default:
      return;
  }
  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
}
