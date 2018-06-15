#include "task.h"
#include "../../drivers/screen.h"
#include "../../libc/include/string.h"
#include "../heap/kheap.h"
#include "../types/task_linked_list.h"
static task_t *runningTask;
static task_t mainTask;
static task_t otherTask;
static task_t diffrentTask;

task_linked_list_t *taskList;

static void otherMain(){
  //for(uint32_t i = 0; i < 256; i++){
    //kprint("A");
    //yield();
//  }
  //otherTask.next = &mainTask;
  //yield();
  while(1){
    kprint("A");
    yield();
  }
}

static void diffrentMain(){
  while(1){
    kprint("B");
    yield();
    //while(1);
  }
}

//static task_t* getKernelTask(){
  //return findEntry(taskList, 0);
//}

void initTasking(){
  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
  __asm__ __volatile__("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
  mainTask.pid.pid = 0;
  taskList = createLinkedList(&mainTask);
  insertEntry(taskList, &mainTask);
  createTask(&otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
  insertEntry(taskList, &otherTask);
  createTask(&diffrentTask, diffrentMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
  insertEntry(taskList, &diffrentTask);
  mainTask.next = &otherTask;
  otherTask.next = &diffrentTask;
  diffrentTask.next = &otherTask;

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
  task->regs.eip = (uint32_t) main;
  task->regs.cr3 = (uint32_t) pagedir;
  task->regs.esp = (uint32_t)kmalloc(0x4000) + 0x4000;
  task->next = 0;
  task->pid.pid = getLastEntry(taskList)->pid.pid + 1;
}

void yield(){
  task_t *last = runningTask;
  //runningTask = findEntry(taskList, 0)->task;
  if(runningTask->pid.pid != getLastEntry(taskList)->pid.pid){
    uint32_t index = getIndex(taskList, runningTask);
    runningTask = findEntry(taskList, index + 1)->task;
  } else {
    runningTask = findEntry(taskList, 0)->task;
  }
  //runningTask = runningTask->next;
  switchTask(&(last->regs), &(runningTask->regs));
}

task_t* getCurrentTask(){
  return runningTask;
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
