#pragma once

#include <stdint.h>
#include <rhino/common.h>
#include <rhino/arch/x86/isr.h>

#define MAX_TASKS 256
#define TASK_MAX_FRAMES 64

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, ss, cs, ds;
  uintptr_t cr3;
} task_registers_t;
typedef uint8_t pid_t;
typedef struct {
  pid_t pid;
  uint32_t user;
  uint32_t flags;
  char *path;
} process_data_t;

typedef struct {
  void* frames[TASK_MAX_FRAMES];
  uint32_t frameIndex;
} task_resources_t;

typedef struct Task {
  task_registers_t regs;
  bool used;
  bool blocked;
  uint32_t argv;
  uint32_t argc;
  process_data_t pid;
  task_resources_t res;
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