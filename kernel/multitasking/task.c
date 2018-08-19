#include "task.h"
#include "../../drivers/screen.h"
#include "../../libk/include/string.h"
#include "../mm/hmm.h"
#include "scheduler.h"
static task_t *runningTask;
task_t tasks[MAX_TASKS];
task_t* taskArray = tasks;
//static task_t emptyTask;

/**
   @brief Compares 2 tasks and returns true if they are thesame and returns false if they aren't.
   @param args List of args.  args[0] is the first task.  args[1] is the second task.
   @return returns a bool if the comparables are the same it will return true, if they aren't it will return false.
 */
static bool compareTasks(task_t a, task_t b){
  if(a.pid.pid != b.pid.pid) return false;
  if(a.pid.user != b.pid.user) return false;
  if(a.pid.flags != b.pid.flags) return false;
  if(a.pid.path != b.pid.path) return false;
  if(a.regs.eax != b.regs.eax) return false;
  if(a.regs.ebx != b.regs.ebx) return false;
  if(a.regs.ecx != b.regs.ecx) return false;
  if(a.regs.edx != b.regs.edx) return false;
  if(a.regs.esi != b.regs.esi) return false;
  if(a.regs.edi != b.regs.edi) return false;
  if(a.regs.esp != b.regs.esp) return false;
  if(a.regs.ebp != b.regs.ebp) return false;
  if(a.regs.eip != b.regs.eip) return false;
  if(a.regs.eflags != b.regs.eflags) return false;
  if(a.regs.cr3 != b.regs.cr3) return false;
  return true;
}

/**
   @brief Searches the tasks array for the last filles element.
   @return returns the index of the final element.
 */
static uint32_t getFinalElement(){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(tasks[i].used == false) return i;
  }
  return 0;
}

/**
   @brief Takes in a task and returns the index of it.
   @param args List of args.  args[0] is the task.
   @return returns the index of the task.
 */
static uint32_t getIndexForTask(task_t task){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(compareTasks(tasks[i], task)){
      return i;
    }
  }
  return 0;
}

/**
   @brief Get a task with pid.
   @param args List of args.  args[0] is the pid to search.
   @return returns a pointer to the task.
 */
static task_t* getTaskForPid(uint32_t pid){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    if(tasks[i].pid.pid == pid) return &(tasks[i]);
  }
  return NULL;
}

/**
   @brief Initialize tasking.
 */
void initTasking(){
  for(uint32_t i = 0; i < MAX_TASKS; i++){
    tasks[i].used = false;
  }


  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(tasks[0].regs.cr3)::"%eax");
  __asm__ __volatile__("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(tasks[0].regs.eflags)::"%eax");
  tasks[0].pid.pid = 0;
  tasks[0].used = true;

  runningTask = &tasks[0];
}

/**
   @brief Create a task.
   @param args List of args.  args[0] the pointer to where to locate the task.  args[1] is a pointer to the main function. args[2] is the EFLAGS register. args[3] is the cr3 register.
 */
void createTask(void(*main)(), uint32_t flags, uintptr_t pagedir){
  task_t* task = &tasks[getFinalElement()];
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eflags = flags;
  task->regs.eip = (uint32_t) main;
  task->regs.cr3 = pagedir;
  task->regs.esp = (uint32_t) kmalloc(0x4000) + 0x4000;
  task->pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
  task->used = true;
}

/**
   @brief Kill a task.
   @param args List of args.  args[0] is the pid of the task to kill.
 */
void kill(uint32_t pid){
  task_t* task = task_for_pid(pid);
  task->used = false;
}


/**
   @brief yield away from the current task into the next.
 */
void yield(){
  task_t *last = runningTask;

  // dirty overflow trick
  uint8_t i = getIndexForTask(*runningTask) + 1;
  while(1){
    if(tasks[i].used == true){
      runningTask = &tasks[i];
      break;
    }
    i++;
  }
  switchTask(&(last->regs), &(runningTask->regs));
}


/**
   @brief get a pointer to the current task.
   @return returns a pointer to the location of the current task.
 */
task_t* get_running_task(){
  return runningTask;
}

/**
   @brief returns the pid of the current task.
   @return returns an integer that is equal to the pid of the current task.
 */
uint32_t get_current_pid(){
  return runningTask->pid.pid;
}


/**
   @brief Get the task for the pid.
   @param args List of args.  args[0] is the pid to search.
   @return returns a pointer to the task with pid in args[0].
 */
task_t* task_for_pid(uint32_t pid){
  return getTaskForPid(pid);
}

/**
   @brief Fork the current task.
   @return returns a pointer to the forked task.
 */
task_t* fork(void){
  uint32_t index = getFinalElement();
  tasks[index].regs.eax = get_running_task()->regs.eax;
  tasks[index].regs.ebx = get_running_task()->regs.ebx;
  tasks[index].regs.ecx = get_running_task()->regs.ecx;
  tasks[index].regs.edx = get_running_task()->regs.edx;
  tasks[index].regs.esi = get_running_task()->regs.esi;
  tasks[index].regs.edi = get_running_task()->regs.edi;
  tasks[index].regs.eflags = get_running_task()->regs.eflags;
  tasks[index].regs.eip = (uint32_t)__builtin_return_address(0);//(uint32_t) get_running_task()->regs.eip;
  tasks[index].regs.cr3 = (uint32_t) get_running_task()->regs.cr3;
  uint32_t stack_bottom = (uint32_t)kmalloc(0x4000);
  memcpy((uint32_t*)stack_bottom, (uint32_t*)(get_running_task()->regs.esp - 0x4000), 0x4000);
  tasks[index].regs.esp = (uint32_t) stack_bottom + 0x4000;
  tasks[index].pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
  tasks[index].used = true;
  return &tasks[index];
}

task_t* fork_sys(uint32_t eip){
  get_running_task()->regs.eip = eip;
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
  memcpy((uint32_t*)stack_bottom, (uint32_t*)(get_running_task()->regs.esp - 0x4000), 0x4000);
  tasks[index].regs.esp = (uint32_t) stack_bottom + 0x4000;
  tasks[index].pid.pid = tasks[getFinalElement() - 1].pid.pid + 1;
  tasks[index].used = true;
  return &tasks[index];
}

/**
   @brief Changes the EIP(instruction pointer) of the task.
   @param args List of args.  args[0] is the task to modify.  args[1] is function to update EIP to.
 */
void exec(task_t* task, void(*main)()){
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eip = (uint32_t) main;
  memset((void*)task->regs.esp, 0, 0x4000);
  yield();
}

/**
   @brief waits until task with pid has exited.
   @param args List of args.  args[0] is the pid of the task to wait for.
 */
void waitpid(uint32_t pid){
  task_t* task = task_for_pid(pid);
  if(task->used == false) return;
  while(task->used);
}

/**
   @brief starts a period of garanteed atomicity for the task.
 */
void start_task_atomic(){
  disable_scheduling();
}

/**
   @brief stops a period of garanteed atomicity for the task.
 */
void end_task_atomic(){
  enable_scheduling();
}
