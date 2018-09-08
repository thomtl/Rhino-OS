#pragma once

#include <stdint.h>
#include <rhino/common.h>

#define MAX_TASKS 256
#define TASK_MAX_FRAMES 64

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags;
  uintptr_t cr3;
} task_registers_t;

typedef struct {
  uint32_t pid;
  uint32_t user;
  uint32_t flags;
  char *path;
} pid_t;

typedef struct {
  void* frames[TASK_MAX_FRAMES];
  uint32_t frameIndex;
} task_resources_t;

typedef struct Task {
  task_registers_t regs;
  bool used;
  pid_t pid;
  task_resources_t res;
} task_t;

void initTasking();

task_t* createTask(void(*)(), uint32_t, uintptr_t);

task_t* get_running_task();
uint32_t get_current_pid();
task_t* task_for_pid(uint32_t pid);

task_t* fork(void);
task_t* fork_sys(uint32_t eip);
void exec(task_t* task, void(*main)());
void yield();
void kill(uint32_t pid);
void kill_kern();
void waitpid(uint32_t pid);


void start_task_atomic();
void end_task_atomic();
void task_register_frame(task_t* task, void* frame);


extern void switchTask(task_registers_t *old, task_registers_t *new);
