#include <rhino/multitasking/scheduler.h>
#include <rhino/arch/x86/drivers/screen.h>
uint32_t seconds_passed = 0;
bool scheduling_enabled = false;


/**
   @brief Called from the PIT interrupt it yields away into the next task if about 1 second has passed.
   @param args List of args.  args[0] is the current tick count.
 */
void schedule(uint32_t tick, registers_t* regs){
  seconds_passed = TICK_TO_SECONDS(tick);
  if(scheduling_enabled){
    if(seconds_passed % TASK_TIME_SLICE == 0){
      yield(regs);
      seconds_passed = 0;
    }
  }
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
