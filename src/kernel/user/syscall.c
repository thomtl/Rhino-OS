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
#include <rhino/user/program.h>
#include <rhino/pwr/power.h>

extern pdirectory* kernel_directory;

static inline void sys_chdir(char* path, uint32_t pid){
  char* p = canonicalize_path(task_get_working_directory(task_for_pid(pid)), path);

  task_set_working_directory(task_for_pid(pid), p);

  kfree(p);
}

static inline void sys_getcwd(char* buf, size_t len){
  char* cwd = task_get_working_directory(get_running_task());

  size_t cwd_len = strlen(cwd);
  if(len < cwd_len) memcpy(buf, cwd, len);
  else memcpy(buf, cwd, cwd_len);
}

static inline void sys_close(uint32_t node){
  close_fs((fs_node_t*)node);
}

static inline struct dirent* sys_readdir(int i){
  fs_node_t* n = kopen(task_get_working_directory(get_running_task()), O_RDONLY);
  struct dirent* dir = readdir_fs(n, i);
  close_fs(n);
  return dir;
}

static inline void sys_reboot(){
  reboot();
}

static inline void sys_shutdown(){
  shutdown();
}

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

static inline fs_node_t* sys_open(char* file, uint32_t flags){
  return kopen(file, flags);
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
        case 0:
          sys_start_task_atomic();
          break;
        case 1:
          sys_end_task_atomic();
          break;
        case 2:
          sys_exit();
          break;
        case 3:
          regs->eax = sys_current_pid();
          break;
        case 4:
          sys_waitpid(regs->ecx);
          break;
        case 5:
          sys_task_set_argv((pid_t)regs->ecx, regs->edx);
          break;
        case 6:
          regs->eax = sys_task_get_argv(regs->ecx);
          break;
        case 7:
          sys_task_set_argc((pid_t)regs->ecx, regs->edx);
          break;
        case 8:
          regs->eax = sys_task_get_argc(regs->ecx);
          break;
        case 9:
          regs->eax = sys_create_process((char*)regs->ecx, (uint32_t*)regs->edx);
          break;
        case 10:
          sys_kill(regs->ecx);
          break;
        case 11:
          sys_chdir((char*)regs->ecx, regs->edx);
          break;
        case 12:
          sys_getcwd((char*)regs->ecx, regs->edx);
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
          regs->eax = (uint32_t)sys_open((char*)regs->ecx, regs->edx);
          break;
        case 2:
          sys_close(regs->ecx);
          break;
        case 3:
          regs->eax = (uint32_t)sys_readdir(regs->ecx);
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
          regs->eax = sys_kbd_getchar();
          break;
        default:
          break;
      }
      break;
    case 6:
      switch(regs->ebx){
        case 0:
          sys_shutdown();
          break;
        case 1:
          sys_reboot();
          break;
        default:
          break;
      }
      break;
    default:
      return;
  }
  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
  debug_log("[SYSCALL]: Syscalls initialized\n");
}
