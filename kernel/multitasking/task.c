#include "task.h"
#include "../../drivers/screen.h"
#include "../../libc/include/string.h"
#include "../heap/kheap.h"

static task_t *runningTask;
static task_t tasks[MAX_TASKS];
static task_t emptyTask;
static uint16_t yy = 0;
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

static task_t* getTaskForPid(uint32_t pid){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(tasks[i].pid.pid == pid) return &(tasks[i]);
  }
  return NULL;
}

static void otherMain(){
  while(1){
    kprint("A");
    if(yy < 4){
       fork();
       yy++;
    }
    yield();
  }
  while(1);
}

static void diffrentMain(){
  while(1){
    kprint("B");
    if(yy < 4){
       fork();
       yy++;
    }
    yield();
  }
  while(1);
}

void initTasking(){
  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(tasks[0].regs.cr3)::"%eax");
  __asm__ __volatile__("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(tasks[0].regs.eflags)::"%eax");
  tasks[0].pid.pid = 0;

  createTask(&(tasks[1]), otherMain, tasks[0].regs.eflags, (uint32_t*)tasks[0].regs.cr3);

  createTask(&(tasks[2]), diffrentMain, tasks[0].regs.eflags, (uint32_t*)tasks[0].regs.cr3);

  runningTask = &tasks[0];
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
  task->regs.esp = (uint32_t) kmalloc(0x4000) + 0x4000;
  task->pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
}

void yield(){
  task_t *last = runningTask;

  uint32_t index = getIndexForTask(*runningTask);
  if(index == getFinalElement() - 1) {
    runningTask = &tasks[0];
  } else {
    runningTask = &tasks[index + 1];
  }
  
  switchTask(&(last->regs), &(runningTask->regs));
}

task_t* get_running_task(){
  return runningTask;
}
uint32_t get_current_pid(){
  return runningTask->pid.pid;
}

task_t* task_for_pid(uint32_t pid){
  return getTaskForPid(pid);
}

task_t* fork(void){
  uint32_t index = getFinalElement();
  tasks[index].regs.eax = get_running_task()->regs.eax;
  tasks[index].regs.ebx = get_running_task()->regs.ebx;
  tasks[index].regs.ecx = get_running_task()->regs.ecx;
  tasks[index].regs.edx = get_running_task()->regs.edx;
  tasks[index].regs.esi = get_running_task()->regs.esi;
  tasks[index].regs.edi = get_running_task()->regs.edi;
  tasks[index].regs.eflags = get_running_task()->regs.eflags;
  tasks[index].regs.eip = (uint32_t) get_running_task()->regs.eip;
  tasks[index].regs.cr3 = (uint32_t) get_running_task()->regs.cr3;
  uint32_t stack_bottom = (uint32_t)kmalloc(0x4000);
  //memcpy((uint32_t*)stack_bottom, (uint32_t*)(get_running_task()->regs.esp - 0x4000), 0x4000);
  tasks[index].regs.esp = (uint32_t) stack_bottom + 0x4000;
  tasks[index].pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
  return &tasks[index];
}
void exec(task_t* task, void(*main)()){
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eip = (uint32_t) main;
  yield();
}
