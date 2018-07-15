#ifndef TASK_H
#define TASK_H
#include <stdint.h>
#include "../common.h"

#define MAX_TASKS 256
typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_registers_t;

typedef struct {
  uint32_t pid;
  uint32_t user;
  uint32_t flags;
  char *path;
} pid_t;

typedef struct Task {
  task_registers_t regs;
  bool used;
  pid_t pid;
} task_t;

void initTasking();

void createTask(task_t*, void(*)(), uint32_t, uint32_t*);

task_t* get_running_task();
uint32_t get_current_pid();
task_t* task_for_pid(uint32_t pid);

task_t* fork(void);
void exec(task_t* task, void(*main)());
void yield();
void kill(uint32_t pid);

extern void switchTask(task_registers_t *old, task_registers_t *new);

#endif
