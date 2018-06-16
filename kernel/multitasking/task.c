#include "task.h"
#include "../../drivers/screen.h"
#include "../../libc/include/string.h"
#include "../heap/kheap.h"

static task_t *runningTask;
static task_t kernelTask;
static task_t tasks[MAX_TASKS];
static task_t emptyTask;
uint32_t f = 1;
static uint32_t compareTasks(task_t a, task_t b){
  if(a.pid.pid != b.pid.pid) return 0;
  if(a.pid.user != b.pid.user) return 0;
  if(a.pid.flags != b.pid.flags) return 0;
  if(a.pid.path != b.pid.path) return 0;
  if(a.regs.eax != b.regs.eax) return 0;
  if(a.regs.ebx != b.regs.ebx) return 0;
  if(a.regs.ecx != b.regs.ecx) return 0;
  if(a.regs.edx != b.regs.edx) return 0;
  if(a.regs.esi != b.regs.esi) return 0 ;
  if(a.regs.edi != b.regs.edi) return 0;
  if(a.regs.esp != b.regs.esp) return 0;
  if(a.regs.ebp != b.regs.ebp) return 0;
  if(a.regs.eip != b.regs.eip) return 0;
  if(a.regs.eflags != b.regs.eflags) return 0;
  if(a.regs.cr3 != b.regs.cr3) return 0;
  return 1;
}

static uint32_t getFinalElement(){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(compareTasks(tasks[i], emptyTask) == 1){
      return i;
    }
  }
  return 0;
}

static uint32_t getIndexForTask(task_t task){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(compareTasks(tasks[i], task) == 1){
      return i;
    }
  }
  return 0;
}

static void otherMain(){
  while(1){
    kprint("A");
    f = 0;
    yield();
  }
  while(1);
}

static void diffrentMain(){
  while(1){
    kprint("B");
    yield();
  }
  while(1);
}

//static task_t* getKernelTask(){
//  return findEntry(taskList, 0)->task;
//}

void initTasking(){
  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(kernelTask.regs.cr3)::"%eax");
  __asm__ __volatile__("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(kernelTask.regs.eflags)::"%eax");
  kernelTask.pid.pid = 0;
  tasks[0] = kernelTask;
  //task_t otherTask;
  //task_t diffrentTask;
  createTask(&(tasks[1]), otherMain, kernelTask.regs.eflags, (uint32_t*)kernelTask.regs.cr3);
  //tasks[1] = otherTask;
  createTask(&(tasks[2]), diffrentMain, kernelTask.regs.eflags, (uint32_t*)kernelTask.regs.cr3);
  //tasks[2] = diffrentTask;
  //kernelTask.next = &otherTask;
  //otherTask.next = &diffrentTask;
  //  diffrentTask.next = &otherTask;

  runningTask = &kernelTask;
}

void createTask(task_t *task, void(*main)(), uint32_t flags, uint32_t *pagedir){
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eflags = flags;
  task->regs.eip = (uint32_t) main;
  task->regs.cr3 = (uint32_t) pagedir;
  task->regs.esp = (uint32_t)kmalloc(0x4000) + 0x4000;
  task->pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
}

void yield(){
  task_t *last = runningTask;
  //runningTask = findEntry(taskList, 0)->task;
  /*if(0){//runningTask->pid.pid == getLastEntry(taskList)->pid.pid){
    runningTask = getKernelTask();
  } else {
    uint32_t index = getIndex(taskList, runningTask);
    runningTask = findEntry(taskList, index + 1)->task;
  }*/
  uint32_t index = getIndexForTask(*runningTask);
  if(index == getFinalElement() - 1) {
    runningTask = &tasks[0];
  } else {
    runningTask = &tasks[index + 1];
  }
  runningTask = &tasks[f];
  //runningTask = findEntry(taskList, 1)->task;//getKernelTask();
  //runningTask = runningTask->next;
  switchTask(&(last->regs), &(runningTask->regs));
}

task_t getCurrentTask(){
  return *runningTask;
}
uint32_t getPID(){
  return runningTask->pid.pid;
}
/*task_t* fork(){
  task_t* task = 0;
  task->regs.eax = getCurrentTask()->regs.eax;
  task->regs.ebx = getCurrentTask()->regs.ebx;
  task->regs.ecx = getCurrentTask()->regs.ecx;
  task->regs.edx = getCurrentTask()->regs.edx;
  task->regs.esi = getCurrentTask()->regs.esi;
  task->regs.edi = getCurrentTask()->regs.edi;
  task->regs.eflags = getCurrentTask()->regs.eflags;
  task->regs.eip = (uint32_t) getCurrentTask()->regs.eip;
  task->regs.cr3 = (uint32_t) getCurrentTask()->regs.cr3;
  uint32_t stack_bottom = (uint32_t)kmalloc(0x4000);
  memcpy((uint32_t*)stack_bottom, (uint32_t*)(getCurrentTask()->regs.esp - 0x4000), 0x4000);
  task->regs.esp = (uint32_t) stack_bottom + 0x4000;
  task->next = 0;
  task->pid.pid = getLastEntry(taskList)->pid.pid + 1;
  insertEntry(taskList, task);
  return task;
}
void exec(task_t* task, void(*main)()){
  task->regs.eip = (uint32_t) main;
}*/
