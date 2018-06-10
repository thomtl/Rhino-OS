#include "task.h"
#include "../../drivers/screen.h"

#include "../heap/kheap.h"
static task_t *runningTask;
static task_t mainTask;
static task_t otherTask;

static void otherMain(){
  kprint("Hello multitasking world!\n");
  yield();
}

void initTasking(){
  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
  __asm__ __volatile__("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
  //__asm__ __volatile__ ("movl %%esp, %0":"=m"(mainTask.regs.esp));

  createTask(&otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
  mainTask.next = &otherTask;
  otherTask.next =  &mainTask;

  runningTask = &mainTask;
}

void createTask(task_t *task, void(*main)(), uint32_t flags, uint32_t *pagedir){
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eflags = flags;
  task->regs.eip = (uint32_t)main;
  task->regs.cr3 = (uint32_t) pagedir;
  task->regs.esp = (uint32_t)kmalloc_a(0x4000) + 0x4000;
  task->next = 0;
}

void yield(){
  task_t *last = runningTask;
  runningTask = runningTask->next;
  switchTask(&last->regs, &runningTask->regs);
}
