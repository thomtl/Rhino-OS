#include <rhino/user/syscall.h>
#include <rhino/common.h>
#include <rhino/mm/hmm.h>
#include <rhino/mm/vmm.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/stdio.h>
#include <libk/string.h>
#include <rhino/multitasking/task.h>
#include <rhino/fs/vfs.h>
#include <rhino/arch/x86/drivers/keyboard.h>
#include <rhino/user/program.h>
#include <rhino/pwr/power.h>

static inline void fd_check(int fd){
  task_t* task = get_running_task();

  if(fd < (int)task->fd_table.length && fd >= 0 && task->fd_table.entries[fd] != NULL) return;

  PANIC_M("FD Invalid, code task killing in case of segmentation fault\n");
}

static inline fs_node_t* fd_entry(int fd){
  fd_check(fd);
  return get_running_task()->fd_table.entries[fd];
}

static inline uint32_t fd_offset(int fd){
  fd_check(fd);
  return get_running_task()->fd_table.offsets[fd];
}



static inline void ptr_check(void* ptr){
  (void)(ptr);
  return; // Implement this
}

static int sys_stat(int fd, uint32_t st){
  ptr_check((void*)st);
  fs_node_t* node = fd_entry(fd);
  struct stat* f = (struct stat*)st;
  f->st_dev = 0;
  f->st_ino = node->inode;

  uint32_t flags = 0;
  if(node->flags & FS_FILE) flags |= _IFREG;
  if(node->flags & FS_DIRECTORY) flags |= _IFDIR;
  if(node->flags & FS_CHARDEVICE) flags |= _IFCHR;
  if(node->flags & FS_BLOCKDEVICE) flags |= _IFBLK;
  if(node->flags & FS_PIPE) flags |= _IFIFO;
  if(node->flags & FS_SYMLINK) flags |= _IFLNK;

  f->st_mode = node->mask | flags;
  f->st_nlink = 0;
  f->st_uid = node->uid;
  f->st_gid = node->gid;
  f->st_rdev = 0;
  f->st_size = node->length;

  return 0;
}

static inline int sys_chdir(char* path, uint32_t pid){
  ptr_check(path);

  char* p = canonicalize_path(task_get_working_directory(task_for_pid(pid)), path);
  fs_node_t* chd = kopen(p, 0);
  if(chd){
    if((chd->flags & FS_DIRECTORY) == 0){
      close_fs(chd);
      debug_log("[MULTITASKING]: tried to chdir to file not directory\n");
      return 1;
    }
    task_set_working_directory(task_for_pid(pid), p);
    close_fs(chd);
  }
  kfree(p);
  return 0;
}

static inline void sys_getcwd(char* buf, size_t len){
  ptr_check(buf);

  char* cwd = task_get_working_directory(get_running_task());

  size_t cwd_len = strlen(cwd);
  if(len < cwd_len) memcpy(buf, cwd, len);
  else memcpy(buf, cwd, cwd_len);
}

static inline void sys_close(uint32_t fd){

  close_fs(fd_entry(fd));
  get_running_task()->fd_table.entries[fd] = NULL;
}

static inline struct dirent* sys_readdir(int fd, int i){
  struct dirent* dir = readdir_fs(fd_entry(fd), i);
  return dir;
}

static inline int sys_open(char* file, uint32_t flags){
  ptr_check(file);

  fs_node_t* node = kopen(file, flags);

  int fd = task_append_fd(get_running_task(), node);

  get_running_task()->fd_table.modes[fd] = 0;
  if(flags & O_APPEND) get_running_task()->fd_table.offsets[fd] = node->length;
  else get_running_task()->fd_table.offsets[fd] = 0;

  return fd;
}

static inline uint32_t sys_read_fs(uint32_t fd, uint32_t size, uint8_t* buffer){
  ptr_check(buffer);

  uint32_t out = read_fs(fd_entry(fd), fd_offset(fd), size, buffer);
  get_running_task()->fd_table.offsets[fd] += out;

  return out;
}

static inline uint32_t sys_write_fs(uint32_t fd, uint32_t size, uint8_t* buffer){
  ptr_check(buffer);
  uint32_t out = write_fs(fd_entry(fd), fd_offset(fd), size, buffer);
  get_running_task()->fd_table.offsets[fd] += out;

  return out;
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
  ptr_check(pid);

  return create_process(prg, pid);
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

static inline void sys_putchar(uint8_t c){
  putchar(c);
  return;
}

static inline void sys_set_color(uint8_t fg, uint8_t bg){
  set_color(fg, bg);
}

static inline void sys_start_task_atomic(){
  start_task_atomic();

}

static inline void sys_end_task_atomic(){
  end_task_atomic();
}

static inline void* sys_malloc(size_t size){
  return kmalloc(size);
}

static inline void sys_free(void* addr){
  ptr_check(addr);

  kfree(addr);
}


void syscall_handler(registers_t *regs){
  switch (regs->eax)
  {
    case 0:
      sys_exit();
      break;

    case 1:
      sys_start_task_atomic();
      break;

    case 2:
      sys_end_task_atomic();
      break;

    case 3:
      regs->eax = sys_current_pid();
      break;

    case 4:
      sys_waitpid(regs->ebx);
      break;

    case 5:
      sys_task_set_argv(regs->ebx, regs->ecx);
      break;
    
    case 6:
      regs->eax = sys_task_get_argv(regs->ebx);
      break;

    case 7:
      sys_task_set_argc(regs->ebx, regs->ecx);
      break;

    case 8:
      regs->eax = sys_task_get_argc(regs->ebx);
      break;

    case 9:
      regs->eax = sys_create_process((char*)regs->ebx, (uint32_t*)regs->ecx);
      break;

    case 10:
      sys_kill(regs->ebx);
      break;

    case 11:
      regs->eax = (uint32_t)sys_chdir((char*)regs->ebx, regs->ecx);
      break;

    case 12:
      sys_getcwd((char*)regs->ebx, regs->ecx);
      break;

    case 13:
      sys_putchar(regs->ebx);
      break;
    
    case 14:
      clear_screen();
      break;

    case 15:
      sys_set_color(regs->ebx, regs->ecx);
      break;

    case 16:
      regs->eax = (uint32_t)sys_malloc(regs->ebx);
      break;

    case 17:
      sys_free((void*)regs->ebx);
      break;

    case 18:
      regs->eax = (uint32_t)sys_open((char*)regs->ebx, regs->ecx);
      break;

    case 19:
      sys_close(regs->ebx);
      break;

    case 20:
      regs->eax = (uint32_t)sys_readdir(regs->ebx, regs->ecx);
      break;

    case 21:
      regs->eax = sys_read_fs(regs->ebx, regs->ecx, (uint8_t*)regs->edx);
      break;

    case 22:
      regs->eax = sys_write_fs(regs->ebx, regs->ecx, (uint8_t*)regs->edx);
      break;

    case 23:
      regs->eax = sys_kbd_getchar();
      break;

    case 24:
      sys_shutdown();
      break;

    case 25:
      sys_reboot();
      break;

    case 26:
      regs->eax = sys_stat(regs->ebx, regs->ecx);
      break;
        
    default:
      break;
  }

  return;
}

void init_syscall(){
  register_interrupt_handler(128, syscall_handler);
  debug_log("[SYSCALL]: Syscalls initialized\n");
}
