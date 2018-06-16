#ifndef TASK_H
#define TASK_H
#include <stdint.h>
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
  pid_t pid;
} task_t;

extern void initTasking();
extern void createTask(task_t*, void(*)(), uint32_t, uint32_t*);
task_t getCurrentTask();
uint32_t getPID();
//task_t* fork();
//void exec(task_t* task, void(*main)());
extern void yield();
extern void switchTask(task_registers_t *old, task_registers_t *new);

#endif
