#include <rhino/multitasking/scheduler.h>
#include <rhino/multitasking/spinlock.h>
#include <rhino/arch/x86/drivers/screen.h>

bool scheduling_enabled = false;
mutex_t lock;
/**
   @brief Called from the PIT interrupt it yields away into the next task if about 1 second has passed.
   @param args List of args.  args[0] is the current tick count.
 */
void schedule(registers_t* regs){
  acquire_spinlock(&lock);
  if(scheduling_enabled){
    task_t *last = get_running_task();
    task_t *tasks = get_task_array();
    // dirty overflow trick
    uint8_t i = get_current_pid() + 1;
    while(1){
      if(tasks[i].used == true && tasks[i].state == TASK_ACTIVE){
        set_running_task(&tasks[i]);
        break;
      }
      i++;
    }
    switch_context(last, get_running_task(), regs);
  }
  release_spinlock(&lock);
}

/**
   @brief Enables scheduling.
 */
void enable_scheduling(){
  scheduling_enabled = true;
}

/**
  @brief Disables scheduling
 */
void disable_scheduling(){
  scheduling_enabled = false;
}
