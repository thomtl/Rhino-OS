#ifndef TASK_H
#define TASK_H
#include <stdint.h>

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_registers_t;

typedef struct Task {
  task_registers_t regs;
  struct Task *next;
} task_t;

extern void initTasking();
extern void createTask(task_t*, void(*)(), uint32_t, uint32_t*);

extern void yield();
extern void switchTask(task_registers_t *old, task_registers_t *new);

#endif
