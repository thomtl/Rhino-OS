#include <rhino/user/syscall.h>
#include <rhino/common.h>
#include <rhino/mm/hmm.h>
#include <rhino/mm/vmm.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/stdio.h>
#include <libk/string.h>
#include <rhino/multitasking/task.h>
#include <rhino/fs/vfs.h>
#include <rhino/fs/initrd.h>
#include <rhino/arch/x86/drivers/keyboard.h>
#include <rhino/user/init.h>
extern pdirectory* kernel_directory;

static inline void sys_task_set_argv(pid_t pid, uint32_t argv){
  task_set_argv(pid, argv);
}

static inline uint32_t sys_task_get_argv(pid_t pid){
  return task_get_argv(pid);
}

static inline void sys_task_set_argc(pid_t pid, uint32_t argc){
  task_set_argc(pid, argc);
}

static inline uint32_t sys_task_get_argc(pid_t pid){
  return task_get_argc(pid);
}

static inline struct dirent* sys_readir(uint32_t i){
  return readdir_fs(fs_root, i);
}

static inline void sys_waitpid(uint32_t pid){
  waitpid(pid);
}

static inline void sys_kill(uint32_t pid){
  kill(pid);
}

static inline bool sys_create_process(char* prg, uint32_t* pid){
  return create_process(prg, pid);
}

static inline void sys_set_color(uint8_t fg, uint8_t bg){
  set_color(fg, bg);
}

static inline fs_node_t* sys_finddir_fs(char* file){
  return finddir_fs(fs_root, file);
}

static inline void* sys_malloc(size_t size){
  return umalloc(size);
}

static inline void sys_free(void* addr){
  ufree(addr);
}

static inline uint32_t sys_read_fs(fs_node_t* node, uint32_t size, uint8_t* buffer){
  return read_fs(node, 0, size, buffer);
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

static inline void sys_putchar(uint8_t c){
  putchar(c);
  return;
}

void syscall_handler(registers_t *regs){
  switch (regs->eax) {
    case 0:
      switch(regs->ebx){
        case 2:
          sys_start_task_atomic();
          break;
        case 3:
          sys_end_task_atomic();
          break;
        case 5:
          sys_exit();
          break;
        case 7:
          regs->eax = sys_current_pid();
          break;
        case 8:
          sys_waitpid(regs->ecx);
          break;
        case 9:
          sys_task_set_argv((pid_t)regs->ecx, regs->edx);
          break;
        case 10:
          regs->eax = sys_task_get_argv(regs->ecx);
          break;
        case 11:
          sys_task_set_argc((pid_t)regs->ecx, regs->edx);
          break;
        case 12:
          regs->eax = sys_task_get_argc(regs->ecx);
          break;
        case 13:
          regs->eax = sys_create_process((char*)regs->ecx, (uint32_t*)regs->edx);
          break;
        case 14:
          sys_kill(regs->ecx);
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
        case 3:
          sys_set_color(regs->ecx, regs->edx);
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
        case 2:
          regs->eax = (uint32_t)sys_readir(regs->ecx);
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
          regs->eax = sys_kbd_getchar();
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
