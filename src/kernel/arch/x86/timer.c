#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/io.h>
#include <rhino/common.h>
#include <rhino/multitasking/scheduler.h>
#include <rhino/arch/x86/drivers/pit.h>
#include <rhino/arch/x86/drivers/hpet.h>

uint32_t irq0s = 0;
uint32_t irq8s = 0;
static void irq0_callback(registers_t *regs){
  irq0s++;
  schedule(irq0s, regs);
}

static void irq8_callback(registers_t *regs){
  irq8s++;
  UNUSED(regs);
}

void init_timer(){
  register_interrupt_handler(IRQ0, irq0_callback);
  register_interrupt_handler(IRQ8, irq8_callback);

  if(detect_hpet()){
    if(!init_hpet(1000000)){
      goto pit;
    }
  } else {
    pit:
    init_pit(100);
  }

}

uint32_t get_tick(){
  return irq0s;
}