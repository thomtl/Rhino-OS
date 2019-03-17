#pragma once

#include <stdint.h>
#include <rhino/common.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/fs/vfs.h>

#define MAX_TASKS 256
#define TASK_MAX_FRAMES 64

#define TASK_ACTIVE 0x0
#define TASK_BLOCKED 0x1

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, ss, cs, ds;
  uintptr_t cr3;

  uint8_t fpu_regs[512];

} task_registers_t;

typedef uint8_t pid_t;

typedef struct {
  pid_t pid;
  uint32_t user;
  uint32_t flags;
  char *path;
  uint32_t time_running;
} process_data_t;

typedef struct {
  void* frames[TASK_MAX_FRAMES];
  uint32_t frameIndex;
} task_resources_t;

typedef struct {
  fs_node_t** entries;
  uint64_t* offsets;
  int* modes;
  size_t length;
  size_t capacity;
  size_t refs;
} fd_table_t;

typedef struct Task {
  task_registers_t regs;
  bool used;
  uint32_t state;
  uint32_t argv;
  uint32_t argc;
  process_data_t pid;
  task_resources_t res;
  fd_table_t fd_table;
} task_t;

void initTasking();

task_t* createTask(void(*)(), uint32_t, uintptr_t);

task_t* get_running_task();
void set_running_task(task_t* task);
uint32_t get_current_pid();
task_t* task_for_pid(uint32_t pid);

void kill(uint32_t pid);
void kill_kern();
void waitpid(uint32_t pid);

void start_task_atomic();
void end_task_atomic();
void task_register_frame(task_t* task, void* frame);

void task_set_argv(pid_t pid, uint32_t argv);
uint32_t task_get_argv(pid_t pid);

void task_set_argc(pid_t pid, uint32_t argc);
uint32_t task_get_argc(pid_t pid);

task_t* get_task_array();

void switch_context(task_t *old, task_t *new, registers_t* r);

void block_task(uint32_t reason);
void unblock_task();

void task_set_working_directory(task_t* task, char* dir);
char* task_get_working_directory(task_t* task);

uint32_t task_append_fd(task_t* task, fs_node_t* node);
